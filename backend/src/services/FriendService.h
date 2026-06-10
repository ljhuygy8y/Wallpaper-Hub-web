#ifndef WM_SERVICES_FRIEND_SERVICE_H
#define WM_SERVICES_FRIEND_SERVICE_H

#include <string>
#include <vector>

#include "domain/Friend.h"

namespace wm {

class FriendService {
public:
    explicit FriendService(std::vector<Friend>& friendships);

    bool addFriendship(int userId, int friendId, std::string& error);
    bool areFriends(int userId, int friendId) const;
    std::vector<int> getFriendIds(int userId) const;

private:
    std::vector<Friend>& friendships_;

    static Friend normalize(int userId, int friendId);
};

}

#endif
