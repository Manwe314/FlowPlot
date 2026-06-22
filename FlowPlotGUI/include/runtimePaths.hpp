#pragma once

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#if defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

namespace FlowPlotGui {

inline std::optional<std::filesystem::path> executableDirectory()
{
#if defined(__linux__)
	char buffer[PATH_MAX]{};
	const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (length > 0)
	{
		buffer[length] = '\0';
		return std::filesystem::path(buffer).parent_path();
	}
#endif
	return std::nullopt;
}

inline std::vector<std::filesystem::path> resourceRoots()
{
	std::vector<std::filesystem::path> roots{};

#if defined(__linux__)
	if (const char* appDir = std::getenv("APPDIR"); appDir != nullptr && appDir[0] != '\0')
	{
		const std::filesystem::path appDirPath(appDir);
		roots.push_back(appDirPath / "usr" / "share" / "FlowPlotGUI");
		roots.push_back(appDirPath);
	}

	roots.emplace_back("/usr/share/FlowPlotGUI");
#endif

	if (const std::optional<std::filesystem::path> exeDir = executableDirectory())
	{
		roots.push_back(*exeDir);
		roots.push_back(*exeDir / "..");
		roots.push_back(*exeDir / ".." / "share" / "FlowPlotGUI");
	}

	roots.push_back(std::filesystem::current_path());
	roots.push_back(std::filesystem::current_path() / "..");
	roots.push_back(std::filesystem::current_path() / "share" / "FlowPlotGUI");
	roots.push_back(std::filesystem::current_path() / ".." / "share" / "FlowPlotGUI");

#ifdef FLOWPLOTGUI_SOURCE_ROOT
	roots.emplace_back(FLOWPLOTGUI_SOURCE_ROOT);
#endif

	return roots;
}

inline std::optional<std::filesystem::path> findResourcePath(const std::filesystem::path& relativePath)
{
	std::error_code ec;
	for (const std::filesystem::path& root : resourceRoots())
	{
		const std::filesystem::path candidate = root / relativePath;
		if (std::filesystem::exists(candidate, ec) && !ec)
			return candidate;
		ec.clear();
	}
	return std::nullopt;
}

inline std::filesystem::path resourcePathOrSource(
	const std::filesystem::path& relativePath,
	const std::filesystem::path& sourceFallback)
{
	if (std::optional<std::filesystem::path> path = findResourcePath(relativePath))
		return *path;
	return sourceFallback;
}

} // namespace FlowPlotGui
