#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "FlowPlotGui.hpp"

struct enumPickerMenuParams {
	std::span<const std::string> options{};
	std::string value = "";
	std::string defaultValue = "";
	std::string emptyValueText = "<select>";
	std::string emptyOptionsText = "<no options>";
	std::function<void(std::string_view)> onChange = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(100.0f, 180.0f), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = Clay_Padding{8, 8, 6, 6};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color hoverBackgroundColor = FlowUi::Flow_Color("#20252dff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(5);
	Clay_Color borderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	uint16_t childGap = 8;

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color valueTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode valueTextWrapMode = CLAY_TEXT_WRAP_WORDS;
	Clay_TextAlignment valueTextAlignment = CLAY_TEXT_ALIGN_LEFT;

	Clay_Sizing arrowButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(22), .height = CLAY_SIZING_FIXED(22)};
	Clay_Padding arrowButtonPadding = CLAY_PADDING_ALL(3);
	Clay_Color arrowButtonBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color arrowButtonHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	Clay_CornerRadius arrowButtonCornerRadius = CLAY_CORNER_RADIUS(4);
	Clay_Color arrowButtonBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth arrowButtonBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Sizing arrowIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(14), .height = CLAY_SIZING_FIXED(14)};
	Clay_Color arrowIconTintColor = FlowUi::Flow_Color("#ffffffff");

	int16_t outsideDismissZIndex = 200;
	int16_t dropdownZIndex = 201;
	uint16_t dropdownGapPx = 2;
	Clay_Sizing dropdownSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(100.0f, 220.0f), .height = CLAY_SIZING_FIT(0)};
	Clay_Color dropdownBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color dropdownBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth dropdownBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 1};
	Clay_CornerRadius dropdownCornerRadius = CLAY_CORNER_RADIUS(5);

	Clay_Padding optionPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color optionTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Color optionHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	Clay_Color separatorColor = FlowUi::Flow_Color("#303640ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	enumPickerMenuParams,
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, value),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, defaultValue),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, emptyValueText),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, emptyOptionsText),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, hoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, valueTextColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, valueTextWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, valueTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, arrowIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, outsideDismissZIndex),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownZIndex),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownGapPx),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, dropdownCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, optionPadding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, optionTextColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, optionHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuParams, separatorColor));

struct enumPickerMenuState {
	bool isExpanded = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	enumPickerMenuState,
	FLOWUI_DEV_REFLECT_FIELD(enumPickerMenuState, isExpanded));

