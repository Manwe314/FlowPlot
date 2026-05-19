#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicInputField.hpp"
#include "FlowPlotGui.hpp"

enum class numericInputValueType : std::uint8_t {
	Int,
	Int8,
	Int16,
	Int32,
	UInt8,
	UInt16,
	UInt32,
	Float,
	Double,
};

FLOWUI_DEV_REGISTER_ENUM(
	numericInputValueType,
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Int),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Int8),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Int16),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Int32),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::UInt8),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::UInt16),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::UInt32),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Float),
	FLOWUI_DEV_ENUM_VALUE(numericInputValueType::Double));

inline bool numericInputTypeIsFloating(numericInputValueType type)
{
	return type == numericInputValueType::Float || type == numericInputValueType::Double;
}

inline const char* numericInputTypeLabel(numericInputValueType type)
{
	switch (type)
	{
	case numericInputValueType::Int:
		return "int";
	case numericInputValueType::Int8:
		return "int8";
	case numericInputValueType::Int16:
		return "int16";
	case numericInputValueType::Int32:
		return "int32";
	case numericInputValueType::UInt8:
		return "uint8";
	case numericInputValueType::UInt16:
		return "uint16";
	case numericInputValueType::UInt32:
		return "uint32";
	case numericInputValueType::Float:
		return "float";
	case numericInputValueType::Double:
		return "double";
	}
	return "number";
}

inline double numericInputTypeMin(numericInputValueType type)
{
	switch (type)
	{
	case numericInputValueType::Int:
	case numericInputValueType::Int32:
		return static_cast<double>(std::numeric_limits<std::int32_t>::min());
	case numericInputValueType::Int8:
		return static_cast<double>(std::numeric_limits<std::int8_t>::min());
	case numericInputValueType::Int16:
		return static_cast<double>(std::numeric_limits<std::int16_t>::min());
	case numericInputValueType::UInt8:
		return 0.0;
	case numericInputValueType::UInt16:
		return 0.0;
	case numericInputValueType::UInt32:
		return 0.0;
	case numericInputValueType::Float:
		return -static_cast<double>(std::numeric_limits<float>::max());
	case numericInputValueType::Double:
		return -std::numeric_limits<double>::max();
	}
	return -std::numeric_limits<double>::max();
}

inline double numericInputTypeMax(numericInputValueType type)
{
	switch (type)
	{
	case numericInputValueType::Int:
	case numericInputValueType::Int32:
		return static_cast<double>(std::numeric_limits<std::int32_t>::max());
	case numericInputValueType::Int8:
		return static_cast<double>(std::numeric_limits<std::int8_t>::max());
	case numericInputValueType::Int16:
		return static_cast<double>(std::numeric_limits<std::int16_t>::max());
	case numericInputValueType::UInt8:
		return static_cast<double>(std::numeric_limits<std::uint8_t>::max());
	case numericInputValueType::UInt16:
		return static_cast<double>(std::numeric_limits<std::uint16_t>::max());
	case numericInputValueType::UInt32:
		return static_cast<double>(std::numeric_limits<std::uint32_t>::max());
	case numericInputValueType::Float:
		return static_cast<double>(std::numeric_limits<float>::max());
	case numericInputValueType::Double:
		return std::numeric_limits<double>::max();
	}
	return std::numeric_limits<double>::max();
}

inline double numericInputDomainMin(numericInputValueType type, double minValue, double maxValue)
{
	if (!std::isfinite(minValue))
	{
		minValue = numericInputTypeMin(type);
	}
	if (!std::isfinite(maxValue))
	{
		maxValue = numericInputTypeMax(type);
	}
	if (maxValue < minValue)
	{
		maxValue = minValue;
	}
	const double typeMin = numericInputTypeMin(type);
	const double typeMax = numericInputTypeMax(type);
	return std::clamp(minValue, typeMin, typeMax);
}

inline double numericInputDomainMax(numericInputValueType type, double minValue, double maxValue)
{
	if (!std::isfinite(minValue))
	{
		minValue = numericInputTypeMin(type);
	}
	if (!std::isfinite(maxValue))
	{
		maxValue = numericInputTypeMax(type);
	}
	if (maxValue < minValue)
	{
		maxValue = minValue;
	}
	const double typeMin = numericInputTypeMin(type);
	const double typeMax = numericInputTypeMax(type);
	const double domainMin = std::clamp(minValue, typeMin, typeMax);
	return std::max(domainMin, std::clamp(maxValue, typeMin, typeMax));
}

