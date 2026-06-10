#ifndef WM_DOMAIN_USER_H
#define WM_DOMAIN_USER_H

#include <string>
#include <utility>

namespace wm {

class User {
public:
    User() = default;

    User(int userId, std::string userName, std::string userPassword,
         std::string displayName, std::string userLanguage, std::string userTheme)
        : id(userId),
          username(std::move(userName)),
          password(std::move(userPassword)),
          name(std::move(displayName)),
          language(std::move(userLanguage)),
          theme(std::move(userTheme)) {}

    int id{0};
    std::string username;
    std::string password;
    std::string name;
    std::string language;
    std::string theme{"dark"};
};

}

#endif
