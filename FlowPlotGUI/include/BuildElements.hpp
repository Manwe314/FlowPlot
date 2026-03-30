#pragma once

#include <functional>

#include <FlowUi/Flow.hpp>
#include "FlowPlotGui.hpp"


struct basicInputFieldParams {
	std::string defaultText = "...";
	Clay_Padding padding = CLAY_PADDING_ALL(10);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(30, 90), .height = CLAY_SIZING_FIT(0)};
	Clay_Color borderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#cfcfcfff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_ChildAlignment childTextAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#000000ff");
};

using BasicInputFieldDef = FlowUi::ElementDefinition<basicInputFieldParams, void, void, FLOW_DEF_ID("Basic input field")>;

inline const BasicInputFieldDef kBasicInputField = {
	+[](BasicInputFieldDef::InteractionContext& context) {
		(void)context;
	},
	+[](BasicInputFieldDef::InteractionContext& context) {
		context.uiManager.inputFields().requestCaret(
			context.elementID,
			FlowUi::InputFieldManager::CaretRequestKind::SetPrimary);
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicInputFieldDef::BuildContext& context) {
		const Clay_ElementId contentId = context.uiManager.toClayEID(context.elementID);
		const std::string textElementPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textElementPath);

		auto result = context.uiManager.inputFields().requestField({
			.fieldId = context.elementID,
			.initialText = context.params.defaultText,
			.config = FlowUi::InputFieldManager::FieldConfig{
				.readOnly = false,
				.allowNewline = false,
				.allowArrowNavigation = true,
			},
			.textElementId = textId,
			.contentElementId = contentId,
		});
		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childTextAlignment;

		Clay_ElementDeclaration root{};
		root.id = contentId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.clip = {.horizontal = true, .vertical = true};
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

		CLAY(root){
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(result.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};
	},
};

struct basicButtonParams;
using BasicButtonDef = FlowUi::ElementDefinition<basicButtonParams, void, void, FLOW_DEF_ID("Basic button")>;
using BasicButtonInteractionContext = BasicButtonDef::InteractionContext;

struct basicButtonParams {
	enum class ContentMode : uint8_t {
		None,
		TextOnly,
		IconOnly,
		IconThenText,
		TextThenIcon,
	};

	std::string text = "";
	FlowUi::TextureRef icon = FlowUi::TextureRef{};
	std::function<void(BasicButtonInteractionContext)> onHoveredCallback = nullptr;
	std::function<void(BasicButtonInteractionContext)> onPressedCallback = nullptr;
	ContentMode contentMode = ContentMode::None;

	Clay_Padding padding = CLAY_PADDING_ALL(10);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#cfcfcfff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_Color borderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_LayoutDirection childLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t childGap = 8;

	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#000000ff");

	Clay_Sizing iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Padding iconContainerPadding = CLAY_PADDING_ALL(0);
	Clay_LayoutDirection iconContainerChildLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment iconContainerChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t iconContainerChildGap = 0;
	Clay_Color iconContainerBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color iconContainerBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth iconContainerBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing iconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color iconTintColor = FlowUi::Flow_Color("#00000000");
};

