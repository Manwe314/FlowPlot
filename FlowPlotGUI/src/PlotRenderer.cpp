#include "PlotRenderer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "internal/Vma.hpp"
#include "runtimePaths.hpp"

namespace FlowPlotGui {
namespace {

constexpr const char* kBoxVertexShaderFile = "plot_viewport_box.vert.spv";
constexpr const char* kBoxFragmentShaderFile = "plot_viewport_box.frag.spv";
constexpr const char* kMarkerVertexShaderFile = "plot_viewport_marker.vert.spv";
constexpr const char* kMarkerFragmentShaderFile = "plot_viewport_marker.frag.spv";
constexpr const char* kTextVertexShaderFile = "plot_viewport_text.vert.spv";
constexpr const char* kTextFragmentShaderFile = "plot_viewport_text.frag.spv";
constexpr const char* kPolylineVertexShaderFile = "plot_viewport_polyline.vert.spv";
constexpr const char* kPolylineFragmentShaderFile = "plot_viewport_polyline.frag.spv";

struct DataViewBuildResult {
	std::unordered_map<FlowInternal::DataKey, FlowInternal::DataView, FlowInternal::DataKeyHash> views{};
	std::vector<std::unique_ptr<bool[]>> boolScratch{};
};

PlotRenderer::PackedColor packColor(const FlowPlot::Color& color)
{
	constexpr float inv255 = 1.0f / 255.0f;
	const auto srgbToLinear = [](float srgb) {
		if (srgb <= 0.04045f)
		{
			return srgb / 12.92f;
		}
		return std::pow((srgb + 0.055f) / 1.055f, 2.4f);
	};
	const float r = static_cast<float>(color.r) * inv255;
	const float g = static_cast<float>(color.g) * inv255;
	const float b = static_cast<float>(color.b) * inv255;
	return PlotRenderer::PackedColor{
		.r = srgbToLinear(r),
		.g = srgbToLinear(g),
		.b = srgbToLinear(b),
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

FlowPlot::PointF addPoint(const FlowPlot::PointF& lhs, const FlowPlot::PointF& rhs)
{
	return FlowPlot::PointF{.x = lhs.x + rhs.x, .y = lhs.y + rhs.y};
}

FlowPlot::PointF subtractPoints(const FlowPlot::PointF& lhs, const FlowPlot::PointF& rhs)
{
	return FlowPlot::PointF{.x = lhs.x - rhs.x, .y = lhs.y - rhs.y};
}

FlowPlot::PointF multiplyPoints(const FlowPlot::PointF& point, float scale)
{
	return FlowPlot::PointF{.x = point.x * scale, .y = point.y * scale};
}

float dotPoint(const FlowPlot::PointF& lhs, const FlowPlot::PointF& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

float crossPoint(const FlowPlot::PointF& lhs, const FlowPlot::PointF& rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

float lengthPoint(const FlowPlot::PointF& point)
{
	return std::sqrt(dotPoint(point, point));
}

FlowPlot::PointF perpendicularPoint(const FlowPlot::PointF& point)
{
	return FlowPlot::PointF{.x = -point.y, .y = point.x};
}

FlowPlot::PointF pointFromAngle(float angle, float radius)
{
	return FlowPlot::PointF{.x = std::cos(angle) * radius, .y = std::sin(angle) * radius};
}

float normalizeAngleDelta(float delta)
{
	constexpr float kPi = 3.14159265358979323846f;
	constexpr float kTwoPi = kPi * 2.0f;
	while (delta <= -kPi)
	{
		delta += kTwoPi;
	}
	while (delta > kPi)
	{
		delta -= kTwoPi;
	}
	return delta;
}

bool lineIntersection(
	const FlowPlot::PointF& p,
	const FlowPlot::PointF& r,
	const FlowPlot::PointF& q,
	const FlowPlot::PointF& s,
	FlowPlot::PointF& out)
{
	const float denom = crossPoint(r, s);
	if (std::abs(denom) <= 1.0e-6f)
	{
		return false;
	}

	const float t = crossPoint(subtractPoints(q, p), s) / denom;
	out = addPoint(p, multiplyPoints(r, t));
	return true;
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
	const std::filesystem::path relativePath = std::filesystem::path("shaders") / fileName;
	if (const std::optional<std::filesystem::path> packagedPath = FlowPlotGui::findResourcePath(relativePath))
		return readFile(packagedPath->string());

#ifdef FLOWPLOTGUI_SHADER_OUTPUT_DIR
	const std::string buildPath = std::string(FLOWPLOTGUI_SHADER_OUTPUT_DIR) + "/" + fileName;
	std::ifstream buildProbe(buildPath, std::ios::binary);
	if (buildProbe.good())
	{
		buildProbe.close();
		return readFile(buildPath);
	}
#endif

	throw std::runtime_error("Failed to locate shader file: " + relativePath.string());
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
	blendAttachment.blendEnable = VK_TRUE;
	blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
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

void retireBuffer(
	std::vector<PlotRenderer::AllocatedBuffer>& retiredBuffers,
	PlotRenderer::AllocatedBuffer& buffer)
{
	if (buffer.buffer != VK_NULL_HANDLE)
	{
		retiredBuffers.push_back(buffer);
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
void uploadVector(
	VmaAllocator allocator,
	PlotRenderer::AllocatedBuffer& buffer,
	const std::vector<T>& values,
	std::vector<PlotRenderer::AllocatedBuffer>& retiredBuffers)
{
	const VkDeviceSize byteSize = static_cast<VkDeviceSize>(values.size() * sizeof(T));
	retireBuffer(retiredBuffers, buffer);
	if (byteSize == 0)
	{
		return;
	}
	if (allocator == nullptr)
	{
		throw std::runtime_error("Cannot upload plot renderer buffers without a VMA allocator.");
	}
	createMappedBuffer(allocator, byteSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffer);
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
				scratch[i] = column.data[i] != 0U;
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
		const std::vector<char> boxVertexShaderBytes = readShaderFile(kBoxVertexShaderFile);
		const std::vector<char> boxFragmentShaderBytes = readShaderFile(kBoxFragmentShaderFile);
		const std::vector<char> markerVertexShaderBytes = readShaderFile(kMarkerVertexShaderFile);
		const std::vector<char> markerFragmentShaderBytes = readShaderFile(kMarkerFragmentShaderFile);
		const std::vector<char> textVertexShaderBytes = readShaderFile(kTextVertexShaderFile);
		const std::vector<char> textFragmentShaderBytes = readShaderFile(kTextFragmentShaderFile);
		const std::vector<char> polylineVertexShaderBytes = readShaderFile(kPolylineVertexShaderFile);
		const std::vector<char> polylineFragmentShaderBytes = readShaderFile(kPolylineFragmentShaderFile);

		VkShaderModule boxVertexShader = createShaderModule(device_, boxVertexShaderBytes);
		VkShaderModule boxFragmentShader = createShaderModule(device_, boxFragmentShaderBytes);
		VkShaderModule markerVertexShader = createShaderModule(device_, markerVertexShaderBytes);
		VkShaderModule markerFragmentShader = createShaderModule(device_, markerFragmentShaderBytes);
		VkShaderModule textVertexShader = createShaderModule(device_, textVertexShaderBytes);
		VkShaderModule textFragmentShader = createShaderModule(device_, textFragmentShaderBytes);
		VkShaderModule polylineVertexShader = createShaderModule(device_, polylineVertexShaderBytes);
		VkShaderModule polylineFragmentShader = createShaderModule(device_, polylineFragmentShaderBytes);

		std::array<VkDescriptorSetLayoutBinding, 5> bindings{};
		bindings[0].binding = 0;
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[1].binding = 1;
		bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[1].descriptorCount = 1;
		bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[2].binding = 2;
		bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[2].descriptorCount = 1;
		bindings[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[3].binding = 3;
		bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[3].descriptorCount = 1;
		bindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		bindings[4].binding = 4;
		bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		bindings[4].descriptorCount = 1;
		bindings[4].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo setLayoutInfo{};
		setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setLayoutInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
		setLayoutInfo.pBindings = bindings.data();
		vkCheck(
			vkCreateDescriptorSetLayout(device_, &setLayoutInfo, nullptr, &descriptorSetLayout_),
			"Failed to create plot descriptor set layout.");

		const std::uint32_t descriptorFrameCount = std::max<std::uint32_t>(1u, interop.framesInFlight);
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[0].descriptorCount = descriptorFrameCount * 4u;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = descriptorFrameCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = descriptorFrameCount;
		poolInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		vkCheck(
			vkCreateDescriptorPool(device_, &poolInfo, nullptr, &descriptorPool_),
			"Failed to create plot descriptor pool.");

		std::vector<VkDescriptorSetLayout> setLayouts(descriptorFrameCount, descriptorSetLayout_);
		descriptorSets_.assign(descriptorFrameCount, VK_NULL_HANDLE);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool_;
		allocInfo.descriptorSetCount = descriptorFrameCount;
		allocInfo.pSetLayouts = setLayouts.data();
		vkCheck(
			vkAllocateDescriptorSets(device_, &allocInfo, descriptorSets_.data()),
			"Failed to allocate plot descriptor sets.");
		retiredBuffersByFrame_.assign(descriptorFrameCount, {});

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PlotPushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		vkCheck(
			vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout_),
			"Failed to create plot pipeline layout.");

		boxPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, boxVertexShader, boxFragmentShader);
		markerPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, markerVertexShader, markerFragmentShader);
		textPipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, textVertexShader, textFragmentShader);
		polylinePipeline_ = createPipeline(device_, colorFormat_, pipelineLayout_, polylineVertexShader, polylineFragmentShader);

		vkDestroyShaderModule(device_, boxVertexShader, nullptr);
		vkDestroyShaderModule(device_, boxFragmentShader, nullptr);
		vkDestroyShaderModule(device_, markerVertexShader, nullptr);
		vkDestroyShaderModule(device_, markerFragmentShader, nullptr);
		vkDestroyShaderModule(device_, textVertexShader, nullptr);
		vkDestroyShaderModule(device_, textFragmentShader, nullptr);
		vkDestroyShaderModule(device_, polylineVertexShader, nullptr);
		vkDestroyShaderModule(device_, polylineFragmentShader, nullptr);
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
		// Full renderer teardown is rare; wait so pipelines/descriptors are not
		// destroyed while older submitted viewport command buffers still use them.
		(void)vkDeviceWaitIdle(device_);
		destroyRetiredBuffers();
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
		if (descriptorPool_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
		}
		if (descriptorSetLayout_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
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
	else
	{
		destroyRetiredBuffers();
	}
	destroyBuffer(allocator_, boxBuffer_);
	destroyBuffer(allocator_, markerBuffer_);
	destroyBuffer(allocator_, textGlyphBuffer_);
	destroyBuffer(allocator_, polylineBuffer_);
	retiredBuffersByFrame_.clear();

	device_ = VK_NULL_HANDLE;
	allocator_ = nullptr;
	colorFormat_ = VK_FORMAT_UNDEFINED;
	boxPipeline_ = VK_NULL_HANDLE;
	markerPipeline_ = VK_NULL_HANDLE;
	textPipeline_ = VK_NULL_HANDLE;
	polylinePipeline_ = VK_NULL_HANDLE;
	pipelineLayout_ = VK_NULL_HANDLE;
	descriptorSetLayout_ = VK_NULL_HANDLE;
	descriptorPool_ = VK_NULL_HANDLE;
	descriptorSets_.clear();
	vertexShader_ = VK_NULL_HANDLE;
	fragmentShader_ = VK_NULL_HANDLE;
	drawPlan_ = {};
	cachedDataViews_.clear();
	cachedBoolScratch_.clear();
	cacheKey_ = {};
	flowCommandsDirty_ = true;
	runsDirty_ = true;
	viewportDirty_ = true;
}

void PlotRenderer::setInput(PlotRendererInput input)
{
	const float previousZoom = std::max(input_.camera.zoom, 1.0e-6f);
	const float nextZoom = std::max(input.camera.zoom, 1.0e-6f);
	if (std::abs(previousZoom - nextZoom) > 1.0e-6f)
	{
		runsDirty_ = true;
	}
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

	reclaimRetiredBuffers(ctx.frameIndex);
	rebuildIfNeeded(ctx);
	recordDrawPlan(ctx);
}

void PlotRenderer::rebuildIfNeeded(const FlowUi::ViewPortRenderContext& ctx)
{
	state* guiState = input_.guiState;
	if (guiState == nullptr)
	{
		drawPlan_.plot = {};
		drawPlan_.runs.clear();
		return;
	}

	const bool templateChanged = flowCommandsDirty_ || cacheKey_.templateRevision != guiState->templateRevision;
	const bool datasetsChanged = flowCommandsDirty_ || cacheKey_.datasetRevision != guiState->datasetRevision;
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
	viewportDirty_ = false;

	cacheKey_.templateRevision = guiState->templateRevision;
	cacheKey_.datasetRevision = guiState->datasetRevision;
}

FlowPlot::RenderPlot PlotRenderer::buildFlowPlotCommands()
{
	if (input_.guiState == nullptr)
	{
		return {};
	}

	state& guiState = *input_.guiState;
	if (flowCommandsDirty_ || cacheKey_.datasetRevision != guiState.datasetRevision)
	{
		DataViewBuildResult dataViews = makeDataViews(guiState.datasets);
		cachedDataViews_ = std::move(dataViews.views);
		cachedBoolScratch_ = std::move(dataViews.boolScratch);
	}

	try
	{
		FlowInternal::BoundIR::PlotBoundIR bound = FlowInternal::buildBoundIR(guiState.activeTemplate, cachedDataViews_);
		FlowInternal::ResolvedIR::PlotResolvedIR resolved =
			FlowInternal::resolvePlotIR(guiState.activeTemplate, bound, guiState.textEngine.get());
		clearDiagnosticsBySource(guiState, "compile");
		return FlowInternal::buildRenderPlot(resolved);
	}
	catch (const std::exception& e)
	{
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "compile",
			.message = formatFlowPlotExceptionMessage(guiState.activeTemplate, e.what()),
		});
		return drawPlan_.plot;
	}
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

	auto beginRun = [&](RunType type, const FlowPlot::RectF* scissorOverride = nullptr) {
		const FlowPlot::RectF& scissor = scissorOverride != nullptr ? *scissorOverride : scissorStack.back();
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

	beginRun(RunType::Box);
	drawPlan_.boxes.push_back(BoxInstance{
		.x = 0.0f,
		.y = 0.0f,
		.w = static_cast<float>(drawPlan_.plot.width),
		.h = static_cast<float>(drawPlan_.plot.height),
		.strokeWidth = 0.0f,
		.fill = packColor(drawPlan_.plot.background),
		.stroke = packColor(FlowPlot::Color{0, 0, 0, 0}),
	});
	runBarrier = true;

	auto clampStrokeWidth = [&](float strokeWidth) {
		if (strokeWidth <= 0.0f)
			return 0.0f;
		const float minStrokeWidth = 1.0f / std::max(input_.camera.zoom, 1e-6f);
		return  std::max(strokeWidth, minStrokeWidth);
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
						.strokeWidth = clampStrokeWidth(concreteCommand.strokeWidth),
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
					const FlowUi::FontManager::FontId fontId = input_.fontManager->resolveFont(
						concreteCommand.fontFamily,
						concreteCommand.fontWeight,
						toFlowUiFontStyle(concreteCommand.fontStyle));
					const FlowUi::Font::FontFaceData* fontFace = input_.fontManager->getFontById(fontId);
					const FlowUi::Font::FontVariantData* variant = fontFace != nullptr ? fontFace->defaultVariant() : nullptr;
					if (fontFace == nullptr || variant == nullptr)
					{
						return;
					}

					const bool vertical = concreteCommand.orientation != FlowPlot::Spec::TextOrientation::Horizontal;
					const float layoutBoxWidth = vertical ? concreteCommand.box.h : concreteCommand.box.w;
					const float layoutBoxHeight = vertical ? concreteCommand.box.w : concreteCommand.box.h;
					const float layoutMaxWidth = concreteCommand.wrapMode == FlowPlot::Spec::TextWrapMode::None
						? std::numeric_limits<float>::infinity()
						: std::max(layoutBoxWidth, 0.0f);
					const FlowPlot::LaidOutText layout = input_.guiState->textEngine->layoutText(
						concreteCommand.fontFamily,
						concreteCommand.fontWeight,
						concreteCommand.fontStyle,
						concreteCommand.fontSize,
						concreteCommand.text,
						FlowPlot::TextLayoutOptions{layoutMaxWidth, concreteCommand.wrapMode});

					float originX = 0.0f;
					if (concreteCommand.hAlign == FlowPlot::HorizontalAlign::Center)
					{
						originX += (layoutBoxWidth - layout.width) * 0.5f;
					}
					else if (concreteCommand.hAlign == FlowPlot::HorizontalAlign::Right)
					{
						originX += layoutBoxWidth - layout.width;
					}

					float originY = 0.0f;
					if (concreteCommand.vAlign == FlowPlot::VerticalAlign::Middle)
					{
						originY += (layoutBoxHeight - layout.height) * 0.5f;
					}
					else if (concreteCommand.vAlign == FlowPlot::VerticalAlign::Bottom)
					{
						originY += layoutBoxHeight - layout.height;
					}

					const FlowPlot::RectF textScissor = concreteCommand.clipToBox
						? intersectRect(scissorStack.back(), concreteCommand.box)
						: scissorStack.back();
					beginRun(RunType::Text, &textScissor);
					for (const FlowPlot::GlyphPlacement& glyphPlacement : layout.glyphs)
					{
						const auto glyphIt = variant->unicodeToGlyphIndex.find(glyphPlacement.codepoint);
						if (glyphIt == variant->unicodeToGlyphIndex.end() || glyphIt->second >= variant->glyphs.size())
						{
							continue;
						}
						const FlowUi::Font::GlyphData& glyph = variant->glyphs[glyphIt->second];
						const float scale = concreteCommand.fontSize / std::max(variant->emSize, 1.0e-6f);
						const float baselineY = originY + glyphPlacement.y;
						const float sourceX = originX + glyphPlacement.x + glyph.planeLeft * scale;
						const float sourceY = baselineY - glyph.planeTop * scale;
						const float sourceWidth = (glyph.planeRight - glyph.planeLeft) * scale;
						const float sourceHeight = (glyph.planeTop - glyph.planeBottom) * scale;
						float glyphX = concreteCommand.box.x + sourceX;
						float glyphY = concreteCommand.box.y + sourceY;
						float xBasisX = sourceWidth;
						float xBasisY = 0.0f;
						float yBasisX = 0.0f;
						float yBasisY = sourceHeight;
						if (concreteCommand.orientation == FlowPlot::Spec::TextOrientation::VerticalClockwise)
						{
							glyphX = concreteCommand.box.x + concreteCommand.box.w - sourceY;
							glyphY = concreteCommand.box.y + sourceX;
							xBasisX = 0.0f;
							xBasisY = sourceWidth;
							yBasisX = -sourceHeight;
							yBasisY = 0.0f;
						}
						else if (concreteCommand.orientation == FlowPlot::Spec::TextOrientation::VerticalCounterClockwise)
						{
							glyphX = concreteCommand.box.x + sourceY;
							glyphY = concreteCommand.box.y + concreteCommand.box.h - sourceX;
							xBasisX = 0.0f;
							xBasisY = -sourceWidth;
							yBasisX = sourceHeight;
							yBasisY = 0.0f;
						}
						const float invAtlasW = fontFace->atlasWidth > 0 ? 1.0f / static_cast<float>(fontFace->atlasWidth) : 0.0f;
						const float invAtlasH = fontFace->atlasHeight > 0 ? 1.0f / static_cast<float>(fontFace->atlasHeight) : 0.0f;
						const float sourceAtlasHeight = static_cast<float>(fontFace->sourceAtlasHeight);
						drawPlan_.textGlyphs.push_back(TextGlyphInstance{
							.x = glyphX,
							.y = glyphY,
							.xBasisX = xBasisX,
							.xBasisY = xBasisY,
							.yBasisX = yBasisX,
							.yBasisY = yBasisY,
							.u0 = (static_cast<float>(fontFace->sourceAtlasX) + glyph.imageLeft) * invAtlasW,
							.v0 = (static_cast<float>(fontFace->sourceAtlasY) + (sourceAtlasHeight - glyph.imageTop)) * invAtlasH,
							.u1 = (static_cast<float>(fontFace->sourceAtlasX) + glyph.imageRight) * invAtlasW,
							.v1 = (static_cast<float>(fontFace->sourceAtlasY) + (sourceAtlasHeight - glyph.imageBottom)) * invAtlasH,
							.atlasLayer = fontFace->atlasLayer,
							.distanceRangePx = variant->distanceRange > 0.0f ? variant->distanceRange : 2.0f,
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
	if (retiredBuffersByFrame_.empty())
	{
		retiredBuffersByFrame_.resize(std::max<std::size_t>(descriptorSets_.size(), 1u));
	}
	std::vector<AllocatedBuffer>& retiredBuffers =
		retiredBuffersByFrame_[ctx.frameIndex % retiredBuffersByFrame_.size()];
	uploadVector(allocator_, boxBuffer_, drawPlan_.boxes, retiredBuffers);
	uploadVector(allocator_, markerBuffer_, drawPlan_.markers, retiredBuffers);
	uploadVector(allocator_, textGlyphBuffer_, drawPlan_.textGlyphs, retiredBuffers);
	uploadVector(allocator_, polylineBuffer_, drawPlan_.polylineVertices, retiredBuffers);
}

void PlotRenderer::reclaimRetiredBuffers(std::uint32_t frameIndex)
{
	if (retiredBuffersByFrame_.empty())
	{
		return;
	}

	std::vector<AllocatedBuffer>& retiredBuffers =
		retiredBuffersByFrame_[frameIndex % retiredBuffersByFrame_.size()];
	for (AllocatedBuffer& buffer : retiredBuffers)
	{
		destroyBuffer(allocator_, buffer);
	}
	retiredBuffers.clear();
}

void PlotRenderer::destroyRetiredBuffers()
{
	for (std::vector<AllocatedBuffer>& retiredBuffers : retiredBuffersByFrame_)
	{
		for (AllocatedBuffer& buffer : retiredBuffers)
		{
			destroyBuffer(allocator_, buffer);
		}
		retiredBuffers.clear();
	}
}

void PlotRenderer::updateDescriptorSet(std::uint32_t frameIndex)
{
	if (device_ == VK_NULL_HANDLE || descriptorSets_.empty())
	{
		return;
	}

	const std::uint32_t frameSlot = frameIndex % static_cast<std::uint32_t>(descriptorSets_.size());
	const VkDescriptorSet descriptorSet = descriptorSets_[frameSlot];
	if (descriptorSet == VK_NULL_HANDLE)
	{
		return;
	}

	std::array<VkDescriptorBufferInfo, 4> bufferInfos{};
	std::array<VkDescriptorImageInfo, 1> imageInfos{};
	std::array<VkWriteDescriptorSet, 5> writes{};
	std::uint32_t writeCount = 0;

	if (boxBuffer_.buffer != VK_NULL_HANDLE)
	{
		bufferInfos[0].buffer = boxBuffer_.buffer;
		bufferInfos[0].offset = 0;
		bufferInfos[0].range = boxBuffer_.size;

		VkWriteDescriptorSet& write = writes[writeCount++];
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &bufferInfos[0];
	}

	if (textGlyphBuffer_.buffer != VK_NULL_HANDLE)
	{
		bufferInfos[1].buffer = textGlyphBuffer_.buffer;
		bufferInfos[1].offset = 0;
		bufferInfos[1].range = textGlyphBuffer_.size;

		VkWriteDescriptorSet& write = writes[writeCount++];
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = 1;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &bufferInfos[1];
	}

	if (markerBuffer_.buffer != VK_NULL_HANDLE)
	{
		bufferInfos[2].buffer = markerBuffer_.buffer;
		bufferInfos[2].offset = 0;
		bufferInfos[2].range = markerBuffer_.size;

		VkWriteDescriptorSet& write = writes[writeCount++];
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = 2;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &bufferInfos[2];
	}

	if (polylineBuffer_.buffer != VK_NULL_HANDLE)
	{
		bufferInfos[3].buffer = polylineBuffer_.buffer;
		bufferInfos[3].offset = 0;
		bufferInfos[3].range = polylineBuffer_.size;

		VkWriteDescriptorSet& write = writes[writeCount++];
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = 4;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &bufferInfos[3];
	}

	if (input_.fontManager != nullptr)
	{
		const FlowUi::Font::AtlasArrayResource& atlas = input_.fontManager->getAtlasResource();
		if (atlas.view != VK_NULL_HANDLE && atlas.sampler != VK_NULL_HANDLE && atlas.layersUsed > 0u)
		{
			imageInfos[0].sampler = atlas.sampler;
			imageInfos[0].imageView = atlas.view;
			imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet& write = writes[writeCount++];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = descriptorSet;
			write.dstBinding = 3;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.pImageInfo = &imageInfos[0];
		}
	}

	if (writeCount > 0)
	{
		vkUpdateDescriptorSets(device_, writeCount, writes.data(), 0, nullptr);
	}
}

void PlotRenderer::triangulatePolyline(const FlowPlot::PolylineCommand& cmd)
{
	if (cmd.points.size() < 2U || cmd.width <= 0.0f || cmd.color.a == 0U)
	{
		return;
	}

	struct Segment {
		FlowPlot::PointF previousPoint{};
		FlowPlot::PointF currentPoint{};
		FlowPlot::PointF direction{};
		FlowPlot::PointF normal{};
	};

	const PackedColor color = packColor(cmd.color);
	const float minWorldWidth = 1.0f / std::max(input_.camera.zoom, 1.0e-6f);
	const float halfWidth = std::max(cmd.width, minWorldWidth) * 0.5f;
	constexpr float kJoinEpsilon = 1.0e-4f;
	constexpr float kMiterLimit = 200.0f;
	constexpr int kRoundCapSegments = 16;
	constexpr int kRoundJoinSegments = 16;

	std::vector<Segment> segments{};
	segments.reserve(cmd.points.size() - 1U);
	for (std::size_t i = 1; i < cmd.points.size(); ++i)
	{
		const FlowPlot::PointF previousPoint = cmd.points[i - 1U];
		const FlowPlot::PointF currentPoint = cmd.points[i];
		const FlowPlot::PointF delta = subtractPoints(currentPoint, previousPoint);
		const float length = lengthPoint(delta);
		if (length <= 1.0e-6f)
		{
			continue;
		}

		const FlowPlot::PointF direction = multiplyPoints(delta, 1.0f / length);
		segments.push_back(Segment{
			.previousPoint = previousPoint,
			.currentPoint = currentPoint,
			.direction = direction,
			.normal = perpendicularPoint(direction),
		});
	}

	if (segments.empty())
	{
		return;
	}

	auto appendVertex = [&](const FlowPlot::PointF& point) {
		drawPlan_.polylineVertices.push_back(PolylineVertex{
			.x = point.x,
			.y = point.y,
			.color = color,
		});
	};

	auto appendTriangle = [&](const FlowPlot::PointF& a, const FlowPlot::PointF& b, const FlowPlot::PointF& c) {
		appendVertex(a);
		appendVertex(b);
		appendVertex(c);
	};

	auto appendQuad = [&](const FlowPlot::PointF& a, const FlowPlot::PointF& b, const FlowPlot::PointF& c, const FlowPlot::PointF& d) {
		appendTriangle(a, b, c);
		appendTriangle(a, c, d);
	};

	auto segmentCorner = [&](const Segment& segment, bool atStart, float normalSign) {
		const FlowPlot::PointF base = atStart ? segment.previousPoint : segment.currentPoint;
		return addPoint(base, multiplyPoints(segment.normal, normalSign * halfWidth));
	};

	auto appendArcFan = [&](
		const FlowPlot::PointF& center,
		const FlowPlot::PointF& startVector,
		const FlowPlot::PointF& endVector,
		int segmentCount,
		const FlowPlot::PointF& preferredMidVector) {
		const float radius = std::max(lengthPoint(startVector), lengthPoint(endVector));
		if (radius <= 1.0e-6f)
		{
			return;
		}

		const float startAngle = std::atan2(startVector.y, startVector.x);
		float sweep = normalizeAngleDelta(std::atan2(endVector.y, endVector.x) - startAngle);
		if (lengthPoint(preferredMidVector) > 1.0e-6f)
		{
			constexpr float kTwoPi = 6.28318530717958647692f;
			const float alternateSweep = sweep > 0.0f ? sweep - kTwoPi : sweep + kTwoPi;
			const FlowPlot::PointF sweepMid = pointFromAngle(startAngle + sweep * 0.5f, 1.0f);
			const FlowPlot::PointF alternateMid = pointFromAngle(startAngle + alternateSweep * 0.5f, 1.0f);
			if (dotPoint(alternateMid, preferredMidVector) > dotPoint(sweepMid, preferredMidVector))
			{
				sweep = alternateSweep;
			}
		}
		const int steps = std::max(1, segmentCount);
		FlowPlot::PointF previous = addPoint(center, startVector);
		for (int i = 1; i <= steps; ++i)
		{
			const float t0 = static_cast<float>(i) / static_cast<float>(steps);
			const FlowPlot::PointF current = addPoint(center, pointFromAngle(startAngle + sweep * t0, radius));
			appendTriangle(center, previous, current);
			previous = current;
		}
	};

	for (const Segment& segment : segments)
	{
		const FlowPlot::PointF startPlus = segmentCorner(segment, true, 1.0f);
		const FlowPlot::PointF startMinus = segmentCorner(segment, true, -1.0f);
		const FlowPlot::PointF endPlus = segmentCorner(segment, false, 1.0f);
		const FlowPlot::PointF endMinus = segmentCorner(segment, false, -1.0f);
		appendQuad(startPlus, endPlus, endMinus, startMinus);
	}

	const Segment& firstSegment = segments.front();
	const Segment& lastSegment = segments.back();
	if (cmd.cap == FlowPlot::LineCap::Square)
	{
		const FlowPlot::PointF startExtension = multiplyPoints(firstSegment.direction, -halfWidth);
		appendQuad(
			addPoint(segmentCorner(firstSegment, true, 1.0f), startExtension),
			segmentCorner(firstSegment, true, 1.0f),
			segmentCorner(firstSegment, true, -1.0f),
			addPoint(segmentCorner(firstSegment, true, -1.0f), startExtension));

		const FlowPlot::PointF endExtension = multiplyPoints(lastSegment.direction, halfWidth);
		appendQuad(
			segmentCorner(lastSegment, false, 1.0f),
			addPoint(segmentCorner(lastSegment, false, 1.0f), endExtension),
			addPoint(segmentCorner(lastSegment, false, -1.0f), endExtension),
			segmentCorner(lastSegment, false, -1.0f));
	}
	else if (cmd.cap == FlowPlot::LineCap::Round)
	{
		appendArcFan(
			firstSegment.previousPoint,
			multiplyPoints(firstSegment.normal, -halfWidth),
			multiplyPoints(firstSegment.normal, halfWidth),
			kRoundCapSegments,
			multiplyPoints(firstSegment.direction, -1.0f));
		appendArcFan(
			lastSegment.currentPoint,
			multiplyPoints(lastSegment.normal, halfWidth),
			multiplyPoints(lastSegment.normal, -halfWidth),
			kRoundCapSegments,
			lastSegment.direction);
	}

	for (std::size_t i = 1; i < segments.size(); ++i)
	{
		const Segment& previousSegment = segments[i - 1U];
		const Segment& currentSegment = segments[i];
		const FlowPlot::PointF& currentPoint = previousSegment.currentPoint;
		const float turn = crossPoint(previousSegment.direction, currentSegment.direction);
		if (std::abs(turn) <= kJoinEpsilon)
		{
			continue;
		}

		const float outerSign = turn > 0.0f ? 1.0f : -1.0f;
		const FlowPlot::PointF previousOuter = addPoint(currentPoint, multiplyPoints(previousSegment.normal, outerSign * halfWidth));
		const FlowPlot::PointF currentOuter = addPoint(currentPoint, multiplyPoints(currentSegment.normal, outerSign * halfWidth));

		if (cmd.join == FlowPlot::LineJoin::Round)
		{
			appendArcFan(
				currentPoint,
				subtractPoints(previousOuter, currentPoint),
				subtractPoints(currentOuter, currentPoint),
				kRoundJoinSegments,
				addPoint(subtractPoints(previousOuter, currentPoint), subtractPoints(currentOuter, currentPoint)));
			continue;
		}

		if (cmd.join == FlowPlot::LineJoin::Miter)
		{
			FlowPlot::PointF miterPoint{};
			const bool hasMiter = lineIntersection(
				previousOuter,
				previousSegment.direction,
				currentOuter,
				currentSegment.direction,
				miterPoint);
			if (hasMiter && lengthPoint(subtractPoints(miterPoint, currentPoint)) <= kMiterLimit)
			{
				appendTriangle(previousOuter, miterPoint, currentPoint);
				appendTriangle(miterPoint, currentOuter, currentPoint);
				continue;
			}
		}

		appendTriangle(previousOuter, currentOuter, currentPoint);
	}
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
	updateDescriptorSet(ctx.frameIndex);

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
		const FlowUi::Font::AtlasArrayResource* atlas = nullptr;
		if (input_.fontManager != nullptr)
		{
			atlas = &input_.fontManager->getAtlasResource();
		}
		const bool hasFontAtlas = atlas != nullptr
			&& atlas->view != VK_NULL_HANDLE
			&& atlas->sampler != VK_NULL_HANDLE
			&& atlas->layersUsed > 0u;
		const bool canDrawBox = run.type == RunType::Box
			&& boxPipeline_ != VK_NULL_HANDLE
			&& boxBuffer_.buffer != VK_NULL_HANDLE;
		const bool canDrawMarker = run.type == RunType::Marker
			&& markerPipeline_ != VK_NULL_HANDLE
			&& markerBuffer_.buffer != VK_NULL_HANDLE;
		const bool canDrawText = run.type == RunType::Text
			&& textPipeline_ != VK_NULL_HANDLE
			&& textGlyphBuffer_.buffer != VK_NULL_HANDLE
			&& hasFontAtlas;
		const bool canDrawPolyline = run.type == RunType::PolylineMesh
			&& polylinePipeline_ != VK_NULL_HANDLE
			&& polylineBuffer_.buffer != VK_NULL_HANDLE;
		if ((!canDrawBox && !canDrawMarker && !canDrawText && !canDrawPolyline) || descriptorSets_.empty())
		{
			continue;
		}

		const VkRect2D runScissor = worldScissorToVk(run.scissor);
		if (runScissor.extent.width == 0 || runScissor.extent.height == 0)
		{
			continue;
		}
		vkCmdSetScissor(ctx.commandBuffer, 0, 1, &runScissor);
		VkPipeline pipeline = textPipeline_;
		if (canDrawBox)
		{
			pipeline = boxPipeline_;
		}
		else if (canDrawMarker)
		{
			pipeline = markerPipeline_;
		}
		else if (canDrawPolyline)
		{
			pipeline = polylinePipeline_;
		}
		vkCmdBindPipeline(ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		const VkDescriptorSet descriptorSet =
			descriptorSets_[ctx.frameIndex % static_cast<std::uint32_t>(descriptorSets_.size())];
		vkCmdBindDescriptorSets(
			ctx.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout_,
			0,
			1,
			&descriptorSet,
			0,
			nullptr);
		vkCmdPushConstants(
			ctx.commandBuffer,
			pipelineLayout_,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			static_cast<std::uint32_t>(sizeof(PlotPushConstants)),
			&pushConstants);

		if (canDrawPolyline)
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
