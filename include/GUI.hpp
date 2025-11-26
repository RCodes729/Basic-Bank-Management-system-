#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "BankService.hpp"
#include "User.hpp"
#include "Account.hpp"

namespace bank {

/**
 * @brief Application states/screens
 */
enum class AppState {
    Login,
    Register,
    Dashboard,
    CreateAccount,
    Deposit,
    Withdraw,
    Transfer,
    TransactionHistory,
    Settings
};

/**
 * @brief Simple button class for SFML GUI
 */
class Button {
public:
    Button(float x, float y, float width, float height, 
           const std::string& text, const sf::Font& font);

    void render(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2i& mousePos);
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    sf::RectangleShape m_shape;
    sf::Text m_text;
    bool m_enabled;
};

/**
 * @brief Text input field for SFML GUI
 */
class TextInput {
public:
    TextInput(float x, float y, float width, float height,
              const std::string& placeholder, const sf::Font& font,
              bool isPassword = false);

    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    std::string getText() const { return m_text; }
    void setText(const std::string& text) { m_text = text; }
    void clear() { m_text.clear(); }
    bool isFocused() const { return m_focused; }
    void setFocused(bool focused) { m_focused = focused; }
    bool contains(const sf::Vector2i& mousePos) const;

private:
    sf::RectangleShape m_shape;
    sf::Text m_displayText;
    sf::Text m_placeholderText;
    std::string m_text;
    std::string m_placeholder;
    bool m_isPassword;
    bool m_focused;
    sf::Clock m_cursorClock;
};

/**
 * @brief Main GUI application class
 */
class BankGUI {
public:
    /**
     * @brief Construct a new Bank GUI object
     * @param service Shared pointer to bank service
     */
    explicit BankGUI(std::shared_ptr<BankService> service);

    /**
     * @brief Run the GUI application
     */
    void run();

private:
    // Window and rendering
    sf::RenderWindow m_window;
    sf::Font m_font;
    std::shared_ptr<BankService> m_service;

    // Application state
    AppState m_currentState;
    std::optional<User> m_currentUser;
    std::vector<Account> m_userAccounts;
    int m_selectedAccountIndex;
    std::string m_statusMessage;
    sf::Color m_statusColor;

    // Screen-specific data
    std::vector<Transaction> m_transactions;

    // Event handling
    void handleEvents();
    void handleLoginEvents(const sf::Event& event);
    void handleRegisterEvents(const sf::Event& event);
    void handleDashboardEvents(const sf::Event& event);
    void handleCreateAccountEvents(const sf::Event& event);
    void handleDepositEvents(const sf::Event& event);
    void handleWithdrawEvents(const sf::Event& event);
    void handleTransferEvents(const sf::Event& event);
    void handleTransactionHistoryEvents(const sf::Event& event);

    // Rendering
    void render();
    void renderLogin();
    void renderRegister();
    void renderDashboard();
    void renderCreateAccount();
    void renderDeposit();
    void renderWithdraw();
    void renderTransfer();
    void renderTransactionHistory();

    // Helper methods
    void showStatus(const std::string& message, bool isError = false);
    void clearInputs();
    void refreshAccounts();
    void logout();
    void drawCenteredText(const std::string& text, float y, unsigned int size, 
                          sf::Color color = sf::Color::White);

    // Input fields for different screens
    std::unique_ptr<TextInput> m_usernameInput;
    std::unique_ptr<TextInput> m_passwordInput;
    std::unique_ptr<TextInput> m_confirmPasswordInput;
    std::unique_ptr<TextInput> m_fullNameInput;
    std::unique_ptr<TextInput> m_emailInput;
    std::unique_ptr<TextInput> m_phoneInput;
    std::unique_ptr<TextInput> m_amountInput;
    std::unique_ptr<TextInput> m_targetAccountInput;
    std::unique_ptr<TextInput> m_descriptionInput;

    // Current focused input
    TextInput* m_focusedInput;
};

} // namespace bank

#endif // GUI_HPP
