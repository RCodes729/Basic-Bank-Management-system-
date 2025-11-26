#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <string>
#include <vector>
#include <memory>
#include <libpq-fe.h>

namespace bank {

/**
 * @brief Database connection wrapper for PostgreSQL
 */
class Database {
public:
    /**
     * @brief Construct a new Database object
     * @param host Database host
     * @param port Database port
     * @param dbname Database name
     * @param user Database user
     * @param password Database password
     */
    Database(const std::string& host = "localhost",
             const std::string& port = "5432",
             const std::string& dbname = "bank_management",
             const std::string& user = "postgres",
             const std::string& password = "");

    /**
     * @brief Destroy the Database object
     */
    ~Database();

    // Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    /**
     * @brief Connect to the database
     * @return true if connection successful
     */
    bool connect();

    /**
     * @brief Disconnect from the database
     */
    void disconnect();

    /**
     * @brief Check if connected to the database
     * @return true if connected
     */
    bool isConnected() const;

    /**
     * @brief Execute a query without expecting results
     * @param query SQL query to execute
     * @return true if successful
     */
    bool execute(const std::string& query);

    /**
     * @brief Execute a query with parameters
     * @param query SQL query with $1, $2, etc. placeholders
     * @param params Parameters to substitute
     * @return true if successful
     */
    bool executeParams(const std::string& query, const std::vector<std::string>& params);

    /**
     * @brief Execute a query and return results
     * @param query SQL query to execute
     * @return Vector of rows, each row is a vector of column values
     */
    std::vector<std::vector<std::string>> query(const std::string& query);

    /**
     * @brief Execute a parameterized query and return results
     * @param query SQL query with $1, $2, etc. placeholders
     * @param params Parameters to substitute
     * @return Vector of rows, each row is a vector of column values
     */
    std::vector<std::vector<std::string>> queryParams(const std::string& query, 
                                                       const std::vector<std::string>& params);

    /**
     * @brief Get the last error message
     * @return Error message string
     */
    std::string getLastError() const;

    /**
     * @brief Begin a transaction
     * @return true if successful
     */
    bool beginTransaction();

    /**
     * @brief Commit a transaction
     * @return true if successful
     */
    bool commitTransaction();

    /**
     * @brief Rollback a transaction
     * @return true if successful
     */
    bool rollbackTransaction();

private:
    std::string m_host;
    std::string m_port;
    std::string m_dbname;
    std::string m_user;
    std::string m_password;
    PGconn* m_connection;
    std::string m_lastError;
};

} // namespace bank

#endif // DATABASE_HPP
