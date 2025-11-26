#include "GUI.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace bank {

// Button implementation
Button::Button(float x, float y, float width, float height, 
               const std::string& text, const sf::Font& font)
    : m_enabled(true)
{
    m_shape.setPosition(x, y);
    m_shape.setSize(sf::Vector2f(width, height));
    m_shape.setFillColor(sf::Color(70, 130, 180));
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2);

    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::White);
    
    // Center text in button
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.0f, 
                     textBounds.top + textBounds.height / 2.0f);
    m_text.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::render(sf::RenderWindow& window) {
    if (m_enabled) {
        m_shape.setFillColor(sf::Color(70, 130, 180));
    } else {
        m_shape.setFillColor(sf::Color(100, 100, 100));
    }
    window.draw(m_shape);
    window.draw(m_text);
}

bool Button::isClicked(const sf::Vector2i& mousePos) {
    if (!m_enabled) return false;
    return m_shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), 
                                               static_cast<float>(mousePos.y));
}

// TextInput implementation
TextInput::TextInput(float x, float y, float width, float height,
                     const std::string& placeholder, const sf::Font& font,
                     bool isPassword)
    : m_placeholder(placeholder)
    , m_isPassword(isPassword)
    , m_focused(false)
{
    m_shape.setPosition(x, y);
    m_shape.setSize(sf::Vector2f(width, height));
    m_shape.setFillColor(sf::Color(50, 50, 50));
    m_shape.setOutlineColor(sf::Color(100, 100, 100));
    m_shape.setOutlineThickness(2);

    m_displayText.setFont(font);
    m_displayText.setCharacterSize(16);
    m_displayText.setFillColor(sf::Color::White);
    m_displayText.setPosition(x + 10, y + (height - 20) / 2);

    m_placeholderText.setFont(font);
    m_placeholderText.setString(placeholder);
    m_placeholderText.setCharacterSize(16);
    m_placeholderText.setFillColor(sf::Color(150, 150, 150));
    m_placeholderText.setPosition(x + 10, y + (height - 20) / 2);
}

void TextInput::render(sf::RenderWindow& window) {
    if (m_focused) {
        m_shape.setOutlineColor(sf::Color(70, 130, 180));
    } else {
        m_shape.setOutlineColor(sf::Color(100, 100, 100));
    }
    
    window.draw(m_shape);
    
    if (m_text.empty()) {
        window.draw(m_placeholderText);
    } else {
        std::string displayStr = m_isPassword ? std::string(m_text.length(), '*') : m_text;
        
        // Add cursor if focused
        if (m_focused && m_cursorClock.getElapsedTime().asSeconds() < 0.5f) {
            displayStr += "|";
        }
        if (m_focused && m_cursorClock.getElapsedTime().asSeconds() >= 1.0f) {
            m_cursorClock.restart();
        }
        
        m_displayText.setString(displayStr);
        window.draw(m_displayText);
    }
}

void TextInput::handleEvent(const sf::Event& event) {
    if (!m_focused) return;
    
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') { // Backspace
            if (!m_text.empty()) {
                m_text.pop_back();
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 127) {
            m_text += static_cast<char>(event.text.unicode);
        }
    }
}

bool TextInput::contains(const sf::Vector2i& mousePos) const {
    return m_shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), 
                                               static_cast<float>(mousePos.y));
}

