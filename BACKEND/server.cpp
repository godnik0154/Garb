#include "string"
#include "vector"
#include "unordered_set"

#include "crow.h"
#include "crow/middlewares/cors.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/instance.hpp>

#include <cstdint>
#include <iostream>
#include <vector>

#include <boost/json/src.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "Module/Module.h"
#include "Utility/Utility.h"
#include "User/User.h"
#include "server.h"

struct Request {
    std::string request_id;
    std::string name;
    std::string project_owner;
    std::string assigned_manager;
    std::string status;
    std::string item_id;

    int quantity;
    int expense;

    std::string convertString() {
        std::string alpha = "{'request_id': '" + request_id + "', 'name': '" + name + "', 'project_owner': '" + project_owner + "', 'assigned_manager': '" + assigned_manager + "', 'status': '" + status + "', 'item_id': '" + item_id + ", 'quantity': " + std::to_string(int(quantity)) + ", 'expense': " + std::to_string(int(expense)) + "}|";
        return alpha;
    }
};

// void LiveHandlerModule(crow::App<crow::CORSHandler,UserMiddleware> *server, mongocxx::database *db_loc, std::unordered_set<crow::websocket::connection *> *request_list_users, std::unordered_set<crow::websocket::connection *> *inventory_list_users) {
//     crow::App<crow::CORSHandler,UserMiddleware> &app = *server;
//     mongocxx::database &db = *db_loc;

//     std::mutex mtx;
//     std::unordered_set<crow::websocket::connection *> &iusers = *inventory_list_users;
//     std::unordered_set<crow::websocket::connection *> &rusers = *request_list_users;

//         CROW_WEBSOCKET_ROUTE(app, "/api/list/view/live")
//       .onopen([&](crow::websocket::connection& conn) {
//           CROW_LOG_INFO << "new websocket connection from " << conn.get_remote_ip();
//           std::lock_guard<std::mutex> _(mtx);
//       })
//       .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
//           CROW_LOG_INFO << "websocket connection closed: " << reason;
//           std::lock_guard<std::mutex> _(mtx);
//       })
//       .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
//           std::lock_guard<std::mutex> _(mtx);
//       });
// }

void createRoutes(){

    // Module
    Module _module;
    _module.createRoutes();

    // User
    User user;
    user.createRoutes();

    // Uitlity
    Utility utility;
    utility.createRoutes();
}

int main() {


    crow::App<crow::CORSHandler,VerifyUserMiddleware,LoginMiddleware> app;

    // Customize CORS
    auto &cors = app.get_middleware<crow::CORSHandler>();

    cors
      .global()
        .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
      .prefix("/")
        .origin("*");

    // Database
    mongocxx::instance inst{};
    const auto uri = mongocxx::uri{"mongodb+srv://manish:1234@cluster.d2khew0.mongodb.net/?appName=Cluster"};


    mongocxx::client conn{uri};
    mongocxx::database db = conn["IMS"];

    Server* s = Server::getInstance();
    s->setServerData(&app,&db);

    // WS
    std::unordered_set<crow::websocket::connection *> inventory_list_users;
    std::unordered_set<crow::websocket::connection *> request_list_users;

    CROW_ROUTE(app, "/")
    ([db]() {
        return "<h1>IMS Status OK 4</h1>";
    });

    CROW_ROUTE(app, "/favicon.ico")
    .methods(crow::HTTPMethod::GET)
    ([db]() {
        crow::response res;
        std::string static_dir_ = "static/ico/light.ico";
        std::filesystem::path pt = std::filesystem::path(std::string(CURRENT_FUNCTION_LIST_DIR)) / std::filesystem::path(std::string(PROJECT_PATH)) / std::filesystem::path(static_dir_);
        static_dir_ = pt.string();

        res.set_static_file_info(static_dir_);
        return res;
    });

    createRoutes();

    // LiveHandlerModule(&app, &db, &request_list_users, &inventory_list_users);

    app.bindaddr("127.0.0.1")
    .port(5000)
        .multithreaded()
        .run();
}