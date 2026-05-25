#pragma once

#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "FlowPlotGui.hpp"

inline int colorPickerHexDigitValue(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	if (c >= 'a' && c <= 'f')
	{
		return 10 + c - 'a';
	}
	if (c >= 'A' && c <= 'F')
	{
		return 10 + c - 'A';
	}
	return -1;
}

inline std::string colorPickerHexFromRgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	std::ostringstream stream{};
	stream << '#'
		<< std::hex << std::nouppercase << std::setfill('0')
		<< std::setw(2) << static_cast<int>(r)
		<< std::setw(2) << static_cast<int>(g)
		<< std::setw(2) << static_cast<int>(b);
	return stream.str();
}

inline std::string colorPickerNormalizeHex(std::string_view text)
{
	std::string digits{};
	digits.reserve(6);
	for (const char c : text)
	{
		if (c == '#')
		{
			continue;
		}
		if (colorPickerHexDigitValue(c) >= 0)
		{
			digits.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
			if (digits.size() == 6u)
			{
				break;
			}
		}
	}

	if (digits.size() == 3u)
	{
		std::string expanded{};
		expanded.reserve(6);
		for (const char c : digits)
		{
			expanded.push_back(c);
			expanded.push_back(c);
		}
		digits = std::move(expanded);
	}
	while (digits.size() < 6u)
	{
		digits.push_back('0');
	}
	return "#" + digits;
}

inline std::array<std::uint8_t, 3> colorPickerRgbFromHex(std::string_view text)
{
	const std::string hex = colorPickerNormalizeHex(text);
	auto parseByte = [&](std::size_t offset) -> std::uint8_t {
		const int high = colorPickerHexDigitValue(hex[offset]);
		const int low = colorPickerHexDigitValue(hex[offset + 1u]);
		return static_cast<std::uint8_t>(std::clamp(high * 16 + low, 0, 255));
	};
	return {parseByte(1), parseByte(3), parseByte(5)};
}

inline Clay_Color colorPickerClayColorFromHex(std::string_view text)
{
	const std::array<std::uint8_t, 3> rgb = colorPickerRgbFromHex(text);
	return Clay_Color{
		.r = static_cast<float>(rgb[0]),
		.g = static_cast<float>(rgb[1]),
		.b = static_cast<float>(rgb[2]),
		.a = 255.0f,
	};
}

inline bool colorPickerHexEqual(std::string_view lhs, std::string_view rhs)
{
	return colorPickerNormalizeHex(lhs) == colorPickerNormalizeHex(rhs);
}

inline double sliderRoundTo2Decimals(double value)
{
	double rounded = std::round(value * 100.0) / 100.0;
	if (std::fabs(rounded) < 0.005)
	{
		rounded = 0.0;
	}
	return rounded;
}

struct sliderParams {
	double minValue = 0.0;
	double maxValue = 1.0;
	double value = 0.0;
	std::function<void(double)> onValueChangedCallback = nullptr;
	std::function<void()> onEditBegin = nullptr;
	std::function<void()> onEditEnd = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");

	uint16_t functionalWidthPx = 160;
	uint16_t handleWidthPx = 8;
	uint16_t trackHeightPx = 8;
	uint16_t handleHeightPx = 14;
	float trackCornerRadius = 5.0f;
	float handleCornerRadius = 4.0f;

	Clay_Color fillColor = FlowUi::Flow_Color("#4b8c5aff");
	Clay_Color unfillColor = FlowUi::Flow_Color("#2b3039ff");
	Clay_Color handleColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_Color handleHoverColor = FlowUi::Flow_Color("#adb3beff");
	Clay_Color handleActiveColor = FlowUi::Flow_Color("#d5d9e0ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	sliderParams,
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, minValue),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, maxValue),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, value),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, functionalWidthPx),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleWidthPx),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, trackHeightPx),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleHeightPx),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, trackCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, fillColor),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, unfillColor),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleColor),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleHoverColor),
	FLOWUI_DEV_REFLECT_FIELD(sliderParams, handleActiveColor));

