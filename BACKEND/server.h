#ifndef SERVER_H_
#define SERVER_H_

#include "string"
#include "vector"
#include "chrono"
#include "unordered_set"
#include "sstream"

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "unordered_set"
#include "vector"
#include "Helper/Authorization.h"

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

//Middleware
#include "User/Middleware.h"

struct UserMiddleware: crow::ILocalMiddleware
{
    struct context
    {
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        Session& s = Session::getInstance();

        try
        {
            if((req.method == crow::HTTPMethod::PUT || req.method == crow::HTTPMethod::POST) && req.body=="")
                throw std::invalid_argument("Enter Payload");

            if(req.url=="/api/login") return;

            std::string jwtToken = req.get_header_value("Authorization");

            if(jwtToken=="")
                throw std::invalid_argument("Enter Authorization Header");

            std::pair<bool,std::string>ans = s.verify_session(jwtToken);

            if(!ans.first)
                throw std::runtime_error(ans.second);
        }
        catch (const std::exception& e) {
            res.code = 403;
            res.body = "";
            res.end(e.what());
        }
    }


    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        if(res.code == 403) return;
        if(req.url!="/api/login") return;

        Session& s = Session::getInstance();

        try
        {
            boost::json::object data = boost::json::parse(res.body).as_object();

            std::stringstream ss;
            data.erase("_id");
            data.erase("name");

            std::string jwt_str = "";

            std::pair<bool,std::string>ans = s.create_session(data,jwt_str);

            data = boost::json::parse(res.body).as_object();

            if(!ans.first)
                throw std::runtime_error(ans.second);

            data["jwt"] = ans.second;

            ss << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - std::chrono::system_clock::from_time_t(0)).count();
            data["session_created"] = ss.str();
            ss.str("");
            ss<<data;
            res.body = ss.str();
        }
        catch (const std::exception& e) {
            res.code = crow::status::FORBIDDEN;
            res.body = "";
            res.end(e.what());
        }
    }
};

class Server {
public:
    crow::App<crow::CORSHandler,VerifyUserMiddleware,LoginMiddleware> *app;
    mongocxx::database *db;

    void startServer()
    {
        
    }

    static Server* getInstance() {
        static Server instance;
        return &instance;
    }

    void setServerData(crow::App<crow::CORSHandler,VerifyUserMiddleware,LoginMiddleware> *server, mongocxx::database *db_loc) {
        app = server;
        db = db_loc;
    }

    private:
    Server() {};


};

#endif