inline const BasicButtonDef kBasicButton = {
	+[](BasicButtonDef::InteractionContext& context) {
		if (context.params.onHoveredCallback != nullptr)
		{
			context.params.onHoveredCallback(context);
		}
	},
	+[](BasicButtonDef::InteractionContext& context) {
		if (context.params.onPressedCallback != nullptr)
		{
			context.params.onPressedCallback(context);
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicButtonDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);

		const basicButtonParams::ContentMode contentMode = context.params.contentMode;
		const bool needsText =
			contentMode == basicButtonParams::ContentMode::TextOnly ||
			contentMode == basicButtonParams::ContentMode::IconThenText ||
			contentMode == basicButtonParams::ContentMode::TextThenIcon;
		const bool needsIcon =
			contentMode == basicButtonParams::ContentMode::IconOnly ||
			contentMode == basicButtonParams::ContentMode::IconThenText ||
			contentMode == basicButtonParams::ContentMode::TextThenIcon;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = context.params.childLayoutDirection;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = context.params.childGap;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		Clay_ElementId textId{};
		if (needsText)
		{
			const std::string textPath = context.createChildElementId("text");
			textId = context.uiManager.toClayEID(textPath);
			textConfig.textColor = context.params.textColor;
			textConfig.fontSize = context.params.fontSize;
			textConfig.wrapMode = context.params.textWrapMode;
			textConfig.textAlignment = context.params.textAlignment;
			textConfig.fontId = context.params.fontId;
		}

		Clay_ElementDeclaration iconContainer{};
		Clay_ElementDeclaration iconElement{};
		if (needsIcon)
		{
			const std::string iconContainerPath = context.createChildElementId("icon-container");
			const std::string iconPath = context.createChildElementId("icon");
			const Clay_ElementId iconContainerId = context.uiManager.toClayEID(iconContainerPath);
			const Clay_ElementId iconId = context.uiManager.toClayEID(iconPath);

			Clay_LayoutConfig iconContainerLayout{};
			iconContainerLayout.layoutDirection = context.params.iconContainerChildLayoutDirection;
			iconContainerLayout.sizing = context.params.iconContainerSizing;
			iconContainerLayout.padding = context.params.iconContainerPadding;
			iconContainerLayout.childAlignment = context.params.iconContainerChildAlignment;
			iconContainerLayout.childGap = context.params.iconContainerChildGap;

			iconContainer.id = iconContainerId;
			iconContainer.layout = iconContainerLayout;
			iconContainer.backgroundColor = context.params.iconContainerBackgroundColor;
			iconContainer.border = {.color = context.params.iconContainerBorderColor, .width = context.params.iconContainerBorderWidth};

			Clay_LayoutConfig iconLayout{};
			iconLayout.sizing = context.params.iconSizing;

			iconElement.id = iconId;
			iconElement.layout = iconLayout;
			iconElement.backgroundColor = context.params.iconTintColor;
			iconElement.image = {
				.imageData = context.uiManager.storeTexture(context.params.icon),
			};
		}

		auto drawTextChild = [&]() {
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};

		auto drawIconChild = [&]() {
			CLAY(iconContainer){
				CLAY(iconElement){};
			};
		};

		CLAY(root){
			switch (contentMode)
			{
			case basicButtonParams::ContentMode::None:
				break;
			case basicButtonParams::ContentMode::TextOnly:
				drawTextChild();
				break;
			case basicButtonParams::ContentMode::IconOnly:
				drawIconChild();
				break;
			case basicButtonParams::ContentMode::IconThenText:
				drawIconChild();
				drawTextChild();
				break;
			case basicButtonParams::ContentMode::TextThenIcon:
				drawTextChild();
				drawIconChild();
				break;
			}
		};
	},
};