struct sliderState {
	bool initialized = false;
	bool dragging = false;
	float pressMouseX = 0.0f;
	double pressValue = 0.0;
	double currentValue = 0.0;
};

FLOWUI_DEV_REGISTER_STRUCT(
	sliderState,
	FLOWUI_DEV_REFLECT_FIELD(sliderState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(sliderState, dragging),
	FLOWUI_DEV_REFLECT_FIELD(sliderState, pressMouseX),
	FLOWUI_DEV_REFLECT_FIELD(sliderState, pressValue),
	FLOWUI_DEV_REFLECT_FIELD(sliderState, currentValue));

using SliderDef = FlowUi::ElementDefinition<
	sliderParams,
	sliderState,
	void,
	FLOW_DEF_ID("Slider")>;

inline const SliderDef kSlider = {
	+[](SliderDef::InteractionContext& context) {
		const Clay_ElementId barId = context.uiManager.toClayEID(context.createChildElementId("bar"));
		if (context.previousInteraction.isHovered(barId))
		{
			context.uiManager.requestCursor(FlowUi::CursorType::PointingHand);
		}
	},
	+[](SliderDef::InteractionContext& context) {
		const Clay_ElementId barId = context.uiManager.toClayEID(context.createChildElementId("bar"));
		if (!context.previousInteraction.isPressed(barId))
		{
			return;
		}

		sliderState& state = SliderDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const double lower = std::min(context.params.minValue, context.params.maxValue);
		const double upper = std::max(context.params.minValue, context.params.maxValue);
		state.dragging = true;
		state.pressMouseX = context.uiManager.getCurrentFrameInput().mouseX;
		state.pressValue = std::clamp(state.currentValue, lower, upper);
		if (context.params.onEditBegin != nullptr)
		{
			context.params.onEditBegin();
		}
	},
	nullptr,
	+[](SliderDef::InteractionContext& context) {
		sliderState& state = SliderDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const bool wasDragging = state.dragging;
		state.dragging = false;
		if (wasDragging && context.params.onEditEnd != nullptr)
		{
			context.params.onEditEnd();
		}
	},
	+[](SliderDef::InteractionContext& context) {
		sliderState& state = SliderDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const FlowUi::FrameInput& input = context.uiManager.getCurrentFrameInput();
		if (!input.mouseDown[0])
		{
			const bool wasDragging = state.dragging;
			state.dragging = false;
			if (wasDragging && context.params.onEditEnd != nullptr)
			{
				context.params.onEditEnd();
			}
			return;
		}
		if (!state.dragging)
		{
			return;
		}

		const double lower = std::min(context.params.minValue, context.params.maxValue);
		const double upper = std::max(context.params.minValue, context.params.maxValue);
		const double range = upper - lower;
		if (range <= 0.0)
		{
			if (state.currentValue != lower)
			{
				state.currentValue = lower;
				if (context.params.onValueChangedCallback != nullptr)
				{
					context.params.onValueChangedCallback(state.currentValue);
				}
			}
			return;
		}

		const double functionalWidth = static_cast<double>(std::max<uint16_t>(1u, context.params.functionalWidthPx));
		const double valuePerPixel = range / functionalWidth;
		const double pressRatio = std::clamp((state.pressValue - lower) / range, 0.0, 1.0);
		const double pressFillPx = std::round(pressRatio * functionalWidth);
		const double deltaPx = std::round(static_cast<double>(input.mouseX - state.pressMouseX));
		const double nextFillPx = std::clamp(pressFillPx + deltaPx, 0.0, functionalWidth);
		const double nextValue = sliderRoundTo2Decimals(std::clamp(lower + nextFillPx * valuePerPixel, lower, upper));

		if (std::fabs(nextValue - state.currentValue) <= 1.0e-12)
		{
			return;
		}

		state.currentValue = nextValue;
		if (context.params.onValueChangedCallback != nullptr)
		{
			context.params.onValueChangedCallback(state.currentValue);
		}
	},
	nullptr,
	+[](SliderDef::BuildContext& context) {
		sliderState& state = SliderDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const double lower = std::min(context.params.minValue, context.params.maxValue);
		const double upper = std::max(context.params.minValue, context.params.maxValue);
		const double normalizedIncoming = sliderRoundTo2Decimals(std::clamp(context.params.value, lower, upper));

		if (!state.initialized)
		{
			state.currentValue = normalizedIncoming;
			state.initialized = true;
		}
		else if (!state.dragging)
		{
			state.currentValue = normalizedIncoming;
		}

		const float functionalWidthPx = static_cast<float>(std::max<uint16_t>(1u, context.params.functionalWidthPx));
		const float handleWidthPx = static_cast<float>(std::max<uint16_t>(1u, context.params.handleWidthPx));
		const float trackHeightPx = static_cast<float>(std::max<uint16_t>(1u, context.params.trackHeightPx));
		const float handleHeightPx = static_cast<float>(
			std::max<uint16_t>(context.params.trackHeightPx, context.params.handleHeightPx));

		float fillWidthPx = 0.0f;
		if (upper > lower)
		{
			const double ratio = std::clamp((state.currentValue - lower) / (upper - lower), 0.0, 1.0);
			fillWidthPx = static_cast<float>(std::round(ratio * static_cast<double>(functionalWidthPx)));
		}
		fillWidthPx = std::clamp(fillWidthPx, 0.0f, functionalWidthPx);
		const float unfillWidthPx = std::max(0.0f, functionalWidthPx - fillWidthPx);

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId fillId = context.uiManager.toClayEID(context.createChildElementId("fill"));
		const Clay_ElementId barId = context.uiManager.toClayEID(context.createChildElementId("bar"));
		const Clay_ElementId unfillId = context.uiManager.toClayEID(context.createChildElementId("unfill"));

		const bool barHovered = context.uiManager.getPreviousFramesInteraction().isHovered(barId);
		Clay_Color barColor = context.params.handleColor;
		if (state.dragging)
		{
			barColor = context.params.handleActiveColor;
		}
		else if (barHovered)
		{
			barColor = context.params.handleHoverColor;
		}

		Clay_ElementDeclaration root{};
		root.layout.sizing = context.params.sizing;
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.backgroundColor = context.params.backgroundColor;

		CLAY(rootId, root)
		{
			if (fillWidthPx > 0.0f)
			{
				Clay_ElementDeclaration fill{};
				fill.layout.sizing = {
					.width = CLAY_SIZING_FIXED(fillWidthPx),
					.height = CLAY_SIZING_FIXED(trackHeightPx),
				};
				fill.backgroundColor = context.params.fillColor;
				fill.cornerRadius = Clay_CornerRadius{
					.topLeft = context.params.trackCornerRadius,
					.topRight = 0.0f,
					.bottomLeft = context.params.trackCornerRadius,
					.bottomRight = 0.0f,
				};
				CLAY(fillId, fill){};
			}

			Clay_ElementDeclaration bar{};
			bar.layout.sizing = {
				.width = CLAY_SIZING_FIXED(handleWidthPx),
				.height = CLAY_SIZING_FIXED(handleHeightPx),
			};
			bar.backgroundColor = barColor;
			bar.cornerRadius = CLAY_CORNER_RADIUS(context.params.handleCornerRadius);
			CLAY(barId, bar){};

			if (unfillWidthPx > 0.0f)
			{
				Clay_ElementDeclaration unfill{};
				unfill.layout.sizing = {
					.width = CLAY_SIZING_FIXED(unfillWidthPx),
					.height = CLAY_SIZING_FIXED(trackHeightPx),
				};
				unfill.backgroundColor = context.params.unfillColor;
				unfill.cornerRadius = Clay_CornerRadius{
					.topLeft = 0.0f,
					.topRight = context.params.trackCornerRadius,
					.bottomLeft = 0.0f,
					.bottomRight = context.params.trackCornerRadius,
				};
				CLAY(unfillId, unfill){};
			}
		};
	},
};

struct colorPickerPreviewButtonParams {
	std::string value = "#000000";
	std::function<void()> onPressed = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(36), .height = CLAY_SIZING_FIXED(28)};
	Clay_Padding padding = CLAY_PADDING_ALL(3);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(5);
	Clay_Color borderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius previewCornerRadius = CLAY_CORNER_RADIUS(3);
};

