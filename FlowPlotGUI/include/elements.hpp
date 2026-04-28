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
#include "BuildElements/propertiesContent.hpp"

#include "ConstructElements/RootBackground.hpp"
#include "ConstructElements/MainContent.hpp"
#include "ConstructElements/MainContentPanel.hpp"
#include "ConstructElements/TemplatePanel.hpp"
#include "ConstructElements/PropsPanel.hpp"

FLOWUI_DEV_REGISTER_ELEMENT(BasicButtonDef, "BasicButton");

FLOWUI_DEV_REGISTER_ELEMENT(BasicInputFieldDef, "BasicInputField");

FLOWUI_DEV_REGISTER_ELEMENT(BasicTitleDef, "BasicTitle");

FLOWUI_DEV_REGISTER_ELEMENT(PanelTitleDef, "PanelTitle");

FLOWUI_DEV_REGISTER_ELEMENT(PlotviewPortDef, "PlotviewPort");

FLOWUI_DEV_REGISTER_ELEMENT(DataInputDef, "DataInput");
FLOWUI_DEV_REGISTER_ELEMENT(DynamicSeparatorDef, "DynamicSeparator");

FLOWUI_DEV_REGISTER_ELEMENT(TemplateLayerDef, "TemplateLayer");

FLOWUI_DEV_REGISTER_ELEMENT(NavBarDef, "NavBar");

FLOWUI_DEV_REGISTER_ELEMENT(PropertiesContentDef, "PropertiesContent");

FLOWUI_DEV_REGISTER_ELEMENT(RootBackgroundDef, "RootBackground");

FLOWUI_DEV_REGISTER_ELEMENT(MainContentDef, "MainContent");

FLOWUI_DEV_REGISTER_ELEMENT(MainContentPanelDef, "MainContentPanel");

FLOWUI_DEV_REGISTER_ELEMENT(TemplatePanelDef, "TemplatePanel");

FLOWUI_DEV_REGISTER_ELEMENT(PropsPanelDef, "PropsPanel");
