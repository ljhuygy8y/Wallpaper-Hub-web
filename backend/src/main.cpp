#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cctype>

#include "crow.h"
#include "domain/User.h"
#include "domain/Wallpaper.h"
#include "domain/Friend.h"
#include "infrastructure/FileManager.h"
#include "services/UserService.h"
#include "services/WallpaperService.h"
#include "services/FriendService.h"

struct CORSMiddleware {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        if (req.method == crow::HTTPMethod::Options) {
            res.add_header("Access-Control-Allow-Origin", "http://127.0.0.1:8080");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.code = 200;
            res.end();
        }
    }
    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "http://127.0.0.1:8080");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

std::string read_static_file(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file && path.rfind("public/", 0) == 0) {
        file.clear();
        file.open("frontend/" + path.substr(7), std::ios::in | std::ios::binary);
    }
    if (!file) return "";
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

// --- helpers: base64 decode, file write, mime detection ---
static const std::string b64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64_char(unsigned char c) {
    return std::isalnum(c) || (c == '+') || (c == '/');
}

static std::vector<unsigned char> base64_decode(const std::string& input) {
    std::vector<unsigned char> out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < (int)b64_chars.size(); ++i) T[(unsigned char)b64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) {
            if (c == '=') break;
            continue;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static bool write_binary_file(const std::string& path, const std::vector<unsigned char>& data) {
    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(path).parent_path(), ec);
    std::ofstream of(path, std::ios::binary);
    if (!of) return false;
    of.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    return of.good();
}

