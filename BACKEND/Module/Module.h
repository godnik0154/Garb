#ifndef MODULE_H_
#define MODULE_H_

#include "crow.h"
#include "crow/middlewares/cors.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include "Billing/Billing.h"
#include "Request/RequestManagement.h"


class Module
{
    public:
        void createRoutes(){
            // Billing
            Billing billing;
            billing.createRoutes();

            // Request
            RequestManagement request;
            request.createRoutes();
        }
};

#endif