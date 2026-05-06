#include "PlotRenderer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "internal/Vma.hpp"

namespace FlowPlotGui {
namespace {

constexpr const char* kSceneVertexShaderFile = "plot_viewport_solid.vert.spv";
constexpr const char* kSceneFragmentShaderFile = "plot_viewport_solid.frag.spv";

struct DataViewBuildResult {
	std::unordered_map<FlowInternal::DataKey, FlowInternal::DataView, FlowInternal::DataKeyHash> views{};
	std::vector<std::unique_ptr<bool[]>> boolScratch{};
};

PlotRenderer::PackedColor packColor(const FlowPlot::Color& color)
{
	constexpr float inv255 = 1.0f / 255.0f;
	return PlotRenderer::PackedColor{
		.r = static_cast<float>(color.r) * inv255,
		.g = static_cast<float>(color.g) * inv255,
		.b = static_cast<float>(color.b) * inv255,
		.a = static_cast<float>(color.a) * inv255,
	};
}

std::uint32_t markerShapeId(FlowPlot::MarkerShape shape)
{
	return static_cast<std::uint32_t>(shape);
}

FlowUi::FontStyle toFlowUiFontStyle(FlowPlot::FontStyle style)
{
	return style == FlowPlot::FontStyle::Italic || style == FlowPlot::FontStyle::Oblique
		? FlowUi::FontStyle::Italic
		: FlowUi::FontStyle::Normal;
}

FlowPlot::RectF fullScissorForPlot(const FlowPlot::RenderPlot& plot)
{
	return FlowPlot::RectF{
		.x = 0.0f,
		.y = 0.0f,
		.w = static_cast<float>(plot.width),
		.h = static_cast<float>(plot.height),
	};
}

bool sameRect(const FlowPlot::RectF& lhs, const FlowPlot::RectF& rhs)
{
	constexpr float epsilon = 0.001f;
	return std::abs(lhs.x - rhs.x) <= epsilon
		&& std::abs(lhs.y - rhs.y) <= epsilon
		&& std::abs(lhs.w - rhs.w) <= epsilon
		&& std::abs(lhs.h - rhs.h) <= epsilon;
}

FlowPlot::RectF intersectRect(const FlowPlot::RectF& lhs, const FlowPlot::RectF& rhs)
{
	const float x0 = std::max(lhs.x, rhs.x);
	const float y0 = std::max(lhs.y, rhs.y);
	const float x1 = std::min(lhs.x + lhs.w, rhs.x + rhs.w);
	const float y1 = std::min(lhs.y + lhs.h, rhs.y + rhs.h);
	return FlowPlot::RectF{
		.x = x0,
		.y = y0,
		.w = std::max(0.0f, x1 - x0),
		.h = std::max(0.0f, y1 - y0),
	};
}

void vkCheck(VkResult result, const char* message)
{
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error(message);
	}
}

std::vector<char> readFile(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open shader file: " + path);
	}

	const std::streamsize fileSize = file.tellg();
	if (fileSize <= 0)
	{
		throw std::runtime_error("Shader file is empty: " + path);
	}

	std::vector<char> buffer(static_cast<std::size_t>(fileSize));
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	if (!file)
	{
		throw std::runtime_error("Failed to read shader file: " + path);
	}

	return buffer;
}

std::vector<char> readShaderFile(const char* fileName)
{
	const std::string relativePath = std::string("shaders/") + fileName;
	std::ifstream relativeProbe(relativePath, std::ios::binary);
	if (relativeProbe.good())
	{
		relativeProbe.close();
		return readFile(relativePath);
	}

#ifdef FLOWPLOTGUI_SHADER_OUTPUT_DIR
	const std::string buildPath = std::string(FLOWPLOTGUI_SHADER_OUTPUT_DIR) + "/" + fileName;
	std::ifstream buildProbe(buildPath, std::ios::binary);
	if (buildProbe.good())
	{
		buildProbe.close();
		return readFile(buildPath);
	}
#endif

	throw std::runtime_error("Failed to locate shader file: " + std::string(fileName));
}

VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code)
{
	if (code.empty() || (code.size() % sizeof(std::uint32_t)) != 0)
	{
		throw std::runtime_error("Invalid SPIR-V shader bytecode.");
	}

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const std::uint32_t*>(code.data());

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	vkCheck(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Failed to create plot shader module.");
	return shaderModule;
}

VkPipeline createPipeline(
	VkDevice device,
	VkFormat colorFormat,
	VkPipelineLayout pipelineLayout,
	VkShaderModule vertexShader,
	VkShaderModule fragmentShader)
{
	if (colorFormat == VK_FORMAT_UNDEFINED)
	{
		throw std::runtime_error("Cannot create plot pipeline with undefined color format.");
	}

	VkPipelineShaderStageCreateInfo shaderStages[2]{};
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShader;
	shaderStages[0].pName = "main";
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragmentShader;
	shaderStages[1].pName = "main";

	VkPipelineVertexInputStateCreateInfo vertexInput{};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterization{};
	rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization.cullMode = VK_CULL_MODE_NONE;
	rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisample{};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blendState{};
	blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendState.attachmentCount = 1;
	blendState.pAttachments = &blendAttachment;

	const std::array<VkDynamicState, 2> dynamicStates{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineRenderingCreateInfo rendering{};
	rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	rendering.colorAttachmentCount = 1;
	rendering.pColorAttachmentFormats = &colorFormat;

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &rendering;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterization;
	pipelineInfo.pMultisampleState = &multisample;
	pipelineInfo.pColorBlendState = &blendState;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;

	VkPipeline pipeline = VK_NULL_HANDLE;
	vkCheck(
		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline),
		"Failed to create plot graphics pipeline.");
	return pipeline;
}

void destroyBuffer(VmaAllocator allocator, PlotRenderer::AllocatedBuffer& buffer)
{
	if (allocator != nullptr && buffer.buffer != VK_NULL_HANDLE)
	{
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
	}
	buffer = {};
}

void createMappedBuffer(
	VmaAllocator allocator,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	PlotRenderer::AllocatedBuffer& outBuffer)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	allocationCreateInfo.flags =
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo allocationInfo{};
	vkCheck(
		vmaCreateBuffer(
			allocator,
			&bufferInfo,
			&allocationCreateInfo,
			&outBuffer.buffer,
			&outBuffer.allocation,
			&allocationInfo),
		"Failed to create plot renderer mapped buffer.");

	outBuffer.size = size;
	outBuffer.mapped = allocationInfo.pMappedData;
	if (outBuffer.mapped == nullptr)
	{
		vkCheck(vmaMapMemory(allocator, outBuffer.allocation, &outBuffer.mapped), "Failed to map plot renderer buffer.");
	}
}

template <typename T>
void uploadVector(VmaAllocator allocator, PlotRenderer::AllocatedBuffer& buffer, const std::vector<T>& values)
{
	const VkDeviceSize byteSize = static_cast<VkDeviceSize>(values.size() * sizeof(T));
	if (byteSize == 0)
	{
		return;
	}
	if (allocator == nullptr)
	{
		throw std::runtime_error("Cannot upload plot renderer buffers without a VMA allocator.");
	}
	if (buffer.buffer == VK_NULL_HANDLE || buffer.size < byteSize)
	{
		destroyBuffer(allocator, buffer);
		createMappedBuffer(allocator, byteSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer);
	}
	if (buffer.mapped == nullptr)
	{
		throw std::runtime_error("Plot renderer upload buffer is not mapped.");
	}
	std::memcpy(buffer.mapped, values.data(), static_cast<std::size_t>(byteSize));
	vkCheck(vmaFlushAllocation(allocator, buffer.allocation, 0, byteSize), "Failed to flush plot renderer upload buffer.");
}

void addDataView(
	DataViewBuildResult& result,
	const std::string& datasetName,
	const std::string& fieldName,
	const void* data,
	std::size_t count,
	std::size_t elemSize,
	std::type_index type,
	FlowInternal::DataView::ValueType valueType)
{
	if (datasetName.empty() || fieldName.empty())
	{
		return;
	}

	FlowInternal::DataView view{};
	view.ptr = data;
	view.count = count;
	view.elemSize = elemSize;
	view.type = type;
	view.valueType = valueType;
	result.views[FlowInternal::DataKey{datasetName, fieldName}] = view;
}

