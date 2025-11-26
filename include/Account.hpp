#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>
#include <ctime>

namespace bank {

/**
 * @brief Account types supported by the system
 */
enum class AccountType {
    Savings,
    Checking,
    FixedDeposit
};

/**
 * @brief Account status
 */
enum class AccountStatus {
    Active,
    Inactive,
    Frozen
};

/**
 * @brief Represents a bank account
 */
class Account {
public:
    Account();
    Account(int accountId, int userId, const std::string& accountNumber,
            AccountType type, double balance, double interestRate,
            AccountStatus status);

    // Getters
    int getAccountId() const { return m_accountId; }
    int getUserId() const { return m_userId; }
    std::string getAccountNumber() const { return m_accountNumber; }
    AccountType getType() const { return m_type; }
    double getBalance() const { return m_balance; }
    double getInterestRate() const { return m_interestRate; }
    AccountStatus getStatus() const { return m_status; }

    // Setters
    void setAccountId(int id) { m_accountId = id; }
    void setUserId(int id) { m_userId = id; }
    void setAccountNumber(const std::string& number) { m_accountNumber = number; }
    void setType(AccountType type) { m_type = type; }
    void setBalance(double balance) { m_balance = balance; }
    void setInterestRate(double rate) { m_interestRate = rate; }
    void setStatus(AccountStatus status) { m_status = status; }

    // Operations
    bool deposit(double amount);
    bool withdraw(double amount);
    bool transfer(Account& toAccount, double amount);

    // Utility functions
    static std::string typeToString(AccountType type);
    static AccountType stringToType(const std::string& typeStr);
    static std::string statusToString(AccountStatus status);
    static AccountStatus stringToStatus(const std::string& statusStr);
    static std::string generateAccountNumber();

private:
    int m_accountId;
    int m_userId;
    std::string m_accountNumber;
    AccountType m_type;
    double m_balance;
    double m_interestRate;
    AccountStatus m_status;
};

} // namespace bank

#endif // ACCOUNT_HPP