// BankGUI implementation
BankGUI::BankGUI(std::shared_ptr<BankService> service)
    : m_window(sf::VideoMode(800, 600), "Bank Management System")
    , m_service(service)
    , m_currentState(AppState::Login)
    , m_selectedAccountIndex(-1)
    , m_statusColor(sf::Color::White)
    , m_focusedInput(nullptr)
{
    // Load font - try common system font paths
    // For better portability, consider bundling a font in the assets/ directory
    // or using environment variables to configure font paths
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",          // Debian/Ubuntu
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",                       // Arch Linux
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",                    // Fedora
        "/Library/Fonts/Arial.ttf",                                   // macOS
        "C:\\Windows\\Fonts\\arial.ttf"                               // Windows
    };
    
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (m_font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        throw std::runtime_error("Failed to load font. Please install DejaVu or Liberation fonts.");
    }

    m_window.setFramerateLimit(60);
    
    // Initialize input fields
    float centerX = 300;
    float inputWidth = 200;
    float inputHeight = 35;
    
    m_usernameInput = std::make_unique<TextInput>(centerX, 200, inputWidth, inputHeight, 
                                                   "Username", m_font);
    m_passwordInput = std::make_unique<TextInput>(centerX, 250, inputWidth, inputHeight, 
                                                   "Password", m_font, true);
    m_confirmPasswordInput = std::make_unique<TextInput>(centerX, 300, inputWidth, inputHeight, 
                                                          "Confirm Password", m_font, true);
    m_fullNameInput = std::make_unique<TextInput>(centerX, 350, inputWidth, inputHeight, 
                                                   "Full Name", m_font);
    m_emailInput = std::make_unique<TextInput>(centerX, 400, inputWidth, inputHeight, 
                                                "Email", m_font);
    m_phoneInput = std::make_unique<TextInput>(centerX, 450, inputWidth, inputHeight, 
                                                "Phone", m_font);
    m_amountInput = std::make_unique<TextInput>(centerX, 250, inputWidth, inputHeight, 
                                                 "Amount", m_font);
    m_targetAccountInput = std::make_unique<TextInput>(centerX, 300, inputWidth, inputHeight, 
                                                        "Target Account #", m_font);
    m_descriptionInput = std::make_unique<TextInput>(centerX, 350, inputWidth, inputHeight, 
                                                      "Description", m_font);
}

void BankGUI::run() {
    while (m_window.isOpen()) {
        handleEvents();
        render();
    }
}

void BankGUI::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            return;
        }

        // Handle mouse clicks for focus
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            
            // Unfocus all inputs first
            if (m_focusedInput) {
                m_focusedInput->setFocused(false);
                m_focusedInput = nullptr;
            }
            
            // Check which input was clicked
            std::vector<TextInput*> inputs = {
                m_usernameInput.get(), m_passwordInput.get(), 
                m_confirmPasswordInput.get(), m_fullNameInput.get(),
                m_emailInput.get(), m_phoneInput.get(),
                m_amountInput.get(), m_targetAccountInput.get(),
                m_descriptionInput.get()
            };
            
            for (auto* input : inputs) {
                if (input && input->contains(mousePos)) {
                    input->setFocused(true);
                    m_focusedInput = input;
                    break;
                }
            }
        }

        // Handle text input
        if (event.type == sf::Event::TextEntered) {
            if (m_focusedInput) {
                m_focusedInput->handleEvent(event);
            }
        }

        // Handle screen-specific events
        switch (m_currentState) {
            case AppState::Login:
                handleLoginEvents(event);
                break;
            case AppState::Register:
                handleRegisterEvents(event);
                break;
            case AppState::Dashboard:
                handleDashboardEvents(event);
                break;
            case AppState::CreateAccount:
                handleCreateAccountEvents(event);
                break;
            case AppState::Deposit:
                handleDepositEvents(event);
                break;
            case AppState::Withdraw:
                handleWithdrawEvents(event);
                break;
            case AppState::Transfer:
                handleTransferEvents(event);
                break;
            case AppState::TransactionHistory:
                handleTransactionHistoryEvents(event);
                break;
            default:
                break;
        }
    }
}

