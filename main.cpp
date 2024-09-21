#include <crow.h>
#include <unordered_map>
#include <iostream>

using namespace std;

// User model
struct User {
    string username;
    string password; // In a real application, store hashed passwords
};

// Database of registered users
unordered_map<string, User> users;

// Session management
unordered_map<string, string> sessions; // Maps session tokens to usernames

// Middleware to protect routes requiring authentication
void authenticate(crow::request& req, crow::response& res, std::function<void()> next) {
    auto it = sessions.find(req.get_header_value("Authorization"));
    if (it != sessions.end()) {
        // Session token found, user is authenticated
        next();
    } else {
        // Session token not found, redirect to login page
        res.code = 401; // Unauthorized
        res.write("Unauthorized");
        res.end();
    }
}

// Route handler for user registration
void registerUser(const crow::request& req, crow::response& res) {
    // Extract username and password from request body
    string username = req.body["username"];
    string password = req.body["password"];

    // Check if the username already exists
    if (users.find(username) != users.end()) {
        res.code = 400; // Bad request
        res.write("Username already exists");
        res.end();
        return;
    }

    // Create a new user and store in the database
    users[username] = {username, password};

    // Respond with success message
    res.write("User registered successfully");
    res.end();
}

// Route handler for user login
void loginUser(const crow::request& req, crow::response& res) {
    // Extract username and password from request body
    string username = req.body["username"];
    string password = req.body["password"];

    // Check if the user exists and the password matches
    auto it = users.find(username);
    if (it != users.end() && it->second.password == password) {
        // Generate session token and store in session management
        string sessionToken = "random_session_token"; // Generate a random token
        sessions[sessionToken] = username;

        // Respond with success message and session token
        res.write(sessionToken);
    } else {
        // Invalid username or password
        res.code = 401; // Unauthorized
        res.write("Invalid username or password");
    }
    res.end();
}

// Main function
int main() {
    // Create a Crow app
    crow::SimpleApp app;

    // Define routes for user registration and login
    CROW_ROUTE(app, "/register").methods("POST"_method)(registerUser);
    CROW_ROUTE(app, "/login").methods("POST"_method)(loginUser);

    // Protect routes requiring authentication with middleware
    CROW_ROUTE(app, "/protected").middleware(authenticate)([](const crow::request& req, crow::response& res) {
        res.write("Protected route accessed");
        res.end();
    });

    // Start the server
    app.port(8080).multithreaded().run();

    return 0;
}