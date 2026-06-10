#ifndef WM_DOMAIN_WALLPAPER_H
#define WM_DOMAIN_WALLPAPER_H

#include <string>
#include <utility>

namespace wm {

class Wallpaper {
public:
    Wallpaper() = default;

    Wallpaper(int wallpaperId, int userId, std::string wallpaperTitle,
              std::string wallpaperCategory, std::string wallpaperPath,
              int wallpaperRating = 0)
        : id(wallpaperId),
          ownerUserId(userId),
          title(std::move(wallpaperTitle)),
          category(std::move(wallpaperCategory)),
          filePath(std::move(wallpaperPath)),
          rating(wallpaperRating) {}

    int id{0};
    int ownerUserId{0};
    std::string title;
    std::string category;
    std::string filePath;
    int rating{0};
};

}

#endif