inline bool numericInputTryParseDouble(std::string_view text, double& outValue)
{
	std::string trimmed{};
	trimmed.reserve(text.size());
	for (const char c : text)
	{
		if (std::isspace(static_cast<unsigned char>(c)) == 0)
		{
			trimmed.push_back(c);
		}
	}
	if (trimmed.empty())
	{
		outValue = 0.0;
		return false;
	}

	errno = 0;
	char* end = nullptr;
	const double parsed = std::strtod(trimmed.c_str(), &end);
	if (end == nullptr || *end != '\0' || errno == ERANGE || !std::isfinite(parsed))
	{
		outValue = 0.0;
		return false;
	}

	outValue = parsed;
	return true;
}

inline double numericInputNormalizeValue(
	numericInputValueType type,
	double rawValue,
	double minValue,
	double maxValue)
{
	if (!std::isfinite(rawValue))
	{
		rawValue = 0.0;
	}

	const double domainMin = numericInputDomainMin(type, minValue, maxValue);
	const double domainMax = numericInputDomainMax(type, minValue, maxValue);
	double normalized = std::clamp(rawValue, domainMin, domainMax);
	if (!numericInputTypeIsFloating(type))
	{
		normalized = std::floor(normalized);
		normalized = std::clamp(normalized, domainMin, domainMax);
	}
	return normalized;
}

inline std::string numericInputValueToText(numericInputValueType type, double value)
{
	if (!numericInputTypeIsFloating(type))
	{
		return std::to_string(static_cast<std::int64_t>(std::floor(value)));
	}

	std::ostringstream stream{};
	stream << std::setprecision(type == numericInputValueType::Float ? 7 : 15) << value;
	std::string text = stream.str();
	if (text.find('.') != std::string::npos)
	{
		while (!text.empty() && text.back() == '0')
		{
			text.pop_back();
		}
		if (!text.empty() && text.back() == '.')
		{
			text.pop_back();
		}
	}
	if (text.empty() || text == "-0")
	{
		return "0";
	}
	return text;
}

inline bool numericInputValuesEqual(numericInputValueType type, double lhs, double rhs)
{
	if (numericInputTypeIsFloating(type))
	{
		return std::fabs(lhs - rhs) <= 1.0e-9;
	}
	return static_cast<std::int64_t>(std::floor(lhs)) == static_cast<std::int64_t>(std::floor(rhs));
}

struct numericInputFieldParams {
	numericInputValueType valueType = numericInputValueType::Float;
	double value = 0.0;
	double minValue = -1000000.0;
	double maxValue = 1000000.0;
	std::function<void(double)> onChange = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	uint16_t childGap = 6;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color typeHintTextColor = FlowUi::Flow_Color("#aeb2b8ff");
	Clay_Color inputTextColor = FlowUi::Flow_Color("#f4f6f8ff");

	Clay_Padding inputPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color inputBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth inputBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color inputBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_CornerRadius inputCornerRadius = CLAY_CORNER_RADIUS(5);
	Clay_ChildAlignment inputTextAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
};

FLOWUI_DEV_REGISTER_STRUCT(
	numericInputFieldParams,
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, valueType),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, value),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, minValue),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, maxValue),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, typeHintTextColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputTextColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputPadding),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldParams, inputTextAlignment));

struct numericInputFieldState {
	bool initialized = false;
	bool pendingFieldReset = false;
	double normalizedValue = 0.0;
	std::string normalizedText = "0";
};

FLOWUI_DEV_REGISTER_STRUCT(
	numericInputFieldState,
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldState, pendingFieldReset),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldState, normalizedValue),
	FLOWUI_DEV_REFLECT_FIELD(numericInputFieldState, normalizedText));

using NumericInputFieldDef = FlowUi::ElementDefinition<
	numericInputFieldParams,
	numericInputFieldState,
	void,
	FLOW_DEF_ID("NumericInputField")>;

