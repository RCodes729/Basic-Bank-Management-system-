#include "Database.hpp"
#include <iostream>
#include <cstring>

namespace bank {

Database::Database(const std::string& host,
                   const std::string& port,
                   const std::string& dbname,
                   const std::string& user,
                   const std::string& password)
    : m_host(host)
    , m_port(port)
    , m_dbname(dbname)
    , m_user(user)
    , m_password(password)
    , m_connection(nullptr)
{
}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    if (m_connection != nullptr) {
        return true; // Already connected
    }

    std::string connStr = "host=" + m_host +
                          " port=" + m_port +
                          " dbname=" + m_dbname +
                          " user=" + m_user;
    
    if (!m_password.empty()) {
        connStr += " password=" + m_password;
    }

    m_connection = PQconnectdb(connStr.c_str());

    if (PQstatus(m_connection) != CONNECTION_OK) {
        m_lastError = PQerrorMessage(m_connection);
        PQfinish(m_connection);
        m_connection = nullptr;
        return false;
    }

    return true;
}

void Database::disconnect() {
    if (m_connection != nullptr) {
        PQfinish(m_connection);
        m_connection = nullptr;
    }
}

bool Database::isConnected() const {
    return m_connection != nullptr && PQstatus(m_connection) == CONNECTION_OK;
}

bool Database::execute(const std::string& query) {
    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return false;
    }

    PGresult* result = PQexec(m_connection, query.c_str());
    ExecStatusType status = PQresultStatus(result);

    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        m_lastError = PQerrorMessage(m_connection);
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

bool Database::executeParams(const std::string& query, const std::vector<std::string>& params) {
    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return false;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& param : params) {
        paramValues.push_back(param.c_str());
    }

    PGresult* result = PQexecParams(m_connection, query.c_str(),
                                     static_cast<int>(params.size()),
                                     nullptr, paramValues.data(),
                                     nullptr, nullptr, 0);

    ExecStatusType status = PQresultStatus(result);

    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        m_lastError = PQerrorMessage(m_connection);
        PQclear(result);
        return false;
    }

    PQclear(result);
    return true;
}

std::vector<std::vector<std::string>> Database::query(const std::string& queryStr) {
    std::vector<std::vector<std::string>> results;

    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return results;
    }

    PGresult* result = PQexec(m_connection, queryStr.c_str());
    
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        m_lastError = PQerrorMessage(m_connection);
        PQclear(result);
        return results;
    }

    int numRows = PQntuples(result);
    int numCols = PQnfields(result);

    for (int i = 0; i < numRows; ++i) {
        std::vector<std::string> row;
        for (int j = 0; j < numCols; ++j) {
            char* value = PQgetvalue(result, i, j);
            row.push_back(value ? value : "");
        }
        results.push_back(row);
    }

    PQclear(result);
    return results;
}

std::vector<std::vector<std::string>> Database::queryParams(
    const std::string& queryStr, 
    const std::vector<std::string>& params) 
{
    std::vector<std::vector<std::string>> results;

    if (!isConnected()) {
        m_lastError = "Not connected to database";
        return results;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& param : params) {
        paramValues.push_back(param.c_str());
    }

    PGresult* result = PQexecParams(m_connection, queryStr.c_str(),
                                     static_cast<int>(params.size()),
                                     nullptr, paramValues.data(),
                                     nullptr, nullptr, 0);

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        m_lastError = PQerrorMessage(m_connection);
        PQclear(result);
        return results;
    }

    int numRows = PQntuples(result);
    int numCols = PQnfields(result);

    for (int i = 0; i < numRows; ++i) {
        std::vector<std::string> row;
        for (int j = 0; j < numCols; ++j) {
            char* value = PQgetvalue(result, i, j);
            row.push_back(value ? value : "");
        }
        results.push_back(row);
    }

    PQclear(result);
    return results;
}

std::string Database::getLastError() const {
    return m_lastError;
}

bool Database::beginTransaction() {
    return execute("BEGIN");
}

bool Database::commitTransaction() {
    return execute("COMMIT");
}

bool Database::rollbackTransaction() {
    return execute("ROLLBACK");
}

} // namespace bank
