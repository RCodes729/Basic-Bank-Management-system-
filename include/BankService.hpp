#ifndef BANK_SERVICE_HPP
#define BANK_SERVICE_HPP

#include <memory>
#include <vector>
#include <optional>
#include "Database.hpp"
#include "User.hpp"
#include "Account.hpp"
#include "Transaction.hpp"

namespace bank {

/**
 * @brief Service class that handles all banking operations
 */
class BankService {
public:
    /**
     * @brief Construct a new Bank Service object
     * @param db Shared pointer to database connection
     */
    explicit BankService(std::shared_ptr<Database> db);

    // User operations
    std::optional<User> createUser(const std::string& username, const std::string& password,
                                    const std::string& fullName, const std::string& email,
                                    const std::string& phone);
    std::optional<User> authenticateUser(const std::string& username, const std::string& password);
    std::optional<User> getUserById(int userId);
    std::optional<User> getUserByUsername(const std::string& username);
    bool updateUser(const User& user);
    bool deleteUser(int userId);

    // Account operations
    std::optional<Account> createAccount(int userId, AccountType type, double initialDeposit = 0.0);
    std::optional<Account> getAccountById(int accountId);
    std::optional<Account> getAccountByNumber(const std::string& accountNumber);
    std::vector<Account> getAccountsByUserId(int userId);
    bool updateAccountStatus(int accountId, AccountStatus status);
    bool deleteAccount(int accountId);

    // Transaction operations
    bool deposit(int accountId, double amount, const std::string& description = "Deposit");
    bool withdraw(int accountId, double amount, const std::string& description = "Withdrawal");
    bool transfer(int fromAccountId, int toAccountId, double amount, 
                  const std::string& description = "Transfer");
    std::vector<Transaction> getTransactionHistory(int accountId, int limit = 50);
    std::optional<Transaction> getTransactionById(int transactionId);

    // Utility operations
    double getTotalBalance(int userId);
    bool accountExists(const std::string& accountNumber);

private:
    std::shared_ptr<Database> m_db;

    // Helper methods
    bool recordTransaction(int accountId, TransactionType type, double amount,
                           double balanceAfter, const std::string& description,
                           int relatedAccountId = -1);
};

} // namespace bank

#endif // BANK_SERVICE_HPP