inline const NumericInputFieldDef kNumericInputField = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](NumericInputFieldDef::BuildContext& context) {
		numericInputFieldState& state = NumericInputFieldDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const std::string inputPath = context.createChildElementId("input");

		const double paramNormalized = numericInputNormalizeValue(
			context.params.valueType,
			context.params.value,
			context.params.minValue,
			context.params.maxValue);
		if (!state.initialized || !numericInputValuesEqual(context.params.valueType, state.normalizedValue, paramNormalized))
		{
			state.initialized = true;
			state.normalizedValue = paramNormalized;
			state.normalizedText = numericInputValueToText(context.params.valueType, paramNormalized);
			state.pendingFieldReset = true;
		}
		if (state.pendingFieldReset)
		{
			(void)context.uiManager.inputFields().removeField(inputPath);
			state.pendingFieldReset = false;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId typeHintId = context.uiManager.toClayEID(context.createChildElementId("type-hint"));

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = context.params.childGap;
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig typeHintConfig{};
		typeHintConfig.textColor = context.params.typeHintTextColor;
		typeHintConfig.fontSize = context.params.fontSize;
		typeHintConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		typeHintConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		typeHintConfig.fontId = context.params.fontId;

		basicInputFieldParams inputParams{};
		inputParams.fieldId = inputPath;
		inputParams.value = state.normalizedText;
		inputParams.syncValueFromParams = true;
		inputParams.onTextChangedCallback = [
			elementFlowId = FlowUi::toFlowId(context.elementID),
			valueType = context.params.valueType,
			minValue = context.params.minValue,
			maxValue = context.params.maxValue,
			onChange = context.params.onChange
		](std::string_view text) {
			numericInputFieldState* latestState = NumericInputFieldDef::tryGetState(elementFlowId);
			if (latestState == nullptr)
			{
				return;
			}

			double parsed = 0.0;
			const bool parsedOk = numericInputTryParseDouble(text, parsed);
			const double normalized = numericInputNormalizeValue(valueType, parsedOk ? parsed : 0.0, minValue, maxValue);
			const std::string normalizedText = numericInputValueToText(valueType, normalized);
			const bool changed = !numericInputValuesEqual(valueType, latestState->normalizedValue, normalized);

			latestState->normalizedValue = normalized;
			latestState->normalizedText = normalizedText;
			if (!parsedOk || normalizedText != std::string(text))
			{
				latestState->pendingFieldReset = true;
			}

			if (changed && onChange != nullptr)
			{
				onChange(normalized);
			}
		};
		inputParams.padding = context.params.inputPadding;
		inputParams.sizing = Clay_Sizing{
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(0),
		};
		inputParams.borderColor = context.params.inputBorderColor;
		inputParams.borderWidth = context.params.inputBorderWidth;
		inputParams.backgroundColor = context.params.inputBackgroundColor;
		inputParams.cornerRadius = context.params.inputCornerRadius;
		inputParams.childTextAlignment = context.params.inputTextAlignment;
		inputParams.textWrapMode = CLAY_TEXT_WRAP_NONE;
		inputParams.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		inputParams.fontId = context.params.fontId;
		inputParams.fontSize = context.params.fontSize;
		inputParams.textColor = context.params.inputTextColor;

		const std::string typeHint = std::string(numericInputTypeLabel(context.params.valueType)) + ":";

		CLAY(rootId, root)
		{
			if (context.params.typeHintTextColor.a > 0.0f)
			{
				CLAY(typeHintId, {})
				{
					CLAY_TEXT(
						context.uiManager.toClayString(typeHint),
						CLAY_TEXT_CONFIG(typeHintConfig)
					);
				};
			}

			context.uiManager.createElement(kBasicInputField, inputPath)
				.setParameters(std::move(inputParams))
				.draw();
		};
	},
};

struct numericInputCardParams {
	std::string hintText = "Number";
	numericInputValueType valueType = numericInputValueType::Float;
	double value = 0.0;
	double minValue = -1000000.0;
	double maxValue = 1000000.0;
	std::function<void(double)> onChange = nullptr;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	numericInputFieldParams field{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	numericInputCardParams,
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, valueType),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, minValue),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, maxValue),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(numericInputCardParams, field));

using NumericInputCardDef = FlowUi::ElementDefinition<
	numericInputCardParams,
	void,
	void,
	FLOW_DEF_ID("NumericInputCard")>;

inline const NumericInputCardDef kNumericInputCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](NumericInputCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string fieldPath = context.createChildElementId("numeric-field");

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

		numericInputFieldParams fieldParams = context.params.field;
		fieldParams.valueType = context.params.valueType;
		fieldParams.value = context.params.value;
		fieldParams.minValue = context.params.minValue;
		fieldParams.maxValue = context.params.maxValue;
		fieldParams.onChange = context.params.onChange;
		fieldParams.fontId = context.params.fontId;

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

			context.uiManager.createElement(kNumericInputField, fieldPath)
				.setParameters(std::move(fieldParams))
				.draw();
		};
	},
};
