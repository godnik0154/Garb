#ifndef USER_H_
#define USER_H_

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "unordered_set"
#include "vector"
#include "../server.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "Middleware.h"

#include "Admin/Admin.h"

class User {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        User();
        void createRoutes();
        void UserSignUp();
        void UserEdit();
        void UserView();
        void UserViewOne();
        void UserSignin();
};

#endif