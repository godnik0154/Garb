#include "Inventory.h"
#include "../../Helper/helper.h"

Inventory::Inventory() {
    s = Server::getInstance();
    bp = new crow::Blueprint("inventory");
    s->app->register_blueprint(*bp);
    db = s->db;
}

crow::json::wvalue wInventorySchema = {
    {"_id", {
        {"insert", "No"},
        {"update", "No"}
    }},
    {"id", {
        {"insert", "No"},
        {"update", "No"}
    }},
    {"name", {
        {"type", "String"}
    }},
    {"type", {
        {"type", "String"}
    }},
    {"priority", {
        {"type", "String"},
        {"value", {{"high", "medium","low"}}},
        {"skip", "Yes"}        
    }},
    {"cost", {
        {"type", "Double"}
    }},
    {"available", {
        {"type", "Boolean"}
    }},
    {"max_server", {
        {"type", "Integer"}
    }},
    {"expireAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"}
    }},
};


auto inventorySchema = crow::json::load(wInventorySchema.dump());

void Inventory::createRoutes()
{
    InventoryAdd();
    InventoryEdit();
    InventoryDelete();
    InventoryViewOne();
    InventoryView();
    EnsureInventoryIndex();
}

void Inventory::EnsureInventoryIndex() {
    mongocxx::collection collection = (*db)["inventory"];

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

void Inventory::InventoryAdd(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/add")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Post)([db_ref,app](const crow::request &req) {
            try {
                crow::json::rvalue reqj = crow::json::load(req.body);

                std::pair<std::string, bool> check = JsonValid(reqj, inventorySchema); 

                if (!reqj)
                    return crow::response(crow::status::BAD_REQUEST);

                if(!check.second)
                    return crow::response(crow::status::BAD_REQUEST, check.first);

                bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
                auto finalizer = bsoncxx::builder::stream::finalize;

                auto& ctx = app->get_context<VerifyUserMiddleware>(req);
                std::string user_role = ctx.user_data["role"].as_string().c_str();

                if(user_role!="admin")
                    throw std::runtime_error("User is not admin");

                mongocxx::collection collection = db_ref["inventory"];
                bsoncxx::document::value count_view = builder << finalizer;
                std::string input = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::_V2::system_clock::now().time_since_epoch()).count());
                int id = adler_hash(input);

                std::string priority = reqj["priority"].s();
                std::transform(priority.begin(), priority.end(), priority.begin(), ::tolower);

                bsoncxx::builder::stream::document insert_builder{};

                for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                    std::string key = std::string(it->key());
                    if(inventorySchema[key].has("skip") && boost::iequals(inventorySchema[key]["skip"].s(),"Yes")) continue;
                    auto var = convertData(*it);
                    std::visit([&insert_builder,&it,&key](auto& value) { insert_builder<< key<< value; }, var);
                }

                std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now() + std::chrono::hours(24*reqj["life"].i());
                time_t expiry_time_form = std::chrono::system_clock::to_time_t(time_now);

                insert_builder<<"id"<<id;
                insert_builder<<"priority"<<priority;
                insert_builder<<"expireAt"<<bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(expiry_time_form)};

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
                std::string return_str = reqj_wvalue.dump();

                return crow::response(crow::status::CREATED, return_str);
            } catch (const std::exception& e) {
                return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
            }
        });
}

void Inventory::InventoryEdit(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/edit/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Put)
        ([db_ref, app](const crow::request &req,const int& id) {
            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            std::pair<std::string, bool> check = JsonValid(reqj, inventorySchema, 1); 

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();

            if(user_role!="admin")
                throw std::runtime_error("User is not admin");

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

            mongocxx::collection collection = db_ref["inventory"];

            bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_one(filter.view(), update.view());
            if(!result) 
                throw std::runtime_error("Error while Updation");
            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = collection.find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Updation done unable to find modified document");

            const bsoncxx::document::value& finder_str = *finder;
            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::ACCEPTED,json_str);
        });
}


void Inventory::InventoryDelete(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/delete/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Delete)([db_ref, app](const crow::request &req, const int& id) {
            try{
                bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
                auto finalizer = bsoncxx::builder::stream::finalize;

                bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

                mongocxx::collection collection = db_ref["inventory"];
                bsoncxx::stdx::optional<mongocxx::result::delete_result> deleter = collection.delete_one(filter.view());
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

void Inventory::InventoryView(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req) {
        try{
            mongocxx::collection collection = db_ref["inventory"];

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            mongocxx::cursor cursor = collection.find({},mongocxx::options::find{}.projection(projection.view()));

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
        }
    });
}

void Inventory::InventoryViewOne(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req, const int& id) {
        try {

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            mongocxx::collection collection = db_ref["inventory"];
            bsoncxx::stdx::optional<bsoncxx::document::value> finder = collection.find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find document");

            const bsoncxx::document::value& finder_str = *finder;
            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::OK,json_str);
        } catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}