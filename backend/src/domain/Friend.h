#ifndef WM_DOMAIN_FRIEND_H
#define WM_DOMAIN_FRIEND_H

namespace wm {

class Friend {
public:
    Friend() = default;
    Friend(int firstUserId, int secondUserId) : userId(firstUserId), friendId(secondUserId) {}

    int userId{0};
    int friendId{0};
};

}

#endif
