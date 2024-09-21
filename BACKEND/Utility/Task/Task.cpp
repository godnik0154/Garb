#include "Task.h"
#include "../../Helper/helper.h"

Task::Task() {
    s = Server::getInstance();
    bp = new crow::Blueprint("task");
    s->app->register_blueprint(*bp);
    db = s->db;
}

crow::json::wvalue wTaskSchema = {
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
    {"createdAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
    }},
    {"expireAt", {
        {"type", "String"},
        {"required", "No"},
        {"skip", "Yes"},
    }},
    {"priority", {
        {"type", "String"},
        {"value", {{"high", "medium","low"}}},
        {"required", "No"}
    }},
    {"server_id", {
        {"type", "Integer"},
        {"update", "No"},
        {"skip", "Yes"},
        {"size", 9}
    }},
    {"project_id", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"size", 9}
    }},
    {"income_generated", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"total_income", {
        {"type", "Integer"},
        {"skip", "Yes"},
        {"required", "No"}
    }},
    {"pending_quantity", {
        {"type", "Integer"},
        {"required", "No"},
        {"skip", "Yes"}
    }},
    {"completed_quantity", {
        {"type", "Integer"},
        {"required", "No"},
        {"skip", "Yes"}
    }}
};

auto TaskSchema = crow::json::load(wTaskSchema.dump());

void Task::createRoutes()
{
    TaskAdd();
    TaskViewAdmin();
    TaskView();
    TaskViewOne();
    TaskViewOneAdmin();
}

