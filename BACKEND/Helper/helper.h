#ifndef HELPER_H_
#define HELPER_H_

#include "crow.h"
#include "regex"
#include <unordered_map>
#include <variant>
#include <boost/algorithm/string/predicate.hpp>

//adelr hash
inline uint32_t adler_hash(const std::string& data) {
    uint32_t MOD_ADLER = 65521;
    uint32_t a = 1, b = 0;
    
    for (char byte : data) {
        a = (a + byte) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    
    return (b << 16) | a;
}

inline bool isDouble(const std::string& str) {
    char* endptr;
    std::strtod(str.c_str(), &endptr);
    bool isdot = str.find('.') != std::string::npos;
    return *endptr == '\0' && isdot;
}

/**
 * @param res req body.
 * @param schema Schema of the body.
 * @param operation as int @note{0} for put @note{1} for update @note{2} for nothing.
 */
inline std::pair<std::string,bool> JsonValid(crow::json::rvalue &res,crow::json::rvalue &schema,int operation = 0){

    std::string msg="";
    bool valid = true;
    std::unordered_map<std::string,bool>keyValid;

    for (auto it = schema.begin(); it != schema.end(); ++it) {
        auto key = std::string(it->key());
        auto val = *it;

        keyValid[key] = true;

        if((operation==1 && val.has("update") && boost::iequals(val["update"].s(),"NO")) || (val.has("insert") && boost::iequals(val["insert"].s(),"NO")))
                if(res.has(key))
                {
                    msg = key + " should not be present in body";
                    valid = valid && false;
                    break;
                }
                else continue;

        if(!res.has(key)){
            if(operation==1 || operation==2) continue;
            if(val.has("required") && boost::iequals(val["required"].s(),"NO")) continue;
            msg = key + " is not present in the message";
            valid = valid && false;
            break;
        }

        if(val.has("regex"))
        {   
            std::regex pattern(std::string(val["regex"].s()));
            bool is_matched = std::regex_match(std::string(res[key].s()), pattern);

            if(!is_matched)
            {
                msg = key + " is not valid";
                valid = valid && false;
                break;          
            }
        }

        if(val.has("size"))
        {
            int len=0;
            if(res[key].t()==crow::json::type::String)
                len = res[key].s().size();
            else if(res[key].t()==crow::json::type::Number)
                len = int(log10(res[key].i()) + 1);


            if(len!=val["size"].i())
            {
                msg = key + " should of size " + std::to_string(val["size"].i());
                valid = valid && false;
                break;          
            }
        }

        std::string type = get_type_str(res[key].t());

        if(val["type"]=="Boolean"){
            if(type != "True" && type != "False"){
                msg = key + " incorrect datatype should be bool";
                valid = valid && false;
            }
        } else if(val["type"]=="Integer"){
            bool is_double = isDouble(std::string(res[key]));
            if(type!="Number" || is_double){
                msg = key + " incorrect datatype should be integer";
                valid = valid && false;
                break;                
            }
        } else if(val["type"]=="Double"){
            bool is_double = isDouble(std::string(res[key]));
            if(type!="Number" || !is_double){
                msg = key + " incorrect datatype should be double";
                valid = valid && false;
                break;                
            }
        } else if(type != val["type"]){
            msg = key + " incorrect datatype should be " + std::string(val["type"].s());
            valid = valid && false;
            break;   
        }

        if(val.has("value")){
            bool matched = false;

            for(auto x = val["value"].begin();x!=val["value"].end();x++)
                if(boost::iequals(x->s(),res[key].s()))
                    matched = true;

            if(!matched){
                std::string avaliable = "[";

                for(auto x = val["value"].begin();x!=val["value"].end();x++){
                    avaliable+=x->s();
                    if(std::next(x) != val["value"].end()) avaliable+=",";
                }

                avaliable+="]";

                msg = key + " should be one of the following " +  avaliable;
                valid = valid && false;
                break;
            }
        }

    }

    if(msg!="") return {msg,valid};
    
    for (auto it = res.begin(); it != res.end(); ++it) {
        auto key = std::string(it->key());
        if(keyValid.find(key)==keyValid.end()){
            msg = key + " should not be present";
            valid = valid && false;
            break;
        }
    }

    return {msg,valid};
}

inline auto copySchema = [](crow::json::wvalue &wSchema, std::string key) -> crow::json::wvalue {
    return wSchema[key];
};

inline auto convertData(const crow::json::rvalue &data) {

    std::variant<int, bool,double, std::string> value;
    std::string type = get_type_str(data.t());

    if(type == "Number"){
        bool is_double = isDouble(std::string(data));
        if(is_double){
            double d = double(data.d());
            value = d;
        } else {
            int i = int(data.i());
            value = i;
        }
    } else if(type == "True" || type == "False"){
        value = bool(data.b());
    } else {
        std::string val = data.s();
        value = val;
    }

    return value;
}

#endif