void BankGUI::handleLoginEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        // Login button
        Button loginBtn(300, 310, 200, 40, "Login", m_font);
        if (loginBtn.isClicked(mousePos)) {
            auto user = m_service->authenticateUser(m_usernameInput->getText(), 
                                                     m_passwordInput->getText());
            if (user.has_value()) {
                m_currentUser = user;
                refreshAccounts();
                m_currentState = AppState::Dashboard;
                clearInputs();
                showStatus("Welcome, " + user->getFullName() + "!");
            } else {
                showStatus("Invalid username or password", true);
            }
        }
        
        // Register link
        Button registerBtn(300, 360, 200, 40, "Create Account", m_font);
        if (registerBtn.isClicked(mousePos)) {
            m_currentState = AppState::Register;
            clearInputs();
        }
    }
}

void BankGUI::handleRegisterEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        // Register button
        Button registerBtn(300, 510, 200, 40, "Register", m_font);
        if (registerBtn.isClicked(mousePos)) {
            if (m_passwordInput->getText() != m_confirmPasswordInput->getText()) {
                showStatus("Passwords do not match", true);
                return;
            }
            
            if (m_usernameInput->getText().empty() || m_passwordInput->getText().empty() ||
                m_fullNameInput->getText().empty() || m_emailInput->getText().empty()) {
                showStatus("Please fill in all required fields", true);
                return;
            }
            
            auto user = m_service->createUser(
                m_usernameInput->getText(),
                m_passwordInput->getText(),
                m_fullNameInput->getText(),
                m_emailInput->getText(),
                m_phoneInput->getText()
            );
            
            if (user.has_value()) {
                m_currentUser = user;
                m_currentState = AppState::Dashboard;
                clearInputs();
                showStatus("Registration successful!");
            } else {
                showStatus("Registration failed. Username or email may already exist.", true);
            }
        }
        
        // Back button
        Button backBtn(300, 560, 200, 40, "Back to Login", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Login;
            clearInputs();
        }
    }
}

void BankGUI::handleDashboardEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        // Account selection (clickable account boxes)
        float startY = 150;
        for (size_t i = 0; i < m_userAccounts.size() && i < 4; ++i) {
            sf::FloatRect accountBox({50, startY + i * 80}, {300, 70});
            if (accountBox.contains({static_cast<float>(mousePos.x), 
                                     static_cast<float>(mousePos.y)})) {
                m_selectedAccountIndex = static_cast<int>(i);
            }
        }
        
        // Action buttons
        Button newAccountBtn(400, 150, 150, 40, "New Account", m_font);
        if (newAccountBtn.isClicked(mousePos)) {
            m_currentState = AppState::CreateAccount;
        }
        
        Button depositBtn(400, 200, 150, 40, "Deposit", m_font);
        if (depositBtn.isClicked(mousePos) && m_selectedAccountIndex >= 0) {
            m_currentState = AppState::Deposit;
        }
        
        Button withdrawBtn(560, 200, 150, 40, "Withdraw", m_font);
        if (withdrawBtn.isClicked(mousePos) && m_selectedAccountIndex >= 0) {
            m_currentState = AppState::Withdraw;
        }
        
        Button transferBtn(400, 250, 150, 40, "Transfer", m_font);
        if (transferBtn.isClicked(mousePos) && m_selectedAccountIndex >= 0) {
            m_currentState = AppState::Transfer;
        }
        
        Button historyBtn(560, 250, 150, 40, "History", m_font);
        if (historyBtn.isClicked(mousePos) && m_selectedAccountIndex >= 0) {
            m_transactions = m_service->getTransactionHistory(
                m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)].getAccountId()
            );
            m_currentState = AppState::TransactionHistory;
        }
        
        Button logoutBtn(650, 20, 120, 35, "Logout", m_font);
        if (logoutBtn.isClicked(mousePos)) {
            logout();
        }
    }
}

