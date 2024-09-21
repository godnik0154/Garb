#ifndef UTILITY_H_
#define UTILITY_H_

#include "crow.h"
#include "crow/middlewares/cors.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include "Task/Task.h"
#include "Project/Project.h"
#include "Machine/Machine.h"
#include "Inventory/Inventory.h"
#include "Docs/Docs.h"

class Utility
{
    public:
        void createRoutes(){
            // Project
            Project project;
            project.createRoutes();

            // Machine
            Machine machine;
            machine.createRoutes();

            // Inventory
            Inventory inventory;
            inventory.createRoutes();

            // Task
            Task task;
            task.createRoutes();

            // Docs
            Docs doc;
            doc.createRoutes();
        }
};

#endif