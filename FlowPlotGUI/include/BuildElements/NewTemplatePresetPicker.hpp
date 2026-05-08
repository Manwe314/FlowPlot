#pragma once

#include <functional>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "FlowPlotGui.hpp"
#include "TemplatePresets.hpp"

struct newTemplatePresetPickerParams {
	FlowPlotGui::state* guiState = nullptr;
	bool* isOpen = nullptr;

	Clay_Color overlayColor = Clay_Color{.r = 96.0f, .g = 96.0f, .b = 96.0f, .a = 30.0f};
	int16_t overlayZIndex = 300;
	int16_t listZIndex = 301;

	Clay_Sizing listSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(360), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding listPadding = Clay_Padding{0, 0, 6, 6};
	uint16_t listChildGap = 0;
	Clay_Color listBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color listBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth listBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius listCornerRadius = CLAY_CORNER_RADIUS(8);

	Clay_Padding optionPadding = Clay_Padding{16, 16, 14, 14};
	Clay_Color optionTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Color optionHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	uint16_t optionFontId = 0;
	uint16_t optionFontSize = 18;

	float separatorLineWidth = 1.0f;
	Clay_Color separatorColor = FlowUi::Flow_Color("#303640ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	newTemplatePresetPickerParams,
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, overlayColor),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, overlayZIndex),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listZIndex),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listSizing),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listPadding),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listChildGap),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, listCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, optionPadding),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, optionTextColor),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, optionHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, optionFontId),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, optionFontSize),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, separatorLineWidth),
	FLOWUI_DEV_REFLECT_FIELD(newTemplatePresetPickerParams, separatorColor));

using NewTemplatePresetPickerDef = FlowUi::ElementDefinition<
	newTemplatePresetPickerParams,
	void,
	void,
	FLOW_DEF_ID("NewTemplatePresetPicker")>;

inline basicButtonParams newTemplatePresetPickerTransparentButton(
	Clay_Sizing sizing,
	Clay_Padding padding,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.contentMode = basicButtonParams::ContentMode::None;
	params.sizing = sizing;
	params.padding = padding;
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	params.onPressedCallback = std::move(onPressed);
	return params;
}

inline const NewTemplatePresetPickerDef kNewTemplatePresetPicker = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](NewTemplatePresetPickerDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId listId = context.uiManager.toClayEID(context.createChildElementId("list"));

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rootLayout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childGap = 0;
		rootLayout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.overlayColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		root.floating = {
			.zIndex = context.params.overlayZIndex,
			.attachPoints = {
				.element = CLAY_ATTACH_POINT_LEFT_TOP,
				.parent = CLAY_ATTACH_POINT_LEFT_TOP,
			},
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
			.attachTo = CLAY_ATTACH_TO_ROOT,
		};

		Clay_ElementDeclaration list{};
		list.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		list.layout.sizing = context.params.listSizing;
		list.layout.padding = context.params.listPadding;
		list.layout.childGap = context.params.listChildGap;
		list.backgroundColor = context.params.listBackgroundColor;
		list.cornerRadius = context.params.listCornerRadius;
		list.border = {.color = context.params.listBorderColor, .width = context.params.listBorderWidth};
		list.floating = {
			.zIndex = context.params.listZIndex,
			.attachPoints = {
				.element = CLAY_ATTACH_POINT_CENTER_CENTER,
				.parent = CLAY_ATTACH_POINT_CENTER_CENTER,
			},
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
			.attachTo = CLAY_ATTACH_TO_PARENT,
		};

		const auto closePopup = [isOpen = context.params.isOpen](BasicButtonInteractionContext) {
			if (isOpen != nullptr)
			{
				*isOpen = false;
			}
		};

		CLAY(rootId, root)
		{
			context.uiManager.createElement(kBasicButton, context.createChildElementId("dismiss-button"))
				.setParameters(newTemplatePresetPickerTransparentButton(
					Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
					CLAY_PADDING_ALL(0),
					closePopup))
				.draw();

			CLAY(listId, list)
			{
				const std::vector<std::string> presets = FlowPlotGui::templatePresetNames();
				for (std::size_t i = 0; i < presets.size(); ++i)
				{
					const std::string presetName = presets[i];
					context.uiManager.createElement(
						kBasicButton,
						context.createChildElementId("list/option-" + std::to_string(i)))
						.setParameters(basicButtonParams{
							.text = presetName,
							.onPressedCallback = [
								guiState = context.params.guiState,
								isOpen = context.params.isOpen,
								presetName
							](BasicButtonInteractionContext) {
								if (guiState != nullptr)
								{
									FlowPlotGui::replaceWithTemplatePreset(
										*guiState,
										FlowPlotGui::makeTemplatePreset(presetName));
								}
								if (isOpen != nullptr)
								{
									*isOpen = false;
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
							.fontId = context.params.optionFontId,
							.fontSize = context.params.optionFontSize,
							.textColor = context.params.optionTextColor,
						})
						.draw();

					if (i + 1 < presets.size())
					{
						Clay_ElementDeclaration separator{};
						separator.layout.sizing = {
							.width = CLAY_SIZING_GROW(0),
							.height = CLAY_SIZING_FIXED(context.params.separatorLineWidth),
						};
						separator.backgroundColor = context.params.separatorColor;
						CLAY(context.uiManager.toClayEID(
							context.createChildElementId("list/separator-" + std::to_string(i))),
							separator){};
					}
				}
			};
		};
	},
};