struct enumPickerMenuResources {
	enumPickerMenuResources() = default;
	explicit enumPickerMenuResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using EnumPickerMenuDef = FlowUi::ElementDefinition<
	enumPickerMenuParams,
	enumPickerMenuState,
	enumPickerMenuResources,
	FLOW_DEF_ID("EnumPickerMenu")>;
using EnumPickerMenuInteractionContext = EnumPickerMenuDef::InteractionContext;

inline std::string enumPickerMenuCurrentValue(const enumPickerMenuParams& params)
{
	if (!params.value.empty())
	{
		return params.value;
	}
	if (!params.defaultValue.empty())
	{
		return params.defaultValue;
	}
	return params.emptyValueText;
}

inline std::string enumPickerMenuOptionLabel(std::string_view option)
{
	std::string label;
	label.reserve(option.size() + 4);
	for (std::size_t i = 0; i < option.size(); ++i)
	{
		const unsigned char current = static_cast<unsigned char>(option[i]);
		if (current == '_' || current == '-')
		{
			label.push_back(' ');
			continue;
		}

		const bool startsCamelCaseWord = i > 0 && std::isupper(current) &&
			(std::islower(static_cast<unsigned char>(option[i - 1])) ||
				std::isdigit(static_cast<unsigned char>(option[i - 1])) ||
				(i + 1 < option.size() && std::islower(static_cast<unsigned char>(option[i + 1]))));
		if (startsCamelCaseWord && !label.empty() && label.back() != ' ')
		{
			label.push_back(' ');
		}
		label.push_back(static_cast<char>(current));
	}
	return label;
}

inline float enumPickerMenuEstimatedDropdownHeight(const enumPickerMenuParams& params)
{
	const float dropdownWidth = params.dropdownSizing.width.type == CLAY__SIZING_TYPE_FIXED
		? params.dropdownSizing.width.size.minMax.min
		: params.dropdownSizing.width.size.minMax.max;
	const float textWidth = std::max(
		1.0f,
		dropdownWidth - static_cast<float>(params.optionPadding.left + params.optionPadding.right));
	const float lineHeight = std::max(1.0f, static_cast<float>(params.fontSize) * 1.25f);

	auto estimatedOptionHeight = [&](std::string_view text) {
		// Font-independent pre-layout estimate. The slight overestimate is intentional:
		// opening above unnecessarily is preferable to clipping the last row below the viewport.
		const float estimatedTextWidth = static_cast<float>(text.size()) * static_cast<float>(params.fontSize) * 0.6f;
		const float lineCount = std::max(1.0f, std::ceil(estimatedTextWidth / textWidth));
		return lineCount * lineHeight + static_cast<float>(params.optionPadding.top + params.optionPadding.bottom);
	};

	float height = static_cast<float>(params.dropdownBorderWidth.top + params.dropdownBorderWidth.bottom);
	if (params.options.empty())
	{
		return height + estimatedOptionHeight(params.emptyOptionsText);
	}
	for (const std::string& option : params.options)
	{
		height += estimatedOptionHeight(enumPickerMenuOptionLabel(option));
	}
	height += static_cast<float>(params.options.size() - 1) * params.dropdownBorderWidth.betweenChildren;
	return height;
}

inline void enumPickerMenuToggleExpanded(std::string_view elementId)
{
	enumPickerMenuState& state = EnumPickerMenuDef::getOrCreateState(FlowUi::toFlowId(elementId));
	state.isExpanded = !state.isExpanded;
}

inline basicButtonParams enumPickerTransparentButtonParams(
	Clay_Sizing sizing,
	Clay_Padding padding,
	Clay_Color hoverBackgroundColor,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.contentMode = basicButtonParams::ContentMode::None;
	params.onHoveredCallback = [hoverBackgroundColor](BasicButtonInteractionContext buttonContext) {
		buttonContext.params.backgroundColor = hoverBackgroundColor;
	};
	params.onPressedCallback = std::move(onPressed);
	params.padding = padding;
	params.sizing = sizing;
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = hoverBackgroundColor;
	params.cornerRadius = CLAY_CORNER_RADIUS(0);
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	return params;
}

struct enumPickerTriggerParams {
	std::string valueText = "";
	bool hasValueText = false;
	FlowUi::TextureRef disclosureIcon = FlowUi::TextureRef{};
	std::function<void()> onPressed = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = Clay_Padding{2, 0, 0, 0};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color hoverBackgroundColor = FlowUi::Flow_Color("#20252dff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(0);
	uint16_t childGap = 8;

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color valueTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode valueTextWrapMode = CLAY_TEXT_WRAP_WORDS;
	Clay_TextAlignment valueTextAlignment = CLAY_TEXT_ALIGN_LEFT;

	Clay_Sizing arrowButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(22), .height = CLAY_SIZING_FIXED(22)};
	Clay_Padding arrowButtonPadding = CLAY_PADDING_ALL(3);
	Clay_Color arrowButtonBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_CornerRadius arrowButtonCornerRadius = CLAY_CORNER_RADIUS(4);
	Clay_Color arrowButtonBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth arrowButtonBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Sizing arrowIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(14), .height = CLAY_SIZING_FIXED(14)};
	Clay_Color arrowIconTintColor = FlowUi::Flow_Color("#ffffffff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	enumPickerTriggerParams,
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, valueText),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, hasValueText),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, hoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, valueTextColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, valueTextWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, valueTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerTriggerParams, arrowIconTintColor));

using EnumPickerTriggerDef = FlowUi::ElementDefinition<
	enumPickerTriggerParams,
	void,
	void,
	FLOW_DEF_ID("EnumPickerTrigger")>;
using EnumPickerTriggerInteractionContext = EnumPickerTriggerDef::InteractionContext;