static std::string content_type_for_path(const std::string& path) {
    std::string ext = std::filesystem::path(path).extension().string();
    for (auto & c: ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    if (ext == ".webp") return "image/webp";
    if (ext == ".svg") return "image/svg+xml";
    return "image/jpeg";
}

static bool is_allowed_image(const std::string& extension,
                             const std::vector<unsigned char>& data) {
    if ((extension == ".jpg" || extension == ".jpeg") && data.size() >= 3U) {
        return data[0] == 0xFFU && data[1] == 0xD8U && data[2] == 0xFFU;
    }
    if (extension == ".png" && data.size() >= 8U) {
        const unsigned char signature[] = {0x89U, 0x50U, 0x4EU, 0x47U, 0x0DU, 0x0AU, 0x1AU, 0x0AU};
        return std::equal(std::begin(signature), std::end(signature), data.begin());
    }
    if (extension == ".gif" && data.size() >= 6U) {
        const std::string header(data.begin(), data.begin() + 6);
        return header == "GIF87a" || header == "GIF89a";
    }
    if (extension == ".webp" && data.size() >= 12U) {
        return std::equal(data.begin(), data.begin() + 4, "RIFF") &&
               std::equal(data.begin() + 8, data.begin() + 12, "WEBP");
    }
    return false;
}

static std::string detect_image_extension(const std::vector<unsigned char>& data) {
    if (data.size() >= 3U) {
        if (data[0] == 0xFFU && data[1] == 0xD8U && data[2] == 0xFFU) return ".jpg";
    }
    if (data.size() >= 8U) {
        const unsigned char png_sig[] = {0x89U, 0x50U, 0x4EU, 0x47U, 0x0DU, 0x0AU, 0x1AU, 0x0AU};
        if (std::equal(std::begin(png_sig), std::end(png_sig), data.begin())) return ".png";
    }
    if (data.size() >= 6U) {
        const std::string header(data.begin(), data.begin() + 6);
        if (header == "GIF87a" || header == "GIF89a") return ".gif";
    }
    if (data.size() >= 12U) {
        if (std::equal(data.begin(), data.begin() + 4, "RIFF") &&
            std::equal(data.begin() + 8, data.begin() + 12, "WEBP")) return ".webp";
    }
    return std::string();
}

int main() {
    std::vector<wm::User> users;
    std::vector<wm::Wallpaper> wallpapers;
    std::vector<wm::Friend> friendships;

    wm::FileManager fileManager("data");
    wm::UserService userService(users);
    wm::WallpaperService wallpaperService(wallpapers);
    wm::FriendService friendService(friendships);

    std::string error;
    std::mutex data_mutex;

    if (!fileManager.loadUsers(users, error) ||
        !fileManager.loadWallpapers(wallpapers, error) ||
        !fileManager.loadFriends(friendships, error)) {
        std::cerr << "Помилка завантаження: " << error << std::endl;
        return 1;
    }

    crow::App<CORSMiddleware> app;
    // Explicit OPTIONS handlers to ensure browser preflight requests succeed
    CROW_ROUTE(app, "/api/register").methods(crow::HTTPMethod::Options)([](const crow::request& req){
        crow::response res;
        res.set_header("Access-Control-Allow-Origin", "http://127.0.0.1:8080");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.code = 200;
        return res;
    });

    CROW_ROUTE(app, "/api/wallpapers/upload").methods(crow::HTTPMethod::Options)([](const crow::request& req){
        crow::response res;
        res.set_header("Access-Control-Allow-Origin", "http://127.0.0.1:8080");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.code = 200;
        return res;
    });

    CROW_ROUTE(app, "/api/wallpapers/<int>/rate").methods(crow::HTTPMethod::Options)([](const crow::request& req, int){
        crow::response res;
        res.set_header("Access-Control-Allow-Origin", "http://127.0.0.1:8080");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.code = 200;
        return res;
    });


    CROW_ROUTE(app, "/")([]() {
        auto res = crow::response(read_static_file("public/index.html"));
        res.set_header("Content-Type", "text/html; charset=utf-8");
        return res;
    });
    CROW_ROUTE(app, "/style.css")([]() { 
        auto res = crow::response(read_static_file("public/style.css"));
        res.set_header("Content-Type", "text/css");
        return res;
    });
    CROW_ROUTE(app, "/script.js")([]() { 
        auto res = crow::response(read_static_file("public/script.js"));
        res.set_header("Content-Type", "application/javascript; charset=utf-8");
        return res;
    });
    CROW_ROUTE(app, "/samples/<string>")([](const std::string& fileName) {
        if (fileName.find("..") != std::string::npos ||
            fileName.find('/') != std::string::npos ||
            fileName.find('\\') != std::string::npos) {
            return crow::response(400);
        }
        const std::string data = read_static_file("frontend/samples/" + fileName);
        if (data.empty()) return crow::response(404);
        crow::response res(data);
        res.set_header("Content-Type", content_type_for_path(fileName));
        return res;
    });

    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("username") || !x.has("password")) return crow::response(400);

        std::lock_guard<std::mutex> lock(data_mutex);
        const wm::User* user = userService.login(x["username"].s(), x["password"].s());
        if (!user) return crow::response(401, "Invalid credentials");

        crow::json::wvalue res;
        res["status"] = "success";
        res["user"]["userId"] = user->id;
        res["user"]["username"] = user->username;
        res["user"]["name"] = user->name;
        res["user"]["language"] = user->language;
        res["user"]["theme"] = user->theme;
        return crow::response(res);
    });

    // Registration
    CROW_ROUTE(app, "/api/register").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("username") || !x.has("password") || !x.has("name")) return crow::response(400);

        std::lock_guard<std::mutex> lock(data_mutex);
        std::string err;
        const std::string lang = x.has("language") ? x["language"].s() : std::string("uk");
        const std::string theme = x.has("theme") ? x["theme"].s() : std::string("dark");

        if (!userService.registerUser(x["username"].s(), x["password"].s(), x["name"].s(), lang, theme, err)) {
            return crow::response(400, err);
        }

        if (!fileManager.saveUsers(users, err)) {
            return crow::response(500, "Failed to persist users");
        }

        const wm::User* u = userService.login(x["username"].s(), x["password"].s());
        crow::json::wvalue res;
        res["status"] = "success";
        if (u) {
            res["user"]["userId"] = u->id;
            res["user"]["username"] = u->username;
            res["user"]["name"] = u->name;
            res["user"]["language"] = u->language;
            res["user"]["theme"] = u->theme;
        }
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/friends/<int>")([&](int userId) {
        std::lock_guard<std::mutex> lock(data_mutex);
        std::vector<int> friendIds = friendService.getFriendIds(userId);
        crow::json::wvalue res = crow::json::wvalue::list();
        int idx = 0;
        for (int fId : friendIds) {
            const wm::User* u = userService.findById(fId);
            if (u) {
                res[idx]["id"] = u->id;
                res[idx]["name"] = u->name;
                res[idx]["username"] = u->username;
                idx++;
            }
        }
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/friends").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("userId") || !x.has("friendId")) {
            return crow::response(400, "userId and friendId are required");
        }
        const int userId = x["userId"].i();
        const int friendId = x["friendId"].i();
        std::lock_guard<std::mutex> lock(data_mutex);
        if (userService.findById(userId) == nullptr || userService.findById(friendId) == nullptr) {
            return crow::response(404, "User not found");
        }
        std::string err;
        if (!friendService.addFriendship(userId, friendId, err)) return crow::response(400, err);
        if (!fileManager.saveFriends(friendships, err)) {
            return crow::response(500, "Failed to persist friendship");
        }
        crow::json::wvalue res;
        res["status"] = "success";
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/users/<int>")([&](int userId) {
        std::lock_guard<std::mutex> lock(data_mutex);
        const wm::User* user = userService.findById(userId);
        if (user == nullptr) return crow::response(404, "User not found");
        crow::json::wvalue res;
        res["id"] = user->id;
        res["name"] = user->name;
        res["username"] = user->username;
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/wallpapers")([&]() {
        std::lock_guard<std::mutex> lock(data_mutex);
        std::vector<wm::Wallpaper> list = wallpapers;
        std::sort(list.begin(), list.end(), [](const wm::Wallpaper& a, const wm::Wallpaper& b) {
            if (a.category != b.category) return a.category < b.category;
            if (a.title != b.title) return a.title < b.title;
            return a.id < b.id;
        });
        crow::json::wvalue res = crow::json::wvalue::list();
        int idx = 0;
        for (const auto& wp : list) {
            res[idx]["id"] = wp.id;
            res[idx]["ownerUserId"] = wp.ownerUserId;
            res[idx]["title"] = wp.title;
            res[idx]["category"] = wp.category;
            res[idx]["filePath"] = wp.filePath;
            res[idx]["rating"] = wp.rating;
            idx++;
        }
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/wallpapers/<int>")([&](const crow::request& req, int userId) {
        std::lock_guard<std::mutex> lock(data_mutex);
        const char* cat = req.url_params.get("category");
        std::vector<wm::Wallpaper> list;
        if (cat && std::strlen(cat) > 0) {
            list = wallpaperService.listByOwnerAndCategory(userId, std::string(cat));
        } else {
            list = wallpaperService.listByOwner(userId);
        }
        crow::json::wvalue res = crow::json::wvalue::list();
        int idx = 0;
        for (const auto& wp : list) {
            res[idx]["id"] = wp.id;
            res[idx]["ownerUserId"] = wp.ownerUserId;
            res[idx]["title"] = wp.title;
            res[idx]["category"] = wp.category;
            res[idx]["filePath"] = wp.filePath;
            res[idx]["rating"] = wp.rating;
            idx++;
        }
        return crow::response(res);
    });

    // --- ��� ֲ ��� �������� ����������� ��ί �������� ---

    // 1. ��������� ������ (���������� � ����)
    CROW_ROUTE(app, "/api/user/<int>").methods(crow::HTTPMethod::Put)([&](const crow::request& req, int userId) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("name") || !x.has("username")) {
            return crow::response(400, "name and username are required");
        }
        std::lock_guard<std::mutex> lock(data_mutex);
        const wm::User* current = userService.findById(userId);
        if (current == nullptr) return crow::response(404, "User not found");
        std::string err;
        const std::string language = x.has("language") ? x["language"].s() : current->language;
        if (!userService.updateProfile(userId, x["username"].s(), x["name"].s(), language, err)) {
            return crow::response(400, err);
        }
        if (!fileManager.saveUsers(users, err)) return crow::response(500, "Failed to persist user");
        const wm::User* updated = userService.findById(userId);
        crow::json::wvalue res;
        res["user"]["userId"] = updated->id;
        res["user"]["username"] = updated->username;
        res["user"]["name"] = updated->name;
        res["user"]["language"] = updated->language;
        res["user"]["theme"] = updated->theme;
        return crow::response(res);
    });

    CROW_ROUTE(app, "/api/user/<int>/password").methods(crow::HTTPMethod::Put)(
        [&](const crow::request& req, int userId) {
            auto x = crow::json::load(req.body);
            if (!x || !x.has("currentPassword") || !x.has("newPassword")) {
                return crow::response(400, "Both passwords are required");
            }
            std::lock_guard<std::mutex> lock(data_mutex);
            std::string err;
            if (!userService.updatePassword(userId, x["currentPassword"].s(), x["newPassword"].s(), err)) {
                return crow::response(400, err);
            }
            if (!fileManager.saveUsers(users, err)) return crow::response(500, "Failed to persist password");
            crow::json::wvalue res;
            res["status"] = "success";
            return crow::response(res);
        });

    // 2. ����� �����
    CROW_ROUTE(app, "/api/users/search")([&](const crow::request& req) {
        const char* queryParam = req.url_params.get("q");
        const char* excludeParam = req.url_params.get("exclude");
        const std::string query = queryParam == nullptr ? "" : queryParam;
        int excludeUserId = -1;
        if (excludeParam != nullptr) {
            try { excludeUserId = std::stoi(excludeParam); } catch (...) { excludeUserId = -1; }
        }
        std::lock_guard<std::mutex> lock(data_mutex);
        const std::vector<wm::User> found = userService.search(query, excludeUserId);
        crow::json::wvalue res = crow::json::wvalue::list();
        int idx = 0;
        for (const auto& u : found) {
            res[idx]["id"] = u.id;
            res[idx]["username"] = u.username;
            res[idx]["name"] = u.name;
            res[idx]["isFriend"] = excludeUserId > 0 && friendService.areFriends(excludeUserId, u.id);
            idx++;
        }
        return crow::response(res);
    });

    // 3. ³����� ��������� �� ��������
    CROW_ROUTE(app, "/api/wallpapers/image/<int>")([&](int wallpaperId) {
        std::lock_guard<std::mutex> lock(data_mutex);
        std::string foundPath = "";
        for (const auto& wp : wallpapers) {
            if (wp.id == wallpaperId) { foundPath = wp.filePath; break; }
        }
        std::string img_data;
        if (!foundPath.empty()) img_data = read_static_file(foundPath);

        if (img_data.empty()) {
            // return a simple SVG placeholder
            const std::string svg =
                "<svg xmlns='http://www.w3.org/2000/svg' width='400' height='250'>"
                "<rect width='100%' height='100%' fill='#1f2933'/>"
                "<text x='50%' y='50%' fill='#9ca3af' font-size='20' text-anchor='middle' dominant-baseline='middle'>No image</text>"
                "</svg>";
            crow::response res(svg);
            res.set_header("Content-Type", "image/svg+xml");
            return res;
        }

        crow::response res(img_data);
        res.set_header("Content-Type", content_type_for_path(foundPath));
        return res;
    });

    // Upload new wallpaper (base64 payload)
    CROW_ROUTE(app, "/api/wallpapers/upload").methods(crow::HTTPMethod::Post)([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("ownerUserId") || !x.has("data") || !x.has("filename") || !x.has("title")) return crow::response(400);

        int owner = x["ownerUserId"].i();
        std::string data = x["data"].s();
        std::string filename = x["filename"].s();
        std::string title = x["title"].s();
        if (!x.has("category")) return crow::response(400, "Category required");
        std::string category = x["category"].s();
        if (category.empty()) return crow::response(400, "Category required");

        // strip data:...;base64, prefix if present
        auto comma = data.find(',');
        if (comma != std::string::npos) data = data.substr(comma + 1);

        // clean to base64 chars only
        std::string cleaned;
        cleaned.reserve(data.size());
        for (char ch : data) {
            if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '+' || ch == '/' || ch == '=') cleaned.push_back(ch);
        }

        auto decoded = base64_decode(cleaned);
        if (decoded.empty()) return crow::response(400, "Invalid image data");
        if (decoded.size() > 10U * 1024U * 1024U) {
            return crow::response(413, "Image is too large");
        }

        std::string ext;
        const auto pos = filename.find_last_of('.');
        if (pos != std::string::npos) ext = filename.substr(pos);
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });

        // If no extension provided or extension doesn't match decoded bytes,
        // try to detect the image type from the file signature.
        if (ext.empty()) {
            ext = detect_image_extension(decoded);
            if (ext.empty()) {
                return crow::response(400, "Only valid PNG, JPEG, GIF, or WebP images are allowed");
            }
        } else {
            if (!is_allowed_image(ext, decoded)) {
                std::string detected = detect_image_extension(decoded);
                if (!detected.empty()) {
                    ext = detected;
                } else {
                    return crow::response(400, "Only valid PNG, JPEG, GIF, or WebP images are allowed");
                }
            }
        }

        auto now = std::chrono::system_clock::now().time_since_epoch().count();
        std::ostringstream ss; ss << "wp_" << owner << "_" << now << ext;
        std::filesystem::path dest = std::filesystem::path("data") / "uploads" / ss.str();

        if (!write_binary_file(dest.string(), decoded)) return crow::response(500, "Failed to save file");

        std::lock_guard<std::mutex> lock(data_mutex);
        std::string err;
        if (!wallpaperService.addWallpaper(owner, title, category, dest.string(), err)) {
            std::error_code e2; std::filesystem::remove(dest, e2);
            return crow::response(400, err);
        }

        if (!fileManager.saveWallpapers(wallpapers, err)) {
            return crow::response(500, "Failed to persist wallpapers");
        }

        const auto& wp = wallpapers.empty() ? nullptr : &wallpapers.back();
        crow::json::wvalue res;
        res["status"] = "success";
        if (wp) {
            res["wallpaper"]["id"] = wp->id;
            res["wallpaper"]["filePath"] = wp->filePath;
            res["wallpaper"]["title"] = wp->title;
            res["wallpaper"]["category"] = wp->category;
            res["wallpaper"]["rating"] = wp->rating;
        }
        return crow::response(res);
    });

    // Set a wallpaper rating from 1 to 5.
    CROW_ROUTE(app, "/api/wallpapers/<int>/rate").methods(crow::HTTPMethod::Post)([&](const crow::request& req, int wallpaperId) {
        auto x = crow::json::load(req.body);
        if (!x || !x.has("rating")) return crow::response(400, "Rating is required");
        const int rating = x["rating"].i();
        if (rating < 1 || rating > 5) return crow::response(400, "Rating must be from 1 to 5");

        std::lock_guard<std::mutex> lock(data_mutex);
        wm::Wallpaper* wp = wallpaperService.findById(wallpaperId);
        if (!wp) return crow::response(404, "Not found");
        wp->rating = rating;

        std::string err;
        if (!fileManager.saveWallpapers(wallpapers, err)) {
            return crow::response(500, "Failed to persist rating");
        }

        crow::json::wvalue res;
        res["rating"] = rating;
        return crow::response(res);
    });

    app.bindaddr("127.0.0.1").port(8080).multithreaded().run();
}
