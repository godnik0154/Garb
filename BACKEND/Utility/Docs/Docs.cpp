#include "Docs.h"

Docs::Docs() {
    s = Server::getInstance();
    bp = new crow::Blueprint("docs","Static");
    s->app->register_blueprint(*bp);
    db = s->db;
}

void Docs::createRoutes()
{
    DocsView();
}

void Docs::DocsView()
{
    mongocxx::database& db_ref = *db;
    auto &app = s->app;

    CROW_BP_ROUTE((*bp), "/")
    .methods(crow::HTTPMethod::Get)([db_ref, app](const crow::request &req) {
        
        crow::mustache::template_t page = crow::mustache::load("docs.html");
        crow::mustache::context ctx ({
            {"person", "Bob"}
        });
        return page.render(ctx);
    });
}