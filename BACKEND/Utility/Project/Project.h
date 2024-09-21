#ifndef PROJECT_H_
#define PROJECT_H_

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

class Project {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Project();
        void createRoutes();
        void ProjectAdd();
        void ProjectView();
        void ProjectViewOne();
        void ProjectAssign();
        void EnsureProjectIndex();
};

#endif