void BankGUI::handleCreateAccountEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        // Account type buttons
        Button savingsBtn(250, 200, 140, 50, "Savings", m_font);
        Button checkingBtn(400, 200, 140, 50, "Checking", m_font);
        Button fixedBtn(250, 260, 290, 50, "Fixed Deposit", m_font);
        
        AccountType type;
        bool typeSelected = false;
        
        if (savingsBtn.isClicked(mousePos)) {
            type = AccountType::Savings;
            typeSelected = true;
        } else if (checkingBtn.isClicked(mousePos)) {
            type = AccountType::Checking;
            typeSelected = true;
        } else if (fixedBtn.isClicked(mousePos)) {
            type = AccountType::FixedDeposit;
            typeSelected = true;
        }
        
        if (typeSelected) {
            auto account = m_service->createAccount(m_currentUser->getUserId(), type, 0);
            if (account.has_value()) {
                refreshAccounts();
                showStatus("Account created: " + account->getAccountNumber());
                m_currentState = AppState::Dashboard;
            } else {
                showStatus("Failed to create account", true);
            }
        }
        
        Button backBtn(300, 350, 200, 40, "Back", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Dashboard;
        }
    }
}

void BankGUI::handleDepositEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        Button depositBtn(300, 310, 200, 40, "Deposit", m_font);
        if (depositBtn.isClicked(mousePos)) {
            try {
                double amount = std::stod(m_amountInput->getText());
                int accountId = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)].getAccountId();
                
                if (m_service->deposit(accountId, amount, m_descriptionInput->getText())) {
                    refreshAccounts();
                    showStatus("Deposit successful!");
                    clearInputs();
                    m_currentState = AppState::Dashboard;
                } else {
                    showStatus("Deposit failed", true);
                }
            } catch (...) {
                showStatus("Invalid amount", true);
            }
        }
        
        Button backBtn(300, 360, 200, 40, "Back", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Dashboard;
            clearInputs();
        }
    }
}

void BankGUI::handleWithdrawEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        Button withdrawBtn(300, 310, 200, 40, "Withdraw", m_font);
        if (withdrawBtn.isClicked(mousePos)) {
            try {
                double amount = std::stod(m_amountInput->getText());
                int accountId = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)].getAccountId();
                
                if (m_service->withdraw(accountId, amount, m_descriptionInput->getText())) {
                    refreshAccounts();
                    showStatus("Withdrawal successful!");
                    clearInputs();
                    m_currentState = AppState::Dashboard;
                } else {
                    showStatus("Withdrawal failed. Check balance.", true);
                }
            } catch (...) {
                showStatus("Invalid amount", true);
            }
        }
        
        Button backBtn(300, 360, 200, 40, "Back", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Dashboard;
            clearInputs();
        }
    }
}

void BankGUI::handleTransferEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        Button transferBtn(300, 410, 200, 40, "Transfer", m_font);
        if (transferBtn.isClicked(mousePos)) {
            try {
                double amount = std::stod(m_amountInput->getText());
                int fromAccountId = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)].getAccountId();
                
                auto toAccount = m_service->getAccountByNumber(m_targetAccountInput->getText());
                if (!toAccount.has_value()) {
                    showStatus("Target account not found", true);
                    return;
                }
                
                if (m_service->transfer(fromAccountId, toAccount->getAccountId(), amount, 
                                         m_descriptionInput->getText())) {
                    refreshAccounts();
                    showStatus("Transfer successful!");
                    clearInputs();
                    m_currentState = AppState::Dashboard;
                } else {
                    showStatus("Transfer failed. Check balance.", true);
                }
            } catch (...) {
                showStatus("Invalid amount", true);
            }
        }
        
        Button backBtn(300, 460, 200, 40, "Back", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Dashboard;
            clearInputs();
        }
    }
}

void BankGUI::handleTransactionHistoryEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        
        Button backBtn(300, 530, 200, 40, "Back", m_font);
        if (backBtn.isClicked(mousePos)) {
            m_currentState = AppState::Dashboard;
        }
    }
}

