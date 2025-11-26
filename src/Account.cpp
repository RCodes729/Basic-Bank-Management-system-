#include "Account.hpp"
#include <random>
#include <sstream>
#include <iomanip>

namespace bank {

Account::Account()
    : m_accountId(0)
    , m_userId(0)
    , m_accountNumber("")
    , m_type(AccountType::Savings)
    , m_balance(0.0)
    , m_interestRate(0.0)
    , m_status(AccountStatus::Active)
{
}

Account::Account(int accountId, int userId, const std::string& accountNumber,
                 AccountType type, double balance, double interestRate,
                 AccountStatus status)
    : m_accountId(accountId)
    , m_userId(userId)
    , m_accountNumber(accountNumber)
    , m_type(type)
    , m_balance(balance)
    , m_interestRate(interestRate)
    , m_status(status)
{
}

bool Account::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    if (m_status != AccountStatus::Active) {
        return false;
    }
    m_balance += amount;
    return true;
}

bool Account::withdraw(double amount) {
    if (amount <= 0) {
        return false;
    }
    if (m_status != AccountStatus::Active) {
        return false;
    }
    if (amount > m_balance) {
        return false;
    }
    m_balance -= amount;
    return true;
}

bool Account::transfer(Account& toAccount, double amount) {
    if (!withdraw(amount)) {
        return false;
    }
    if (!toAccount.deposit(amount)) {
        // Rollback the withdrawal
        m_balance += amount;
        return false;
    }
    return true;
}

std::string Account::typeToString(AccountType type) {
    switch (type) {
        case AccountType::Savings:
            return "savings";
        case AccountType::Checking:
            return "checking";
        case AccountType::FixedDeposit:
            return "fixed_deposit";
        default:
            return "savings";
    }
}

AccountType Account::stringToType(const std::string& typeStr) {
    if (typeStr == "checking") {
        return AccountType::Checking;
    } else if (typeStr == "fixed_deposit") {
        return AccountType::FixedDeposit;
    }
    return AccountType::Savings;
}

std::string Account::statusToString(AccountStatus status) {
    switch (status) {
        case AccountStatus::Active:
            return "active";
        case AccountStatus::Inactive:
            return "inactive";
        case AccountStatus::Frozen:
            return "frozen";
        default:
            return "active";
    }
}

AccountStatus Account::stringToStatus(const std::string& statusStr) {
    if (statusStr == "inactive") {
        return AccountStatus::Inactive;
    } else if (statusStr == "frozen") {
        return AccountStatus::Frozen;
    }
    return AccountStatus::Active;
}

std::string Account::generateAccountNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    std::stringstream ss;
    ss << "ACC";
    for (int i = 0; i < 10; ++i) {
        ss << dis(gen);
    }
    return ss.str();
}

} // namespace bank