inline const FlowUi::ElementDefinition kTemplateLayer = {
	// elementTypeName: unique element type key (used for registration/lookups).
	"TemplateLayer",

	// initializeDefaultParameters: optional defaults merged with per-instance overrides.
	[](FlowUi::ElementParameters& defaults) {
		defaults.setValue("sizing", Clay_Sizing{.width = CLAY_SIZING_GROW(0, 10000), .height = CLAY_SIZING_FIT(0)});
		defaults.setValue("background color", FlowUi::Flow_Color("#00000000"));
		defaults.setValue("border color", FlowUi::Flow_Color("#00000000"));
		defaults.setValue("border width", Clay_BorderWidth{0,0,0,0,0});
		defaults.setValue("child alignment", Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER});

		defaults.setValue("indicator color", FlowUi::Flow_Color("#4ea9ffff"));

		defaults.setValue("left spacer min width", 0);
		defaults.setValue("left spacer max width", 120);

		defaults.setValue("main content min width", 180);
		defaults.setValue("main content max width", 420);
		defaults.setValue("main content padding", CLAY_PADDING_ALL(0));
		defaults.setValue("main content child gap", 8);
		defaults.setValue("main content child alignment", Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER});
		defaults.setValue("main content background color", FlowUi::Flow_Color("#00000000"));
		defaults.setValue("main content border color", FlowUi::Flow_Color("#00000000"));
		defaults.setValue("main content border width", Clay_BorderWidth{0,0,0,0,0});

		defaults.setValue("show main leading button", false);
		defaults.setValue("main leading button icon", FlowUi::TextureRef{});
		defaults.setValue("main leading button slot sizing", Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)});
		defaults.setValue("main leading button sizing", Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)});

		defaults.setValue("main icon", FlowUi::TextureRef{});
		defaults.setValue("main icon container sizing", Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)});
		defaults.setValue("main icon sizing", Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)});
		defaults.setValue("main icon tint color", FlowUi::Flow_Color("#00000000"));

		defaults.setValue("main text", std::string("Template Layer"));
		defaults.setValue("main text wrap mode", CLAY_TEXT_WRAP_NONE);
		defaults.setValue("main text alignment", CLAY_TEXT_ALIGN_LEFT);
		defaults.setValue("main text font id", 0);
		defaults.setValue("main text font size", 16);
		defaults.setValue("main text color", FlowUi::Flow_Color("#000000ff"));

		defaults.setValue("show trailing button", false);
		defaults.setValue("trailing button icon", FlowUi::TextureRef{});
	},

	// onHovered: optional callback when this element was hovered in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onPressed: optional callback when this element was pressed in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onHeld: optional callback when this element was held in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onReleased: optional callback when this element was released in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// runLogic: optional per-frame logic callback before buildElement executes.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	[](FlowUi::ElementBuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},

	// buildElement: required callback where the element's Clay UI is built.
	[](FlowUi::ElementBuildContext& context) {
		const Clay_ElementId rootId = context.elementId;
		const Clay_ElementId indicatorId = context.createChildElementId("indicator");
		const Clay_ElementId leftSpacerId = context.createChildElementId("left-spacer");
		const Clay_ElementId mainContentId = context.createChildElementId("main-content");
		const Clay_ElementId mainLeadingButtonSlotId = context.createChildElementId("main-content/leading-button-slot");
		const Clay_ElementId mainIconContainerId = context.createChildElementId("main-content/icon-container");
		const Clay_ElementId mainIconId = context.createChildElementId("main-content/icon");
		const Clay_ElementId mainTextId = context.createChildElementId("main-content/text");
		const Clay_ElementId rightSpacerId = context.createChildElementId("right-spacer");

		int leftSpacerMinWidth = context.parameters.getValue<int>("left spacer min width", 0);
		int leftSpacerMaxWidth = context.parameters.getValue<int>("left spacer max width", leftSpacerMinWidth);
		if (leftSpacerMaxWidth < leftSpacerMinWidth)
		{
			leftSpacerMaxWidth = leftSpacerMinWidth;
		}

		int mainContentMinWidth = context.parameters.getValue<int>("main content min width", 0);
		int mainContentMaxWidth = context.parameters.getValue<int>("main content max width", mainContentMinWidth);
		if (mainContentMaxWidth < mainContentMinWidth)
		{
			mainContentMaxWidth = mainContentMinWidth;
		}

		const bool showMainLeadingButton = context.parameters.getValue<bool>("show main leading button", false);
		const bool showTrailingButton = context.parameters.getValue<bool>("show trailing button", false);

		const FlowUi::TextureRef mainLeadingButtonIcon = context.parameters.getValue<FlowUi::TextureRef>("main leading button icon");
		const FlowUi::TextureRef mainIconTexture = context.parameters.getValue<FlowUi::TextureRef>("main icon");
		const FlowUi::TextureRef trailingButtonIcon = context.parameters.getValue<FlowUi::TextureRef>("trailing button icon");
		const bool hasMainIcon = mainIconTexture.id != 0;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.parameters.getValue<Clay_Sizing>("sizing");
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = context.parameters.getValue<Clay_ChildAlignment>("child alignment");
		rootLayout.childGap = 0;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.parameters.getValue<Clay_Color>("background color");
		root.cornerRadius = CLAY_CORNER_RADIUS(0); // Constant: template layer stays as a sharp-corner rectangle.
		root.border = {
			.color = context.parameters.getValue<Clay_Color>("border color"),
			.width = context.parameters.getValue<Clay_BorderWidth>("border width"),
		};

		Clay_LayoutConfig indicatorLayout{};
		indicatorLayout.sizing = {
			.width = CLAY_SIZING_FIXED(2),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration indicator{};
		indicator.id = indicatorId;
		indicator.layout = indicatorLayout;
		indicator.backgroundColor = context.parameters.getValue<Clay_Color>("indicator color");
		indicator.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig leftSpacerLayout{};
		leftSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(leftSpacerMinWidth), static_cast<float>(leftSpacerMaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration leftSpacer{};
		leftSpacer.id = leftSpacerId;
		leftSpacer.layout = leftSpacerLayout;

		Clay_LayoutConfig mainContentLayout{};
		mainContentLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainContentLayout.sizing = {
			.width = CLAY_SIZING_FIT(static_cast<float>(mainContentMinWidth), static_cast<float>(mainContentMaxWidth)),
			.height = CLAY_SIZING_FIT(0),
		};
		mainContentLayout.padding = context.parameters.getValue<Clay_Padding>("main content padding");
		mainContentLayout.childAlignment = context.parameters.getValue<Clay_ChildAlignment>("main content child alignment");
		mainContentLayout.childGap = static_cast<uint16_t>(context.parameters.getValue<int>("main content child gap", 0));

		Clay_ElementDeclaration mainContent{};
		mainContent.id = mainContentId;
		mainContent.layout = mainContentLayout;
		mainContent.backgroundColor = context.parameters.getValue<Clay_Color>("main content background color");
		mainContent.cornerRadius = CLAY_CORNER_RADIUS(0);
		mainContent.border = {
			.color = context.parameters.getValue<Clay_Color>("main content border color"),
			.width = context.parameters.getValue<Clay_BorderWidth>("main content border width"),
		};

		Clay_LayoutConfig mainLeadingButtonSlotLayout{};
		mainLeadingButtonSlotLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainLeadingButtonSlotLayout.sizing = context.parameters.getValue<Clay_Sizing>("main leading button slot sizing");
		mainLeadingButtonSlotLayout.padding = CLAY_PADDING_ALL(0);
		mainLeadingButtonSlotLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainLeadingButtonSlotLayout.childGap = 0;

		Clay_ElementDeclaration mainLeadingButtonSlot{};
		mainLeadingButtonSlot.id = mainLeadingButtonSlotId;
		mainLeadingButtonSlot.layout = mainLeadingButtonSlotLayout;
		mainLeadingButtonSlot.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainLeadingButtonSlot.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig mainIconContainerLayout{};
		mainIconContainerLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainIconContainerLayout.sizing = context.parameters.getValue<Clay_Sizing>("main icon container sizing");
		mainIconContainerLayout.padding = CLAY_PADDING_ALL(0);
		mainIconContainerLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainIconContainerLayout.childGap = 0;

		Clay_ElementDeclaration mainIconContainer{};
		mainIconContainer.id = mainIconContainerId;
		mainIconContainer.layout = mainIconContainerLayout;
		mainIconContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainIconContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig mainIconLayout{};
		mainIconLayout.sizing = context.parameters.getValue<Clay_Sizing>("main icon sizing");

		Clay_ElementDeclaration mainIcon{};
		mainIcon.id = mainIconId;
		mainIcon.layout = mainIconLayout;
		mainIcon.backgroundColor = context.parameters.getValue<Clay_Color>("main icon tint color");
		mainIcon.image = {
			.imageData = context.userInterface.storeTexture(mainIconTexture),
		};

		Clay_TextElementConfig mainTextConfig{};
		mainTextConfig.textColor = context.parameters.getValue<Clay_Color>("main text color");
		mainTextConfig.fontSize = static_cast<uint16_t>(context.parameters.getValue<int>("main text font size"));
		mainTextConfig.wrapMode = context.parameters.getValue<Clay_TextElementConfigWrapMode>("main text wrap mode");
		mainTextConfig.textAlignment = context.parameters.getValue<Clay_TextAlignment>("main text alignment");
		mainTextConfig.fontId = static_cast<uint16_t>(context.parameters.getValue<int>("main text font id"));

		Clay_LayoutConfig rightSpacerLayout{};
		rightSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(0, 100000),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration rightSpacer{};
		rightSpacer.id = rightSpacerId;
		rightSpacer.layout = rightSpacerLayout;

		const std::string mainLeadingButtonPath = std::string(context.instanceIdPath) + "/main-content/leading-button";
		const std::string trailingButtonPath = std::string(context.instanceIdPath) + "/trailing-button";
		const std::string_view mainText = context.parameters.getString("main text");

		CLAY(root){
			CLAY(indicator){};
			CLAY(leftSpacer){};

			CLAY(mainContent){
				CLAY(mainLeadingButtonSlot){
					if (showMainLeadingButton)
					{
						context.userInterface.createElement(kBasicButton, mainLeadingButtonPath)
							.set("text", "")
							.set("icon", mainLeadingButtonIcon)
							.set("sizing", context.parameters.getValue<Clay_Sizing>("main leading button sizing"))
							.draw();
					}
				};

				CLAY(mainIconContainer){
					if (hasMainIcon)
					{
						CLAY(mainIcon){};
					}
				};

				CLAY({.id = mainTextId}){
					CLAY_TEXT(
						context.userInterface.toClayString(mainText),
						CLAY_TEXT_CONFIG(mainTextConfig)
					);
				};
			};

			CLAY(rightSpacer){};

			if (showTrailingButton)
			{
				context.userInterface.createElement(kBasicButton, trailingButtonPath)
					.set("text", "")
					.set("icon", trailingButtonIcon)
					.draw();
			}
		};
	},
};

inline const FlowUi::ElementDefinition kNavBar = {
	// elementTypeName: unique element type key (used for registration/lookups).
	"NavBar",

	// initializeDefaultParameters: optional defaults merged with per-instance overrides.
	[](FlowUi::ElementParameters& defaults) {
		defaults.setValue("background color", FlowUi::Flow_Color("#20232aff"));
		defaults.setValue("height min", 52);
		defaults.setValue("height max", 72);
		defaults.setValue("spacer 1 min width", 22);
		defaults.setValue("spacer 1 max width", 68);
		defaults.setValue("spacer 2 min width", 22);
		defaults.setValue("spacer 2 max width", 68);
	},

	// onHovered: optional callback when this element was hovered in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onPressed: optional callback when this element was pressed in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onHeld: optional callback when this element was held in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onReleased: optional callback when this element was released in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// runLogic: optional per-frame logic callback before buildElement executes.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	[](FlowUi::ElementBuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},

	// buildElement: required callback where the element's Clay UI is built.
	[](FlowUi::ElementBuildContext& context) {
		int navHeightMin = context.parameters.getValue<int>("height min", 0);
		int navHeightMax = context.parameters.getValue<int>("height max", navHeightMin);
		if (navHeightMax < navHeightMin)
		{
			navHeightMax = navHeightMin;
		}

		int spacer1MinWidth = context.parameters.getValue<int>("spacer 1 min width", 0);
		int spacer1MaxWidth = context.parameters.getValue<int>("spacer 1 max width", spacer1MinWidth);
		if (spacer1MaxWidth < spacer1MinWidth)
		{
			spacer1MaxWidth = spacer1MinWidth;
		}

		int spacer2MinWidth = context.parameters.getValue<int>("spacer 2 min width", 0);
		int spacer2MaxWidth = context.parameters.getValue<int>("spacer 2 max width", spacer2MinWidth);
		if (spacer2MaxWidth < spacer2MinWidth)
		{
			spacer2MaxWidth = spacer2MinWidth;
		}

		const Clay_ElementId rootId = context.elementId;
		const Clay_ElementId child1Id = context.createChildElementId("child-1");
		const Clay_ElementId child1IconId = context.createChildElementId("child-1/icon");
		const Clay_ElementId child1TextId = context.createChildElementId("child-1/text");
		const Clay_ElementId spacer1Id = context.createChildElementId("spacer-1");
		const Clay_ElementId spacer1LineId = context.createChildElementId("spacer-1/line");
		const Clay_ElementId child2Id = context.createChildElementId("child-2");
		const Clay_ElementId child2GrowSpacerId = context.createChildElementId("child-2/grow-spacer");
		const Clay_ElementId spacer2Id = context.createChildElementId("spacer-2");
		const Clay_ElementId spacer2LineId = context.createChildElementId("spacer-2/line");
		const Clay_ElementId child3Id = context.createChildElementId("child-3");

		const std::string button21Path = std::string(context.instanceIdPath) + "/child-2/button-1";
		const std::string button22Path = std::string(context.instanceIdPath) + "/child-2/button-2";
		const std::string button31Path = std::string(context.instanceIdPath) + "/child-3/button-1";
		const std::string button32Path = std::string(context.instanceIdPath) + "/child-3/button-2";

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.childGap = 0;
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		rootLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(static_cast<float>(navHeightMin), static_cast<float>(navHeightMax)),
		};

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.parameters.getValue<Clay_Color>("background color");
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig child1Layout{};
		child1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child1Layout.childGap = 8;
		child1Layout.padding = CLAY_PADDING_ALL(0);
		child1Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child1Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child1{};
		child1.id = child1Id;
		child1.layout = child1Layout;
		child1.backgroundColor = FlowUi::Flow_Color("#00000000");
		child1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig child1IconLayout{};
		child1IconLayout.sizing = {.width = CLAY_SIZING_FIXED(12), .height = CLAY_SIZING_FIXED(12)};

		Clay_ElementDeclaration child1Icon{};
		child1Icon.id = child1IconId;
		child1Icon.layout = child1IconLayout;
		child1Icon.backgroundColor = FlowUi::Flow_Color("#72b6ffff");
		child1Icon.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_TextElementConfig child1TextConfig{};
		child1TextConfig.textColor = FlowUi::Flow_Color("#f4f6f8ff");
		child1TextConfig.fontSize = 16;
		child1TextConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		child1TextConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		child1TextConfig.fontId = 0;

		Clay_LayoutConfig spacer1Layout{};
		spacer1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer1Layout.childGap = 0;
		spacer1Layout.padding = CLAY_PADDING_ALL(0);
		spacer1Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer1Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer1MinWidth), static_cast<float>(spacer1MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer1{};
		spacer1.id = spacer1Id;
		spacer1.layout = spacer1Layout;
		spacer1.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig spacerLineLayout{};
		spacerLineLayout.sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(20)};

		Clay_ElementDeclaration spacer1Line{};
		spacer1Line.id = spacer1LineId;
		spacer1Line.layout = spacerLineLayout;
		spacer1Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer1Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child2Layout{};
		child2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child2Layout.childGap = 8;
		child2Layout.padding = CLAY_PADDING_ALL(0);
		child2Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child2Layout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child2{};
		child2.id = child2Id;
		child2.layout = child2Layout;
		child2.backgroundColor = FlowUi::Flow_Color("#00000000");
		child2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_LayoutConfig child2GrowSpacerLayout{};
		child2GrowSpacerLayout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_PERCENT(1.0f)};

		Clay_ElementDeclaration child2GrowSpacer{};
		child2GrowSpacer.id = child2GrowSpacerId;
		child2GrowSpacer.layout = child2GrowSpacerLayout;

		Clay_LayoutConfig spacer2Layout{};
		spacer2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer2Layout.childGap = 0;
		spacer2Layout.padding = CLAY_PADDING_ALL(0);
		spacer2Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer2Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer2MinWidth), static_cast<float>(spacer2MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer2{};
		spacer2.id = spacer2Id;
		spacer2.layout = spacer2Layout;
		spacer2.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		Clay_ElementDeclaration spacer2Line{};
		spacer2Line.id = spacer2LineId;
		spacer2Line.layout = spacerLineLayout;
		spacer2Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer2Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child3Layout{};
		child3Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child3Layout.childGap = 8;
		child3Layout.padding = CLAY_PADDING_ALL(0);
		child3Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child3Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child3{};
		child3.id = child3Id;
		child3.layout = child3Layout;
		child3.backgroundColor = FlowUi::Flow_Color("#00000000");
		child3.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};

		CLAY(root){
			CLAY(child1){
				CLAY(child1Icon){};
				CLAY({.id = child1TextId}){
					CLAY_TEXT(
						context.userInterface.toClayString("FlowPlot"),
						CLAY_TEXT_CONFIG(child1TextConfig)
					);
				};
			};

			CLAY(spacer1){
				CLAY(spacer1Line){};
			};

			CLAY(child2){
				context.userInterface.createElement(kBasicButton, button21Path)
					.set("text", "Templates")
					.draw();

				context.userInterface.createElement(kBasicButton, button22Path)
					.set("text", "Layers")
					.draw();

				CLAY(child2GrowSpacer){};
			};

			CLAY(spacer2){
				CLAY(spacer2Line){};
			};

			CLAY(child3){
				context.userInterface.createElement(kBasicButton, button31Path)
					.set("text", "Export")
					.draw();

				context.userInterface.createElement(kBasicButton, button32Path)
					.set("text", "Settings")
					.draw();
			};
		};
	},
};
