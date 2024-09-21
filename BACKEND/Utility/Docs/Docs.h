#ifndef DOCS_H_
#define DOCS_H_

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "../../server.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include "../Middleware.h"

class Docs {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Docs();
        void DocsView();
        void createRoutes();
};

#endif