#include "Project.h"
#include "../Helper/helper.h"

Project::Project() {
    s = Server::getInstance();
    bp = new crow::Blueprint("project");
    s->app->register_blueprint(*bp);
    db = s->db;
}

crow::json::wvalue wProjectSchema = {
    {"_id", {
        {"insert", "No"},
        {"update", "No"}
    }},
    {"id", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"},
        {"size", 9}
    }},
    {"name", {
        {"type", "String"}
    }},
    {"type", {
        {"type", "String"}
    }},
    {"quantity", {
        {"type", "Integer"}
    }},
    {"income_generated", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"expense", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"expected_time", { // In Hours
        {"type", "Integer"}
    }},
    {"createdAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"assignAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"completedAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"employee_id", {
        {"type", "Integer"},
        {"required", "No"},
        {"skip", "Yes"},
        {"size", 9}
    }},
    {"request_id", {
        {"type", "List"},
        {"required", "No"},
        {"skip", "Yes"},
    }},
    {"billing_id", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
    }},
    {"priority", {
        {"type", "String"},
        {"value", {{"high", "medium","low"}}}
    }},
    {"status", {
        {"type", "String"},
        {"value", {{"stale", "running","completed"}}}
    }},
    {"manager_id", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
    }},
    {"task_count", {
        {"type", "Integer"},
        {"required", "No"},
        {"skip", "Yes"},
    }}
};


void Project::createRoutes() {
    ProjectAdd();
    ProjectView();
    ProjectAssign();
    ProjectViewOne();
    EnsureProjectIndex();
}

void Project::EnsureProjectIndex() {
    mongocxx::collection collection = (*db)["project"];

    mongocxx::cursor indexes = collection.indexes().list();
    bool ttl_index_exists = false;

    for (const auto& index : indexes) {
        auto key = index["key"].get_document().view();
        if (key.find("expireAt") != key.end() && index.find("expireAfterSeconds") != index.end()) {
            ttl_index_exists = true;
            break;
        }
    }

    if (!ttl_index_exists) {
        bsoncxx::document::value rule = bsoncxx::builder::stream::document{} << "expireAt" << 1 << bsoncxx::builder::stream::finalize;
        collection.create_index(
            rule.view(),
            mongocxx::options::index{}.expire_after(std::chrono::seconds(0))
        );
    }
}

void Project::ProjectAssign(){
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/assign")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Post)    
    ([db_ref,app](const crow::request& req) {
        try{
            mongocxx::collection collection = db_ref["project"];

            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            crow::json::wvalue wAssignSchema;

            wAssignSchema["employee_id"] = copySchema(wProjectSchema,"employee_id");
            wAssignSchema["id"] = copySchema(wProjectSchema,"id");

            auto assignSchema = crow::json::load(wAssignSchema.dump());
            std::pair<std::string, bool> check = JsonValid(reqj, assignSchema);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role=="employee")
                throw std::runtime_error("User should be manager or admin");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;

            bsoncxx::document::value projection = builder << "_id" << 1 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<reqj["employee_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["user"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find Employee");

            const bsoncxx::document::value& finder_str = *finder;

            filter = builder<<"id"<<reqj["id"].i()<<finalizer;

            bsoncxx::builder::stream::document update_builder{};

            std::stringstream ss;
            ss << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - std::chrono::system_clock::from_time_t(0)).count();

            update_builder<<"$set"<<open<<"employee_id"<<finder_str.view()["_id"].get_oid()<<"createdAt"<<bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))}<<close;

            bsoncxx::document::value update = update_builder << finalizer;

            bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_one(filter.view(), update.view());
            if(!result) 
                throw std::runtime_error("Error while Updation");

            return crow::response(crow::status::OK,"Project Updated Successfully");
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}


void Project::ProjectView() {
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Get)
    ([db_ref,app](const crow::request& req) {
        try{
            mongocxx::collection collection = db_ref["project"];

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            bsoncxx::builder::basic::document filter = bsoncxx::builder::basic::document{};
            bsoncxx::document::value projection = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", 0));

            if (user_role == "admin") {
                filter.append(bsoncxx::builder::basic::kvp("id", bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$gt", 0))));            } else if (user_role == "manager") {
                filter.append(bsoncxx::builder::basic::kvp("manager_id", bsoncxx::oid{user_id}));
            } else {
                filter.append(bsoncxx::builder::basic::kvp("employee_id", bsoncxx::oid{user_id}));
            }

            mongocxx::cursor cursor = collection.find(filter.view(), mongocxx::options::find{}.projection(projection.view()));

            std::string main_str = "[";

            for(mongocxx::cursor::iterator it = cursor.begin();it != cursor.end();){
                main_str += (bsoncxx::to_json(*it));
                if(std::next(it) != cursor.end()) main_str+=",";
            }

            main_str += "]";

            return crow::response(crow::status::OK, main_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}

void Project::ProjectViewOne() {
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Get)
    ([db_ref,app](const crow::request& req, const int& id) {
        try{
            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            mongocxx::collection collection = db_ref["project"];
            bsoncxx::stdx::optional<bsoncxx::document::value> finder = collection.find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find document");

            const bsoncxx::document::value& finder_str = *finder;

            if(user_role=="manager" && finder_str["manager_id"].get_oid().value.to_string()!=user_id)
                throw std::runtime_error("Unable to find document");
            if(user_role=="employee" && finder_str["employee_id"].get_oid().value.to_string()!=user_id)
                throw std::runtime_error("Unable to find document");

            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::OK,json_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}

void Project::ProjectAdd() {
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/add")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Post)
    ([db_ref,app](const crow::request &req)
    {
        try
        {
            mongocxx::collection collection = db_ref["project"];

            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            auto projectSchema = crow::json::load(wProjectSchema.dump());
            std::pair<std::string, bool> check = JsonValid(reqj, projectSchema); 

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role != "manager")
                return crow::response(crow::status::UNAUTHORIZED, "User Should be manager");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;

            std::string input = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::_V2::system_clock::now().time_since_epoch()).count());
            int id = adler_hash(input);

            bsoncxx::builder::stream::document insert_builder{};

            for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                std::string key = std::string(it->key());
                if(projectSchema[key].has("skip") && boost::iequals(projectSchema[key]["skip"].s(),"Yes")) continue;
                auto var = convertData(*it);
                std::visit([&insert_builder,&it,&key](auto& value) { insert_builder<< key<< value; }, var);
            }

            insert_builder<<"id"<<id;
            insert_builder<<"manager_id"<<bsoncxx::oid(user_id);
            insert_builder<<"task_count"<<0;
            insert_builder<<"income_generated"<<0;
            insert_builder<<"expense"<<0;

            std::string json_str = bsoncxx::to_json(insert_builder);

            bsoncxx::document::value doc_value = insert_builder << finalizer;
            bsoncxx::document::view docview = doc_value.view();

            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(docview);
            const mongocxx::result::insert_one& result_str = *result;

            if(!result) throw std::runtime_error("Error while insertion");

            std::ostringstream oss;
            oss << result_str.inserted_id().get_oid().value.to_string();

            crow::json::wvalue reqj_wvalue = crow::json::rvalue(reqj);
            reqj_wvalue["_id"] = oss.str();
            reqj_wvalue["manager_id"] = user_id;
            reqj_wvalue["id"] = id;
            reqj_wvalue["income_generated"] = 0;
            reqj_wvalue["expense"] = 0;

            std::string return_str = reqj_wvalue.dump();

            return crow::response(crow::status::CREATED, return_str);
        }
        catch (const std::exception& e) 
        {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}