DataViewBuildResult makeDataViews(const std::vector<RunningDataset>& datasets)
{
	DataViewBuildResult result{};
	std::size_t fieldCount = 0;
	std::size_t boolFieldCount = 0;
	for (const RunningDataset& dataset : datasets)
	{
		fieldCount += dataset.numericColumns.size() + dataset.stringColumns.size() + dataset.boolColumns.size();
		boolFieldCount += dataset.boolColumns.size();
	}
	result.views.reserve(fieldCount);
	result.boolScratch.reserve(boolFieldCount);

	for (const RunningDataset& dataset : datasets)
	{
		for (const numericColumn& column : dataset.numericColumns)
		{
			addDataView(
				result,
				dataset.name,
				column.name,
				column.data.data(),
				column.data.size(),
				sizeof(double),
				std::type_index(typeid(double)),
				FlowInternal::DataView::ValueType::Number);
		}

		for (const stringColumn& column : dataset.stringColumns)
		{
			addDataView(
				result,
				dataset.name,
				column.name,
				column.data.data(),
				column.data.size(),
				sizeof(std::string),
				std::type_index(typeid(std::string)),
				FlowInternal::DataView::ValueType::String);
		}

		for (const boolColumn& column : dataset.boolColumns)
		{
			std::unique_ptr<bool[]> scratch = std::make_unique<bool[]>(column.data.size());
			for (std::size_t i = 0; i < column.data.size(); ++i)
			{
				scratch[i] = column.data[i];
			}
			const bool* scratchData = scratch.get();
			result.boolScratch.push_back(std::move(scratch));

			addDataView(
				result,
				dataset.name,
				column.name,
				scratchData,
				column.data.size(),
				sizeof(bool),
				std::type_index(typeid(bool)),
				FlowInternal::DataView::ValueType::Boolean);
		}
	}

	return result;
}

} // namespace

void PlotRenderer::init(const FlowUi::ViewPortVulkanInterop& interop, VkFormat colorFormat)
{
	if (interop.device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Cannot initialize PlotRenderer without a valid Vulkan device.");
	}

	if (device_ == interop.device && colorFormat_ == colorFormat && boxPipeline_ != VK_NULL_HANDLE)
	{
		return;
	}

	destroy();
	device_ = interop.device;
	allocator_ = interop.allocator;
	colorFormat_ = colorFormat;

	try
	{
		const std::vector<char> vertexShaderBytes = readShaderFile(kSceneVertexShaderFile);
		const std::vector<char> fragmentShaderBytes = readShaderFile(kSceneFragmentShaderFile);

		vertexShader_ = createShaderModule(device_, vertexShaderBytes);
		fragmentShader_ = createShaderModule(device_, fragmentShaderBytes);

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PlotPushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		vkCheck(
			vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_),
			"Failed to create plot pipeline layout.");

		boxPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, vertexShader_, fragmentShader_);
		markerPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, vertexShader_, fragmentShader_);
		textPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, vertexShader_, fragmentShader_);
		polylinePipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, vertexShader_, fragmentShader_);

		vkDestroyShaderModule(device_, vertexShader_, nullptr);
		vkDestroyShaderModule(device_, fragmentShader_, nullptr);
		vertexShader_ = VK_NULL_HANDLE;
		fragmentShader_ = VK_NULL_HANDLE;
	}
	catch (...)
	{
		destroy();
		throw;
	}
}

void PlotRenderer::destroy()
{
	if (device_ != VK_NULL_HANDLE)
	{
		if (boxPipeline_ != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device_, boxPipeline_, nullptr);
		}
		if (markerPipeline_ != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device_, markerPipeline_, nullptr);
		}
		if (textPipeline_ != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device_, textPipeline_, nullptr);
		}
		if (polylinePipeline_ != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device_, polylinePipeline_, nullptr);
		}
		if (pipelineLayout_ != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
		}
		if (vertexShader_ != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device_, vertexShader_, nullptr);
		}
		if (fragmentShader_ != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device_, fragmentShader_, nullptr);
		}
	}
	destroyBuffer(allocator_, boxBuffer_);
	destroyBuffer(allocator_, markerBuffer_);
	destroyBuffer(allocator_, textGlyphBuffer_);
	destroyBuffer(allocator_, polylineBuffer_);

	device_ = VK_NULL_HANDLE;
	allocator_ = nullptr;
	colorFormat_ = VK_FORMAT_UNDEFINED;
	boxPipeline_ = VK_NULL_HANDLE;
	markerPipeline_ = VK_NULL_HANDLE;
	textPipeline_ = VK_NULL_HANDLE;
	polylinePipeline_ = VK_NULL_HANDLE;
	pipelineLayout_ = VK_NULL_HANDLE;
	vertexShader_ = VK_NULL_HANDLE;
	fragmentShader_ = VK_NULL_HANDLE;
	drawPlan_ = {};
	cacheKey_ = {};
	flowCommandsDirty_ = true;
	runsDirty_ = true;
	viewportDirty_ = true;
}

