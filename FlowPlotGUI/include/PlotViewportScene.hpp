#pragma once

#include <memory>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <vulkan/vulkan.h>

#include "PlotRenderer.hpp"

namespace FlowPlotGui {

struct PlotViewportSceneResources {
	VkDevice device = VK_NULL_HANDLE;
	VkFormat colorFormat = VK_FORMAT_UNDEFINED;
	PlotRenderer renderer{};

	void setInput(PlotRendererInput input);
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
