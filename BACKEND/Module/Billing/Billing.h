#ifndef BILLING_H_
#define BILLING_H_

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "../../server.h"
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

class Billing {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Billing();
        void BillingAdd();
        void createRoutes();
        void BillingView();
        void BillingEdit();
        void BillingViewOne();
        void BillingDelete();
};

#endif