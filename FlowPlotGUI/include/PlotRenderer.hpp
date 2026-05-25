#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <FlowUi/Flow.hpp>
#include <vulkan/vulkan.h>

#include "FlowPlot.hpp"
#include "FlowPlotGui.hpp"
#include "managers/FontManager.hpp"

struct VmaAllocation_T;
struct VmaAllocator_T;

namespace FlowPlotGui {

struct PlotRendererInput {
	state* guiState = nullptr;
	const FlowUi::FontManager* fontManager = nullptr;
	PlotCamera camera{};
};

class PlotRenderer {
public:
	void init(const FlowUi::ViewPortVulkanInterop& interop, VkFormat colorFormat);
	void destroy();

	void setInput(PlotRendererInput input);
	void invalidateTemplate();
	void invalidateDatasets();
	void invalidateViewport();

	void record(const FlowUi::ViewPortRenderContext& ctx);

	~PlotRenderer();

	enum class RunType : std::uint8_t {
		Box,
		Marker,
		Text,
		PolylineMesh,
	};

	struct PackedColor {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

	struct AllocatedBuffer {
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation_T* allocation = nullptr;
		void* mapped = nullptr;
		VkDeviceSize size = 0;
	};

private:
	struct CacheKey {
		std::uint64_t templateRevision = 0;
		std::uint64_t datasetRevision = 0;
	};

	struct PlotPushConstants {
		float viewportWidth = 0.0f;
		float viewportHeight = 0.0f;
		float imageWidth = 0.0f;
		float imageHeight = 0.0f;
		float cameraCenterX = 0.0f;
		float cameraCenterY = 0.0f;
		float zoom = 1.0f;
		float _pad = 0.0f;
	};

	struct PlotRun {
		RunType type = RunType::Box;
		FlowPlot::RectF scissor{};
		std::uint32_t first = 0;
		std::uint32_t count = 0;
	};

	struct BoxInstance {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
		float strokeWidth = 0.0f;
		PackedColor fill{};
		PackedColor stroke{};
	};

	struct MarkerInstance {
		float centerX = 0.0f;
		float centerY = 0.0f;
		float size = 0.0f;
		float strokeWidth = 0.0f;
		std::uint32_t shape = 0;
		PackedColor fill{};
		PackedColor stroke{};
	};

	struct TextGlyphInstance {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 0.0f;
		float v1 = 0.0f;
		std::uint32_t atlasLayer = 0;
		float distanceRangePx = 2.0f;
		PackedColor color{};
	};

	struct PolylineVertex {
		float x = 0.0f;
		float y = 0.0f;
		PackedColor color{};
	};

	struct GpuDrawPlan {
		FlowPlot::RenderPlot plot{};
		std::vector<PlotRun> runs{};
		std::vector<BoxInstance> boxes{};
		std::vector<MarkerInstance> markers{};
		std::vector<TextGlyphInstance> textGlyphs{};
		std::vector<PolylineVertex> polylineVertices{};
	};

	void rebuildIfNeeded(const FlowUi::ViewPortRenderContext& ctx);
	FlowPlot::RenderPlot buildFlowPlotCommands();
	void buildRuns();
	void uploadRuns(const FlowUi::ViewPortRenderContext& ctx);
	void triangulatePolyline(const FlowPlot::PolylineCommand& cmd);
	void recordDrawPlan(const FlowUi::ViewPortRenderContext& ctx);
	void updateDescriptorSet(std::uint32_t frameIndex);
	void reclaimRetiredBuffers(std::uint32_t frameIndex);
	void destroyRetiredBuffers();

	VkDevice device_ = VK_NULL_HANDLE;
	VmaAllocator_T* allocator_ = nullptr;
	VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
	VkPipeline boxPipeline_ = VK_NULL_HANDLE;
	VkPipeline markerPipeline_ = VK_NULL_HANDLE;
	VkPipeline textPipeline_ = VK_NULL_HANDLE;
	VkPipeline polylinePipeline_ = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> descriptorSets_{};
	VkShaderModule vertexShader_ = VK_NULL_HANDLE;
	VkShaderModule fragmentShader_ = VK_NULL_HANDLE;
	AllocatedBuffer boxBuffer_{};
	AllocatedBuffer markerBuffer_{};
	AllocatedBuffer textGlyphBuffer_{};
	AllocatedBuffer polylineBuffer_{};
	std::vector<std::vector<AllocatedBuffer>> retiredBuffersByFrame_{};

	PlotRendererInput input_{};
	CacheKey cacheKey_{};
	bool flowCommandsDirty_ = true;
	bool runsDirty_ = true;
	bool viewportDirty_ = true;

	GpuDrawPlan drawPlan_{};
	std::unordered_map<FlowInternal::DataKey, FlowInternal::DataView, FlowInternal::DataKeyHash> cachedDataViews_{};
	std::vector<std::unique_ptr<bool[]>> cachedBoolScratch_{};
};

} // namespace FlowPlotGui