void BankGUI::render() {
    m_window.clear(sf::Color(30, 30, 40));
    
    switch (m_currentState) {
        case AppState::Login:
            renderLogin();
            break;
        case AppState::Register:
            renderRegister();
            break;
        case AppState::Dashboard:
            renderDashboard();
            break;
        case AppState::CreateAccount:
            renderCreateAccount();
            break;
        case AppState::Deposit:
            renderDeposit();
            break;
        case AppState::Withdraw:
            renderWithdraw();
            break;
        case AppState::Transfer:
            renderTransfer();
            break;
        case AppState::TransactionHistory:
            renderTransactionHistory();
            break;
        default:
            break;
    }
    
    // Render status message
    if (!m_statusMessage.empty()) {
        sf::Text statusText;
        statusText.setFont(m_font);
        statusText.setString(m_statusMessage);
        statusText.setCharacterSize(14);
        statusText.setFillColor(m_statusColor);
        sf::FloatRect textBounds = statusText.getLocalBounds();
        statusText.setPosition(400.f - textBounds.width / 2.f, 560.f);
        m_window.draw(statusText);
    }
    
    m_window.display();
}

void BankGUI::renderLogin() {
    drawCenteredText("Bank Management System", 50, 32, sf::Color(70, 130, 180));
    drawCenteredText("Login", 120, 24);
    
    // Labels
    sf::Text usernameLabel;
    usernameLabel.setFont(m_font);
    usernameLabel.setString("Username:");
    usernameLabel.setCharacterSize(16);
    usernameLabel.setPosition(200, 205);
    m_window.draw(usernameLabel);
    
    sf::Text passwordLabel;
    passwordLabel.setFont(m_font);
    passwordLabel.setString("Password:");
    passwordLabel.setCharacterSize(16);
    passwordLabel.setPosition(200, 255);
    m_window.draw(passwordLabel);
    
    // Input fields
    m_usernameInput->render(m_window);
    m_passwordInput->render(m_window);
    
    // Buttons
    Button loginBtn(300, 310, 200, 40, "Login", m_font);
    loginBtn.render(m_window);
    
    Button registerBtn(300, 360, 200, 40, "Create Account", m_font);
    registerBtn.render(m_window);
}

void BankGUI::renderRegister() {
    drawCenteredText("Bank Management System", 50, 32, sf::Color(70, 130, 180));
    drawCenteredText("Registration", 120, 24);
    
    // Labels and inputs
    sf::Text label;
    label.setFont(m_font);
    label.setCharacterSize(14);
    
    label.setString("Username:");
    label.setPosition(200, 205);
    m_window.draw(label);
    m_usernameInput->render(m_window);
    
    label.setString("Password:");
    label.setPosition(200, 255);
    m_window.draw(label);
    m_passwordInput->render(m_window);
    
    label.setString("Confirm:");
    label.setPosition(200, 305);
    m_window.draw(label);
    m_confirmPasswordInput->render(m_window);
    
    label.setString("Full Name:");
    label.setPosition(200, 355);
    m_window.draw(label);
    m_fullNameInput->render(m_window);
    
    label.setString("Email:");
    label.setPosition(200, 405);
    m_window.draw(label);
    m_emailInput->render(m_window);
    
    label.setString("Phone:");
    label.setPosition(200, 455);
    m_window.draw(label);
    m_phoneInput->render(m_window);
    
    // Buttons
    Button registerBtn(300, 510, 200, 40, "Register", m_font);
    registerBtn.render(m_window);
    
    Button backBtn(300, 560, 200, 40, "Back to Login", m_font);
    backBtn.render(m_window);
}

