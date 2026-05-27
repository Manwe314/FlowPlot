#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"

struct templateLayerState;

struct templateLayerParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0, 10000), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color hoverBackgroundColor = FlowUi::Flow_Color("#242428ff");
	Clay_Color focusedBackgroundColor = FlowUi::Flow_Color("#1f3a4cff");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	Clay_Color indicatorColor = FlowUi::Flow_Color("#4ea9ffff");
	bool focused = false;
	FlowPlotGui::state* guiState = nullptr;
	FlowPlotGui::TemplateNodeKey nodeKey{};
	std::function<void()> onPressedCallback = nullptr;

	int depth = 0;
	int pixelsPerDepth = 18;

	int mainContentMinWidth = 180;
	int mainContentMaxWidth = 420;
	Clay_Padding mainContentPadding = CLAY_PADDING_ALL(0);
	int mainContentChildGap = 8;
	Clay_ChildAlignment mainContentChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	Clay_Color mainContentBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color mainContentBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth mainContentBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	bool showExpanderButton = false;
	bool showAdderButton = false;
	bool showDeleterButton = false;

	Clay_Sizing buttonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)};
	Clay_Padding buttonPadding = CLAY_PADDING_ALL(0);
	Clay_Color buttonBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color buttonHoverBackgroundColor = Clay_Color{.r = 46.0f, .g = 48.0f, .b = 54.0f, .a = 255.0f};
	Clay_CornerRadius buttonCornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_Color buttonBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth buttonBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Sizing buttonIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Color buttonIconTintColor = FlowUi::Flow_Color("#ffffffff");
	Clay_Color adderButtonIconTintColor = FlowUi::Flow_Color("#00908fff");
	Clay_Color adderButtonHoverIconTintColor = FlowUi::Flow_Color("#00c9c7ff");
	Clay_Color deleterButtonIconTintColor = Clay_Color{.r = 175.0f, .g = 180.0f, .b = 179.0f, .a = 255.0f};
	Clay_Color deleterButtonHoverIconTintColor = Clay_Color{.r = 255.0f, .g = 107.0f, .b = 107.0f, .a = 255.0f};
	FlowUi::TextureRef expandedIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef collapsedIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef adderIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef deleterIcon = FlowUi::TextureRef{};
	std::function<void()> onAddPressedCallback = nullptr;
	std::function<void()> onDeletePressedCallback = nullptr;

	FlowUi::TextureRef mainIcon = FlowUi::TextureRef{};
	Clay_Sizing mainIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Sizing mainIconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color mainIconTintColor = FlowUi::Flow_Color("#ffffffff");

	std::string mainText = "Template Layer";
	Clay_TextElementConfigWrapMode mainTextWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment mainTextAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t mainTextFontId = 0;
	uint16_t mainTextFontSize = 16;
	Clay_Color mainTextColor = FlowUi::Flow_Color("#000000ff");
};

struct templateLayerState {
	bool isExpanded = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	templateLayerParams,
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, hoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, focusedBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, childAlignment),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, indicatorColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, focused),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, depth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, pixelsPerDepth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentMinWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentMaxWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentPadding),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentChildGap),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentChildAlignment),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, showExpanderButton),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, showAdderButton),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, showDeleterButton),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonSizing),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonPadding),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, buttonIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, adderButtonIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, adderButtonHoverIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, deleterButtonIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, deleterButtonHoverIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainIconSizing),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainText),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextFontId),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextFontSize),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextColor));

FLOWUI_DEV_REGISTER_STRUCT(
	templateLayerState,
	FLOWUI_DEV_REFLECT_FIELD(templateLayerState, isExpanded));

enum class templateLayerButtonType : uint8_t {
	Expander,
	Adder,
	Deleter,
};

using TemplateLayerDef = FlowUi::ElementDefinition<
	templateLayerParams,
	templateLayerState,
	void,
	FLOW_DEF_ID("TemplateLayer")>;

