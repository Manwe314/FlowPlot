#include "templateImport.hpp"

#include "fontImport.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string_view>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <nfd_glfw3.h>

namespace FlowPlotGui {
namespace {

std::string readFileText(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("could not open " + path.string());
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>());
}

rapidjson::Document parseTemplateJson(std::string_view text)
{
	rapidjson::Document document;
	document.Parse(text.data(), text.size());
	if (document.HasParseError())
	{
		throw std::runtime_error(
			std::string("template JSON parse error at offset ")
			+ std::to_string(document.GetErrorOffset())
			+ ": "
			+ rapidjson::GetParseError_En(document.GetParseError()));
	}
	return document;
}

std::vector<std::filesystem::path> readExistingFontPaths(const rapidjson::Value& root)
{
	std::vector<std::filesystem::path> paths{};
	if (!root.IsObject())
	{
		return paths;
	}

	const auto fontsIt = root.FindMember("fonts");
	if (fontsIt == root.MemberEnd() || !fontsIt->value.IsArray())
	{
		return paths;
	}

	for (const rapidjson::Value& fontValue : fontsIt->value.GetArray())
	{
		const rapidjson::Value* pathValue = nullptr;
		if (fontValue.IsObject())
		{
			const auto pathIt = fontValue.FindMember("path");
			if (pathIt != fontValue.MemberEnd())
			{
				pathValue = &pathIt->value;
			}
		}
		else if (fontValue.IsString())
		{
			pathValue = &fontValue;
		}

		if (pathValue == nullptr || !pathValue->IsString())
		{
			continue;
		}

		std::filesystem::path fontPath(pathValue->GetString());
		std::error_code ec{};
		if (!std::filesystem::exists(fontPath, ec) || ec)
		{
			continue;
		}
		paths.push_back(std::filesystem::absolute(fontPath));
	}

	std::sort(paths.begin(), paths.end());
	paths.erase(std::unique(paths.begin(), paths.end()), paths.end());
	return paths;
}

void logImportResult(const TemplateImportResult& result)
{
	for (const std::string& error : result.errors)
	{
		std::fprintf(stderr, "[FlowPlotGUI] Template import: %s\n", error.c_str());
	}
	if (!result.path.empty() && result.errors.empty() && !result.cancelled)
	{
		std::fprintf(
			stderr,
			"[FlowPlotGUI] Imported template from %s\n",
			result.path.string().c_str());
	}
}

} // namespace

std::vector<RunningDataset> makeRunningDatasetsForTemplate(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate)
{
	std::vector<RunningDataset> datasets{};
	datasets.reserve(activeTemplate.datasets.size());

	for (const FlowPlot::Spec::DatasetSpec& datasetSpec : activeTemplate.datasets)
	{
		RunningDataset dataset{};
		dataset.name = datasetSpec.name;

		std::vector<std::string> fieldNames{};
		fieldNames.reserve(datasetSpec.schema.size());
		for (const auto& [fieldName, _] : datasetSpec.schema)
		{
			fieldNames.push_back(fieldName);
		}
		std::sort(fieldNames.begin(), fieldNames.end());

		for (const std::string& fieldName : fieldNames)
		{
			switch (datasetSpec.schema.at(fieldName))
			{
			case FlowPlot::Spec::DatasetSpec::FieldType::Number: {
				numericColumn column{};
				column.name = fieldName;
				column.data.push_back(0.0);
				dataset.numericColumns.push_back(std::move(column));
				break;
			}
			case FlowPlot::Spec::DatasetSpec::FieldType::String: {
				stringColumn column{};
				column.name = fieldName;
				column.data.emplace_back();
				dataset.stringColumns.push_back(std::move(column));
				break;
			}
			case FlowPlot::Spec::DatasetSpec::FieldType::Boolean: {
				boolColumn column{};
				column.name = fieldName;
				column.data.push_back(false);
				dataset.boolColumns.push_back(std::move(column));
				break;
			}
			}
		}

		datasets.push_back(std::move(dataset));
	}

	return datasets;
}

TemplateImportResult importTemplateFromPath(
	state& guiState,
	FontManager* fontManager,
	const std::filesystem::path& path)
{
	TemplateImportResult result{};
	result.path = path;

	try
	{
		const std::string jsonText = readFileText(path);
		rapidjson::Document document = parseTemplateJson(jsonText);
		FlowPlot::Spec::MasterTemplateSpec activeTemplate = FlowInternal::compileTemplateToSpec(document);
		std::vector<RunningDataset> runningDatasets = makeRunningDatasetsForTemplate(activeTemplate);

		const std::vector<std::filesystem::path> fontPaths = readExistingFontPaths(document);

		guiState.activeTemplate = std::move(activeTemplate);
		guiState.datasets = std::move(runningDatasets);
		guiState.selectedNode.reset();
		if (path.has_parent_path())
		{
			guiState.lastTemplateDialogDirectory = path.parent_path();
		}

		markTemplateChanged(guiState);
		markDatasetsChanged(guiState);
		markViewportChanged(guiState);

		if (!fontPaths.empty())
		{
			FontImportResult fontResult = importFontFiles(guiState, fontManager, fontPaths);
			result.importedFontCount = fontResult.addedCount;
			result.errors.insert(result.errors.end(), fontResult.errors.begin(), fontResult.errors.end());
		}
	}
	catch (const std::exception& e)
	{
		result.errors.emplace_back(e.what());
	}

	logImportResult(result);
	return result;
}

TemplateImportResult ImportTemplateWithImportDialog(
	state& guiState,
	FontManager* fontManager,
	void* nativeWindowHandle)
{
	TemplateImportResult result{};
	if (NFD_Init() != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "NFD initialization failed");
		logImportResult(result);
		return result;
	}

	NFD_SetDisplayPropertiesFromGLFW();

	nfdu8filteritem_t filters[] = {
		{"FlowPlot template", "json"},
	};
	std::string defaultPath;
	if (!guiState.lastTemplateDialogDirectory.empty())
	{
		defaultPath = guiState.lastTemplateDialogDirectory.string();
	}

	nfdopendialogu8args_t args{};
	args.filterList = filters;
	args.filterCount = 1;
	args.defaultPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
	if (nativeWindowHandle != nullptr)
	{
		NFD_GetNativeWindowFromGLFWWindow(static_cast<GLFWwindow*>(nativeWindowHandle), &args.parentWindow);
	}

	nfdu8char_t* rawPath = nullptr;
	const nfdresult_t dialogResult = NFD_OpenDialogU8_With(&rawPath, &args);
	if (dialogResult == NFD_CANCEL)
	{
		result.cancelled = true;
		NFD_Quit();
		return result;
	}
	if (dialogResult != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "template import dialog failed");
		logImportResult(result);
		NFD_Quit();
		return result;
	}
	if (rawPath == nullptr)
	{
		result.errors.emplace_back("template import dialog returned an empty path");
		logImportResult(result);
		NFD_Quit();
		return result;
	}

	const std::filesystem::path selectedPath(reinterpret_cast<const char*>(rawPath));
	NFD_FreePathU8(rawPath);
	NFD_Quit();

	return importTemplateFromPath(guiState, fontManager, selectedPath);
}

} // namespace FlowPlotGui
