#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "FlowPlotGui.hpp"

inline constexpr std::array<std::uint8_t, 4> kNineSplitPaddingSlots{3u, 5u, 1u, 7u};

inline double nineSplitPaddingValueForSlot(const Clay_Padding& padding, std::uint8_t slotId)
{
	switch (slotId)
	{
	case 3u:
		return static_cast<double>(padding.left);
	case 5u:
		return static_cast<double>(padding.right);
	case 1u:
		return static_cast<double>(padding.top);
	case 7u:
		return static_cast<double>(padding.bottom);
	default:
		return 0.0;
	}
}

inline uint16_t nineSplitNormalizePaddingValue(double value)
{
	const double normalized = numericInputNormalizeValue(
		numericInputValueType::UInt16,
		value,
		0.0,
		static_cast<double>(std::numeric_limits<uint16_t>::max()));
	return static_cast<uint16_t>(std::clamp(
		normalized,
		0.0,
		static_cast<double>(std::numeric_limits<uint16_t>::max())));
}

inline void nineSplitSetPaddingSlot(Clay_Padding& padding, std::uint8_t slotId, uint16_t value)
{
	switch (slotId)
	{
	case 3u:
		padding.left = value;
		break;
	case 5u:
		padding.right = value;
		break;
	case 1u:
		padding.top = value;
		break;
	case 7u:
		padding.bottom = value;
		break;
	default:
		break;
	}
}

struct nineSplitMatrixParams {
	Clay_Padding value = CLAY_PADDING_ALL(0);
	std::function<void(Clay_Padding)> onChange = nullptr;

	bool linkDefaultEnabled = true;
	Clay_Color linkEnabledBackgroundColor = FlowUi::Flow_Color("#4b8c5aff");
	Clay_Color linkDisabledBackgroundColor = FlowUi::Flow_Color("#2f323aff");
	Clay_Color linkHoverBackgroundColor = FlowUi::Flow_Color("#5b9d6bff");
	Clay_Color linkBorderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth linkBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius linkCornerRadius = CLAY_CORNER_RADIUS(10);
	Clay_Padding linkPadding = CLAY_PADDING_ALL(2);
	Clay_Sizing linkSizing = Clay_Sizing{
		.width = CLAY_SIZING_FIXED(18),
		.height = CLAY_SIZING_FIXED(18),
	};
	Clay_Sizing linkIconContainerSizing = Clay_Sizing{
		.width = CLAY_SIZING_FIXED(10),
		.height = CLAY_SIZING_FIXED(10),
	};
	Clay_Color linkIconTintColor = FlowUi::Flow_Color("#ffffffff");

	Clay_Sizing sizing = Clay_Sizing{
		.width = CLAY_SIZING_FIT(0),
		.height = CLAY_SIZING_FIT(0),
	};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	uint16_t rowGap = 4;
	uint16_t columnGap = 4;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");

	Clay_Sizing slotSizing = Clay_Sizing{
		.width = CLAY_SIZING_FIXED(50),
		.height = CLAY_SIZING_FIXED(50),
	};
	Clay_Padding slotPadding = CLAY_PADDING_ALL(2);
	uint16_t cornerInnerPadding = 2;
	uint16_t cornerOuterPadding = 5;
	Clay_Color slotBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color slotBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth slotBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_CornerRadius slotCornerRadius = CLAY_CORNER_RADIUS(6);

	Clay_Sizing numericSizing = Clay_Sizing{
		.width = CLAY_SIZING_FIXED(46),
		.height = CLAY_SIZING_FIT(0),
	};
	Clay_Padding numericInputPadding = CLAY_PADDING_ALL(5);
	Clay_Color numericInputBackgroundColor = FlowUi::Flow_Color("#252932ff");
	Clay_Color numericInputBorderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth numericInputBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius numericInputCornerRadius = CLAY_CORNER_RADIUS(6);

	uint16_t fontId = 0;
	uint16_t fontSize = 12;
	Clay_Color typeHintTextColor = FlowUi::Flow_Color("#00000000");
	Clay_Color valueTextColor = FlowUi::Flow_Color("#ffffffff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	nineSplitMatrixParams,
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, value),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkDefaultEnabled),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkEnabledBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkDisabledBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkSizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, linkIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, rowGap),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, columnGap),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotSizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, cornerInnerPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, cornerOuterPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, slotCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericSizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericInputPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericInputBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericInputBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericInputBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, numericInputCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, typeHintTextColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixParams, valueTextColor));

