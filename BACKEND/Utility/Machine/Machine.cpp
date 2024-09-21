#include "Machine.h"
#include "../../Helper/helper.h"
#include "unordered_set"

Machine::Machine() {
    s = Server::getInstance();
    bp = new crow::Blueprint("server");
    s->app->register_blueprint(*bp);
    db = s->db;
}

crow::json::wvalue wMachineSchema = {
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
    {"income_generated", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"storage", {
        {"type", "Integer"}
    }},
    {"inventory_id", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"size", 9}
    }},
    {"task_count", {
        {"type", "Integer"},
        {"required", "No"},
        {"skip", "Yes"}
    }},
    {"priority", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
        {"value", {{"high", "medium","low"}}}
    }}
};

auto machineSchema = crow::json::load(wMachineSchema.dump());

void Machine::createRoutes()
{
    MachineAdd();
    MachineViewEmployee();
    MachineView();
    MachineViewOneEmployee();
    MachineViewOne();
    MachineEdit();
    MachineDelete();
}


void Machine::MachineAdd()
{
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/add")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Post)
    ([db_ref,app](const crow::request &req)
    {
        try
        {
            mongocxx::collection collection = db_ref["server"];

            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);
    
            std::pair<std::string, bool> check = JsonValid(reqj, machineSchema); 

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role != "admin")
                return crow::response(crow::status::UNAUTHORIZED, "User Should be admin");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;

            std::string input = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::_V2::system_clock::now().time_since_epoch()).count());
            int id = adler_hash(input);

            bsoncxx::document::value projection = builder << "_id" << 1 <<"max_server"<<1<<"priority"<<1 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<reqj["inventory_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["inventory"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find Inventory");
            const bsoncxx::document::value& finder_str = *finder;

            int max_server = int(finder_str["max_server"].get_int32().value);

            if(max_server == 0)
                throw std::runtime_error("No more server can be allocated to this inventory");

            bsoncxx::builder::stream::document update_builder{};
            update_builder << "$inc" << open << "max_server" << -1 << close;

            bsoncxx::document::value update = update_builder << finalizer;

            bsoncxx::stdx::optional<mongocxx::result::update> result_inven = db_ref["inventory"].update_one(filter.view(), update.view());
            if(!result_inven) 
                throw std::runtime_error("Error while Updating Inventory");

            bsoncxx::builder::stream::document insert_builder{};

            for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                std::string key = std::string(it->key());
                if(machineSchema[key].has("skip") && boost::iequals(machineSchema[key]["skip"].s(),"Yes")) continue;
                auto var = convertData(*it);
                std::visit([&insert_builder,&it,&key](auto& value) { insert_builder<< key<< value; }, var);
            }

            std::string priority = finder_str["priority"].get_string().value.data();

            insert_builder<<"id"<<id;
            insert_builder<<"inventory_id"<<bsoncxx::oid(finder_str["_id"].get_oid().value.to_string());
            insert_builder<<"priority"<<priority;
            insert_builder<<"income_generated"<<0;

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
            reqj_wvalue["priority"] = priority;
            reqj_wvalue["income_generated"] = 0;

            std::string return_str = reqj_wvalue.dump();

            return crow::response(crow::status::CREATED, return_str);
        }
        catch (const std::exception& e) 
        {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}

void Machine::MachineViewEmployee(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view_employee")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req) {
        try {

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role != "employee")
                throw std::runtime_error("User should be employee"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            mongocxx::cursor cursor = db_ref["server"].find({}, mongocxx::options::find{}.projection(projection.view()));

            std::unordered_set<std::string>container_inventory;

            bsoncxx::document::value projection_request = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter_request = builder<<"status"<<"accepted"<<finalizer;;

            mongocxx::cursor cursor_request = db_ref["request"].find(filter_request.view(), mongocxx::options::find{}.projection(projection_request.view()));

            for(mongocxx::cursor::iterator it = cursor_request.begin();it != cursor_request.end();it++){
                bsoncxx::document::view doc_view = *it;
                std::string inventory_id = doc_view["inventory_id"].get_oid().value.to_string();
                std::string employee_id = doc_view["employee_id"].get_oid().value.to_string();
                if(employee_id == user_id)
                    container_inventory.insert(inventory_id);
            }

            bool inside = false;;
            std::string main_str = "[";
            for(mongocxx::cursor::iterator it = cursor.begin();it != cursor.end();it++){
                bsoncxx::document::view doc_view = *it;
                std::string inventory_id = doc_view["inventory_id"].get_oid().value.to_string();                
                if(container_inventory.find(inventory_id)!=container_inventory.end()){
                    main_str += (bsoncxx::to_json(*it));
                    main_str+=",";
                }
                inside = true;
            }
            if(inside) main_str.pop_back();

            main_str += "]";

            return crow::response(crow::status::OK, main_str);
        } catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}

void Machine::MachineView(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req) {
        try{

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();

            if(user_role=="employee")
                throw std::runtime_error("User should not be employee");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            mongocxx::cursor cursor = db_ref["server"].find({},mongocxx::options::find{}.projection(projection.view()));

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

void Machine::MachineViewOneEmployee(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view_employee/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req, const int& id) {
        try {

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role != "employee")
                throw std::runtime_error("User should be employee"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["server"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find document");

            const bsoncxx::document::value& finder_str = *finder;

            bsoncxx::document::value projection_request = builder << "_id" << 0 << "employee_id"<< 1 <<finalizer;
            bsoncxx::document::value filter_request = builder<<"inventory_id"<<bsoncxx::oid(finder_str["inventory_id"].get_oid().value.to_string())<<"status"<<"accepted"<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder_request = db_ref["request"].find_one(filter_request.view(), mongocxx::options::find{}.projection(projection_request.view()));
            if(!finder_request)
                throw std::runtime_error("Request is not created or accepted");

            const bsoncxx::document::value& finder_request_str = *finder_request;



            std::string employee_id = finder_request_str["employee_id"].get_oid().value.to_string();
            if(employee_id != user_id)
                throw std::runtime_error("User should be same as Request generated user");

            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::OK,json_str);
        } catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}

void Machine::MachineViewOne(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req, const int& id) {
        try {

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role == "employee")
                throw std::runtime_error("User should not be employee"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["server"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
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


void Machine::MachineEdit(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/edit/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Put)
        ([db_ref, app](const crow::request &req,const int& id) {
            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);

            std::pair<std::string, bool> check = JsonValid(reqj, machineSchema, 1); 

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

            bsoncxx::document::value filter = builder<<"id"<<id<<"task_count"<<0<<finalizer;

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


            core::v1::optional<bsoncxx::v_noabi::document::value> result = db_ref["server"].find_one_and_update(filter.view(), update.view(), options);
            std::string json_str = bsoncxx::to_json(*result);

            return crow::response(crow::status::ACCEPTED,json_str);
        });
}

void Machine::MachineDelete(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/delete/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
        .methods(crow::HTTPMethod::Delete)([db_ref, app](const crow::request &req, const int& id) {
            try{
                bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
                auto finalizer = bsoncxx::builder::stream::finalize;

                bsoncxx::document::value filter = builder<<"id"<<id<<"task_count"<<0<<finalizer;

                bsoncxx::stdx::optional<mongocxx::result::delete_result> deleter = db_ref["server"].delete_one(filter.view());
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
