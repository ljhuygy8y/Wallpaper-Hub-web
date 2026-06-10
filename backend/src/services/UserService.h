#ifndef WM_SERVICES_USER_SERVICE_H
#define WM_SERVICES_USER_SERVICE_H

#include <string>
#include <vector>

#include "domain/User.h"

namespace wm {

class UserService {
public:
    explicit UserService(std::vector<User>& users);

    bool registerUser(const std::string& username, const std::string& password,
                      const std::string& displayName, const std::string& language,
                      const std::string& theme, std::string& error);

    const User* login(const std::string& username, const std::string& password) const;
    User* findById(int userId);
    const User* findById(int userId) const;
    std::vector<User> search(const std::string& query, int excludeUserId = -1) const;

    bool updateProfile(int userId, const std::string& username,
                       const std::string& displayName, const std::string& language,
                       std::string& error);
    bool updatePassword(int userId, const std::string& currentPassword,
                        const std::string& newPassword, std::string& error);
    bool updateLanguage(int userId, const std::string& language, std::string& error);
    bool updateTheme(int userId, const std::string& theme, std::string& error);

private:
    std::vector<User>& users_;

    int nextId() const;
    bool usernameExists(const std::string& username, int excludeUserId = -1) const;

    static bool isBlank(const std::string& value);
    static bool hasWhitespace(const std::string& value);
    static bool validateUsername(const std::string& username, std::string& error);
    static bool validatePassword(const std::string& password, const std::string& label, std::string& error);
    static bool validateProfile(const std::string& displayName, const std::string& language, std::string& error);
    static std::string normalizeTheme(const std::string& theme);
    static std::string toLower(const std::string& value);
};

}

#endif