FLOWUI_DEV_REGISTER_STRUCT(
	colorPickerPreviewButtonParams,
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, value),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerPreviewButtonParams, previewCornerRadius));

using ColorPickerPreviewButtonDef = FlowUi::ElementDefinition<
	colorPickerPreviewButtonParams,
	void,
	void,
	FLOW_DEF_ID("ColorPickerPreviewButton")>;

inline const ColorPickerPreviewButtonDef kColorPickerPreviewButton = {
	nullptr,
	+[](ColorPickerPreviewButtonDef::InteractionContext& context) {
		if (context.params.onPressed != nullptr)
		{
			context.params.onPressed();
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ColorPickerPreviewButtonDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId previewId = context.uiManager.toClayEID(context.createChildElementId("preview"));

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_ElementDeclaration preview{};
		preview.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		preview.backgroundColor = colorPickerClayColorFromHex(context.params.value);
		preview.cornerRadius = context.params.previewCornerRadius;

		CLAY(rootId, root)
		{
			CLAY(previewId, preview){};
		};
	},
};

struct colorPickerSwatchParams {
	std::string value = "#000000";
	std::function<void(std::string_view)> onChange = nullptr;
	std::function<void()> onEditBegin = nullptr;
	std::function<void()> onEditEnd = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	uint16_t childGap = 8;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");

	colorPickerPreviewButtonParams preview{};

	Clay_Sizing hexInputSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding hexInputPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color hexInputBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth hexInputBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color hexInputBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_CornerRadius hexInputCornerRadius = CLAY_CORNER_RADIUS(5);
	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");

	int16_t outsideDismissZIndex = 200;
	int16_t popupZIndex = 201;
	uint16_t popupGapPx = 4;
	Clay_Sizing popupSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(230), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding popupPadding = Clay_Padding{10, 10, 10, 10};
	uint16_t popupChildGap = 10;
	Clay_Color popupBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color popupBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth popupBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 1};
	Clay_CornerRadius popupCornerRadius = CLAY_CORNER_RADIUS(6);

	sliderParams slider{};
	Clay_Color redSliderFillColor = FlowUi::Flow_Color("#d95d5dff");
	Clay_Color greenSliderFillColor = FlowUi::Flow_Color("#5ebd72ff");
	Clay_Color blueSliderFillColor = FlowUi::Flow_Color("#5e8fdcff");
	Clay_Color sliderHintTextColor = FlowUi::Flow_Color("#aeb2b8ff");
	numericInputFieldParams channelInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	colorPickerSwatchParams,
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, value),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, preview),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputSizing),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputPadding),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, hexInputCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, outsideDismissZIndex),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupZIndex),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupGapPx),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupSizing),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupPadding),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupChildGap),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, popupCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, slider),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, redSliderFillColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, greenSliderFillColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, blueSliderFillColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, sliderHintTextColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchParams, channelInput));

