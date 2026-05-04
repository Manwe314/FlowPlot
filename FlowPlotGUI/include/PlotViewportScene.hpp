#pragma once

#include <chrono>
#include <memory>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <vulkan/vulkan.h>

namespace FlowPlotGui {

struct PlotViewportSceneResources {
	VkDevice device = VK_NULL_HANDLE;
	VkFormat colorFormat = VK_FORMAT_UNDEFINED;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;
	std::chrono::steady_clock::time_point animationStart = std::chrono::steady_clock::now();

	void destroy();
	~PlotViewportSceneResources();
};

struct PlotViewportSceneHandle {
	std::shared_ptr<PlotViewportSceneResources> resources{};
	std::string_view viewportKey{};
};

PlotViewportSceneHandle setupPlotViewportScene(FlowUi::App& app, std::string_view viewportKey);
void shutdownPlotViewportScene();

} // namespace FlowPlotGui
