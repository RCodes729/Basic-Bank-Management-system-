#include "Transaction.hpp"

namespace bank {

Transaction::Transaction()
    : m_transactionId(0)
    , m_accountId(0)
    , m_type(TransactionType::Deposit)
    , m_amount(0.0)
    , m_balanceAfter(0.0)
    , m_description("")
    , m_relatedAccountId(-1)
    , m_createdAt("")
{
}

Transaction::Transaction(int transactionId, int accountId, TransactionType type,
                         double amount, double balanceAfter, const std::string& description,
                         int relatedAccountId)
    : m_transactionId(transactionId)
    , m_accountId(accountId)
    , m_type(type)
    , m_amount(amount)
    , m_balanceAfter(balanceAfter)
    , m_description(description)
    , m_relatedAccountId(relatedAccountId)
    , m_createdAt("")
{
}

std::string Transaction::typeToString(TransactionType type) {
    switch (type) {
        case TransactionType::Deposit:
            return "deposit";
        case TransactionType::Withdrawal:
            return "withdrawal";
        case TransactionType::TransferIn:
            return "transfer_in";
        case TransactionType::TransferOut:
            return "transfer_out";
        default:
            return "deposit";
    }
}

TransactionType Transaction::stringToType(const std::string& typeStr) {
    if (typeStr == "withdrawal") {
        return TransactionType::Withdrawal;
    } else if (typeStr == "transfer_in") {
        return TransactionType::TransferIn;
    } else if (typeStr == "transfer_out") {
        return TransactionType::TransferOut;
    }
    return TransactionType::Deposit;
}

} // namespace bank
