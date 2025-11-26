#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include "Account.hpp"

namespace bank {

/**
 * @brief Represents a bank user/customer
 */
class User {
public:
    User();
    User(int userId, const std::string& username, const std::string& passwordHash,
         const std::string& fullName, const std::string& email, const std::string& phone);

    // Getters
    int getUserId() const { return m_userId; }
    std::string getUsername() const { return m_username; }
    std::string getPasswordHash() const { return m_passwordHash; }
    std::string getFullName() const { return m_fullName; }
    std::string getEmail() const { return m_email; }
    std::string getPhone() const { return m_phone; }

    // Setters
    void setUserId(int id) { m_userId = id; }
    void setUsername(const std::string& username) { m_username = username; }
    void setPasswordHash(const std::string& hash) { m_passwordHash = hash; }
    void setFullName(const std::string& name) { m_fullName = name; }
    void setEmail(const std::string& email) { m_email = email; }
    void setPhone(const std::string& phone) { m_phone = phone; }

    // Password utilities
    static std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password) const;

private:
    int m_userId;
    std::string m_username;
    std::string m_passwordHash;
    std::string m_fullName;
    std::string m_email;
    std::string m_phone;
};

} // namespace bank

#endif // USER_HPP