void BankGUI::renderDashboard() {
    drawCenteredText("Dashboard", 30, 28, sf::Color(70, 130, 180));
    
    if (m_currentUser) {
        sf::Text welcomeText;
        welcomeText.setFont(m_font);
        welcomeText.setString("Welcome, " + m_currentUser->getFullName());
        welcomeText.setCharacterSize(16);
        welcomeText.setPosition(50, 70);
        m_window.draw(welcomeText);
        
        // Total balance
        double totalBalance = m_service->getTotalBalance(m_currentUser->getUserId());
        std::stringstream ss;
        ss << "Total Balance: $" << std::fixed << std::setprecision(2) << totalBalance;
        sf::Text balanceText;
        balanceText.setFont(m_font);
        balanceText.setString(ss.str());
        balanceText.setCharacterSize(18);
        balanceText.setFillColor(sf::Color::Green);
        balanceText.setPosition(50, 100);
        m_window.draw(balanceText);
    }
    
    // Account list
    sf::Text accountsLabel;
    accountsLabel.setFont(m_font);
    accountsLabel.setString("Your Accounts:");
    accountsLabel.setCharacterSize(16);
    accountsLabel.setPosition(50, 130);
    m_window.draw(accountsLabel);
    
    float startY = 150;
    for (size_t i = 0; i < m_userAccounts.size() && i < 4; ++i) {
        const auto& account = m_userAccounts[i];
        
        sf::RectangleShape box(sf::Vector2f(300, 70));
        box.setPosition(50, startY + i * 80);
        
        if (static_cast<int>(i) == m_selectedAccountIndex) {
            box.setFillColor(sf::Color(50, 80, 120));
            box.setOutlineColor(sf::Color(70, 130, 180));
        } else {
            box.setFillColor(sf::Color(40, 40, 50));
            box.setOutlineColor(sf::Color(60, 60, 70));
        }
        box.setOutlineThickness(2);
        m_window.draw(box);
        
        // Account info
        sf::Text accNum;
        accNum.setFont(m_font);
        accNum.setString(account.getAccountNumber());
        accNum.setCharacterSize(14);
        accNum.setPosition(60, startY + i * 80 + 10);
        m_window.draw(accNum);
        
        std::string typeStr = Account::typeToString(account.getType());
        typeStr[0] = static_cast<char>(std::toupper(typeStr[0]));
        sf::Text accType;
        accType.setFont(m_font);
        accType.setString(typeStr);
        accType.setCharacterSize(12);
        accType.setFillColor(sf::Color(150, 150, 150));
        accType.setPosition(60, startY + i * 80 + 30);
        m_window.draw(accType);
        
        std::stringstream ss;
        ss << "$" << std::fixed << std::setprecision(2) << account.getBalance();
        sf::Text accBalance;
        accBalance.setFont(m_font);
        accBalance.setString(ss.str());
        accBalance.setCharacterSize(16);
        accBalance.setFillColor(sf::Color::Green);
        accBalance.setPosition(250, startY + i * 80 + 20);
        m_window.draw(accBalance);
    }
    
    // Action buttons
    Button newAccountBtn(400, 150, 150, 40, "New Account", m_font);
    newAccountBtn.render(m_window);
    
    Button depositBtn(400, 200, 150, 40, "Deposit", m_font);
    depositBtn.setEnabled(m_selectedAccountIndex >= 0);
    depositBtn.render(m_window);
    
    Button withdrawBtn(560, 200, 150, 40, "Withdraw", m_font);
    withdrawBtn.setEnabled(m_selectedAccountIndex >= 0);
    withdrawBtn.render(m_window);
    
    Button transferBtn(400, 250, 150, 40, "Transfer", m_font);
    transferBtn.setEnabled(m_selectedAccountIndex >= 0);
    transferBtn.render(m_window);
    
    Button historyBtn(560, 250, 150, 40, "History", m_font);
    historyBtn.setEnabled(m_selectedAccountIndex >= 0);
    historyBtn.render(m_window);
    
    Button logoutBtn(650, 20, 120, 35, "Logout", m_font);
    logoutBtn.render(m_window);
}

void BankGUI::renderCreateAccount() {
    drawCenteredText("Create New Account", 100, 24);
    drawCenteredText("Select Account Type:", 160, 18);
    
    Button savingsBtn(250, 200, 140, 50, "Savings", m_font);
    savingsBtn.render(m_window);
    
    Button checkingBtn(400, 200, 140, 50, "Checking", m_font);
    checkingBtn.render(m_window);
    
    Button fixedBtn(250, 260, 290, 50, "Fixed Deposit", m_font);
    fixedBtn.render(m_window);
    
    Button backBtn(300, 350, 200, 40, "Back", m_font);
    backBtn.render(m_window);
}