struct colorPickerSwatchState {
	bool isExpanded = false;
	std::string normalizedHex = "#000000";
	bool pendingHexFieldReset = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	colorPickerSwatchState,
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchState, isExpanded),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchState, normalizedHex),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerSwatchState, pendingHexFieldReset));

using ColorPickerSwatchDef = FlowUi::ElementDefinition<
	colorPickerSwatchParams,
	colorPickerSwatchState,
	void,
	FLOW_DEF_ID("ColorPickerSwatch")>;

inline void colorPickerSwatchEmitHex(
	colorPickerSwatchState& state,
	std::string_view value,
	const std::function<void(std::string_view)>& onChange)
{
	const std::string normalizedHex = colorPickerNormalizeHex(value);
	const bool changed = !colorPickerHexEqual(state.normalizedHex, normalizedHex);
	state.normalizedHex = normalizedHex;
	state.pendingHexFieldReset = true;
	if (changed && onChange != nullptr)
	{
		onChange(state.normalizedHex);
	}
}

inline void colorPickerSwatchDrawSliderRow(
	ColorPickerSwatchDef::BuildContext& context,
	std::string_view rowName,
	std::string_view label,
	double value,
	Clay_Color fillColor,
	std::function<void(double)> onValueChanged,
	std::function<void()> onEditBegin,
	std::function<void()> onEditEnd)
{
	const Clay_ElementId rowId = context.uiManager.toClayEID(context.createChildElementId(std::string("popup/") + std::string(rowName)));
	const Clay_ElementId labelId = context.uiManager.toClayEID(context.createChildElementId(std::string("popup/") + std::string(rowName) + "/label"));
	const std::string sliderPath = context.createChildElementId(std::string("popup/") + std::string(rowName) + "/slider");
	const std::string channelInputPath = context.createChildElementId(std::string("popup/") + std::string(rowName) + "/channel-input");

	Clay_ElementDeclaration row{};
	row.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	row.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	row.layout.childGap = 4;

	Clay_TextElementConfig labelConfig{};
	labelConfig.textColor = context.params.sliderHintTextColor;
	labelConfig.fontSize = context.params.fontSize;
	labelConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
	labelConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
	labelConfig.fontId = context.params.fontId;

	sliderParams params = context.params.slider;
	params.minValue = 0.0;
	params.maxValue = 255.0;
	params.value = value;
	params.fillColor = fillColor;
	params.onValueChangedCallback = onValueChanged;
	params.onEditBegin = onEditBegin;
	params.onEditEnd = onEditEnd;

	numericInputFieldParams channelParams = context.params.channelInput;
	channelParams.valueType = numericInputValueType::UInt8;
	channelParams.value = value;
	channelParams.minValue = 0.0;
	channelParams.maxValue = 255.0;
	channelParams.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(96), .height = CLAY_SIZING_FIT(0)};
	channelParams.fontId = context.params.fontId;
	channelParams.fontSize = context.params.fontSize;
	channelParams.onChange = std::move(onValueChanged);
	channelParams.onEditBegin = onEditBegin;
	channelParams.onEditEnd = onEditEnd;

	CLAY(rowId, row)
	{
		CLAY(labelId, {})
		{
			CLAY_TEXT(
				context.uiManager.toClayString(std::string(label)),
				CLAY_TEXT_CONFIG(labelConfig)
			);
		};
		context.uiManager.createElement(kSlider, sliderPath)
			.setParameters(std::move(params))
			.draw();
		context.uiManager.createElement(kNumericInputField, channelInputPath)
			.setParameters(std::move(channelParams))
			.draw();
	};
}

