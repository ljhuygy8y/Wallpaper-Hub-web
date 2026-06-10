#ifndef WM_PLATFORM_WINDOWS_WALLPAPER_API_H
#define WM_PLATFORM_WINDOWS_WALLPAPER_API_H

#include <string>

namespace wm {

class WindowsWallpaperApi {
public:
    static bool fileExists(const std::string& filePath);
    static bool setDesktopWallpaper(const std::string& filePath, std::string& error);
};

}

#endif
