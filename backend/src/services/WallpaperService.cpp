#include "services/WallpaperService.h"

#include <algorithm>
#include <cctype>
#include <set>

#include "platform/WindowsWallpaperApi.h"

namespace wm {

namespace {

bool compareByRating(const Wallpaper& a, const Wallpaper& b) {
    if (a.rating != b.rating) return a.rating > b.rating;
    return a.id < b.id;
}

}

WallpaperService::WallpaperService(std::vector<Wallpaper>& wallpapers) : wallpapers_(wallpapers) {}

bool WallpaperService::addWallpaper(const int ownerUserId, const std::string& title,
                                    const std::string& category, const std::string& filePath,
                                    std::string& error) {
    if (isBlank(title)) {
        error = "Invalid value.";
        return false;
    }
    if (isBlank(filePath)) {
        error = "Invalid value.";
        return false;
    }
    if (!WindowsWallpaperApi::fileExists(filePath)) {
        error = "Invalid value.";
        return false;
    }

    wallpapers_.push_back(Wallpaper(nextId(), ownerUserId, title, normalizeCategory(category), filePath));
    return true;
}

bool WallpaperService::removeWallpaper(const int ownerUserId, const int wallpaperId, std::string& error) {
    for (std::size_t i = 0; i < wallpapers_.size(); ++i) {
        if (wallpapers_[i].id == wallpaperId && wallpapers_[i].ownerUserId == ownerUserId) {
            wallpapers_.erase(wallpapers_.begin() + static_cast<std::ptrdiff_t>(i));
            return true;
        }
    }

    error = "Invalid value.";
    return false;
}

bool WallpaperService::rateWallpaper(const int ownerUserId, const int wallpaperId,
                                     const int rating, std::string& error) {
    if (rating < 1 || rating > 10) {
        error = "Invalid value.";
        return false;
    }

    Wallpaper* wallpaper = findById(wallpaperId);
    if (wallpaper == nullptr || wallpaper->ownerUserId != ownerUserId) {
        error = "Invalid value.";
        return false;
    }

    wallpaper->rating = rating;
    return true;
}

bool WallpaperService::setWallpaper(const int ownerUserId, const int wallpaperId, std::string& error) const {
    const Wallpaper* wallpaper = findById(wallpaperId);
    if (wallpaper == nullptr || wallpaper->ownerUserId != ownerUserId) {
        error = "Invalid value.";
        return false;
    }

    return setWallpaperPath(wallpaper->filePath, error);
}

bool WallpaperService::setWallpaperPath(const std::string& filePath, std::string& error) const {
    if (!WindowsWallpaperApi::fileExists(filePath)) {
        error = "Invalid value.";
        return false;
    }

    return WindowsWallpaperApi::setDesktopWallpaper(filePath, error);
}

std::vector<Wallpaper> WallpaperService::listByOwner(const int ownerUserId) const {
    std::vector<Wallpaper> result;

    for (const Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.ownerUserId == ownerUserId) result.push_back(wallpaper);
    }

    std::sort(result.begin(), result.end(), compareByRating);

    return result;
}

std::vector<Wallpaper> WallpaperService::listByOwnerAndCategory(const int ownerUserId,
                                                                const std::string& category) const {
    std::vector<Wallpaper> result;
    const std::string normalizedCategory = normalizeCategory(category);

    for (const Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.ownerUserId == ownerUserId && wallpaper.category == normalizedCategory) {
            result.push_back(wallpaper);
        }
    }

    std::sort(result.begin(), result.end(), compareByRating);

    return result;
}

std::vector<std::string> WallpaperService::listCategoriesByOwner(const int ownerUserId) const {
    std::set<std::string> categories;

    for (const Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.ownerUserId == ownerUserId) {
            categories.insert(wallpaper.category);
        }
    }

    return {categories.begin(), categories.end()};
}

Wallpaper* WallpaperService::findById(const int wallpaperId) {
    for (Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.id == wallpaperId) return &wallpaper;
    }
    return nullptr;
}

const Wallpaper* WallpaperService::findById(const int wallpaperId) const {
    for (const Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.id == wallpaperId) return &wallpaper;
    }
    return nullptr;
}

int WallpaperService::nextId() const {
    int id = 1;
    for (const Wallpaper& wallpaper : wallpapers_) {
        if (wallpaper.id >= id) id = wallpaper.id + 1;
    }
    return id;
}

bool WallpaperService::isBlank(const std::string& value) {
    for (const char ch : value) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) return false;
    }
    return true;
}

std::string WallpaperService::normalizeCategory(const std::string& category) {
    if (isBlank(category)) {
        return "Invalid value.";
    }

    std::string normalized = category;
    while (!normalized.empty() &&
           std::isspace(static_cast<unsigned char>(normalized.front())) != 0) {
        normalized.erase(normalized.begin());
    }
    while (!normalized.empty() &&
           std::isspace(static_cast<unsigned char>(normalized.back())) != 0) {
        normalized.pop_back();
    }

    return normalized.empty() ? "Invalid value." : normalized;
}

}