void PlotRenderer::setInput(PlotRendererInput input)
{
	input_ = input;
}

void PlotRenderer::invalidateTemplate()
{
	flowCommandsDirty_ = true;
	runsDirty_ = true;
}

void PlotRenderer::invalidateDatasets()
{
	flowCommandsDirty_ = true;
	runsDirty_ = true;
}

void PlotRenderer::invalidateViewport()
{
	viewportDirty_ = true;
}

void PlotRenderer::record(const FlowUi::ViewPortRenderContext& ctx)
{
	if (ctx.commandBuffer == VK_NULL_HANDLE || ctx.extent.width == 0 || ctx.extent.height == 0)
	{
		return;
	}
	if (ctx.vulkan != nullptr && (device_ == VK_NULL_HANDLE || colorFormat_ != ctx.colorFormat))
	{
		init(*ctx.vulkan, ctx.colorFormat);
	}
	if (boxPipeline_ == VK_NULL_HANDLE || pipelineLayout_ == VK_NULL_HANDLE)
	{
		return;
	}

	rebuildIfNeeded(ctx);
	recordDrawPlan(ctx);
}

void PlotRenderer::rebuildIfNeeded(const FlowUi::ViewPortRenderContext& ctx)
{
	const state* guiState = input_.guiState;
	if (guiState == nullptr)
	{
		drawPlan_.plot = {};
		drawPlan_.runs.clear();
		cacheKey_.viewportExtent = ctx.extent;
		return;
	}

	const bool templateChanged = flowCommandsDirty_ || cacheKey_.templateRevision != guiState->templateRevision;
	const bool datasetsChanged = flowCommandsDirty_ || cacheKey_.datasetRevision != guiState->datasetRevision;
	const bool viewportChanged =
		viewportDirty_ ||
		cacheKey_.viewportRevision != guiState->viewportRevision ||
		cacheKey_.viewportExtent.width != ctx.extent.width ||
		cacheKey_.viewportExtent.height != ctx.extent.height;
	bool uploadDirty = false;

	if (templateChanged || datasetsChanged)
	{
		drawPlan_.plot = buildFlowPlotCommands();
		flowCommandsDirty_ = false;
		runsDirty_ = true;
	}

	if (runsDirty_)
	{
		buildRuns();
		runsDirty_ = false;
		uploadDirty = true;
	}

	if (uploadDirty)
	{
		uploadRuns(ctx);
	}
	if (viewportChanged)
	{
		viewportDirty_ = false;
	}

	cacheKey_.templateRevision = guiState->templateRevision;
	cacheKey_.datasetRevision = guiState->datasetRevision;
	cacheKey_.viewportRevision = guiState->viewportRevision;
	cacheKey_.viewportExtent = ctx.extent;
}

FlowPlot::RenderPlot PlotRenderer::buildFlowPlotCommands() const
{
	if (input_.guiState == nullptr)
	{
		return {};
	}

	const state& guiState = *input_.guiState;
	DataViewBuildResult dataViews = makeDataViews(guiState.datasets);
	FlowInternal::BoundIR::PlotBoundIR bound = FlowInternal::buildBoundIR(guiState.activeTemplate, dataViews.views);
	FlowInternal::ResolvedIR::PlotResolvedIR resolved =
		FlowInternal::resolvePlotIR(guiState.activeTemplate, bound, guiState.textEngine.get());
	return FlowInternal::buildRenderPlot(resolved);
}

