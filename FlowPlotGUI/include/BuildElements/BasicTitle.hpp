#pragma once

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct basicTitleParams {
	enum class ContentMode : uint8_t {
		TextOnly,
		IconThenText,
		TextThenIcon,
	};

	std::string text = "Title";
	FlowUi::TextureRef icon = FlowUi::TextureRef{};
	ContentMode contentMode = ContentMode::TextOnly;

	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(0);
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_LayoutDirection childLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t childGap = 8;

	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");

	Clay_Sizing iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(64), .height = CLAY_SIZING_FIXED(64)};
	Clay_Padding iconContainerPadding = CLAY_PADDING_ALL(0);
	Clay_LayoutDirection iconContainerChildLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment iconContainerChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t iconContainerChildGap = 0;
	Clay_Color iconContainerBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color iconContainerBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth iconContainerBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing iconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color iconTintColor = FlowUi::Flow_Color("#72b6ffff");
};

FLOWUI_DEV_REGISTER_ENUM(
	basicTitleParams::ContentMode,
	FLOWUI_DEV_ENUM_VALUE(basicTitleParams::ContentMode::TextOnly),
	FLOWUI_DEV_ENUM_VALUE(basicTitleParams::ContentMode::IconThenText),
	FLOWUI_DEV_ENUM_VALUE(basicTitleParams::ContentMode::TextThenIcon));

FLOWUI_DEV_REGISTER_STRUCT(
	basicTitleParams,
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, text),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, contentMode),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, childLayoutDirection),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, childAlignment),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerPadding),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerChildLayoutDirection),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerChildAlignment),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerChildGap),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconContainerBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconSizing),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, iconTintColor));

using BasicTitleDef = FlowUi::ElementDefinition<basicTitleParams, void, void, FLOW_DEF_ID("Basic title")>;

inline const BasicTitleDef kBasicTitle = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicTitleDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const bool needsIcon = context.params.contentMode != basicTitleParams::ContentMode::TextOnly;

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

		const std::string textPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textPath);

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

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
			switch (context.params.contentMode)
			{
			case basicTitleParams::ContentMode::TextOnly:
				drawTextChild();
				break;
			case basicTitleParams::ContentMode::IconThenText:
				drawIconChild();
				drawTextChild();
				break;
			case basicTitleParams::ContentMode::TextThenIcon:
				drawTextChild();
				drawIconChild();
				break;
			}
		};
	},
};
