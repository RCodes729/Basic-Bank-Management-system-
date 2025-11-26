# Basic Bank Management System

A comprehensive bank management system built with C++, PostgreSQL, and SFML for the graphical user interface.

## Features

- **User Management**
  - User registration with secure password hashing
  - User authentication/login
  - User profile management

- **Account Management**
  - Create multiple account types (Savings, Checking, Fixed Deposit)
  - View account balances
  - Account status management (Active, Inactive, Frozen)

- **Transactions**
  - Deposit funds
  - Withdraw funds
  - Transfer between accounts
  - Transaction history with detailed records

- **Modern GUI**
  - Built with SFML for cross-platform compatibility
  - Clean, intuitive interface
  - Real-time balance updates

## Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- PostgreSQL 10 or higher
- SFML 2.6 or higher

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ libpq-dev libsfml-dev postgresql
```

**Fedora:**
```bash
sudo dnf install cmake gcc-c++ postgresql-devel SFML-devel postgresql-server
```

**macOS (using Homebrew):**
```bash
brew install cmake postgresql sfml
```

**Windows:**
- Install [CMake](https://cmake.org/download/)
- Install [PostgreSQL](https://www.postgresql.org/download/windows/)
- Install [SFML](https://www.sfml-dev.org/download.php)
- Install Visual Studio with C++ support

## Database Setup

1. Start PostgreSQL service:
   ```bash
   sudo service postgresql start
   ```

2. Create the database:
   ```bash
   sudo -u postgres createdb bank_management
   ```

3. Run the schema setup:
   ```bash
   sudo -u postgres psql -d bank_management -f sql/schema.sql
   ```

## Building the Application

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Basic-Bank-Management-system-.git
   cd Basic-Bank-Management-system-
   ```

2. Create build directory and compile:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. The executable will be created as `bank_management` in the build directory.

## Running the Application

### Using Environment Variables

Configure database connection using environment variables:

```bash
export DB_HOST=localhost
export DB_PORT=5432
export DB_NAME=bank_management
export DB_USER=postgres
export DB_PASSWORD=your_password

./bank_management
```

### Default Configuration

If no environment variables are set, the application uses:
- Host: localhost
- Port: 5432
- Database: bank_management
- User: postgres
- Password: (empty)

## Usage Guide

### Login Screen
- Enter your username and password
- Click "Login" to access your dashboard
- Click "Create Account" to register as a new user

### Registration
- Fill in all required fields (Username, Password, Full Name, Email)
- Phone number is optional
- Click "Register" to create your account

### Dashboard
- View all your bank accounts
- See total balance across all accounts
- Click on an account to select it
- Use action buttons to perform operations

### Operations
- **New Account**: Create a new bank account
- **Deposit**: Add funds to selected account
- **Withdraw**: Remove funds from selected account
- **Transfer**: Move funds between accounts
- **History**: View transaction history

## Project Structure

```
Basic-Bank-Management-system-/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # License file
├── include/                # Header files
│   ├── Database.hpp        # PostgreSQL database wrapper
│   ├── Account.hpp         # Account class definition
│   ├── Transaction.hpp     # Transaction class definition
│   ├── User.hpp            # User class definition
│   ├── BankService.hpp     # Business logic service
│   └── GUI.hpp             # SFML GUI classes
├── src/                    # Source files
│   ├── main.cpp            # Application entry point
│   ├── Database.cpp        # Database implementation
│   ├── Account.cpp         # Account implementation
│   ├── Transaction.cpp     # Transaction implementation
│   ├── User.cpp            # User implementation
│   ├── BankService.cpp     # Business logic implementation
│   └── GUI.cpp             # GUI implementation
├── sql/                    # Database scripts
│   └── schema.sql          # Database schema
└── assets/                 # Assets (fonts, images)
```

## Architecture

### Database Layer
- `Database.hpp/cpp`: Wrapper around libpq for PostgreSQL operations
- Supports parameterized queries to prevent SQL injection
- Transaction support (BEGIN, COMMIT, ROLLBACK)

### Business Logic Layer
- `BankService.hpp/cpp`: All banking operations
- User authentication and management
- Account CRUD operations
- Transaction processing with atomicity

### Presentation Layer
- `GUI.hpp/cpp`: SFML-based graphical interface
- Event-driven architecture
- Multiple screens (Login, Register, Dashboard, etc.)

## Security Considerations

- Passwords are hashed before storage (simple hash for demo; use bcrypt/argon2 in production)
- Parameterized SQL queries to prevent SQL injection
- Transaction atomicity for financial operations
- Balance validation before withdrawals/transfers

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [SFML](https://www.sfml-dev.org/) - Simple and Fast Multimedia Library
- [PostgreSQL](https://www.postgresql.org/) - Open source database
- [libpq](https://www.postgresql.org/docs/current/libpq.html) - PostgreSQL C library
