#ifndef MACHINE_H_
#define MACHINE_H_

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

#include "../../User/Middleware.h"

class Machine {
    public:
        crow::Blueprint *bp;
        mongocxx::database *db;
        Server* s;

        Machine();
        void createRoutes();
        void MachineAdd();
        void MachineView();
        void MachineViewEmployee();
        void MachineViewOneEmployee();
        void MachineViewOne();
        void MachineEdit();
        void MachineDelete();
};

#endif