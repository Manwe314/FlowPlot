#pragma once

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#if defined(__linux__)
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
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
#elif defined(__APPLE__)
	uint32_t size = 0;
	(void)_NSGetExecutablePath(nullptr, &size);
	if (size == 0)
		return std::nullopt;

	std::vector<char> buffer(size);
	if (_NSGetExecutablePath(buffer.data(), &size) == 0)
		return std::filesystem::path(buffer.data()).parent_path();
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
#elif defined(__APPLE__)
	if (const std::optional<std::filesystem::path> exeDir = executableDirectory())
		roots.push_back(*exeDir / ".." / "Resources");
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