inline const ColorPickerSwatchDef kColorPickerSwatch = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ColorPickerSwatchDef::InteractionContext& context) {
		colorPickerSwatchState& state = ColorPickerSwatchDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const std::string normalizedHex = colorPickerNormalizeHex(context.params.value);
		if (!colorPickerHexEqual(state.normalizedHex, normalizedHex))
		{
			state.normalizedHex = normalizedHex;
			state.pendingHexFieldReset = true;
		}
	},
	nullptr,
	+[](ColorPickerSwatchDef::BuildContext& context) {
		colorPickerSwatchState& state = ColorPickerSwatchDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const std::string normalizedParamHex = colorPickerNormalizeHex(context.params.value);
		if (!colorPickerHexEqual(state.normalizedHex, normalizedParamHex))
		{
			state.normalizedHex = normalizedParamHex;
			state.pendingHexFieldReset = true;
		}

		const std::string hexInputPath = context.createChildElementId("hex-input");
		if (state.pendingHexFieldReset)
		{
			(void)context.uiManager.inputFields().removeField(hexInputPath);
			state.pendingHexFieldReset = false;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const std::string previewPath = context.createChildElementId("preview-button");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = context.params.childGap;
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		colorPickerPreviewButtonParams previewParams = context.params.preview;
		previewParams.value = state.normalizedHex;
		previewParams.onPressed = [elementFlowId = FlowUi::toFlowId(context.elementID)]() {
			colorPickerSwatchState* latestState = ColorPickerSwatchDef::tryGetState(elementFlowId);
			if (latestState != nullptr)
			{
				latestState->isExpanded = !latestState->isExpanded;
			}
		};

		basicInputFieldParams hexInputParams{};
		hexInputParams.fieldId = hexInputPath;
		hexInputParams.value = state.normalizedHex;
		hexInputParams.syncValueFromParams = true;
		hexInputParams.onTextChangedCallback = [
			elementFlowId = FlowUi::toFlowId(context.elementID),
			onChange = context.params.onChange
		](std::string_view text) {
			colorPickerSwatchState* latestState = ColorPickerSwatchDef::tryGetState(elementFlowId);
			if (latestState == nullptr)
			{
				return;
			}
			colorPickerSwatchEmitHex(*latestState, text, onChange);
		};
		hexInputParams.padding = context.params.hexInputPadding;
		hexInputParams.sizing = context.params.hexInputSizing;
		hexInputParams.borderColor = context.params.hexInputBorderColor;
		hexInputParams.borderWidth = context.params.hexInputBorderWidth;
		hexInputParams.backgroundColor = context.params.hexInputBackgroundColor;
		hexInputParams.cornerRadius = context.params.hexInputCornerRadius;
		hexInputParams.childTextAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		hexInputParams.textWrapMode = CLAY_TEXT_WRAP_NONE;
		hexInputParams.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		hexInputParams.fontId = context.params.fontId;
		hexInputParams.fontSize = context.params.fontSize;
		hexInputParams.textColor = context.params.textColor;

		const std::array<std::uint8_t, 3> rgb = colorPickerRgbFromHex(state.normalizedHex);

		CLAY(rootId, root)
		{
			context.uiManager.createElement(kColorPickerPreviewButton, previewPath)
				.setParameters(std::move(previewParams))
				.draw();

			context.uiManager.createElement(kBasicInputField, hexInputPath)
				.setParameters(std::move(hexInputParams))
				.draw();

			if (state.isExpanded)
			{
				Clay_ElementDeclaration dismissLayer{};
				const Clay_ElementId dismissLayerId = context.uiManager.toClayEID(context.createChildElementId("dismiss-layer"));
				dismissLayer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
				dismissLayer.backgroundColor = FlowUi::Flow_Color("#00000000");
				dismissLayer.floating = {
					.zIndex = context.params.outsideDismissZIndex,
					.attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_TOP, .parent = CLAY_ATTACH_POINT_LEFT_TOP},
					.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
					.attachTo = CLAY_ATTACH_TO_ROOT,
				};

				CLAY(dismissLayerId, dismissLayer)
				{
					basicButtonParams dismissParams{};
					dismissParams.contentMode = basicButtonParams::ContentMode::None;
					dismissParams.padding = CLAY_PADDING_ALL(0);
					dismissParams.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
					dismissParams.backgroundColor = FlowUi::Flow_Color("#00000000");
					dismissParams.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
					dismissParams.borderColor = FlowUi::Flow_Color("#00000000");
					dismissParams.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
					dismissParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
						colorPickerSwatchState* latestState = ColorPickerSwatchDef::tryGetState(elementFlowId);
						if (latestState != nullptr)
						{
							latestState->isExpanded = false;
						}
					};
					context.uiManager.createElement(kBasicButton, context.createChildElementId("dismiss-layer/button"))
						.setParameters(std::move(dismissParams))
						.draw();
				};

				Clay_ElementDeclaration popup{};
				const Clay_ElementId popupId = context.uiManager.toClayEID(context.createChildElementId("popup"));
				popup.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
				popup.layout.sizing = context.params.popupSizing;
				popup.layout.padding = context.params.popupPadding;
				popup.layout.childGap = context.params.popupChildGap;
				popup.backgroundColor = context.params.popupBackgroundColor;
				popup.cornerRadius = context.params.popupCornerRadius;
				popup.border = {.color = context.params.popupBorderColor, .width = context.params.popupBorderWidth};
				popup.floating = {
					.offset = {.x = 0.0f, .y = static_cast<float>(context.params.popupGapPx)},
					.parentId = context.uiManager.toClayEID(previewPath).id,
					.zIndex = context.params.popupZIndex,
					.attachPoints = {
						.element = CLAY_ATTACH_POINT_CENTER_TOP,
						.parent = CLAY_ATTACH_POINT_CENTER_BOTTOM,
					},
					.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
					.attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
				};

				CLAY(popupId, popup)
				{
					auto emitRgb = [
						elementFlowId = FlowUi::toFlowId(context.elementID),
						onChange = context.params.onChange
					](std::uint8_t r, std::uint8_t g, std::uint8_t b) {
						colorPickerSwatchState* latestState = ColorPickerSwatchDef::tryGetState(elementFlowId);
						if (latestState == nullptr)
						{
							return;
						}
						colorPickerSwatchEmitHex(*latestState, colorPickerHexFromRgb(r, g, b), onChange);
					};

					colorPickerSwatchDrawSliderRow(
						context,
						"red",
						"Red",
						static_cast<double>(rgb[0]),
						context.params.redSliderFillColor,
						[emitRgb, rgb](double value) {
							emitRgb(static_cast<std::uint8_t>(std::clamp(std::lround(value), 0l, 255l)), rgb[1], rgb[2]);
						},
						context.params.onEditBegin,
						context.params.onEditEnd);
					colorPickerSwatchDrawSliderRow(
						context,
						"green",
						"Green",
						static_cast<double>(rgb[1]),
						context.params.greenSliderFillColor,
						[emitRgb, rgb](double value) {
							emitRgb(rgb[0], static_cast<std::uint8_t>(std::clamp(std::lround(value), 0l, 255l)), rgb[2]);
						},
						context.params.onEditBegin,
						context.params.onEditEnd);
					colorPickerSwatchDrawSliderRow(
						context,
						"blue",
						"Blue",
						static_cast<double>(rgb[2]),
						context.params.blueSliderFillColor,
						[emitRgb, rgb](double value) {
							emitRgb(rgb[0], rgb[1], static_cast<std::uint8_t>(std::clamp(std::lround(value), 0l, 255l)));
						},
						context.params.onEditBegin,
						context.params.onEditEnd);
				};
			}
		};
	},
};