void Task::TaskAdd()
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
            mongocxx::collection collection = db_ref["task"];

            crow::json::rvalue reqj = crow::json::load(req.body);

            if (!reqj)
                return crow::response(crow::status::BAD_REQUEST);
    
            std::pair<std::string, bool> check = JsonValid(reqj, TaskSchema); 

            if(!check.second)
                return crow::response(crow::status::BAD_REQUEST, check.first);

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role != "employee")
                return crow::response(crow::status::UNAUTHORIZED, "User Should be employee");

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;
            auto open = bsoncxx::builder::stream::open_document;
            auto close = bsoncxx::builder::stream::close_document;

            std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
            time_t start_time_from = std::chrono::system_clock::to_time_t(time_now);

            std::string input = std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::_V2::system_clock::now().time_since_epoch()).count());
            int id = adler_hash(input);

            bsoncxx::document::value projection = builder << "_id" << 1 << "quantity" << 1 << "employee_id" << 1 << "priority" << 1 << "task_count"<< 1 <<"income"<< 1<<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<reqj["project_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["project"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find Project");
            const bsoncxx::document::value& finder_str = *finder;

            if(finder_str["employee_id"].get_oid().value.to_string() != user_id)
                throw std::runtime_error("User should be same as project employee");

            bsoncxx::document::value projection_server = builder << "_id" << 1 << "income_generated" << 1 << "storage" << 1 <<"priority"<<1<<finalizer;
            bsoncxx::document::value filter_server = builder<<"id"<<reqj["server_id"].i()<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder_server = db_ref["server"].find_one(filter_server.view(), mongocxx::options::find{}.projection(projection_server.view()));

            if(!finder_server)
                throw std::runtime_error("Unable to find Server");

            const bsoncxx::document::value& finder_server_str = *finder_server;

            int quantity = std::min(finder_server_str["storage"].get_int32().value,finder_str["quantity"].get_int32().value);

            if(quantity == 0)
                throw std::runtime_error("Server is not free");

            std::string priority = finder_str["priority"].get_string().value.to_string();
            std::transform(priority.begin(), priority.end(), priority.begin(), ::tolower); 
            std::string server_priority = finder_server_str["priority"].get_string().value.to_string();

            double ratio_server = server_priority == "high" ? 0.75 : server_priority == "medium" ? 0.5 : 0.25;
            double ratio_doc = priority == "high" ? 1 : priority == "medium" ? 0.5 : 0.25;

            int num = ceil(quantity / round((ratio_doc*quantity)*(1+ratio_server)));

            std::chrono::system_clock::time_point time_expire = time_now + std::chrono::minutes(30*num);
            time_t expiry_time_form = std::chrono::system_clock::to_time_t(time_expire);
            time_t expiry_time_project = std::chrono::system_clock::to_time_t(time_expire + std::chrono::minutes(10));

            bsoncxx::builder::stream::document update_builder{};
            update_builder << "$inc" << open << "task_count" << 1 << "quantity" << -(quantity) << close;

            update_builder << "$set" << open <<"status"<<"running";
            if(quantity == finder_str["quantity"].get_int32().value)
                update_builder << "expireAt" << bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(expiry_time_project)};

            if(finder_str["task_count"].get_int32().value == 0)
                update_builder << "assign_time" << bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(start_time_from)};
            update_builder << close;

            bsoncxx::document::value update = update_builder << finalizer;

            bsoncxx::stdx::optional<mongocxx::result::update> result_inven = db_ref["project"].update_one(filter.view(), update.view());
            if(!result_inven) 
                throw std::runtime_error("Error while Updating Project");

            bsoncxx::builder::stream::document update_builder_server{};
            update_builder_server << "$inc" << open << "storage" << -(quantity) << "task_count" << 1 << close;

            bsoncxx::document::value update_server = update_builder_server << finalizer;

            bsoncxx::stdx::optional<mongocxx::result::update> result_inven_server = db_ref["server"].update_one(filter_server.view(), update_server.view());
            if(!result_inven_server) 
                throw std::runtime_error("Error while Updating Server");

            bsoncxx::builder::stream::document insert_builder{};

            for (auto it = reqj.begin(); it != reqj.end(); ++it) {
                std::string key = std::string(it->key());
                if(TaskSchema[key].has("skip") && boost::iequals(TaskSchema[key]["skip"].s(),"Yes")) continue;
                auto var = convertData(*it);
                std::visit([&insert_builder,&it,&key](auto& value) { insert_builder<< key<< value; }, var);
            }

            insert_builder<<"id"<<id;
            insert_builder<<"createdAt"<<bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(start_time_from)};
            insert_builder<<"expireAt"<<bsoncxx::types::b_date{std::chrono::system_clock::from_time_t(expiry_time_form)};
            insert_builder<<"id"<<id;
            insert_builder<<"project_id"<<bsoncxx::oid(finder_str["_id"].get_oid().value.to_string());
            insert_builder<<"server_id"<<bsoncxx::oid(finder_server_str["_id"].get_oid().value.to_string());
            insert_builder<<"pending_quantity"<<quantity;
            insert_builder<<"completed_quantity"<<0;
            insert_builder<<"priority"<<priority;
            insert_builder<<"total_income"<<0;
            insert_builder<<"income_generated"<<finder_str["income"].get_int32().value;

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
            reqj_wvalue["pending_quantity"] = quantity;
            reqj_wvalue["completed_quantity"] = 0;
            reqj_wvalue["createdAt"] = start_time_from;
            reqj_wvalue["expireAt"] = expiry_time_form;
            reqj_wvalue["total_income"] = 0;
            reqj_wvalue["income_generated"] = finder_str["income"].get_int32().value;
            reqj_wvalue["project_id"] = finder_str["_id"].get_oid().value.to_string();
            reqj_wvalue["server_id"] = finder_server_str["_id"].get_oid().value.to_string();

            std::string return_str = reqj_wvalue.dump();

            return crow::response(crow::status::CREATED, return_str);
        }
        catch (const std::exception& e) 
        {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        }
    });
}

