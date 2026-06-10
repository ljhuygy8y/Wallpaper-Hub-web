#ifndef WM_SERVICES_WALLPAPER_SERVICE_H
#define WM_SERVICES_WALLPAPER_SERVICE_H

#include <string>
#include <vector>

#include "domain/Wallpaper.h"

namespace wm {

class WallpaperService {
public:
    explicit WallpaperService(std::vector<Wallpaper>& wallpapers);

    bool addWallpaper(int ownerUserId, const std::string& title,
                      const std::string& category, const std::string& filePath,
                      std::string& error);
    bool removeWallpaper(int ownerUserId, int wallpaperId, std::string& error);
    bool rateWallpaper(int ownerUserId, int wallpaperId, int rating, std::string& error);
    bool setWallpaper(int ownerUserId, int wallpaperId, std::string& error) const;
    bool setWallpaperPath(const std::string& filePath, std::string& error) const;

    std::vector<Wallpaper> listByOwner(int ownerUserId) const;
    std::vector<Wallpaper> listByOwnerAndCategory(int ownerUserId, const std::string& category) const;
    std::vector<std::string> listCategoriesByOwner(int ownerUserId) const;
    Wallpaper* findById(int wallpaperId);
    const Wallpaper* findById(int wallpaperId) const;

private:
    std::vector<Wallpaper>& wallpapers_;

    int nextId() const;
    static bool isBlank(const std::string& value);
    static std::string normalizeCategory(const std::string& category);
};

}

#endif
