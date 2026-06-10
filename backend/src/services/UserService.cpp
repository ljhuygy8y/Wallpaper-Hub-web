#include "services/UserService.h"

#include <algorithm>
#include <cctype>

namespace wm {

UserService::UserService(std::vector<User>& users) : users_(users) {}

bool UserService::registerUser(const std::string& username, const std::string& password,
                               const std::string& displayName, const std::string& language,
                               const std::string& theme, std::string& error) {
    if (!validateUsername(username, error)) return false;
    if (!validatePassword(password, "Invalid value.", error)) return false;
    if (!validateProfile(displayName, language, error)) return false;

    if (usernameExists(username)) {
        error = "Invalid value.";
        return false;
    }

    const std::string normalTheme = normalizeTheme(theme);
    if (normalTheme.empty()) {
        error = "Invalid value.";
        return false;
    }

    users_.push_back(User(nextId(), username, password, displayName, language, normalTheme));
    return true;
}

const User* UserService::login(const std::string& username, const std::string& password) const {
    const std::string loginName = toLower(username);

    for (const User& user : users_) {
        if (toLower(user.username) == loginName && user.password == password) {
            return &user;
        }
    }

    return nullptr;
}

User* UserService::findById(const int userId) {
    for (User& user : users_) {
        if (user.id == userId) return &user;
    }
    return nullptr;
}

const User* UserService::findById(const int userId) const {
    for (const User& user : users_) {
        if (user.id == userId) return &user;
    }
    return nullptr;
}

std::vector<User> UserService::search(const std::string& query, const int excludeUserId) const {
    std::vector<User> result;
    const std::string text = toLower(query);

    for (const User& user : users_) {
        const bool current = user.id == excludeUserId;
        const bool found = text.empty() ||
            toLower(user.username).find(text) != std::string::npos ||
            toLower(user.name).find(text) != std::string::npos;

        if (!current && found) {
            result.push_back(user);
        }
    }

    std::sort(result.begin(), result.end(), [](const User& a, const User& b) {
        return UserService::toLower(a.username) < UserService::toLower(b.username);
    });

    return result;
}

bool UserService::updateProfile(const int userId, const std::string& username,
                                const std::string& displayName, const std::string& language,
                                std::string& error) {
    User* user = findById(userId);
    if (user == nullptr) {
        error = "Invalid value.";
        return false;
    }
    if (!validateUsername(username, error)) return false;
    if (!validateProfile(displayName, language, error)) return false;

    if (usernameExists(username, userId)) {
        error = "Invalid value.";
        return false;
    }

    user->username = username;
    user->name = displayName;
    user->language = language;
    return true;
}

bool UserService::updatePassword(const int userId, const std::string& currentPassword,
                                 const std::string& newPassword, std::string& error) {
    User* user = findById(userId);
    if (user == nullptr) {
        error = "Invalid value.";
        return false;
    }
    if (user->password != currentPassword) {
        error = "Invalid value.";
        return false;
    }
    if (!validatePassword(newPassword, "Invalid value.", error)) return false;

    user->password = newPassword;
    return true;
}

bool UserService::updateLanguage(const int userId, const std::string& language, std::string& error) {
    User* user = findById(userId);
    if (user == nullptr) {
        error = "Invalid value.";
        return false;
    }
    if (isBlank(language)) {
        error = "Invalid value.";
        return false;
    }

    user->language = language;
    return true;
}

bool UserService::updateTheme(const int userId, const std::string& theme, std::string& error) {
    User* user = findById(userId);
    if (user == nullptr) {
        error = "Invalid value.";
        return false;
    }

    const std::string normalTheme = normalizeTheme(theme);
    if (normalTheme.empty()) {
        error = "Invalid value.";
        return false;
    }

    user->theme = normalTheme;
    return true;
}

int UserService::nextId() const {
    int id = 1;
    for (const User& user : users_) {
        if (user.id >= id) id = user.id + 1;
    }
    return id;
}

bool UserService::usernameExists(const std::string& username, const int excludeUserId) const {
    const std::string searchedName = toLower(username);

    for (const User& user : users_) {
        if (user.id != excludeUserId && toLower(user.username) == searchedName) {
            return true;
        }
    }

    return false;
}

bool UserService::isBlank(const std::string& value) {
    for (const char ch : value) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) return false;
    }
    return true;
}

bool UserService::hasWhitespace(const std::string& value) {
    for (const char ch : value) {
        if (std::isspace(static_cast<unsigned char>(ch)) != 0) return true;
    }
    return false;
}

bool UserService::validateUsername(const std::string& username, std::string& error) {
    if (isBlank(username)) {
        error = "Invalid value.";
        return false;
    }
    if (username.size() < 3U) {
        error = "Invalid value.";
        return false;
    }
    if (hasWhitespace(username)) {
        error = "Invalid value.";
        return false;
    }
    return true;
}

bool UserService::validatePassword(const std::string& password, const std::string& label, std::string& error) {
    if (isBlank(password)) {
        error = label + "Invalid value.";
        return false;
    }
    if (password.size() < 4U) {
        error = label + "Invalid value.";
        return false;
    }
    return true;
}

bool UserService::validateProfile(const std::string& displayName,
                                  const std::string& language, std::string& error) {
    if (isBlank(displayName)) {
        error = "Invalid value.";
        return false;
    }
    if (isBlank(language)) {
        error = "Invalid value.";
        return false;
    }
    return true;
}

std::string UserService::normalizeTheme(const std::string& theme) {
    const std::string lowered = toLower(theme);
    if (lowered == "light" || lowered == "dark") return lowered;
    return "";
}

std::string UserService::toLower(const std::string& value) {
    std::string result = value;
    for (char& ch : result) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

}