inline const EnumPickerTriggerDef kEnumPickerTrigger = {
	+[](EnumPickerTriggerInteractionContext& context) {
		context.params.backgroundColor = context.params.hoverBackgroundColor;
	},
	+[](EnumPickerTriggerInteractionContext& context) {
		if (context.params.onPressed != nullptr)
		{
			context.params.onPressed();
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](EnumPickerTriggerDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const std::string valueButtonPath = context.createChildElementId("value-button");
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string arrowButtonPath = context.createChildElementId("arrow-button");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = context.params.childGap;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_Color valueTextColor = context.params.hasValueText
			? context.params.valueTextColor
			: Clay_Color{
				.r = context.params.valueTextColor.r,
				.g = context.params.valueTextColor.g,
				.b = context.params.valueTextColor.b,
				.a = context.params.valueTextColor.a * 0.62f,
			};

		basicButtonParams valueButtonParams{};
		valueButtonParams.text = context.params.valueText;
		valueButtonParams.contentMode = basicButtonParams::ContentMode::TextOnly;
			valueButtonParams.padding = CLAY_PADDING_ALL(0);
			valueButtonParams.sizing = Clay_Sizing{
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIT(0),
			};
		valueButtonParams.backgroundColor = FlowUi::Flow_Color("#00000000");
		valueButtonParams.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
		valueButtonParams.cornerRadius = CLAY_CORNER_RADIUS(0);
		valueButtonParams.borderColor = FlowUi::Flow_Color("#00000000");
		valueButtonParams.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
		valueButtonParams.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		valueButtonParams.textWrapMode = context.params.valueTextWrapMode;
		valueButtonParams.textAlignment = context.params.valueTextAlignment;
		valueButtonParams.fontId = context.params.fontId;
		valueButtonParams.fontSize = context.params.fontSize;
		valueButtonParams.textColor = valueTextColor;

		basicButtonParams arrowButtonParams{};
		arrowButtonParams.contentMode = basicButtonParams::ContentMode::IconOnly;
		arrowButtonParams.icon = context.params.disclosureIcon;
		arrowButtonParams.padding = context.params.arrowButtonPadding;
		arrowButtonParams.sizing = context.params.arrowButtonSizing;
		arrowButtonParams.backgroundColor = context.params.arrowButtonBackgroundColor;
		arrowButtonParams.hoverBackgroundColor = context.params.arrowButtonBackgroundColor;
		arrowButtonParams.cornerRadius = context.params.arrowButtonCornerRadius;
		arrowButtonParams.borderColor = context.params.arrowButtonBorderColor;
		arrowButtonParams.borderWidth = context.params.arrowButtonBorderWidth;
			arrowButtonParams.iconContainerSizing = context.params.arrowIconContainerSizing;
			arrowButtonParams.iconTintColor = context.params.arrowIconTintColor;

			Clay_ElementDeclaration spacer{};
			spacer.layout.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_PERCENT(1.0f),
			};

			const FlowUi::ElementDrawOptions passiveDraw =
				FlowUi::ElementDrawOptions::SkipEventCallbacks |
				FlowUi::ElementDrawOptions::SkipLogicCallback;

		CLAY(rootId, root)
		{
				context.uiManager.createElement(kBasicButton, valueButtonPath)
					.setParameters(std::move(valueButtonParams))
					.draw(passiveDraw);

				CLAY(spacerId, spacer){};

				context.uiManager.createElement(kBasicButton, arrowButtonPath)
					.setParameters(std::move(arrowButtonParams))
					.draw(passiveDraw);
		};
	},
};