void BankGUI::renderDeposit() {
    drawCenteredText("Deposit", 100, 24);
    
    if (m_selectedAccountIndex >= 0 && 
        static_cast<size_t>(m_selectedAccountIndex) < m_userAccounts.size()) {
        const auto& account = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)];
        drawCenteredText("Account: " + account.getAccountNumber(), 160, 16);
        
        std::stringstream ss;
        ss << "Current Balance: $" << std::fixed << std::setprecision(2) << account.getBalance();
        drawCenteredText(ss.str(), 190, 14);
    }
    
    sf::Text label;
    label.setFont(m_font);
    label.setString("Amount:");
    label.setCharacterSize(14);
    label.setPosition(230, 255);
    m_window.draw(label);
    m_amountInput->render(m_window);
    
    Button depositBtn(300, 310, 200, 40, "Deposit", m_font);
    depositBtn.render(m_window);
    
    Button backBtn(300, 360, 200, 40, "Back", m_font);
    backBtn.render(m_window);
}

void BankGUI::renderWithdraw() {
    drawCenteredText("Withdraw", 100, 24);
    
    if (m_selectedAccountIndex >= 0 && 
        static_cast<size_t>(m_selectedAccountIndex) < m_userAccounts.size()) {
        const auto& account = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)];
        drawCenteredText("Account: " + account.getAccountNumber(), 160, 16);
        
        std::stringstream ss;
        ss << "Current Balance: $" << std::fixed << std::setprecision(2) << account.getBalance();
        drawCenteredText(ss.str(), 190, 14);
    }
    
    sf::Text label;
    label.setFont(m_font);
    label.setString("Amount:");
    label.setCharacterSize(14);
    label.setPosition(230, 255);
    m_window.draw(label);
    m_amountInput->render(m_window);
    
    Button withdrawBtn(300, 310, 200, 40, "Withdraw", m_font);
    withdrawBtn.render(m_window);
    
    Button backBtn(300, 360, 200, 40, "Back", m_font);
    backBtn.render(m_window);
}

void BankGUI::renderTransfer() {
    drawCenteredText("Transfer", 100, 24);
    
    if (m_selectedAccountIndex >= 0 && 
        static_cast<size_t>(m_selectedAccountIndex) < m_userAccounts.size()) {
        const auto& account = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)];
        drawCenteredText("From Account: " + account.getAccountNumber(), 160, 16);
        
        std::stringstream ss;
        ss << "Current Balance: $" << std::fixed << std::setprecision(2) << account.getBalance();
        drawCenteredText(ss.str(), 190, 14);
    }
    
    sf::Text label;
    label.setFont(m_font);
    label.setCharacterSize(14);
    
    label.setString("Amount:");
    label.setPosition(200, 255);
    m_window.draw(label);
    m_amountInput->render(m_window);
    
    label.setString("To Account:");
    label.setPosition(185, 305);
    m_window.draw(label);
    m_targetAccountInput->render(m_window);
    
    label.setString("Description:");
    label.setPosition(185, 355);
    m_window.draw(label);
    m_descriptionInput->render(m_window);
    
    Button transferBtn(300, 410, 200, 40, "Transfer", m_font);
    transferBtn.render(m_window);
    
    Button backBtn(300, 460, 200, 40, "Back", m_font);
    backBtn.render(m_window);
}

