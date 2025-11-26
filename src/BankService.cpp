#include "BankService.hpp"
#include <sstream>

namespace bank {

BankService::BankService(std::shared_ptr<Database> db)
    : m_db(db)
{
}

// User operations

std::optional<User> BankService::createUser(const std::string& username, 
                                             const std::string& password,
                                             const std::string& fullName, 
                                             const std::string& email,
                                             const std::string& phone) 
{
    std::string passwordHash = User::hashPassword(password);
    
    std::string query = 
        "INSERT INTO users (username, password_hash, full_name, email, phone) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING user_id";
    
    std::vector<std::string> params = {username, passwordHash, fullName, email, phone};
    auto results = m_db->queryParams(query, params);
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    int userId = std::stoi(results[0][0]);
    return User(userId, username, passwordHash, fullName, email, phone);
}

std::optional<User> BankService::authenticateUser(const std::string& username, 
                                                   const std::string& password) 
{
    auto user = getUserByUsername(username);
    if (!user.has_value()) {
        return std::nullopt;
    }
    
    if (!user->verifyPassword(password)) {
        return std::nullopt;
    }
    
    return user;
}

std::optional<User> BankService::getUserById(int userId) {
    std::string query = 
        "SELECT user_id, username, password_hash, full_name, email, phone "
        "FROM users WHERE user_id = $1";
    
    auto results = m_db->queryParams(query, {std::to_string(userId)});
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    const auto& row = results[0];
    return User(std::stoi(row[0]), row[1], row[2], row[3], row[4], row[5]);
}

std::optional<User> BankService::getUserByUsername(const std::string& username) {
    std::string query = 
        "SELECT user_id, username, password_hash, full_name, email, phone "
        "FROM users WHERE username = $1";
    
    auto results = m_db->queryParams(query, {username});
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    const auto& row = results[0];
    return User(std::stoi(row[0]), row[1], row[2], row[3], row[4], row[5]);
}

bool BankService::updateUser(const User& user) {
    std::string query = 
        "UPDATE users SET username = $1, full_name = $2, email = $3, phone = $4 "
        "WHERE user_id = $5";
    
    std::vector<std::string> params = {
        user.getUsername(),
        user.getFullName(),
        user.getEmail(),
        user.getPhone(),
        std::to_string(user.getUserId())
    };
    
    return m_db->executeParams(query, params);
}

bool BankService::deleteUser(int userId) {
    std::string query = "DELETE FROM users WHERE user_id = $1";
    return m_db->executeParams(query, {std::to_string(userId)});
}

// Account operations

std::optional<Account> BankService::createAccount(int userId, AccountType type, 
                                                   double initialDeposit) 
{
    std::string accountNumber = Account::generateAccountNumber();
    std::string typeStr = Account::typeToString(type);
    
    // Default interest rates based on account type
    double interestRate = 0.0;
    switch (type) {
        case AccountType::Savings:
            interestRate = 3.5;
            break;
        case AccountType::Checking:
            interestRate = 0.5;
            break;
        case AccountType::FixedDeposit:
            interestRate = 6.0;
            break;
    }
    
    std::string query = 
        "INSERT INTO accounts (user_id, account_number, account_type, balance, interest_rate) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING account_id";
    
    std::vector<std::string> params = {
        std::to_string(userId),
        accountNumber,
        typeStr,
        std::to_string(initialDeposit),
        std::to_string(interestRate)
    };
    
    auto results = m_db->queryParams(query, params);
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    int accountId = std::stoi(results[0][0]);
    
    // Record initial deposit transaction if applicable
    if (initialDeposit > 0) {
        recordTransaction(accountId, TransactionType::Deposit, initialDeposit, 
                          initialDeposit, "Initial deposit");
    }
    
    return Account(accountId, userId, accountNumber, type, initialDeposit, 
                   interestRate, AccountStatus::Active);
}

std::optional<Account> BankService::getAccountById(int accountId) {
    std::string query = 
        "SELECT account_id, user_id, account_number, account_type, balance, "
        "interest_rate, status FROM accounts WHERE account_id = $1";
    
    auto results = m_db->queryParams(query, {std::to_string(accountId)});
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    const auto& row = results[0];
    return Account(
        std::stoi(row[0]),
        std::stoi(row[1]),
        row[2],
        Account::stringToType(row[3]),
        std::stod(row[4]),
        std::stod(row[5]),
        Account::stringToStatus(row[6])
    );
}

std::optional<Account> BankService::getAccountByNumber(const std::string& accountNumber) {
    std::string query = 
        "SELECT account_id, user_id, account_number, account_type, balance, "
        "interest_rate, status FROM accounts WHERE account_number = $1";
    
    auto results = m_db->queryParams(query, {accountNumber});
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    const auto& row = results[0];
    return Account(
        std::stoi(row[0]),
        std::stoi(row[1]),
        row[2],
        Account::stringToType(row[3]),
        std::stod(row[4]),
        std::stod(row[5]),
        Account::stringToStatus(row[6])
    );
}

std::vector<Account> BankService::getAccountsByUserId(int userId) {
    std::vector<Account> accounts;
    
    std::string query = 
        "SELECT account_id, user_id, account_number, account_type, balance, "
        "interest_rate, status FROM accounts WHERE user_id = $1 ORDER BY created_at";
    
    auto results = m_db->queryParams(query, {std::to_string(userId)});
    
    for (const auto& row : results) {
        accounts.emplace_back(
            std::stoi(row[0]),
            std::stoi(row[1]),
            row[2],
            Account::stringToType(row[3]),
            std::stod(row[4]),
            std::stod(row[5]),
            Account::stringToStatus(row[6])
        );
    }
    
    return accounts;
}

bool BankService::updateAccountStatus(int accountId, AccountStatus status) {
    std::string query = "UPDATE accounts SET status = $1 WHERE account_id = $2";
    return m_db->executeParams(query, {
        Account::statusToString(status),
        std::to_string(accountId)
    });
}

bool BankService::deleteAccount(int accountId) {
    std::string query = "DELETE FROM accounts WHERE account_id = $1";
    return m_db->executeParams(query, {std::to_string(accountId)});
}

// Transaction operations

bool BankService::deposit(int accountId, double amount, const std::string& description) {
    if (amount <= 0) {
        return false;
    }
    
    auto account = getAccountById(accountId);
    if (!account.has_value()) {
        return false;
    }
    
    if (account->getStatus() != AccountStatus::Active) {
        return false;
    }
    
    double newBalance = account->getBalance() + amount;
    
    m_db->beginTransaction();
    
    std::string query = "UPDATE accounts SET balance = $1 WHERE account_id = $2";
    if (!m_db->executeParams(query, {std::to_string(newBalance), std::to_string(accountId)})) {
        m_db->rollbackTransaction();
        return false;
    }
    
    if (!recordTransaction(accountId, TransactionType::Deposit, amount, newBalance, description)) {
        m_db->rollbackTransaction();
        return false;
    }
    
    m_db->commitTransaction();
    return true;
}

bool BankService::withdraw(int accountId, double amount, const std::string& description) {
    if (amount <= 0) {
        return false;
    }
    
    auto account = getAccountById(accountId);
    if (!account.has_value()) {
        return false;
    }
    
    if (account->getStatus() != AccountStatus::Active) {
        return false;
    }
    
    if (account->getBalance() < amount) {
        return false;
    }
    
    double newBalance = account->getBalance() - amount;
    
    m_db->beginTransaction();
    
    std::string query = "UPDATE accounts SET balance = $1 WHERE account_id = $2";
    if (!m_db->executeParams(query, {std::to_string(newBalance), std::to_string(accountId)})) {
        m_db->rollbackTransaction();
        return false;
    }
    
    if (!recordTransaction(accountId, TransactionType::Withdrawal, amount, newBalance, description)) {
        m_db->rollbackTransaction();
        return false;
    }
    
    m_db->commitTransaction();
    return true;
}

bool BankService::transfer(int fromAccountId, int toAccountId, double amount,
                            const std::string& description) 
{
    if (amount <= 0) {
        return false;
    }
    
    auto fromAccount = getAccountById(fromAccountId);
    auto toAccount = getAccountById(toAccountId);
    
    if (!fromAccount.has_value() || !toAccount.has_value()) {
        return false;
    }
    
    if (fromAccount->getStatus() != AccountStatus::Active || 
        toAccount->getStatus() != AccountStatus::Active) {
        return false;
    }
    
    if (fromAccount->getBalance() < amount) {
        return false;
    }
    
    double fromNewBalance = fromAccount->getBalance() - amount;
    double toNewBalance = toAccount->getBalance() + amount;
    
    m_db->beginTransaction();
    
    // Update from account
    std::string query = "UPDATE accounts SET balance = $1 WHERE account_id = $2";
    if (!m_db->executeParams(query, {std::to_string(fromNewBalance), std::to_string(fromAccountId)})) {
        m_db->rollbackTransaction();
        return false;
    }
    
    // Update to account
    if (!m_db->executeParams(query, {std::to_string(toNewBalance), std::to_string(toAccountId)})) {
        m_db->rollbackTransaction();
        return false;
    }
    
    // Record transactions
    std::string transferDesc = description + " to " + toAccount->getAccountNumber();
    if (!recordTransaction(fromAccountId, TransactionType::TransferOut, amount, 
                           fromNewBalance, transferDesc, toAccountId)) {
        m_db->rollbackTransaction();
        return false;
    }
    
    transferDesc = description + " from " + fromAccount->getAccountNumber();
    if (!recordTransaction(toAccountId, TransactionType::TransferIn, amount, 
                           toNewBalance, transferDesc, fromAccountId)) {
        m_db->rollbackTransaction();
        return false;
    }
    
    m_db->commitTransaction();
    return true;
}

std::vector<Transaction> BankService::getTransactionHistory(int accountId, int limit) {
    std::vector<Transaction> transactions;
    
    std::string query = 
        "SELECT transaction_id, account_id, transaction_type, amount, balance_after, "
        "description, related_account_id, created_at "
        "FROM transactions WHERE account_id = $1 "
        "ORDER BY created_at DESC LIMIT $2";
    
    auto results = m_db->queryParams(query, {
        std::to_string(accountId),
        std::to_string(limit)
    });
    
    for (const auto& row : results) {
        Transaction t;
        t.setTransactionId(std::stoi(row[0]));
        t.setAccountId(std::stoi(row[1]));
        t.setType(Transaction::stringToType(row[2]));
        t.setAmount(std::stod(row[3]));
        t.setBalanceAfter(std::stod(row[4]));
        t.setDescription(row[5]);
        t.setRelatedAccountId(row[6].empty() ? -1 : std::stoi(row[6]));
        t.setCreatedAt(row[7]);
        transactions.push_back(t);
    }
    
    return transactions;
}

std::optional<Transaction> BankService::getTransactionById(int transactionId) {
    std::string query = 
        "SELECT transaction_id, account_id, transaction_type, amount, balance_after, "
        "description, related_account_id, created_at "
        "FROM transactions WHERE transaction_id = $1";
    
    auto results = m_db->queryParams(query, {std::to_string(transactionId)});
    
    if (results.empty()) {
        return std::nullopt;
    }
    
    const auto& row = results[0];
    Transaction t;
    t.setTransactionId(std::stoi(row[0]));
    t.setAccountId(std::stoi(row[1]));
    t.setType(Transaction::stringToType(row[2]));
    t.setAmount(std::stod(row[3]));
    t.setBalanceAfter(std::stod(row[4]));
    t.setDescription(row[5]);
    t.setRelatedAccountId(row[6].empty() ? -1 : std::stoi(row[6]));
    t.setCreatedAt(row[7]);
    
    return t;
}

// Utility operations

double BankService::getTotalBalance(int userId) {
    std::string query = "SELECT COALESCE(SUM(balance), 0) FROM accounts WHERE user_id = $1";
    auto results = m_db->queryParams(query, {std::to_string(userId)});
    
    if (results.empty() || results[0].empty()) {
        return 0.0;
    }
    
    return std::stod(results[0][0]);
}

bool BankService::accountExists(const std::string& accountNumber) {
    std::string query = "SELECT 1 FROM accounts WHERE account_number = $1";
    auto results = m_db->queryParams(query, {accountNumber});
    return !results.empty();
}

// Helper methods

bool BankService::recordTransaction(int accountId, TransactionType type, double amount,
                                     double balanceAfter, const std::string& description,
                                     int relatedAccountId) 
{
    std::string query;
    std::vector<std::string> params;
    
    if (relatedAccountId >= 0) {
        query = "INSERT INTO transactions (account_id, transaction_type, amount, "
                "balance_after, description, related_account_id) "
                "VALUES ($1, $2, $3, $4, $5, $6)";
        params = {
            std::to_string(accountId),
            Transaction::typeToString(type),
            std::to_string(amount),
            std::to_string(balanceAfter),
            description,
            std::to_string(relatedAccountId)
        };
    } else {
        query = "INSERT INTO transactions (account_id, transaction_type, amount, "
                "balance_after, description) VALUES ($1, $2, $3, $4, $5)";
        params = {
            std::to_string(accountId),
            Transaction::typeToString(type),
            std::to_string(amount),
            std::to_string(balanceAfter),
            description
        };
    }
    
    return m_db->executeParams(query, params);
}

} // namespace bank
