#include "Admin.h"
#include "../Helper/helper.h"

Admin::Admin() {
    s = Server::getInstance();
    bp = new crow::Blueprint("admin");
    s->app->register_blueprint(*bp);
    db = s->db;
}

void Admin::createRoutes() {
    UserDelete();
}

void Admin::UserEdit() {

}

void Admin::UserDelete()
{

    mongocxx::database &db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/delete/<int>")
    .CROW_MIDDLEWARES((*s->app), VerifyUserMiddleware)
    .methods(crow::HTTPMethod::DELETE)([db_ref,app](const crow::request &req, const int& id) {
        try{
            bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
            auto finalizer = bsoncxx::builder::stream::finalize;

            auto &ctx = app->get_context<VerifyUserMiddleware>(req);

            std::string user_role = ctx.user_data["role"].as_string().data();

            if(user_role != "admin")
                return crow::response(crow::status::UNAUTHORIZED,"User is not Admin");

            bsoncxx::document::value filter = builder<<"id"<<id<<finalizer;

            mongocxx::collection collection = db_ref["user"];
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
        } });
}