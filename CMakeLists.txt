cmake_minimum_required(VERSION 3.16)
project(BankManagement VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
find_package(PostgreSQL REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Database.cpp
    src/Account.cpp
    src/Transaction.cpp
    src/User.cpp
    src/BankService.cpp
    src/GUI.cpp
)

# Header files
set(HEADERS
    include/Database.hpp
    include/Account.hpp
    include/Transaction.hpp
    include/User.hpp
    include/BankService.hpp
    include/GUI.hpp
)

# Create executable
add_executable(bank_management ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(bank_management PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(bank_management PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    ${PostgreSQL_LIBRARIES}
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(bank_management PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS bank_management DESTINATION bin)
install(DIRECTORY sql/ DESTINATION share/bank_management/sql)
install(DIRECTORY assets/ DESTINATION share/bank_management/assets OPTIONAL)
install(FILES README.md DESTINATION share/bank_management)

# Create build directory note
message(STATUS "")
message(STATUS "=== Build Instructions ===")
message(STATUS "mkdir build && cd build")
message(STATUS "cmake ..")
message(STATUS "make")
message(STATUS "")
message(STATUS "=== Database Setup ===")
message(STATUS "createdb bank_management")
message(STATUS "psql -d bank_management -f ../sql/schema.sql")
message(STATUS "")
