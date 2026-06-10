#include "services/FriendService.h"

#include <algorithm>

namespace wm {

FriendService::FriendService(std::vector<Friend>& friendships) : friendships_(friendships) {}

bool FriendService::addFriendship(const int userId, const int friendId, std::string& error) {
    if (userId == friendId) {
        error = "Invalid value.";
        return false;
    }
    if (areFriends(userId, friendId)) {
        error = "Invalid value.";
        return false;
    }

    friendships_.push_back(normalize(userId, friendId));
    return true;
}

bool FriendService::areFriends(const int userId, const int friendId) const {
    const Friend normalized = normalize(userId, friendId);

    for (const Friend& item : friendships_) {
        if (item.userId == normalized.userId && item.friendId == normalized.friendId) {
            return true;
        }
    }

    return false;
}

std::vector<int> FriendService::getFriendIds(const int userId) const {
    std::vector<int> result;

    for (const Friend& item : friendships_) {
        if (item.userId == userId) result.push_back(item.friendId);
        if (item.friendId == userId) result.push_back(item.userId);
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

Friend FriendService::normalize(const int userId, const int friendId) {
    if (userId < friendId) return Friend(userId, friendId);
    return Friend(friendId, userId);
}

}
