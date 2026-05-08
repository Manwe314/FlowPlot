#pragma once

#include <devMode/devApi.hpp>

#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicTitle.hpp"
#include "BuildElements/BoolToggle.hpp"
#include "BuildElements/PanelTitle.hpp"
#include "BuildElements/PlotviewPort.hpp"
#include "BuildElements/DataInput.hpp"
#include "BuildElements/DynamicSeparator.hpp"
#include "BuildElements/NewTemplatePresetPicker.hpp"
#include "BuildElements/TemplateLayer.hpp"
#include "BuildElements/NavBar.hpp"
#include "BuildElements/propertiesContent.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "BuildElements/editors/basicInputs/stringInput.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/doubleEnumPicker.hpp"
#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/twoColumnInput.hpp"
#include "BuildElements/editors/basicInputs/nineSplitInput.hpp"
#include "BuildElements/editors/textSpecEditor.hpp"
#include "BuildElements/editors/layoutEditor.hpp"
#include "BuildElements/editors/gridSettingsEditor.hpp"
#include "BuildElements/editors/lineSettingsEditor.hpp"
#include "BuildElements/editors/tickSettingsEditor.hpp"
#include "BuildElements/editors/variableTypeSettingsEditor.hpp"
#include "BuildElements/editors/scatterColorMappingSettings.hpp"
#include "BuildElements/editors/scatterSizeMappingSettings.hpp"
#include "BuildElements/editors/histogramColorMappingSettings.hpp"
#include "BuildElements/editors/scatterMappingEditor.hpp"
#include "BuildElements/editors/scatterStyleEditor.hpp"
#include "BuildElements/editors/scatterStatsEditor.hpp"
#include "BuildElements/editors/scatterConfigEditor.hpp"
#include "BuildElements/editors/histogramMappingEditor.hpp"
#include "BuildElements/editors/histogramStyleEditor.hpp"
#include "BuildElements/editors/histogramStatsEditor.hpp"
#include "BuildElements/editors/histogramConfigEditor.hpp"

#include "ConstructElements/RootBackground.hpp"
#include "ConstructElements/MainContent.hpp"
#include "ConstructElements/MainContentPanel.hpp"
#include "ConstructElements/TemplatePanel.hpp"
#include "ConstructElements/PropsPanel.hpp"

FLOWUI_DEV_REGISTER_ELEMENT(BasicButtonDef, "BasicButton");

FLOWUI_DEV_REGISTER_ELEMENT(BasicInputFieldDef, "BasicInputField");

FLOWUI_DEV_REGISTER_ELEMENT(BoolToggleDef, "BoolToggle");

FLOWUI_DEV_REGISTER_ELEMENT(BasicTitleDef, "BasicTitle");

FLOWUI_DEV_REGISTER_ELEMENT(PanelTitleDef, "PanelTitle");

FLOWUI_DEV_REGISTER_ELEMENT(PlotviewPortDef, "PlotviewPort");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputDef, "DataInput");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputHeaderDef, "DataInputHeader");

FLOWUI_DEV_REGISTER_ELEMENT(DataTabDef, "DataTab");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputTableDef, "DataInputTable");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputHeaderRowDef, "DataInputHeaderRow");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputDataRowDef, "DataInputDataRow");

FLOWUI_DEV_REGISTER_ELEMENT(DynamicSeparatorDef, "DynamicSeparator");

FLOWUI_DEV_REGISTER_ELEMENT(NewTemplatePresetPickerDef, "NewTemplatePresetPicker");

FLOWUI_DEV_REGISTER_ELEMENT(TemplateLayerDef, "TemplateLayer");

FLOWUI_DEV_REGISTER_ELEMENT(NavBarDef, "NavBar");

FLOWUI_DEV_REGISTER_ELEMENT(PropertiesContentDef, "PropertiesContent");

FLOWUI_DEV_REGISTER_ELEMENT(ToggleSwitchDef, "ToggleSwitch");

