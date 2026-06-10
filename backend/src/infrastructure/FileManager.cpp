#include "infrastructure/FileManager.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <utility>

namespace {

const char* USERS_FILE = "users.bin";
const char* WALLPAPERS_FILE = "wallpapers.bin";
const char* FRIENDS_FILE = "friends.bin";
const std::uint32_t WALLPAPERS_MAGIC = 0x57505232U;
const std::uint32_t WALLPAPERS_VERSION = 2U;

template <typename T>
bool writeValue(std::ofstream& file, const T& value, std::string& error) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    if (file.good()) {
        return true;
    }

    error = "Не вдалося записати бінарні дані.";
    return false;
}

template <typename T>
bool readValue(std::ifstream& file, T& value, std::string& error) {
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (file.good()) {
        return true;
    }

    error = "Пошкоджений бінарний файл.";
    return false;
}

bool writeString(std::ofstream& file, const std::string& value, std::string& error) {
    const std::uint32_t size = static_cast<std::uint32_t>(value.size());
    if (!writeValue(file, size, error)) {
        return false;
    }

    file.write(value.data(), static_cast<std::streamsize>(value.size()));
    if (file.good()) {
        return true;
    }

    error = "Не вдалося записати рядок у бінарний файл.";
    return false;
}

bool readString(std::ifstream& file, std::string& value, std::string& error) {
    std::uint32_t size = 0;
    if (!readValue(file, size, error)) {
        return false;
    }

    value.resize(size);
    if (size == 0U) {
        return true;
    }

    file.read(value.data(), static_cast<std::streamsize>(size));
    if (file.good()) {
        return true;
    }

    error = "Пошкоджений рядок у бінарному файлі.";
    return false;
}

bool isEmptyFile(const std::string& path) {
    std::error_code code;
    return std::filesystem::exists(path, code) &&
           std::filesystem::file_size(path, code) == 0U;
}

bool writeCount(std::ofstream& file, const std::size_t count, std::string& error) {
    if (count > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
        error = "Забагато записів для збереження.";
        return false;
    }

    return writeValue(file, static_cast<std::uint32_t>(count), error);
}

bool openForWrite(const std::string& path, std::ofstream& file, std::string& error) {
    file.open(path, std::ios::binary | std::ios::trunc);
    if (file) {
        return true;
    }

    error = "Не вдалося відкрити файл для запису: " + path;
    return false;
}

bool openForRead(const std::string& path, std::ifstream& file, std::string& error) {
    file.open(path, std::ios::binary);
    if (file) {
        return true;
    }

    error = "Не вдалося відкрити файл для читання: " + path;
    return false;
}

bool readWallpapers(std::ifstream& file, const std::uint32_t count,
                    const bool withRating, std::vector<wm::Wallpaper>& wallpapers,
                    std::string& error) {
    wallpapers.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        int id = 0;
        int ownerUserId = 0;
        int rating = 0;
        std::string title;
        std::string category;
        std::string filePath;

        if (!readValue(file, id, error) ||
            !readValue(file, ownerUserId, error) ||
            !readString(file, title, error) ||
            !readString(file, category, error) ||
            !readString(file, filePath, error)) {
            return false;
        }

        if (withRating && !readValue(file, rating, error)) {
            return false;
        }

        wallpapers.push_back(wm::Wallpaper(id, ownerUserId, title, category, filePath, rating));
    }

    return true;
}

}

namespace wm {

FileManager::FileManager(std::string basePath) : basePath_(std::move(basePath)) {}

std::string FileManager::buildPath(const std::string& fileName) const {
    return (std::filesystem::path(basePath_) / fileName).string();
}

bool FileManager::prepareStorage(const std::string& fileName, std::string& error) const {
    std::error_code code;
    std::filesystem::create_directories(basePath_, code);
    if (code) {
        error = "Не вдалося створити папку: " + basePath_;
        return false;
    }

    const std::string path = buildPath(fileName);
    if (std::filesystem::exists(path)) {
        return true;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        error = "Не вдалося створити файл: " + path;
        return false;
    }

    return true;
}

bool FileManager::loadUsers(std::vector<User>& users, std::string& error) const {
    users.clear();
    if (!prepareStorage(USERS_FILE, error)) return false;

    const std::string path = buildPath(USERS_FILE);
    if (isEmptyFile(path)) {
        return true;
    }

    std::ifstream file;
    if (!openForRead(path, file, error)) {
        return false;
    }

    std::uint32_t count = 0;
    if (!readValue(file, count, error)) {
        return false;
    }

    users.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        int id = 0;
        std::string username;
        std::string password;
        std::string name;
        std::string language;
        std::string theme;

        if (!readValue(file, id, error) ||
            !readString(file, username, error) ||
            !readString(file, password, error) ||
            !readString(file, name, error) ||
            !readString(file, language, error) ||
            !readString(file, theme, error)) {
            return false;
        }

        if (theme.empty()) {
            theme = "dark";
        }

        users.push_back(User(id, username, password, name, language, theme));
    }

