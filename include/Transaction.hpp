#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>
#include <ctime>

namespace bank {

/**
 * @brief Transaction types
 */
enum class TransactionType {
    Deposit,
    Withdrawal,
    TransferIn,
    TransferOut
};

/**
 * @brief Represents a bank transaction
 */
class Transaction {
public:
    Transaction();
    Transaction(int transactionId, int accountId, TransactionType type,
                double amount, double balanceAfter, const std::string& description,
                int relatedAccountId = -1);

    // Getters
    int getTransactionId() const { return m_transactionId; }
    int getAccountId() const { return m_accountId; }
    TransactionType getType() const { return m_type; }
    double getAmount() const { return m_amount; }
    double getBalanceAfter() const { return m_balanceAfter; }
    std::string getDescription() const { return m_description; }
    int getRelatedAccountId() const { return m_relatedAccountId; }
    std::string getCreatedAt() const { return m_createdAt; }

    // Setters
    void setTransactionId(int id) { m_transactionId = id; }
    void setAccountId(int id) { m_accountId = id; }
    void setType(TransactionType type) { m_type = type; }
    void setAmount(double amount) { m_amount = amount; }
    void setBalanceAfter(double balance) { m_balanceAfter = balance; }
    void setDescription(const std::string& desc) { m_description = desc; }
    void setRelatedAccountId(int id) { m_relatedAccountId = id; }
    void setCreatedAt(const std::string& timestamp) { m_createdAt = timestamp; }

    // Utility functions
    static std::string typeToString(TransactionType type);
    static TransactionType stringToType(const std::string& typeStr);

private:
    int m_transactionId;
    int m_accountId;
    TransactionType m_type;
    double m_amount;
    double m_balanceAfter;
    std::string m_description;
    int m_relatedAccountId;
    std::string m_createdAt;
};

} // namespace bank

#endif // TRANSACTION_HPP
