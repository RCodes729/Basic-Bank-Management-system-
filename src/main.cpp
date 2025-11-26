#include <iostream>
#include <memory>
#include <cstdlib>
#include "Database.hpp"
#include "BankService.hpp"
#include "GUI.hpp"

void printUsage() {
    std::cout << "Bank Management System\n\n";
    std::cout << "Environment Variables:\n";
    std::cout << "  DB_HOST     - PostgreSQL host (default: localhost)\n";
    std::cout << "  DB_PORT     - PostgreSQL port (default: 5432)\n";
    std::cout << "  DB_NAME     - Database name (default: bank_management)\n";
    std::cout << "  DB_USER     - Database user (default: postgres)\n";
    std::cout << "  DB_PASSWORD - Database password (default: empty)\n";
    std::cout << "\nUsage:\n";
    std::cout << "  ./bank_management      - Run the GUI application\n";
    std::cout << "  ./bank_management -h   - Show this help\n";
}

int main(int argc, char* argv[]) {
    // Check for help flag
    if (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printUsage();
        return 0;
    }

    // Get database configuration from environment variables
    const char* dbHost = std::getenv("DB_HOST");
    const char* dbPort = std::getenv("DB_PORT");
    const char* dbName = std::getenv("DB_NAME");
    const char* dbUser = std::getenv("DB_USER");
    const char* dbPassword = std::getenv("DB_PASSWORD");

    std::string host = dbHost ? dbHost : "localhost";
    std::string port = dbPort ? dbPort : "5432";
    std::string name = dbName ? dbName : "bank_management";
    std::string user = dbUser ? dbUser : "postgres";
    std::string password = dbPassword ? dbPassword : "";

    std::cout << "Connecting to database " << name << " at " << host << ":" << port << "...\n";

    // Create database connection
    auto db = std::make_shared<bank::Database>(host, port, name, user, password);
    
    if (!db->connect()) {
        std::cerr << "Error: Failed to connect to database!\n";
        std::cerr << "Details: " << db->getLastError() << "\n\n";
        std::cerr << "Please ensure PostgreSQL is running and the database exists.\n";
        std::cerr << "You can create the database and schema using:\n";
        std::cerr << "  createdb " << name << "\n";
        std::cerr << "  psql -d " << name << " -f sql/schema.sql\n";
        return 1;
    }

    std::cout << "Database connected successfully!\n";

    // Create bank service
    auto service = std::make_shared<bank::BankService>(db);

    // Run GUI
    try {
        bank::BankGUI gui(service);
        gui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Goodbye!\n";
    return 0;
}
