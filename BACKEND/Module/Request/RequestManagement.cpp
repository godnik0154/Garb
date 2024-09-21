#include "RequestManagement.h"
#include "../Helper/helper.h"

crow::json::wvalue wRequestSchema = {
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
    {"status", {
        {"type", "String"},
        {"skip", "Yes"},
        {"required", "No"},
        {"value", {{"pending", "accepted","rejected"}}}
    }},
    {"employee_id", {
        {"type", "Integer"},
        {"size", 9},
        {"insert", "Yes"},
        {"update", "No"},
        {"skip", "Yes"}
    }},
    {"inventory_id", {
        {"type", "Integer"},
        {"size", 9},
        {"insert", "Yes"},
        {"update", "No"},
        {"skip", "Yes"}
    }},
    {"reason", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"}
    }}
};

RequestManagement::RequestManagement() {
    s = Server::getInstance();
    bp = new crow::Blueprint("request");
    s->app->register_blueprint(*bp);
    db = s->db;
}

void RequestManagement::createRoutes()
{
    RequestManagementAdd();
    RequestManagementEdit();
    RequestManagementView();
    RequestManagementViewOne();
}

void RequestManagement::RequestManagementAdd() {
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/add")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Post)    
    ([db_ref,app](const crow::request& req) {
        try{
            mongocxx::collection collection = db_ref["request"];

            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            auto requestSchema = crow::json::load(wRequestSchema.dump());
            std::pair<std::string, bool> check = JsonValid(reqj, requestSchema);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role!="employee")
                throw std::runtime_error("User should be employee");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;

            std::string input = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::_V2::system_clock::now().time_since_epoch()).count());
            int id = adler_hash(input);

            bsoncxx::document::value projection = builder << "_id" << 1 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<reqj["employee_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["user"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find Employee");

            const bsoncxx::document::value& finder_str = *finder;

            bsoncxx::document::value projection_inventory = builder << "_id" << 1 <<finalizer;
            bsoncxx::document::value filter_inventory = builder<<"id"<<reqj["inventory_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder_inventory = db_ref["inventory"].find_one(filter_inventory.view(), mongocxx::options::find{}.projection(projection_inventory.view()));
            if(!finder)
                throw std::runtime_error("Unable to find Inventory");

            const bsoncxx::document::value& finder_inventory_str = *finder_inventory;

            bsoncxx::builder::stream::document insert_builder{};

            for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                std::string key = std::string(it->key());
                if(requestSchema[key].has("skip") && boost::iequals(requestSchema[key]["skip"].s(),"Yes")) continue;
                auto var = convertData(*it);
                std::visit([&insert_builder,&it,&key](auto& value) { insert_builder<< key<< value; }, var);
            }

            insert_builder<<"id"<<id;
            insert_builder<<"inventory_id"<<bsoncxx::oid(finder_inventory_str["_id"].get_oid().value.to_string());
            insert_builder<<"employee_id"<<bsoncxx::oid(finder_str["_id"].get_oid().value.to_string());
            insert_builder<<"status"<<"pending";
            insert_builder<<"reason"<<"";

            bsoncxx::document::value doc_value = insert_builder << finalizer;
            bsoncxx::document::view docview = doc_value.view();

            bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(docview);
            const mongocxx::result::insert_one& result_str = *result;

            if(!result) throw std::runtime_error("Error while insertion");

            std::ostringstream oss;
            oss << result_str.inserted_id().get_oid().value.to_string();

            crow::json::wvalue reqj_wvalue = crow::json::rvalue(reqj);
            reqj_wvalue["_id"] = oss.str();
            reqj_wvalue["id"] = id;
            reqj_wvalue["status"] = "pending";
            reqj_wvalue["reason"] = "";

            std::string return_str = reqj_wvalue.dump();

            return crow::response(crow::status::OK,return_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}

void RequestManagement::RequestManagementEdit(){
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/edit/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Put)
        ([db_ref, app](const crow::request &req,const int& id) {
        try{
            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            auto requestSchema = crow::json::load(wRequestSchema.dump());
            std::pair<std::string, bool> check = JsonValid(reqj, requestSchema, 1); 

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();

            if(user_role=="employee")
                throw std::runtime_error("User should not be employee");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;
            auto finalizer = bsoncxx::builder::stream::finalize;

            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            bsoncxx::builder::stream::document update_builder{};

            update_builder<<"$set"<<open;
            for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                auto key = std::string(it->key());
                auto var = convertData(*it);
                std::visit([&update_builder,&it,&key](auto& value) { update_builder<< key<< value; }, var);
            }
            update_builder<<close;

            bsoncxx::document::value update = update_builder << finalizer;    

            mongocxx::options::find_one_and_update options{};
            options.return_document(mongocxx::options::return_document::k_after);

            core::v1::optional<bsoncxx::v_noabi::document::value> result = db_ref["request"].find_one_and_update(filter.view(), update.view(), options);
            std::string json_str = bsoncxx::to_json(*result);

            return crow::response(crow::status::ACCEPTED,json_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}

void RequestManagement::RequestManagementView()
{
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Get)
    ([db_ref,app](const crow::request& req) {
        try{

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            bsoncxx::document::value projection = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", 0));
            bsoncxx::builder::basic::document filter = bsoncxx::builder::basic::document{};

            if(user_role == "employee")
                filter.append(bsoncxx::builder::basic::kvp("employee_id", bsoncxx::oid{user_id}));

            mongocxx::cursor cursor = db_ref["request"].find(filter.view(),mongocxx::options::find{}.projection(projection.view()));

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

void RequestManagement::RequestManagementViewOne()
{
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
            bsoncxx::builder::basic::document filter = bsoncxx::builder::basic::document{};
            filter.append(bsoncxx::builder::basic::kvp("id", id));

            if(user_role == "employee")
                filter.append(bsoncxx::builder::basic::kvp("employee_id", bsoncxx::oid{user_id}));

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["request"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find document");

            const bsoncxx::document::value& finder_str = *finder;

            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::OK,json_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });
}

void RequestManagement::RequestManagementDelete()
{
    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/delete/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Delete)([db_ref, app](const crow::request &req, const int& id) {
            try{
                bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
                auto finalizer = bsoncxx::builder::stream::finalize;

                auto& ctx = app->get_context<VerifyUserMiddleware>(req);
                std::string user_role = ctx.user_data["role"].as_string().c_str();

                if(user_role != "admin")
                    throw std::runtime_error("User should be admin");

                bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;
                bsoncxx::stdx::optional<mongocxx::result::delete_result> deleter = db_ref["request"].delete_one(filter.view());
                if(!deleter)
                    throw std::runtime_error("Unable to delete document");

                const mongocxx::result::delete_result& deleter_str = *deleter;

                std::string message = deleter_str.deleted_count() == 0 ? "No document to delete":"Document deleted sucessfully";
                crow::status status = deleter_str.deleted_count() == 0 ? crow::status::BAD_REQUEST : crow::status::ACCEPTED;

                return crow::response(status,message);
            }
            catch (const std::exception& e) {
                return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
            }
        });
}