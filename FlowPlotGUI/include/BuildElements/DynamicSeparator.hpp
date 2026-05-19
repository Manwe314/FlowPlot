#pragma once

#include <cmath>
#include <functional>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct dynamicSeparatorParams {
	enum class Orientation : uint8_t {
		Vertical,
		Horizontal,
	};

	Orientation orientation = Orientation::Vertical;
	bool reverseDrag = false;
	int width = 4;
	int height = 4;
	Clay_Color color = FlowUi::Flow_Color("#5e646eff");
	Clay_Color hoverColor = FlowUi::Flow_Color("#7a828fff");
	Clay_Color activeColor = FlowUi::Flow_Color("#9aa2aeff");

	int minValue = 0;
	int maxValue = 100000;
	std::function<int()> getValue = nullptr;
	std::function<void(int)> setValue = nullptr;
};

struct dynamicSeparatorState {
	bool isPressed = false;
	bool isDragging = false;
	float pressMouseAxis = 0.0f;
	int pressValue = 0;
	int localValue = 0;
};

FLOWUI_DEV_REGISTER_ENUM(
	dynamicSeparatorParams::Orientation,
	FLOWUI_DEV_ENUM_VALUE(dynamicSeparatorParams::Orientation::Vertical),
	FLOWUI_DEV_ENUM_VALUE(dynamicSeparatorParams::Orientation::Horizontal));

using DynamicSeparatorDef = FlowUi::ElementDefinition<
	dynamicSeparatorParams,
	dynamicSeparatorState,
	void,
	FLOW_DEF_ID("DynamicSeparator")>;

inline const DynamicSeparatorDef kDynamicSeparator = {
	nullptr,
	+[](DynamicSeparatorDef::InteractionContext& context) {
		dynamicSeparatorState& state = DynamicSeparatorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.isPressed = true;
		state.isDragging = true;
		const FlowUi::FrameInput& input = context.uiManager.getCurrentFrameInput();
		state.pressMouseAxis =
			(context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
			? input.mouseY
			: input.mouseX;

		int minValue = context.params.minValue;
		int maxValue = context.params.maxValue;
		if (maxValue < minValue)
		{
			maxValue = minValue;
		}

		int baseValue = state.localValue;
		if (context.params.getValue != nullptr)
		{
			baseValue = context.params.getValue();
		}
		if (baseValue < minValue)
		{
			baseValue = minValue;
		}
		else if (baseValue > maxValue)
		{
			baseValue = maxValue;
		}
		state.pressValue = baseValue;
	},
	nullptr,
	nullptr,
	+[](DynamicSeparatorDef::InteractionContext& context) {
		dynamicSeparatorState& state = DynamicSeparatorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const FlowUi::FrameInput& input = context.uiManager.getCurrentFrameInput();
		const FlowUi::FrameInput& previousInput = context.uiManager.getPreviousFrameInput();

		if (!state.isDragging)
		{
			return;
		}

		const bool primaryMouseReleased = previousInput.mouseDown[0] && !input.mouseDown[0];
		if (primaryMouseReleased)
		{
			state.isPressed = false;
			state.isDragging = false;
			return;
		}

		if (!input.mouseDown[0])
		{
			return;
		}

		int minValue = context.params.minValue;
		int maxValue = context.params.maxValue;
		if (maxValue < minValue)
		{
			maxValue = minValue;
		}

		const float currentAxis =
			(context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
			? input.mouseY
			: input.mouseX;
		const float deltaAxis = currentAxis - state.pressMouseAxis;
		const int deltaPixels = static_cast<int>(std::lround(deltaAxis));
		const int signedDelta = context.params.reverseDrag ? -deltaPixels : deltaPixels;
		int nextValue = state.pressValue + signedDelta;
		if (nextValue < minValue)
		{
			nextValue = minValue;
		}
		else if (nextValue > maxValue)
		{
			nextValue = maxValue;
		}

		state.localValue = nextValue;
		if (context.params.setValue != nullptr)
		{
			context.params.setValue(nextValue);
		}
	},
	nullptr,
	+[](DynamicSeparatorDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const dynamicSeparatorState* state = DynamicSeparatorDef::tryGetStateConst(FlowUi::toFlowId(context.elementID));
		const FlowUi::InteractionSnapshot& previousInteraction = context.uiManager.getPreviousFramesInteraction();

		int width = context.params.width;
		int height = context.params.height;
		if (width < 1)
		{
			width = 1;
		}
		if (height < 1)
		{
			height = 1;
		}

		Clay_Color separatorColor = context.params.color;
		if (state && state->isPressed)
		{
			separatorColor = context.params.activeColor;
		}
		else if (previousInteraction.isHovered(rootId))
		{
			separatorColor = context.params.hoverColor;
		}

		Clay_LayoutConfig rootLayout{};
		if (context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
		{
			rootLayout.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIXED(static_cast<float>(height)),
			};
		}
		else
		{
			rootLayout.sizing = {
				.width = CLAY_SIZING_FIXED(static_cast<float>(width)),
				.height = CLAY_SIZING_GROW(0),
			};
		}

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = separatorColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(rootId, root){};
	},
};

FLOWUI_DEV_REGISTER_STRUCT(
	dynamicSeparatorParams,
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, orientation),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, reverseDrag),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, width),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, height),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, color),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, hoverColor),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, activeColor),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, minValue),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorParams, maxValue));

FLOWUI_DEV_REGISTER_STRUCT(
	dynamicSeparatorState,
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorState, isPressed),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorState, isDragging),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorState, pressMouseAxis),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorState, pressValue),
	FLOWUI_DEV_REFLECT_FIELD(dynamicSeparatorState, localValue));