inline void drawTemplateLayerButton(
	FlowUi::UiManager& uiManager,
	std::string_view path,
	templateLayerButtonType type,
	const templateLayerParams& layerParams,
	std::string_view layerElementId)
{
	const uint64_t layerFlowId = FlowUi::toFlowId(layerElementId);
	const templateLayerState* layerState = TemplateLayerDef::tryGetStateConst(layerFlowId);
	const bool isExpanded = layerState != nullptr && layerState->isExpanded;

	basicButtonParams params{};
	params.text = "";
	params.contentMode = basicButtonParams::ContentMode::IconOnly;
	params.sizing = layerParams.buttonSizing;
	params.padding = layerParams.buttonPadding;
	params.backgroundColor = layerParams.buttonBackgroundColor;
	params.hoverBackgroundColor = layerParams.buttonHoverBackgroundColor;
	params.cornerRadius = layerParams.buttonCornerRadius;
	params.borderColor = layerParams.buttonBorderColor;
	params.borderWidth = layerParams.buttonBorderWidth;
	params.iconContainerSizing = layerParams.buttonIconContainerSizing;
	params.iconTintColor = layerParams.buttonIconTintColor;

	switch (type)
	{
	case templateLayerButtonType::Expander:
		params.icon = isExpanded ? layerParams.expandedIcon : layerParams.collapsedIcon;
		params.onPressedCallback = [layerFlowId](BasicButtonInteractionContext) {
			templateLayerState& state = TemplateLayerDef::getOrCreateState(layerFlowId);
			state.isExpanded = !state.isExpanded;
		};
		break;
	case templateLayerButtonType::Adder:
		params.icon = layerParams.adderIcon;
		params.iconTintColor = layerParams.adderButtonIconTintColor;
		params.onHoveredCallback = [hoverColor = layerParams.adderButtonHoverIconTintColor](BasicButtonInteractionContext buttonContext) {
			buttonContext.params.iconTintColor = hoverColor;
		};
		break;
	case templateLayerButtonType::Deleter:
		params.icon = layerParams.deleterIcon;
		params.iconTintColor = layerParams.deleterButtonIconTintColor;
		params.onHoveredCallback = [hoverColor = layerParams.deleterButtonHoverIconTintColor](BasicButtonInteractionContext buttonContext) {
			buttonContext.params.iconTintColor = hoverColor;
		};
		break;
	}

	if (type == templateLayerButtonType::Adder)
	{
		params.onPressedCallback = [callback = layerParams.onAddPressedCallback, layerElementId = std::string(layerElementId)](BasicButtonInteractionContext buttonContext) {
			(void)buttonContext;
			(void)layerElementId;
			if (callback != nullptr)
			{
				callback();
			}
		};
	}
	else if (type == templateLayerButtonType::Deleter)
	{
		params.onPressedCallback = [callback = layerParams.onDeletePressedCallback, layerElementId = std::string(layerElementId)](BasicButtonInteractionContext buttonContext) {
			(void)buttonContext;
			(void)layerElementId;
			if (callback != nullptr)
			{
				callback();
			}
		};
	}

	uiManager.createElement(kBasicButton, path)
		.setParameters(std::move(params))
		/* V1 cant Update parameters made with variables */
		.mergeParams([](auto& params) {
		    params.iconContainerSizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 18.0f, .max = 18.0f}}, .type = CLAY__SIZING_TYPE_FIXED}, .height = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 18.0f, .max = 18.0f}}, .type = CLAY__SIZING_TYPE_FIXED}};
		    params.padding = Clay_Padding{.left = 3, .right = 3, .top = 3, .bottom = 3};
		    params.sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 0.0f, .max = 0.0f}}, .type = CLAY__SIZING_TYPE_FIT}, .height = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 0.0f, .max = 0.0f}}, .type = CLAY__SIZING_TYPE_FIT}};
		})
		.draw();
}