void BankGUI::renderTransactionHistory() {
    drawCenteredText("Transaction History", 30, 24, sf::Color(70, 130, 180));
    
    if (m_selectedAccountIndex >= 0 && 
        static_cast<size_t>(m_selectedAccountIndex) < m_userAccounts.size()) {
        const auto& account = m_userAccounts[static_cast<size_t>(m_selectedAccountIndex)];
        drawCenteredText("Account: " + account.getAccountNumber(), 70, 16);
    }
    
    // Transaction list
    float startY = 100;
    size_t maxDisplay = 8;
    
    for (size_t i = 0; i < m_transactions.size() && i < maxDisplay; ++i) {
        const auto& trans = m_transactions[i];
        
        sf::RectangleShape box(sf::Vector2f(700, 45));
        box.setPosition(50, startY + i * 50);
        box.setFillColor(sf::Color(40, 40, 50));
        box.setOutlineColor(sf::Color(60, 60, 70));
        box.setOutlineThickness(1);
        m_window.draw(box);
        
        // Type
        std::string typeStr = Transaction::typeToString(trans.getType());
        sf::Text typeText;
        typeText.setFont(m_font);
        typeText.setString(typeStr);
        typeText.setCharacterSize(14);
        sf::Color typeColor;
        if (trans.getType() == TransactionType::Deposit || 
            trans.getType() == TransactionType::TransferIn) {
            typeColor = sf::Color::Green;
        } else {
            typeColor = sf::Color::Red;
        }
        typeText.setFillColor(typeColor);
        typeText.setPosition(60, startY + i * 50 + 12);
        m_window.draw(typeText);
        
        // Amount
        std::stringstream ss;
        ss << (trans.getType() == TransactionType::Deposit || 
               trans.getType() == TransactionType::TransferIn ? "+" : "-");
        ss << "$" << std::fixed << std::setprecision(2) << trans.getAmount();
        sf::Text amountText;
        amountText.setFont(m_font);
        amountText.setString(ss.str());
        amountText.setCharacterSize(14);
        amountText.setFillColor(typeColor);
        amountText.setPosition(200, startY + i * 50 + 12);
        m_window.draw(amountText);
        
        // Description
        sf::Text descText;
        descText.setFont(m_font);
        descText.setString(trans.getDescription());
        descText.setCharacterSize(12);
        descText.setFillColor(sf::Color(150, 150, 150));
        descText.setPosition(320, startY + i * 50 + 14);
        m_window.draw(descText);
        
        // Date
        sf::Text dateText;
        dateText.setFont(m_font);
        dateText.setString(trans.getCreatedAt());
        dateText.setCharacterSize(10);
        dateText.setFillColor(sf::Color(100, 100, 100));
        dateText.setPosition(580, startY + i * 50 + 15);
        m_window.draw(dateText);
    }
    
    Button backBtn(300, 530, 200, 40, "Back", m_font);
    backBtn.render(m_window);
}

void BankGUI::showStatus(const std::string& message, bool isError) {
    m_statusMessage = message;
    m_statusColor = isError ? sf::Color::Red : sf::Color::Green;
}

void BankGUI::clearInputs() {
    m_usernameInput->clear();
    m_passwordInput->clear();
    m_confirmPasswordInput->clear();
    m_fullNameInput->clear();
    m_emailInput->clear();
    m_phoneInput->clear();
    m_amountInput->clear();
    m_targetAccountInput->clear();
    m_descriptionInput->clear();
    m_statusMessage.clear();
}

void BankGUI::refreshAccounts() {
    if (m_currentUser) {
        m_userAccounts = m_service->getAccountsByUserId(m_currentUser->getUserId());
        if (!m_userAccounts.empty() && m_selectedAccountIndex < 0) {
            m_selectedAccountIndex = 0;
        }
    }
}

void BankGUI::logout() {
    m_currentUser.reset();
    m_userAccounts.clear();
    m_selectedAccountIndex = -1;
    m_currentState = AppState::Login;
    clearInputs();
}

void BankGUI::drawCenteredText(const std::string& text, float y, unsigned int size, 
                                sf::Color color) 
{
    sf::Text sfText;
    sfText.setFont(m_font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sf::FloatRect textBounds = sfText.getLocalBounds();
    sfText.setPosition(400.f - textBounds.width / 2.f, y);
    m_window.draw(sfText);
}

} // namespace bank
