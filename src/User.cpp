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
    // WARNING: This is a demonstration-only hash function using std::hash.
    // For production use, replace with a secure password hashing library such as:
    // - bcrypt (libsodium's crypto_pwhash or libbcrypt)
    // - Argon2 (libsodium's crypto_pwhash with Argon2id)
    // - scrypt
    // 
    // std::hash is NOT cryptographically secure and should never be used
    // for real password storage as it lacks:
    // - Salt (vulnerable to rainbow table attacks)
    // - Computational cost (vulnerable to brute force)
    // - Memory hardness (vulnerable to GPU/ASIC attacks)
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
