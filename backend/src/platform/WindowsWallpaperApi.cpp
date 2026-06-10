#include "platform/WindowsWallpaperApi.h"

#include <filesystem>

namespace wm {

bool WindowsWallpaperApi::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath);
}

bool WindowsWallpaperApi::setDesktopWallpaper(const std::string& filePath, std::string& error) {
    (void)filePath;
    error = "Changing the desktop wallpaper is disabled in the server build.";
    return false;
}

}
