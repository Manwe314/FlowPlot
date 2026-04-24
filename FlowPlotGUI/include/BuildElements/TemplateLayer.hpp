#pragma once

#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"

struct templateLayerParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0, 10000), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	Clay_Color indicatorColor = FlowUi::Flow_Color("#4ea9ffff");
	bool focused = false;

	int leftSpacerMinWidth = 0;
	int leftSpacerMaxWidth = 120;

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

	FlowUi::TextureRef mainIcon = FlowUi::TextureRef{};
	Clay_Sizing mainIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Sizing mainIconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color mainIconTintColor = FlowUi::Flow_Color("#00000000");

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
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, childAlignment),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, indicatorColor),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, focused),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, leftSpacerMinWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, leftSpacerMaxWidth),
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

using BasicButtonBuilder = FlowUi::ElementBuilder<basicButtonParams, void, void, FLOW_DEF_ID("Basic button")>;

struct templateLayerResources {
	BasicButtonBuilder expanderBuilder;
	BasicButtonBuilder adderBuilder;

	explicit templateLayerResources(FlowUi::UiManager& uiManager) :
		expanderBuilder(makeBuilder(uiManager, "TemplateLayer/shared/expander")),
		adderBuilder(makeBuilder(uiManager, "TemplateLayer/shared/adder")) {}

private:
	static BasicButtonBuilder makeBuilder(FlowUi::UiManager& uiManager, std::string_view path)
	{
		basicButtonParams params{};
		params.text = "";
		params.contentMode = basicButtonParams::ContentMode::IconOnly;
		params.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)};
		params.padding = CLAY_PADDING_ALL(0);
		params.backgroundColor = FlowUi::Flow_Color("#00000000");
		params.borderColor = FlowUi::Flow_Color("#00000000");
		params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

		BasicButtonBuilder builder = uiManager.createElement(kBasicButton, path);
		builder.setParameters(std::move(params));
		return builder;
	}
};

using TemplateLayerDef = FlowUi::ElementDefinition<
	templateLayerParams,
	templateLayerState,
	templateLayerResources,
	FLOW_DEF_ID("TemplateLayer")>;

inline const TemplateLayerDef kTemplateLayer = {
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
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
		if (!TemplateLayerDef::resources.has_value())
		{
			TemplateLayerDef::resources.emplace(context.uiManager);
		}
		templateLayerResources& resources = *TemplateLayerDef::resources;

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId indicatorId = context.uiManager.toClayEID(context.createChildElementId("indicator"));
		const Clay_ElementId leftSpacerId = context.uiManager.toClayEID(context.createChildElementId("left-spacer"));
		const Clay_ElementId mainContentId = context.uiManager.toClayEID(context.createChildElementId("main-content"));
		const Clay_ElementId mainLeadingButtonSlotId = context.uiManager.toClayEID(context.createChildElementId("main-content/leading-button-slot"));
		const Clay_ElementId mainIconContainerId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon-container"));
		const Clay_ElementId mainIconId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon"));
		const Clay_ElementId mainTextId = context.uiManager.toClayEID(context.createChildElementId("main-content/text"));
		const Clay_ElementId rightSpacerId = context.uiManager.toClayEID(context.createChildElementId("right-spacer"));

		int leftSpacerMinWidth = context.params.leftSpacerMinWidth;
		int leftSpacerMaxWidth = context.params.leftSpacerMaxWidth;
		if (leftSpacerMaxWidth < leftSpacerMinWidth)
		{
			leftSpacerMaxWidth = leftSpacerMinWidth;
		}

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

		Clay_LayoutConfig leftSpacerLayout{};
		leftSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(leftSpacerMinWidth), static_cast<float>(leftSpacerMaxWidth)),
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
		mainLeadingButtonSlotLayout.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)};
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

		CLAY(rootId, root){
			if (context.params.focused)
			{
				CLAY(indicatorId, indicator){};
			}
			CLAY(leftSpacerId, leftSpacer){};

			CLAY(mainContentId, mainContent){
				CLAY(mainLeadingButtonSlotId, mainLeadingButtonSlot){
					if (context.params.showExpanderButton)
					{
						resources.expanderBuilder
							.withElementID(expanderPath)
							.draw();
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

			if (context.params.showAdderButton)
			{
				resources.adderBuilder
					.withElementID(adderPath)
					.draw();
			}
		};
	},
};