void Task::TaskViewAdmin()
{
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view_admin")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
		.methods(crow::HTTPMethod::Get)
    ([db_ref,app](const crow::request& req) {
        try{

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            bsoncxx::document::value projection = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", 0));

            if (user_role != "admin")
                throw std::runtime_error("User is not admin");

            mongocxx::cursor cursor = db_ref["task"].find({},mongocxx::options::find{}.projection(projection.view()));

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

void Task::TaskView(){

    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req) {
        try {

            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();
            std::string user_id = ctx.user_data["_id"].as_object()["$oid"].as_string().c_str();

            if(user_role == "admin")
                throw std::runtime_error("User should be employee or manager"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            mongocxx::cursor cursor = db_ref["task"].find({}, mongocxx::options::find{}.projection(projection.view()));

            std::unordered_set<std::string>container_project;

            bsoncxx::builder::basic::document filter_project = bsoncxx::builder::basic::document{};
            bsoncxx::builder::basic::document projection_project = bsoncxx::builder::basic::document{};
            projection_project.append(bsoncxx::builder::basic::kvp("_id", 1));

            if (user_role == "manager") {
                filter_project.append(bsoncxx::builder::basic::kvp("manager_id", bsoncxx::oid{user_id}));
                projection_project.append(bsoncxx::builder::basic::kvp("manager_id", 1));
            } else {
                filter_project.append(bsoncxx::builder::basic::kvp("employee_id", bsoncxx::oid{user_id}));
                projection_project.append(bsoncxx::builder::basic::kvp("employee_id", 1));
            }

            mongocxx::cursor cursor_project = db_ref["project"].find(filter_project.view(), mongocxx::options::find{}.projection(projection_project.view()));

            for(mongocxx::cursor::iterator it = cursor_project.begin();it != cursor_project.end();it++){
                bsoncxx::document::view doc_view = *it;
                std::string project_id = doc_view["_id"].get_oid().value.to_string();
                std::string required_id = user_role == "manager" ? doc_view["manager_id"].get_oid().value.to_string() : doc_view["employee_id"].get_oid().value.to_string();
                if(required_id == user_id)
                    container_project.insert(project_id);
            }

            bool inside = false;;
            std::string main_str = "[";
            for(mongocxx::cursor::iterator it = cursor.begin();it != cursor.end();it++){
                bsoncxx::document::view doc_view = *it;
                std::string project_id = doc_view["project_id"].get_oid().value.to_string();                
                if(container_project.find(project_id)!=container_project.end()){
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

void Task::TaskViewOne()
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

            if(user_role == "admin")
                throw std::runtime_error("User should be employee or manager"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["task"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
            if(!finder)
                throw std::runtime_error("Unable to find document");

            const bsoncxx::document::value& finder_str = *finder;

            bsoncxx::builder::basic::document filter_project = bsoncxx::builder::basic::document{};
            bsoncxx::builder::basic::document projection_project = bsoncxx::builder::basic::document{};
            projection_project.append(bsoncxx::builder::basic::kvp("_id", 1));
            filter_project.append(bsoncxx::builder::basic::kvp("_id", bsoncxx::oid{finder_str["project_id"].get_oid().value.to_string()}));

            if (user_role == "manager") {
                projection_project.append(bsoncxx::builder::basic::kvp("manager_id", 1));
            } else {
                projection_project.append(bsoncxx::builder::basic::kvp("employee_id", 1));
            }

            bsoncxx::stdx::optional<bsoncxx::document::value> finder_project = db_ref["project"].find_one(filter_project.view(), mongocxx::options::find{}.projection(projection_project.view()));
            if(!finder_project)
                throw std::runtime_error("Request is not created or accepted");

            const bsoncxx::document::value& finder_project_str = *finder_project;

            std::string required_id = user_role == "manager" ? finder_project_str["manager_id"].get_oid().value.to_string() : finder_project_str["employee_id"].get_oid().value.to_string();

            if(required_id != user_id)
            {
                std::string msg = user_role + " and the current user is not same";
                throw std::runtime_error(msg);
            }

            std::string json_str = bsoncxx::to_json(finder_str);

            return crow::response(crow::status::OK,json_str);
        }
        catch (const std::exception& e) {
            return crow::response(crow::status::INTERNAL_SERVER_ERROR, e.what());
        } });    
}

void Task::TaskViewOneAdmin()
{
    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/view_admin/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::Get)
    ([db_ref,app](const crow::request& req, const int& id) {
        try{
            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto& ctx = app->get_context<VerifyUserMiddleware>(req);
            std::string user_role = ctx.user_data["role"].as_string().c_str();

            if(user_role != "admin")
                throw std::runtime_error("User should be admin"); 

            bsoncxx::document::value projection = builder << "_id" << 0 <<finalizer;
            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            bsoncxx::stdx::optional<bsoncxx::document::value> finder = db_ref["task"].find_one(filter.view(), mongocxx::options::find{}.projection(projection.view()));
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