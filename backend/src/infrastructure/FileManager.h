#ifndef WM_INFRASTRUCTURE_FILE_MANAGER_H
#define WM_INFRASTRUCTURE_FILE_MANAGER_H

#include <string>
#include <vector>

#include "domain/Friend.h"
#include "domain/User.h"
#include "domain/Wallpaper.h"

namespace wm {

class FileManager {
public:
    explicit FileManager(std::string basePath = "data");

    bool loadUsers(std::vector<User>& users, std::string& error) const;
    bool saveUsers(const std::vector<User>& users, std::string& error) const;
    bool loadWallpapers(std::vector<Wallpaper>& wallpapers, std::string& error) const;
    bool saveWallpapers(const std::vector<Wallpaper>& wallpapers, std::string& error) const;
    bool loadFriends(std::vector<Friend>& friendships, std::string& error) const;
    bool saveFriends(const std::vector<Friend>& friendships, std::string& error) const;

private:
    std::string basePath_;

    std::string buildPath(const std::string& fileName) const;
    bool prepareStorage(const std::string& fileName, std::string& error) const;
};

}

#endif
