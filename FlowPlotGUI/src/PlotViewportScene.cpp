#include "PlotViewportScene.hpp"

#include <stdexcept>

namespace FlowPlotGui {
namespace {

std::shared_ptr<PlotViewportSceneResources>& sharedSceneResources()
{
	static std::shared_ptr<PlotViewportSceneResources> resources;
	return resources;
}

} // namespace

void PlotViewportSceneResources::setInput(PlotRendererInput input)
{
	renderer.setInput(input);
}

void PlotViewportSceneResources::destroy()
{
	renderer.destroy();
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

	const FlowUi::ViewPortVulkanInterop& interop = viewPortManager.getVulkanInterop();
	if (resources->device != interop.device || resources->colorFormat != createInfo.colorFormat)
	{
		resources->renderer.init(interop, createInfo.colorFormat);
		resources->device = interop.device;
		resources->colorFormat = createInfo.colorFormat;
	}

	viewport->setRenderCallback(resources, [](const FlowUi::ViewPortRenderContext& ctx, PlotViewportSceneResources& sceneResources) {
		sceneResources.renderer.record(ctx);
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
