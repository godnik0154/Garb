#ifndef ADMIN_H_
#define ADMIN_H_

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "unordered_set"
#include "vector"
#include "../../server.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../Middleware.h"

class Admin {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Admin();
        void createRoutes();
        void UserEdit();
        void UserDelete();
};

#endif