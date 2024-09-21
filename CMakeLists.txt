# cmake_minimum_required(VERSION 3.15)
# project (api)
# set (INCLUDE_PATHS boost_1_81_0 crow/include)
# add_executable (api combined/server.cpp)
# target_include_directories (api PUBLIC ${INCLUDE_PATHS})

cmake_minimum_required(VERSION 3.0)
project(Server)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    BACKEND/server.cpp
    BACKEND/Module/Billing/Billing.cpp
    BACKEND/Module/Request/RequestManagement.cpp
    BACKEND/User/Admin/Admin.cpp
    BACKEND/User/User.cpp
    BACKEND/Utility/Project/Project.cpp
    BACKEND/Utility/Machine/Machine.cpp
    BACKEND/Utility/Inventory/Inventory.cpp
    BACKEND/Utility/Task/Task.cpp
    BACKEND/Utility/Docs/Docs.cpp
)

# Include directories
set(INCLUDE_DIRS
    /usr/local/include/mongocxx/v_noabi
    /usr/local/include/libmongoc-1.0
    /usr/local/include/bsoncxx/v_noabi
    /usr/local/include/libbson-1.0
    /usr/local/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc
    boost/include
    crow/include
    jwt/
    BACKEND/
    BACKEND/Helper/
    BACKEND/Module/Billing/
    BACKEND/Module/Request/
    BACKEND/User/
    BACKEND/User/Admin/
    BACKEND/Utility/
    BACKEND/Utility/Project/
    BACKEND/Utility/Machine/
    BACKEND/Utility/Inventory/
    BACKEND/Utility/Task/
    BACKEND/Utility/Docs/
)

# Link directories
link_directories(/usr/local/lib)

add_compile_definitions(CURRENT_FUNCTION_LIST_DIR="${CMAKE_CURRENT_LIST_DIR}")
add_compile_definitions(PROJECT_PATH="BACKEND")

# Executable
add_executable(app.out ${SOURCES})

# Definations
add_definitions(-DCROW_USE_LOCALTIMEZONE -DCROW_FANCY_LOG)

target_compile_definitions(app.out PRIVATE BOOST_JSON_STANDALONE) 
target_compile_features(app.out PRIVATE cxx_std_17) 

# Include directories
target_include_directories(app.out PRIVATE ${INCLUDE_DIRS})

# Link libraries
target_link_libraries(app.out PRIVATE mongocxx bsoncxx pthread boost_system ssl crypto)


# c++ --std=c++11 combined/server.cpp -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 -L/usr/local/lib -lmongocxx -lbsoncxx -lpthread -lboost_system -o app.out
