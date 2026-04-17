#pragma once

#include <devMode/devApi.hpp>

#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicTitle.hpp"
#include "BuildElements/PanelTitle.hpp"
#include "BuildElements/PlotviewPort.hpp"
#include "BuildElements/DataInput.hpp"
#include "BuildElements/DynamicSeparator.hpp"
#include "BuildElements/TemplateLayer.hpp"
#include "BuildElements/NavBar.hpp"

#include "ConstructElements/RootBackground.hpp"
#include "ConstructElements/MainContent.hpp"
#include "ConstructElements/MainContentPanel.hpp"
#include "ConstructElements/TemplatePanel.hpp"
#include "ConstructElements/PropsPanel.hpp"

FLOWUI_DEV_REGISTER_STRUCT(
	basicButtonParams,
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, text),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, contentMode),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(basicButtonParams, childLayoutDirection));
FLOWUI_DEV_REGISTER_ELEMENT(BasicButtonDef, "BasicButton");

FLOWUI_DEV_REGISTER_STRUCT(
	basicInputFieldParams,
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, defaultText),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, fontSize));
FLOWUI_DEV_REGISTER_ELEMENT(BasicInputFieldDef, "BasicInputField");

FLOWUI_DEV_REGISTER_STRUCT(
	basicTitleParams,
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, text),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, contentMode),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(basicTitleParams, fontSize));
FLOWUI_DEV_REGISTER_ELEMENT(BasicTitleDef, "BasicTitle");

FLOWUI_DEV_REGISTER_STRUCT(
	panelTitleParams,
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, minHeight),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, maxHeight),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, contentChildGap),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, leftColumnChildGap),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, titleText),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, titleFontId),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, titleFontSize),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, showSecondaryTitle),
	FLOWUI_DEV_REFLECT_FIELD(panelTitleParams, rightContentMode));
FLOWUI_DEV_REGISTER_ELEMENT(PanelTitleDef, "PanelTitle");

FLOWUI_DEV_REGISTER_STRUCT(plotviewPortParams);
FLOWUI_DEV_REGISTER_ELEMENT(PlotviewPortDef, "PlotviewPort");

FLOWUI_DEV_REGISTER_STRUCT(dataInputParams);
FLOWUI_DEV_REGISTER_ELEMENT(DataInputDef, "DataInput");

FLOWUI_DEV_REGISTER_STRUCT(
	templateLayerParams,
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, focused),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, leftSpacerMinWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, leftSpacerMaxWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentMinWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentMaxWidth),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainContentChildGap),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, showExpanderButton),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, showAdderButton),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainText),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextFontId),
	FLOWUI_DEV_REFLECT_FIELD(templateLayerParams, mainTextFontSize));
FLOWUI_DEV_REGISTER_STRUCT(
	templateLayerState,
	FLOWUI_DEV_REFLECT_FIELD(templateLayerState, isExpanded));
FLOWUI_DEV_REGISTER_ELEMENT(TemplateLayerDef, "TemplateLayer");

FLOWUI_DEV_REGISTER_STRUCT(
	navBarParams,
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, heightMin),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, heightMax),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer1MinWidth),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer1MaxWidth),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer2MinWidth),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer2MaxWidth));
FLOWUI_DEV_REGISTER_ELEMENT(NavBarDef, "NavBar");

FLOWUI_DEV_REGISTER_STRUCT(rootBackgroundParams);
FLOWUI_DEV_REGISTER_ELEMENT(RootBackgroundDef, "RootBackground");

FLOWUI_DEV_REGISTER_STRUCT(mainContentParams);
FLOWUI_DEV_REGISTER_ELEMENT(MainContentDef, "MainContent");

FLOWUI_DEV_REGISTER_STRUCT(mainContentPanelParams);
FLOWUI_DEV_REGISTER_ELEMENT(MainContentPanelDef, "MainContentPanel");

FLOWUI_DEV_REGISTER_STRUCT(
	templatePanelParams,
	FLOWUI_DEV_REFLECT_FIELD(templatePanelParams, maxWidth));
FLOWUI_DEV_REGISTER_STRUCT(
	templatePanelState,
	FLOWUI_DEV_REFLECT_FIELD(templatePanelState, minWidth));
FLOWUI_DEV_REGISTER_ELEMENT(TemplatePanelDef, "TemplatePanel");

FLOWUI_DEV_REGISTER_STRUCT(
	propsPanelParams,
	FLOWUI_DEV_REFLECT_FIELD(propsPanelParams, maxWidth));
FLOWUI_DEV_REGISTER_STRUCT(
	propsPanelState,
	FLOWUI_DEV_REFLECT_FIELD(propsPanelState, minWidth));
FLOWUI_DEV_REGISTER_ELEMENT(PropsPanelDef, "PropsPanel");