void PlotRenderer::buildRuns()
{
	drawPlan_.runs.clear();
	drawPlan_.boxes.clear();
	drawPlan_.markers.clear();
	drawPlan_.textGlyphs.clear();
	drawPlan_.polylineVertices.clear();
	if (drawPlan_.plot.commands.empty())
	{
		return;
	}

	std::vector<FlowPlot::RectF> scissorStack{};
	scissorStack.push_back(fullScissorForPlot(drawPlan_.plot));
	bool runBarrier = false;

	auto currentCountFor = [&](RunType type) -> std::uint32_t {
		switch (type)
		{
			case RunType::Box:
				return static_cast<std::uint32_t>(drawPlan_.boxes.size());
			case RunType::Marker:
				return static_cast<std::uint32_t>(drawPlan_.markers.size());
			case RunType::Text:
				return static_cast<std::uint32_t>(drawPlan_.textGlyphs.size());
			case RunType::PolylineMesh:
				return static_cast<std::uint32_t>(drawPlan_.polylineVertices.size());
		}
		return 0;
	};

	auto closeRun = [&]() {
		if (drawPlan_.runs.empty())
		{
			return;
		}
		PlotRun& run = drawPlan_.runs.back();
		run.count = currentCountFor(run.type) - run.first;
		if (run.count == 0)
		{
			drawPlan_.runs.pop_back();
		}
	};

	auto beginRun = [&](RunType type) {
		const FlowPlot::RectF& scissor = scissorStack.back();
		if (drawPlan_.runs.empty() || runBarrier)
		{
			closeRun();
			drawPlan_.runs.push_back(PlotRun{
				.type = type,
				.scissor = scissor,
				.first = currentCountFor(type),
				.count = 0,
			});
			runBarrier = false;
			return;
		}

		const PlotRun& current = drawPlan_.runs.back();
		if (current.type != type || !sameRect(current.scissor, scissor))
		{
			closeRun();
			drawPlan_.runs.push_back(PlotRun{
				.type = type,
				.scissor = scissor,
				.first = currentCountFor(type),
				.count = 0,
			});
		}
	};

	for (const FlowPlot::RenderCommand& command : drawPlan_.plot.commands)
	{
		std::visit(
			[&](const auto& concreteCommand) {
				using T = std::decay_t<decltype(concreteCommand)>;
				if constexpr (std::is_same_v<T, FlowPlot::PushClipRectCommand>)
				{
					closeRun();
					scissorStack.push_back(intersectRect(scissorStack.back(), concreteCommand.rect));
					runBarrier = true;
				}
				else if constexpr (std::is_same_v<T, FlowPlot::PopClipRectCommand>)
				{
					closeRun();
					if (scissorStack.size() > 1)
					{
						scissorStack.pop_back();
					}
					runBarrier = true;
				}
				else if constexpr (std::is_same_v<T, FlowPlot::BoxCommand>)
				{
					beginRun(RunType::Box);
					drawPlan_.boxes.push_back(BoxInstance{
						.x = concreteCommand.rect.x,
						.y = concreteCommand.rect.y,
						.w = concreteCommand.rect.w,
						.h = concreteCommand.rect.h,
						.strokeWidth = concreteCommand.strokeWidth,
						.fill = packColor(concreteCommand.fill),
						.stroke = packColor(concreteCommand.stroke),
					});
				}
				else if constexpr (std::is_same_v<T, FlowPlot::MarkersCommand>)
				{
					beginRun(RunType::Marker);
					for (std::size_t i = 0; i < concreteCommand.positions.size(); ++i)
					{
						const FlowPlot::PointF& position = concreteCommand.positions[i];
						const FlowPlot::Color& fill = concreteCommand.fills.empty()
							? FlowPlot::Color{0, 0, 255, 255}
							: concreteCommand.fills[concreteCommand.fills.size() == 1 ? 0 : std::min(i, concreteCommand.fills.size() - 1)];
						const float size = concreteCommand.sizes.empty()
							? 6.0f
							: concreteCommand.sizes[concreteCommand.sizes.size() == 1 ? 0 : std::min(i, concreteCommand.sizes.size() - 1)];
						drawPlan_.markers.push_back(MarkerInstance{
							.centerX = position.x,
							.centerY = position.y,
							.size = size,
							.strokeWidth = concreteCommand.strokeWidth,
							.shape = markerShapeId(concreteCommand.shape),
							.fill = packColor(fill),
							.stroke = packColor(concreteCommand.stroke),
						});
					}
				}
				else if constexpr (std::is_same_v<T, FlowPlot::TextCommand>)
				{
					if (input_.guiState == nullptr || !input_.guiState->textEngine || input_.fontManager == nullptr)
					{
						return;
					}
					const FontManager::FontId fontId = input_.fontManager->resolveFont(
						concreteCommand.fontFamily,
						concreteCommand.fontWeight,
						toFlowUiFontStyle(concreteCommand.fontStyle));
					const FontManager::FontFaceData* fontFace = input_.fontManager->getFontById(fontId);
					const FontManager::FontVariantData* variant = fontFace != nullptr ? fontFace->defaultVariant() : nullptr;
					if (fontFace == nullptr || variant == nullptr)
					{
						return;
					}

					const FlowPlot::LaidOutText layout = input_.guiState->textEngine->layoutText(
						concreteCommand.fontFamily,
						concreteCommand.fontWeight,
						concreteCommand.fontStyle,
						concreteCommand.fontSize,
						concreteCommand.text,
						concreteCommand.clipToBox ? concreteCommand.box.w : std::numeric_limits<float>::infinity());

					float originX = concreteCommand.box.x;
					if (concreteCommand.hAlign == FlowPlot::HorizontalAlign::Center)
					{
						originX += (concreteCommand.box.w - layout.width) * 0.5f;
					}
					else if (concreteCommand.hAlign == FlowPlot::HorizontalAlign::Right)
					{
						originX += concreteCommand.box.w - layout.width;
					}

					float originY = concreteCommand.box.y;
					if (concreteCommand.vAlign == FlowPlot::VerticalAlign::Middle)
					{
						originY += (concreteCommand.box.h - layout.height) * 0.5f;
					}
					else if (concreteCommand.vAlign == FlowPlot::VerticalAlign::Bottom)
					{
						originY += concreteCommand.box.h - layout.height;
					}
					originY += layout.ascent;

					beginRun(RunType::Text);
					for (const FlowPlot::GlyphPlacement& glyphPlacement : layout.glyphs)
					{
						const auto glyphIt = variant->unicodeToGlyphIndex.find(glyphPlacement.codepoint);
						if (glyphIt == variant->unicodeToGlyphIndex.end() || glyphIt->second >= variant->glyphs.size())
						{
							continue;
						}
						const FontManager::GlyphData& glyph = variant->glyphs[glyphIt->second];
						const float scale = concreteCommand.fontSize / std::max(variant->emSize, 1.0e-6f);
						const float x0 = originX + glyphPlacement.x + glyph.planeLeft * scale;
						const float y0 = originY - glyphPlacement.y - glyph.planeTop * scale;
						const float x1 = originX + glyphPlacement.x + glyph.planeRight * scale;
						const float y1 = originY - glyphPlacement.y - glyph.planeBottom * scale;
						const float invAtlasW = fontFace->atlasWidth > 0 ? 1.0f / static_cast<float>(fontFace->atlasWidth) : 0.0f;
						const float invAtlasH = fontFace->atlasHeight > 0 ? 1.0f / static_cast<float>(fontFace->atlasHeight) : 0.0f;
						drawPlan_.textGlyphs.push_back(TextGlyphInstance{
							.x = x0,
							.y = y0,
							.w = x1 - x0,
							.h = y1 - y0,
							.u0 = (static_cast<float>(fontFace->sourceAtlasX) + glyph.imageLeft) * invAtlasW,
							.v0 = (static_cast<float>(fontFace->sourceAtlasY) + glyph.imageTop) * invAtlasH,
							.u1 = (static_cast<float>(fontFace->sourceAtlasX) + glyph.imageRight) * invAtlasW,
							.v1 = (static_cast<float>(fontFace->sourceAtlasY) + glyph.imageBottom) * invAtlasH,
							.atlasLayer = fontFace->atlasLayer,
							.color = packColor(concreteCommand.color),
						});
					}
				}
				else if constexpr (std::is_same_v<T, FlowPlot::PolylineCommand>)
				{
					beginRun(RunType::PolylineMesh);
					triangulatePolyline(concreteCommand);
				}
			},
			command);
	}

	closeRun();
}