inline const EnumPickerMenuDef kEnumPickerMenu = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](EnumPickerMenuDef::InteractionContext& context) {
		(void)EnumPickerMenuDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	nullptr,
	+[](EnumPickerMenuDef::BuildContext& context) {
		enumPickerMenuState& state = EnumPickerMenuDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const uint64_t elementFlowId = FlowUi::toFlowId(context.elementID);
		const std::string currentValue = enumPickerMenuCurrentValue(context.params);
		const bool hasValueText = currentValue != context.params.emptyValueText;
		const std::string currentValueLabel = hasValueText
			? enumPickerMenuOptionLabel(currentValue)
			: currentValue;

		FlowUi::TextureRef disclosureIcon{};
		if (EnumPickerMenuDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? EnumPickerMenuDef::resources->downArrowIcon
				: EnumPickerMenuDef::resources->upArrowIcon;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const std::string triggerPath = context.createChildElementId("trigger");
		const Clay_ElementData previousRootData = Clay_GetElementData(rootId);
		const Clay_Dimensions viewportDimensions = Clay_GetLayoutDimensions();
		const bool openAbove = previousRootData.found &&
			viewportDimensions.height -
				(previousRootData.boundingBox.y + previousRootData.boundingBox.height) <
				enumPickerMenuEstimatedDropdownHeight(context.params) + context.params.dropdownGapPx;

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = context.params.childGap;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		enumPickerTriggerParams triggerParams{};
		triggerParams.valueText = currentValueLabel;
		triggerParams.hasValueText = hasValueText;
		triggerParams.disclosureIcon = disclosureIcon;
		triggerParams.onPressed = [elementId = context.elementID]() {
			enumPickerMenuToggleExpanded(elementId);
		};
		triggerParams.backgroundColor = FlowUi::Flow_Color("#00000000");
		triggerParams.hoverBackgroundColor = context.params.hoverBackgroundColor;
		triggerParams.childGap = context.params.childGap;
		triggerParams.fontId = context.params.fontId;
		triggerParams.fontSize = context.params.fontSize;
		triggerParams.valueTextColor = context.params.valueTextColor;
		triggerParams.valueTextWrapMode = context.params.valueTextWrapMode;
		triggerParams.valueTextAlignment = context.params.valueTextAlignment;
		triggerParams.arrowButtonSizing = context.params.arrowButtonSizing;
		triggerParams.arrowButtonPadding = context.params.arrowButtonPadding;
		triggerParams.arrowButtonBackgroundColor = context.params.arrowButtonBackgroundColor;
		triggerParams.arrowButtonCornerRadius = context.params.arrowButtonCornerRadius;
		triggerParams.arrowButtonBorderColor = context.params.arrowButtonBorderColor;
		triggerParams.arrowButtonBorderWidth = context.params.arrowButtonBorderWidth;
		triggerParams.arrowIconContainerSizing = context.params.arrowIconContainerSizing;
		triggerParams.arrowIconTintColor = context.params.arrowIconTintColor;

		CLAY(rootId, root)
		{
			context.uiManager.createElement(kEnumPickerTrigger, triggerPath)
				.setParameters(std::move(triggerParams))
				.draw();

			if (state.isExpanded)
			{
				Clay_ElementDeclaration dismissLayer{};
				const Clay_ElementId dismissLayerId = context.uiManager.toClayEID(context.createChildElementId("dismiss-layer"));
				dismissLayer.layout.sizing = {
					.width = CLAY_SIZING_GROW(0),
					.height = CLAY_SIZING_GROW(0),
				};
				dismissLayer.backgroundColor = FlowUi::Flow_Color("#00000000");
				dismissLayer.floating = {
					.zIndex = context.params.outsideDismissZIndex,
					.attachPoints = {
						.element = CLAY_ATTACH_POINT_LEFT_TOP,
						.parent = CLAY_ATTACH_POINT_LEFT_TOP,
					},
					.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
					.attachTo = CLAY_ATTACH_TO_ROOT,
				};

				CLAY(dismissLayerId, dismissLayer)
				{
					context.uiManager.createElement(kBasicButton, context.createChildElementId("dismiss-layer/button"))
						.setParameters(enumPickerTransparentButtonParams(
							Clay_Sizing{
								.width = CLAY_SIZING_GROW(0),
								.height = CLAY_SIZING_GROW(0),
							},
							CLAY_PADDING_ALL(0),
							FlowUi::Flow_Color("#00000000"),
							[elementFlowId](BasicButtonInteractionContext) {
								enumPickerMenuState* latestState = EnumPickerMenuDef::tryGetState(elementFlowId);
								if (latestState != nullptr)
								{
									latestState->isExpanded = false;
								}
							}))
						.draw();
				};

				Clay_ElementDeclaration dropdown{};
				const Clay_ElementId dropdownId = context.uiManager.toClayEID(context.createChildElementId("dropdown"));
				dropdown.layout.sizing = context.params.dropdownSizing;
				dropdown.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
				dropdown.backgroundColor = context.params.dropdownBackgroundColor;
				dropdown.cornerRadius = context.params.dropdownCornerRadius;
				dropdown.border = {
					.color = context.params.dropdownBorderColor,
					.width = context.params.dropdownBorderWidth,
				};
				dropdown.floating = {
					.offset = {
						.x = 0.0f,
						.y = openAbove
							? -static_cast<float>(context.params.dropdownGapPx)
							: static_cast<float>(context.params.dropdownGapPx),
					},
					.zIndex = context.params.dropdownZIndex,
					.attachPoints = {
						.element = openAbove ? CLAY_ATTACH_POINT_RIGHT_BOTTOM : CLAY_ATTACH_POINT_RIGHT_TOP,
						.parent = openAbove ? CLAY_ATTACH_POINT_RIGHT_TOP : CLAY_ATTACH_POINT_RIGHT_BOTTOM,
					},
					.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
					.attachTo = CLAY_ATTACH_TO_PARENT,
				};

				CLAY(dropdownId, dropdown)
				{
					if (context.params.options.empty())
					{
						Clay_TextElementConfig emptyTextConfig{};
						emptyTextConfig.textColor = context.params.optionTextColor;
						emptyTextConfig.fontSize = context.params.fontSize;
						emptyTextConfig.wrapMode = CLAY_TEXT_WRAP_WORDS;
						emptyTextConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
						emptyTextConfig.fontId = context.params.fontId;

						CLAY(context.uiManager.toClayEID(context.createChildElementId("dropdown/empty")), {
							.layout = {
								.sizing = {
									.width = CLAY_SIZING_GROW(0),
									.height = CLAY_SIZING_FIT(0),
								},
								.padding = context.params.optionPadding,
							},
						})
						{
							CLAY_TEXT(
								context.uiManager.toClayString(context.params.emptyOptionsText),
								CLAY_TEXT_CONFIG(emptyTextConfig)
							);
						};
					}
					else
					{
						for (std::size_t i = 0; i < context.params.options.size(); ++i)
						{
							const std::string option = context.params.options[i];
							const std::string optionLabel = enumPickerMenuOptionLabel(option);
							context.uiManager.createElement(
								kBasicButton,
								context.createChildElementId("dropdown/option-" + std::to_string(i)))
								.setParameters(basicButtonParams{
									.text = optionLabel,
									.onHoveredCallback = [hoverColor = context.params.optionHoverBackgroundColor](
										BasicButtonInteractionContext buttonContext) {
										buttonContext.params.backgroundColor = hoverColor;
									},
									.onPressedCallback = [
										elementFlowId,
										onChange = context.params.onChange,
										currentValue,
										option
									](BasicButtonInteractionContext) {
										enumPickerMenuState* latestState = EnumPickerMenuDef::tryGetState(elementFlowId);
										if (latestState != nullptr)
										{
											latestState->isExpanded = false;
										}
										if (onChange != nullptr && option != currentValue)
										{
											onChange(option);
										}
									},
									.contentMode = basicButtonParams::ContentMode::TextOnly,
									.padding = context.params.optionPadding,
									.sizing = {
										.width = CLAY_SIZING_GROW(0),
										.height = CLAY_SIZING_FIT(0),
									},
									.backgroundColor = FlowUi::Flow_Color("#00000000"),
									.hoverBackgroundColor = context.params.optionHoverBackgroundColor,
									.cornerRadius = CLAY_CORNER_RADIUS(0),
									.borderColor = FlowUi::Flow_Color("#00000000"),
									.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0},
									.childAlignment = {
										.x = CLAY_ALIGN_X_LEFT,
										.y = CLAY_ALIGN_Y_CENTER,
									},
									.textWrapMode = CLAY_TEXT_WRAP_WORDS,
									.textAlignment = CLAY_TEXT_ALIGN_LEFT,
									.fontId = context.params.fontId,
									.fontSize = context.params.fontSize,
									.textColor = context.params.optionTextColor,
								})
								.draw();

							if (i + 1 < context.params.options.size())
							{
								Clay_ElementDeclaration separator{};
								separator.layout.sizing = {
									.width = CLAY_SIZING_GROW(0),
									.height = CLAY_SIZING_FIXED(1),
								};
								separator.backgroundColor = context.params.separatorColor;
								CLAY(context.uiManager.toClayEID(
									context.createChildElementId("dropdown/separator-" + std::to_string(i))),
									separator){};
							}
						}
					}
				};
			}
		};
	},
};

struct enumPickerCardParams {
	std::string hintText = "Option";
	std::span<const std::string> options{};
	std::string value = "";
	std::string defaultValue = "";
	std::function<void(std::string_view)> onChange = nullptr;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};
	uint16_t cardChildGap = 0;

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	enumPickerMenuParams menu{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	enumPickerCardParams,
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, defaultValue),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(enumPickerCardParams, menu));

using EnumPickerCardDef = FlowUi::ElementDefinition<
	enumPickerCardParams,
	void,
	void,
	FLOW_DEF_ID("EnumPickerCard")>;

inline const EnumPickerCardDef kEnumPickerCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](EnumPickerCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string menuPath = context.createChildElementId("menu");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = context.params.cardLayout;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = context.params.cardChildGap;
		root.backgroundColor = FlowUi::Flow_Color("#00000000");
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		enumPickerMenuParams menuParams = context.params.menu;
		menuParams.options = context.params.options;
		menuParams.value = context.params.value;
		menuParams.defaultValue = context.params.defaultValue;
		menuParams.onChange = context.params.onChange;
		menuParams.fontId = context.params.fontId;

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

			context.uiManager.createElement(kEnumPickerMenu, menuPath)
				.setParameters(std::move(menuParams))
				.draw();
		};
	},
};
