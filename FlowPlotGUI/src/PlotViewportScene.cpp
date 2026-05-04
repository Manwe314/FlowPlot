#include "PlotViewportScene.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace FlowPlotGui {
namespace {

constexpr const char* kSceneVertexShaderFile = "plot_viewport_solid.vert.spv";
constexpr const char* kSceneFragmentShaderFile = "plot_viewport_solid.frag.spv";

struct ScenePushConstants {
	float width = 0.0f;
	float height = 0.0f;
	float cycleValue = 0.0f;
	float _pad = 0.0f;
};

std::shared_ptr<PlotViewportSceneResources>& sharedSceneResources()
{
	static std::shared_ptr<PlotViewportSceneResources> resources;
	return resources;
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
	vkCheck(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule), "Failed to create scene shader module.");
	return shaderModule;
}

VkPipeline createScenePipeline(
	VkDevice device,
	VkFormat colorFormat,
	VkPipelineLayout pipelineLayout,
	VkShaderModule vertexShader,
	VkShaderModule fragmentShader)
{
	if (colorFormat == VK_FORMAT_UNDEFINED)
	{
		throw std::runtime_error("Cannot create scene pipeline with undefined color format.");
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
		"Failed to create scene graphics pipeline.");
	return pipeline;
}

void makeResources(
	const std::shared_ptr<PlotViewportSceneResources>& resources,
	const FlowUi::ViewPortVulkanInterop& interop,
	VkFormat colorFormat)
{
	if (!resources)
	{
		throw std::runtime_error("Scene resources pointer is null.");
	}
	if (interop.device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Cannot create scene pipeline without a valid Vulkan device.");
	}

	resources->destroy();
	resources->device = interop.device;
	resources->colorFormat = colorFormat;

	try
	{
		const std::vector<char> vertexShaderBytes = readShaderFile(kSceneVertexShaderFile);
		const std::vector<char> fragmentShaderBytes = readShaderFile(kSceneFragmentShaderFile);

		resources->vertexShader = createShaderModule(resources->device, vertexShaderBytes);
		resources->fragmentShader = createShaderModule(resources->device, fragmentShaderBytes);

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ScenePushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		vkCheck(
			vkCreatePipelineLayout(resources->device, &pipelineLayoutInfo, nullptr, &resources->pipelineLayout),
			"Failed to create scene pipeline layout.");

		resources->graphicsPipeline = createScenePipeline(
			resources->device,
			colorFormat,
			resources->pipelineLayout,
			resources->vertexShader,
			resources->fragmentShader);

		vkDestroyShaderModule(resources->device, resources->vertexShader, nullptr);
		vkDestroyShaderModule(resources->device, resources->fragmentShader, nullptr);
		resources->vertexShader = VK_NULL_HANDLE;
		resources->fragmentShader = VK_NULL_HANDLE;
	}
	catch (...)
	{
		resources->destroy();
		throw;
	}
}

void recordScene(const FlowUi::ViewPortRenderContext& ctx, PlotViewportSceneResources& sceneResources)
{
	if (sceneResources.graphicsPipeline == VK_NULL_HANDLE || sceneResources.pipelineLayout == VK_NULL_HANDLE)
	{
		return;
	}
	if (ctx.commandBuffer == VK_NULL_HANDLE || ctx.extent.width == 0 || ctx.extent.height == 0)
	{
		return;
	}

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

	vkCmdBindPipeline(ctx.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sceneResources.graphicsPipeline);

	const auto now = std::chrono::steady_clock::now();
	const float elapsedSeconds = std::chrono::duration<float>(now - sceneResources.animationStart).count();
	const ScenePushConstants pushConstants{
		.width = static_cast<float>(ctx.extent.width),
		.height = static_cast<float>(ctx.extent.height),
		.cycleValue = elapsedSeconds,
		._pad = 0.0f,
	};
	vkCmdPushConstants(
		ctx.commandBuffer,
		sceneResources.pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		static_cast<std::uint32_t>(sizeof(ScenePushConstants)),
		&pushConstants);

	vkCmdDraw(ctx.commandBuffer, 6, 1, 0, 0);
}

} // namespace

void PlotViewportSceneResources::destroy()
{
	if (device != VK_NULL_HANDLE)
	{
		if (graphicsPipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(device, graphicsPipeline, nullptr);
		}
		if (pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		}
		if (vertexShader != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, vertexShader, nullptr);
		}
		if (fragmentShader != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(device, fragmentShader, nullptr);
		}
	}

	graphicsPipeline = VK_NULL_HANDLE;
	pipelineLayout = VK_NULL_HANDLE;
	vertexShader = VK_NULL_HANDLE;
	fragmentShader = VK_NULL_HANDLE;
	device = VK_NULL_HANDLE;
	colorFormat = VK_FORMAT_UNDEFINED;
}

PlotViewportSceneResources::~PlotViewportSceneResources()
{
	destroy();
}

PlotViewportSceneHandle setupPlotViewportScene(FlowUi::App& app, std::string_view viewportKey)
{
	FlowUi::ViewPortManager& viewPortManager = app.viewPorts();
	FlowUi::ViewPortCreateInfo createInfo{};
	createInfo.colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	createInfo.clearEveryFrame = true;
	viewPortManager.create(viewportKey, createInfo);

	FlowUi::ViewPort* viewport = viewPortManager.getViewPort(viewportKey);
	if (viewport == nullptr)
	{
		throw std::runtime_error("Failed to get plot viewport.");
	}

	std::shared_ptr<PlotViewportSceneResources>& resources = sharedSceneResources();
	if (!resources)
	{
		resources = std::make_shared<PlotViewportSceneResources>();
	}

	if (resources->graphicsPipeline == VK_NULL_HANDLE || resources->colorFormat != createInfo.colorFormat)
	{
		makeResources(resources, viewPortManager.getVulkanInterop(), createInfo.colorFormat);
	}

	viewport->setRenderCallback(resources, [](const FlowUi::ViewPortRenderContext& ctx, PlotViewportSceneResources& sceneResources) {
		recordScene(ctx, sceneResources);
	});

	return PlotViewportSceneHandle{
		.resources = resources,
		.viewportKey = viewportKey,
	};
}

void shutdownPlotViewportScene()
{
	std::shared_ptr<PlotViewportSceneResources>& resources = sharedSceneResources();
	if (resources)
	{
		resources->destroy();
		resources.reset();
	}
}

} // namespace FlowPlotGui