void PlotRenderer::uploadRuns(const FlowUi::ViewPortRenderContext& ctx)
{
	(void)ctx;
	uploadVector(allocator_, boxBuffer_, drawPlan_.boxes);
	uploadVector(allocator_, markerBuffer_, drawPlan_.markers);
	uploadVector(allocator_, textGlyphBuffer_, drawPlan_.textGlyphs);
	uploadVector(allocator_, polylineBuffer_, drawPlan_.polylineVertices);
}

void PlotRenderer::triangulatePolyline(const FlowPlot::PolylineCommand& cmd)
{
	(void)cmd;
	// Stub: CPU-side cap/join-aware polyline triangulation will append triangles
	// into drawPlan_.polylineVertices while preserving command order.
}

void PlotRenderer::recordDrawPlan(const FlowUi::ViewPortRenderContext& ctx)
{
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(ctx.extent.width);
	viewport.height = static_cast<float>(ctx.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(ctx.commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = ctx.extent;
	vkCmdSetScissor(ctx.commandBuffer, 0, 1, &scissor);

	const PlotPushConstants pushConstants{
		.viewportWidth = static_cast<float>(ctx.extent.width),
		.viewportHeight = static_cast<float>(ctx.extent.height),
		.imageWidth = static_cast<float>(drawPlan_.plot.width),
		.imageHeight = static_cast<float>(drawPlan_.plot.height),
		.cameraCenterX = input_.camera.centerX,
		.cameraCenterY = input_.camera.centerY,
		.zoom = std::max(input_.camera.zoom, 1.0e-6f),
		._pad = 0.0f,
	};

	auto pipelineFor = [&](RunType type) -> VkPipeline {
		switch (type)
		{
			case RunType::Box:
				return boxPipeline_;
			case RunType::Marker:
				return markerPipeline_;
			case RunType::Text:
				return textPipeline_;
			case RunType::PolylineMesh:
				return polylinePipeline_;
		}
		return boxPipeline_;
	};

	auto worldScissorToVk = [&](const FlowPlot::RectF& world) {
		const float zoom = std::max(input_.camera.zoom, 1.0e-6f);
		const float viewportW = static_cast<float>(ctx.extent.width);
		const float viewportH = static_cast<float>(ctx.extent.height);
		const float x0 = (world.x - input_.camera.centerX) * zoom + viewportW * 0.5f;
		const float y0 = (world.y - input_.camera.centerY) * zoom + viewportH * 0.5f;
		const float x1 = (world.x + world.w - input_.camera.centerX) * zoom + viewportW * 0.5f;
		const float y1 = (world.y + world.h - input_.camera.centerY) * zoom + viewportH * 0.5f;
		const int left = std::clamp(static_cast<int>(std::floor(std::min(x0, x1))), 0, static_cast<int>(ctx.extent.width));
		const int top = std::clamp(static_cast<int>(std::floor(std::min(y0, y1))), 0, static_cast<int>(ctx.extent.height));
		const int right = std::clamp(static_cast<int>(std::ceil(std::max(x0, x1))), 0, static_cast<int>(ctx.extent.width));
		const int bottom = std::clamp(static_cast<int>(std::ceil(std::max(y0, y1))), 0, static_cast<int>(ctx.extent.height));
		VkRect2D out{};
		out.offset = {left, top};
		out.extent = {
			static_cast<std::uint32_t>(std::max(0, right - left)),
			static_cast<std::uint32_t>(std::max(0, bottom - top)),
		};
		return out;
	};

	for (const PlotRun& run : drawPlan_.runs)
	{
		if (run.count == 0)
		{
			continue;
		}

		const VkRect2D runScissor = worldScissorToVk(run.scissor);
		if (runScissor.extent.width == 0 || runScissor.extent.height == 0)
		{
			continue;
		}
		vkCmdSetScissor(ctx.commandBuffer, 0, 1, &runScissor);
		vkCmdBindPipeline(ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineFor(run.type));
		vkCmdPushConstants(
			ctx.commandBuffer,
			pipelineLayout_,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			static_cast<std::uint32_t>(sizeof(PlotPushConstants)),
			&pushConstants);

		if (run.type == RunType::PolylineMesh)
		{
			vkCmdDraw(ctx.commandBuffer, run.count, 1, run.first, 0);
		}
		else
		{
			vkCmdDraw(ctx.commandBuffer, 6, run.count, 0, run.first);
		}
	}
}

PlotRenderer::~PlotRenderer()
{
	destroy();
}

} // namespace FlowPlotGui