FLOWUI_DEV_REGISTER_ELEMENT(ToggleCardDef, "ToggleCard");

FLOWUI_DEV_REGISTER_ELEMENT(StringInputCardDef, "StringInputCard");

FLOWUI_DEV_REGISTER_ELEMENT(EnumPickerMenuDef, "EnumPickerMenu");

FLOWUI_DEV_REGISTER_ELEMENT(EnumPickerCardDef, "EnumPickerCard");

FLOWUI_DEV_REGISTER_ELEMENT(NumericInputFieldDef, "NumericInputField");

FLOWUI_DEV_REGISTER_ELEMENT(NumericInputCardDef, "NumericInputCard");

FLOWUI_DEV_REGISTER_ELEMENT(DoubleEnumPickerCardDef, "DoubleEnumPickerCard");

FLOWUI_DEV_REGISTER_ELEMENT(SliderDef, "Slider");

FLOWUI_DEV_REGISTER_ELEMENT(ColorPickerPreviewButtonDef, "ColorPickerPreviewButton");

FLOWUI_DEV_REGISTER_ELEMENT(ColorPickerSwatchDef, "ColorPickerSwatch");

FLOWUI_DEV_REGISTER_ELEMENT(ColorPickerCardDef, "ColorPickerCard");

FLOWUI_DEV_REGISTER_ELEMENT(TwoColumnInputTableDef, "TwoColumnInputTable");

FLOWUI_DEV_REGISTER_ELEMENT(TwoColumnInputCardDef, "TwoColumnInputCard");

FLOWUI_DEV_REGISTER_ELEMENT(NineSplitMatrixDef, "NineSplitMatrix");

FLOWUI_DEV_REGISTER_ELEMENT(NineSplitCardDef, "NineSplitCard");

FLOWUI_DEV_REGISTER_ELEMENT(TextSpecEditorDef, "TextSpecEditor");

FLOWUI_DEV_REGISTER_ELEMENT(LayoutEditorDef, "LayoutEditor");

FLOWUI_DEV_REGISTER_ELEMENT(GridSettingsEditorDef, "GridSettingsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(LineSettingsEditorDef, "LineSettingsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(TickSettingsEditorDef, "TickSettingsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(VariableTypeSettingsEditorDef, "VariableTypeSettingsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterColorMappingSettingsDef, "ScatterColorMappingSettings");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterSizeMappingSettingsDef, "ScatterSizeMappingSettings");

FLOWUI_DEV_REGISTER_ELEMENT(HistogramColorMappingSettingsDef, "HistogramColorMappingSettings");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterMappingSettingsEditorDef, "ScatterMappingEditor");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterStyleSettingsEditorDef, "ScatterStyleEditor");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterStatsSettingsEditorDef, "ScatterStatsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(ScatterConfigSettingsEditorDef, "ScatterConfigEditor");

FLOWUI_DEV_REGISTER_ELEMENT(HistogramMappingSettingsEditorDef, "HistogramMappingEditor");

FLOWUI_DEV_REGISTER_ELEMENT(HistogramStyleSettingsEditorDef, "HistogramStyleEditor");

FLOWUI_DEV_REGISTER_ELEMENT(HistogramStatsSettingsEditorDef, "HistogramStatsEditor");

FLOWUI_DEV_REGISTER_ELEMENT(HistogramConfigSettingsEditorDef, "HistogramConfigEditor");

FLOWUI_DEV_REGISTER_ELEMENT(RootBackgroundDef, "RootBackground");

FLOWUI_DEV_REGISTER_ELEMENT(MainContentDef, "MainContent");

FLOWUI_DEV_REGISTER_ELEMENT(MainContentPanelDef, "MainContentPanel");

FLOWUI_DEV_REGISTER_ELEMENT(TemplatePanelDef, "TemplatePanel");

FLOWUI_DEV_REGISTER_ELEMENT(PropsPanelDef, "PropsPanel");