    return true;
}

bool FileManager::loadWallpapers(std::vector<Wallpaper>& wallpapers, std::string& error) const {
    wallpapers.clear();
    if (!prepareStorage(WALLPAPERS_FILE, error)) return false;

    const std::string path = buildPath(WALLPAPERS_FILE);
    if (isEmptyFile(path)) {
        return true;
    }

    std::ifstream file;
    if (!openForRead(path, file, error)) {
        return false;
    }

    std::uint32_t firstValue = 0;
    if (!readValue(file, firstValue, error)) {
        return false;
    }

    if (firstValue == WALLPAPERS_MAGIC) {
        std::uint32_t version = 0;
        std::uint32_t count = 0;
        if (!readValue(file, version, error) || !readValue(file, count, error)) {
            return false;
        }

        if (version != WALLPAPERS_VERSION) {
            error = "Непідтримувана версія файлу шпалер.";
            return false;
        }

        return readWallpapers(file, count, true, wallpapers, error);
    }

    return readWallpapers(file, firstValue, false, wallpapers, error);
}

bool FileManager::loadFriends(std::vector<Friend>& friendships, std::string& error) const {
    friendships.clear();
    if (!prepareStorage(FRIENDS_FILE, error)) return false;

    const std::string path = buildPath(FRIENDS_FILE);
    if (isEmptyFile(path)) {
        return true;
    }

    std::ifstream file;
    if (!openForRead(path, file, error)) {
        return false;
    }

    std::uint32_t count = 0;
    if (!readValue(file, count, error)) {
        return false;
    }

    friendships.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        int userId = 0;
        int friendId = 0;
        if (!readValue(file, userId, error) ||
            !readValue(file, friendId, error)) {
            return false;
        }

        friendships.push_back(Friend(userId, friendId));
    }

    return true;
}

bool FileManager::saveUsers(const std::vector<User>& users, std::string& error) const {
    if (!prepareStorage(USERS_FILE, error)) return false;

    std::ofstream file;
    if (!openForWrite(buildPath(USERS_FILE), file, error)) {
        return false;
    }

    if (!writeCount(file, users.size(), error)) {
        return false;
    }

    for (const User& user : users) {
        if (!writeValue(file, user.id, error) ||
            !writeString(file, user.username, error) ||
            !writeString(file, user.password, error) ||
            !writeString(file, user.name, error) ||
            !writeString(file, user.language, error) ||
            !writeString(file, user.theme, error)) {
            return false;
        }
    }

    return true;
}

bool FileManager::saveWallpapers(const std::vector<Wallpaper>& wallpapers, std::string& error) const {
    if (!prepareStorage(WALLPAPERS_FILE, error)) return false;

    std::ofstream file;
    if (!openForWrite(buildPath(WALLPAPERS_FILE), file, error)) {
        return false;
    }

    if (!writeValue(file, WALLPAPERS_MAGIC, error) ||
        !writeValue(file, WALLPAPERS_VERSION, error) ||
        !writeCount(file, wallpapers.size(), error)) {
        return false;
    }

    for (const Wallpaper& wallpaper : wallpapers) {
        if (!writeValue(file, wallpaper.id, error) ||
            !writeValue(file, wallpaper.ownerUserId, error) ||
            !writeString(file, wallpaper.title, error) ||
            !writeString(file, wallpaper.category, error) ||
            !writeString(file, wallpaper.filePath, error) ||
            !writeValue(file, wallpaper.rating, error)) {
            return false;
        }
    }

    return true;
}

bool FileManager::saveFriends(const std::vector<Friend>& friendships, std::string& error) const {
    if (!prepareStorage(FRIENDS_FILE, error)) return false;

    std::ofstream file;
    if (!openForWrite(buildPath(FRIENDS_FILE), file, error)) {
        return false;
    }

    if (!writeCount(file, friendships.size(), error)) {
        return false;
    }

    for (const Friend& friendship : friendships) {
        if (!writeValue(file, friendship.userId, error) ||
            !writeValue(file, friendship.friendId, error)) {
            return false;
        }
    }

    return true;
}

}