struct nineSplitMatrixState {
	bool initialized = false;
	bool linkEnabled = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	nineSplitMatrixState,
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitMatrixState, linkEnabled));

struct nineSplitMatrixResources {
	nineSplitMatrixResources() = default;
	explicit nineSplitMatrixResources(FlowUi::App& app) :
		linkIconPrepared(true),
		linkIcon(app.icons().textureRef("Link"))
	{}

	bool linkIconPrepared = false;
	FlowUi::TextureRef linkIcon = FlowUi::TextureRef{};
};

using NineSplitMatrixDef = FlowUi::ElementDefinition<
	nineSplitMatrixParams,
	nineSplitMatrixState,
	nineSplitMatrixResources,
	FLOW_DEF_ID("NineSplitMatrix"),
	true>;

inline const NineSplitMatrixDef kNineSplitMatrix = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](NineSplitMatrixDef::BuildContext& context) {
		nineSplitMatrixState& state = NineSplitMatrixDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.linkEnabled = context.params.linkDefaultEnabled;
		}

		FlowUi::TextureRef linkIcon{};
		if (NineSplitMatrixDef::resources.has_value())
		{
			const nineSplitMatrixResources& resources = *NineSplitMatrixDef::resources;
			if (resources.linkIconPrepared)
			{
				linkIcon = resources.linkIcon;
			}
		}

		Clay_ElementDeclaration root{};
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		root.layout.sizing = context.params.sizing;
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.padding = context.params.padding;
		root.layout.childGap = context.params.rowGap;
		root.layout.childAlignment = {
			.x = CLAY_ALIGN_X_LEFT,
			.y = CLAY_ALIGN_Y_TOP,
		};
		root.backgroundColor = context.params.backgroundColor;

		CLAY(rootId, root)
		{
			for (int row = 0; row < 3; ++row)
			{
				Clay_ElementDeclaration rowDeclaration{};
				const Clay_ElementId rowDeclarationId = context.uiManager.toClayEID(
					context.createChildElementId("row-" + std::to_string(row)));
				rowDeclaration.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
				rowDeclaration.layout.childGap = context.params.columnGap;
				rowDeclaration.backgroundColor = FlowUi::Flow_Color("#00000000");

				CLAY(rowDeclarationId, rowDeclaration)
				{
					for (int col = 0; col < 3; ++col)
					{
						const uint8_t slotId = static_cast<uint8_t>(row * 3 + col);
						const bool isNumericSlot = std::find(
							kNineSplitPaddingSlots.begin(),
							kNineSplitPaddingSlots.end(),
							slotId) != kNineSplitPaddingSlots.end();

						Clay_ElementDeclaration slotDeclaration{};
						const Clay_ElementId slotDeclarationId = context.uiManager.toClayEID(
							context.createChildElementId("slot-" + std::to_string(slotId)));
						slotDeclaration.layout.sizing = context.params.slotSizing;
						slotDeclaration.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
						Clay_Padding resolvedSlotPadding = context.params.slotPadding;
						if (isNumericSlot && (slotId == 1u || slotId == 3u || slotId == 5u || slotId == 7u))
						{
							resolvedSlotPadding.left = context.params.cornerInnerPadding;
							resolvedSlotPadding.right = context.params.cornerInnerPadding;
							resolvedSlotPadding.top = context.params.cornerInnerPadding;
							resolvedSlotPadding.bottom = context.params.cornerInnerPadding;
						}
						slotDeclaration.layout.padding = resolvedSlotPadding;
						slotDeclaration.layout.childAlignment = {
							.x = CLAY_ALIGN_X_CENTER,
							.y = CLAY_ALIGN_Y_CENTER,
						};
						slotDeclaration.backgroundColor = context.params.slotBackgroundColor;
						slotDeclaration.border = {
							.color = context.params.slotBorderColor,
							.width = context.params.slotBorderWidth,
						};
						slotDeclaration.cornerRadius = context.params.slotCornerRadius;

						CLAY(slotDeclarationId, slotDeclaration)
						{
							if (slotId == 4u)
							{
								context.uiManager
									.createElement(kBasicButton, context.createChildElementId("center-link"))
									.setParameters(basicButtonParams{
										.icon = linkIcon,
										.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](
																 BasicButtonInteractionContext) {
											nineSplitMatrixState* latestState =
												NineSplitMatrixDef::tryGetState(elementFlowId);
											if (latestState != nullptr)
											{
												latestState->linkEnabled = !latestState->linkEnabled;
											}
										},
										.contentMode = basicButtonParams::ContentMode::IconOnly,
										.padding = context.params.linkPadding,
										.sizing = context.params.linkSizing,
										.backgroundColor = state.linkEnabled
											? context.params.linkEnabledBackgroundColor
											: context.params.linkDisabledBackgroundColor,
										.hoverBackgroundColor = context.params.linkHoverBackgroundColor,
										.cornerRadius = context.params.linkCornerRadius,
										.borderColor = context.params.linkBorderColor,
										.borderWidth = context.params.linkBorderWidth,
										.childLayoutDirection = CLAY_LEFT_TO_RIGHT,
										.childAlignment = {
											.x = CLAY_ALIGN_X_CENTER,
											.y = CLAY_ALIGN_Y_CENTER,
										},
										.childGap = 0,
										.iconContainerSizing = context.params.linkIconContainerSizing,
										.iconTintColor = context.params.linkIconTintColor,
									})
									.draw();
							}
							else if (isNumericSlot)
							{
								numericInputFieldParams numericParams{};
								numericParams.valueType = numericInputValueType::UInt16;
								numericParams.value = nineSplitPaddingValueForSlot(context.params.value, slotId);
								numericParams.minValue = 0.0;
								numericParams.maxValue = static_cast<double>(std::numeric_limits<uint16_t>::max());
								numericParams.onChange = [
									elementFlowId = FlowUi::toFlowId(context.elementID),
									slotId,
									currentValue = context.params.value,
									onChange = context.params.onChange
								](double changedValue) {
									if (onChange == nullptr)
									{
										return;
									}

									const nineSplitMatrixState* latestState =
										NineSplitMatrixDef::tryGetStateConst(elementFlowId);
									const bool linkEnabled = latestState == nullptr || latestState->linkEnabled;
									const uint16_t nextValue = nineSplitNormalizePaddingValue(changedValue);
									Clay_Padding nextPadding = currentValue;
									if (linkEnabled)
									{
										nextPadding.left = nextValue;
										nextPadding.right = nextValue;
										nextPadding.top = nextValue;
										nextPadding.bottom = nextValue;
									}
									else
									{
										nineSplitSetPaddingSlot(nextPadding, slotId, nextValue);
									}
									onChange(nextPadding);
								};
								numericParams.sizing = context.params.numericSizing;
								numericParams.padding = CLAY_PADDING_ALL(0);
								numericParams.childGap = 0;
								numericParams.backgroundColor = FlowUi::Flow_Color("#00000000");
								numericParams.fontId = context.params.fontId;
								numericParams.fontSize = context.params.fontSize;
								numericParams.typeHintTextColor = context.params.typeHintTextColor;
								numericParams.inputTextColor = context.params.valueTextColor;
								numericParams.inputPadding = context.params.numericInputPadding;
								numericParams.inputBackgroundColor = context.params.numericInputBackgroundColor;
								numericParams.inputBorderColor = context.params.numericInputBorderColor;
								numericParams.inputBorderWidth = context.params.numericInputBorderWidth;
								numericParams.inputCornerRadius = context.params.numericInputCornerRadius;
								numericParams.inputTextAlignment = {
									.x = CLAY_ALIGN_X_CENTER,
									.y = CLAY_ALIGN_Y_CENTER,
								};

								context.uiManager
									.createElement(
										kNumericInputField,
										context.createChildElementId("slot-input-" + std::to_string(slotId)))
									.setParameters(std::move(numericParams))
									.draw();
							}
						};
					}
				};
			}
		};
	},
};

struct nineSplitCardParams {
	std::string hintText = "Padding";
	Clay_Padding value = CLAY_PADDING_ALL(0);
	std::function<void(Clay_Padding)> onChange = nullptr;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	nineSplitMatrixParams matrix{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	nineSplitCardParams,
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(nineSplitCardParams, matrix));

using NineSplitCardDef = FlowUi::ElementDefinition<
	nineSplitCardParams,
	void,
	void,
	FLOW_DEF_ID("NineSplitCard")>;

inline const NineSplitCardDef kNineSplitCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](NineSplitCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string matrixPath = context.createChildElementId("nine-split-matrix");

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
		spacer.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		nineSplitMatrixParams matrixParams = context.params.matrix;
		matrixParams.value = context.params.value;
		matrixParams.onChange = context.params.onChange;
		matrixParams.fontId = context.params.fontId;

		CLAY(rootId, root)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};

			CLAY(spacerId, spacer) {};

			context.uiManager.createElement(kNineSplitMatrix, matrixPath)
				.setParameters(std::move(matrixParams))
				.draw();
		};
	},
};
