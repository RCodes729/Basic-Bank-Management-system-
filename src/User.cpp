#include "User.hpp"
#include <functional>
#include <sstream>
#include <iomanip>

namespace bank {

User::User()
    : m_userId(0)
    , m_username("")
    , m_passwordHash("")
    , m_fullName("")
    , m_email("")
    , m_phone("")
{
}

User::User(int userId, const std::string& username, const std::string& passwordHash,
           const std::string& fullName, const std::string& email, const std::string& phone)
    : m_userId(userId)
    , m_username(username)
    , m_passwordHash(passwordHash)
    , m_fullName(fullName)
    , m_email(email)
    , m_phone(phone)
{
}

std::string User::hashPassword(const std::string& password) {
    // Simple hash function - in production, use a proper cryptographic library like bcrypt
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return ss.str();
}

bool User::verifyPassword(const std::string& password) const {
    return m_passwordHash == hashPassword(password);
}

} // namespace bank