struct colorPickerCardParams {
	std::string hintText = "Color";
	std::string value = "#000000";
	std::function<void(std::string_view)> onChange = nullptr;
	std::function<void()> onEditBegin = nullptr;
	std::function<void()> onEditEnd = nullptr;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	colorPickerSwatchParams swatch{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	colorPickerCardParams,
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(colorPickerCardParams, swatch));

using ColorPickerCardDef = FlowUi::ElementDefinition<
	colorPickerCardParams,
	void,
	void,
	FLOW_DEF_ID("ColorPickerCard")>;

inline const ColorPickerCardDef kColorPickerCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ColorPickerCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string swatchPath = context.createChildElementId("swatch");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = context.params.cardLayout;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = 0;
		root.backgroundColor = FlowUi::Flow_Color("#00000000");
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(1.0f)};

		colorPickerSwatchParams swatchParams = context.params.swatch;
		swatchParams.value = context.params.value;
		swatchParams.onChange = context.params.onChange;
		swatchParams.onEditBegin = context.params.onEditBegin;
		swatchParams.onEditEnd = context.params.onEditEnd;
		swatchParams.fontId = context.params.fontId;
		swatchParams.fontSize = context.params.fontSize;
		swatchParams.textColor = context.params.textColor;

		CLAY(rootId, root)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};
			CLAY(spacerId, spacer){};
			context.uiManager.createElement(kColorPickerSwatch, swatchPath)
				.setParameters(std::move(swatchParams))
				.draw();
		};
	},
};
