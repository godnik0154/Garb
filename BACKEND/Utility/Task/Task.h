#ifndef TASK_H_
#define TASK_H_

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "../../server.h"
#include "unordered_set"
#include "vector"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

class Task {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Task();
        void createRoutes();
        void TaskAdd();
        void TaskViewAdmin();
        void TaskView();
        void TaskViewOne();
        void TaskViewOneAdmin();
};

#endif