inline const TemplateLayerDef kTemplateLayer = {
	+[](TemplateLayerDef::InteractionContext& context) {
		context.params.backgroundColor = context.params.hoverBackgroundColor;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		if (context.params.guiState != nullptr)
		{
			FlowPlotGui::TemplateNodeKey key = context.params.nodeKey;
			key.flowElementId = context.elementID;
			context.params.guiState->selectedNode = std::move(key);
		}
		if (context.params.onPressedCallback != nullptr)
		{
			context.params.onPressedCallback();
		}
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)TemplateLayerDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	+[](TemplateLayerDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](TemplateLayerDef::BuildContext& context) {
		(void)TemplateLayerDef::getOrCreateState(FlowUi::toFlowId(context.elementID));

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId indicatorId = context.uiManager.toClayEID(context.createChildElementId("indicator"));
		const Clay_ElementId leftSpacerId = context.uiManager.toClayEID(context.createChildElementId("left-spacer"));
		const Clay_ElementId mainContentId = context.uiManager.toClayEID(context.createChildElementId("main-content"));
		const Clay_ElementId mainLeadingButtonSlotId = context.uiManager.toClayEID(context.createChildElementId("main-content/leading-button-slot"));
		const Clay_ElementId mainIconContainerId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon-container"));
		const Clay_ElementId mainIconId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon"));
		const Clay_ElementId mainTextId = context.uiManager.toClayEID(context.createChildElementId("main-content/text"));
		const Clay_ElementId rightSpacerId = context.uiManager.toClayEID(context.createChildElementId("right-spacer"));

		int mainContentMinWidth = context.params.mainContentMinWidth;
		int mainContentMaxWidth = context.params.mainContentMaxWidth;
		if (mainContentMaxWidth < mainContentMinWidth)
		{
			mainContentMaxWidth = mainContentMinWidth;
		}

		const bool hasMainIcon = context.params.mainIcon.id != 0;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = 0;

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {
			.color = context.params.borderColor,
			.width = context.params.borderWidth,
		};

		Clay_LayoutConfig indicatorLayout{};
		indicatorLayout.sizing = {
			.width = CLAY_SIZING_FIXED(2),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration indicator{};
		indicator.layout = indicatorLayout;
		indicator.backgroundColor = context.params.indicatorColor;
		indicator.cornerRadius = CLAY_CORNER_RADIUS(0);
		const bool showIndicator =
			context.params.focused ||
			(context.params.guiState != nullptr &&
			 context.params.guiState->selectedNode.has_value() &&
			 (context.params.guiState->selectedNode->flowElementId == context.elementID ||
			  FlowPlotGui::sameTemplateNodeKey(*context.params.guiState->selectedNode, context.params.nodeKey)));
		const bool rootHovered = context.uiManager.getPreviousFramesInteraction().isHovered(rootId);
		const bool showNodeActionButtons = showIndicator || rootHovered;
		if (showIndicator)
		{
			root.backgroundColor = context.params.focusedBackgroundColor;
		}

		Clay_LayoutConfig leftSpacerLayout{};
		const int normalizedDepth = context.params.depth < 0 ? 0 : context.params.depth;
		const int normalizedPixelsPerDepth = context.params.pixelsPerDepth < 0 ? 0 : context.params.pixelsPerDepth;
		const float leftSpacerWidth = static_cast<float>(normalizedPixelsPerDepth * (normalizedDepth + 1));
		leftSpacerLayout.sizing = {
			.width = CLAY_SIZING_FIXED(leftSpacerWidth),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration leftSpacer{};
		leftSpacer.layout = leftSpacerLayout;

		Clay_LayoutConfig mainContentLayout{};
		mainContentLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainContentLayout.sizing = {
			.width = CLAY_SIZING_FIT(static_cast<float>(mainContentMinWidth), static_cast<float>(mainContentMaxWidth)),
			.height = CLAY_SIZING_FIT(0),
		};
		mainContentLayout.padding = context.params.mainContentPadding;
		mainContentLayout.childAlignment = context.params.mainContentChildAlignment;
		mainContentLayout.childGap = static_cast<uint16_t>(context.params.mainContentChildGap);

		Clay_ElementDeclaration mainContent{};
		mainContent.layout = mainContentLayout;
		mainContent.backgroundColor = context.params.mainContentBackgroundColor;
		mainContent.cornerRadius = CLAY_CORNER_RADIUS(0);
		mainContent.border = {
			.color = context.params.mainContentBorderColor,
			.width = context.params.mainContentBorderWidth,
		};

		Clay_LayoutConfig mainLeadingButtonSlotLayout{};
		mainLeadingButtonSlotLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainLeadingButtonSlotLayout.sizing = context.params.buttonSizing;
		mainLeadingButtonSlotLayout.padding = CLAY_PADDING_ALL(0);
		mainLeadingButtonSlotLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainLeadingButtonSlotLayout.childGap = 0;

		Clay_ElementDeclaration mainLeadingButtonSlot{};
		mainLeadingButtonSlot.layout = mainLeadingButtonSlotLayout;
		mainLeadingButtonSlot.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainLeadingButtonSlot.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig mainIconContainerLayout{};
		mainIconContainerLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainIconContainerLayout.sizing = context.params.mainIconContainerSizing;
		mainIconContainerLayout.padding = CLAY_PADDING_ALL(0);
		mainIconContainerLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainIconContainerLayout.childGap = 0;

		Clay_ElementDeclaration mainIconContainer{};
		mainIconContainer.layout = mainIconContainerLayout;
		mainIconContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainIconContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig mainIconLayout{};
		mainIconLayout.sizing = context.params.mainIconSizing;

		Clay_ElementDeclaration mainIcon{};
		mainIcon.layout = mainIconLayout;
		if (context.params.mainIconTintColor.a > 0.0f)
		{
			context.params.mainIcon.tintEnabled = true;
		}
		mainIcon.backgroundColor = context.params.mainIconTintColor;
		mainIcon.image = {
			.imageData = context.uiManager.storeTexture(context.params.mainIcon),
		};

		Clay_TextElementConfig mainTextConfig{};
		mainTextConfig.textColor = context.params.mainTextColor;
		mainTextConfig.fontSize = context.params.mainTextFontSize;
		mainTextConfig.wrapMode = context.params.mainTextWrapMode;
		mainTextConfig.textAlignment = context.params.mainTextAlignment;
		mainTextConfig.fontId = context.params.mainTextFontId;

		Clay_LayoutConfig rightSpacerLayout{};
		rightSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(0, 100000),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration rightSpacer{};
		rightSpacer.layout = rightSpacerLayout;

		const std::string expanderPath = context.createChildElementId("main-content/expander");
		const std::string adderPath = context.createChildElementId("adder");
		const std::string deleterPath = context.createChildElementId("deleter");

		CLAY(rootId, root){
			if (showIndicator)
			{
				CLAY(indicatorId, indicator){};
			}
			CLAY(leftSpacerId, leftSpacer){};

			CLAY(mainContentId, mainContent){
				CLAY(mainLeadingButtonSlotId, mainLeadingButtonSlot){
					if (context.params.showExpanderButton)
					{
						drawTemplateLayerButton(
							context.uiManager,
							expanderPath,
							templateLayerButtonType::Expander,
							context.params,
							context.elementID);
					}
				};

				CLAY(mainIconContainerId, mainIconContainer){
					if (hasMainIcon)
					{
						CLAY(mainIconId, mainIcon){};
					}
				};

				CLAY(mainTextId, {}){
					CLAY_TEXT(
						context.uiManager.toClayString(context.params.mainText),
						CLAY_TEXT_CONFIG(mainTextConfig)
					);
				};
			};

			CLAY(rightSpacerId, rightSpacer){};

			if (context.params.showAdderButton && showNodeActionButtons)
			{
				drawTemplateLayerButton(
					context.uiManager,
					adderPath,
					templateLayerButtonType::Adder,
					context.params,
					context.elementID);
			}

			if (context.params.showDeleterButton && showNodeActionButtons)
			{
				drawTemplateLayerButton(
					context.uiManager,
					deleterPath,
					templateLayerButtonType::Deleter,
					context.params,
					context.elementID);
			}
		};
	},
};
