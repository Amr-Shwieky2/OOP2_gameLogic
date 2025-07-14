#pragma once
#include <stdexcept>
#include <string>

class GameException : public std::runtime_error {
public:
    GameException(const std::string& message, const char* file, int line)
        : std::runtime_error(formatMessage(message, file, line)),
          m_message(message), m_file(file), m_line(line) {}

    const std::string& originalMessage() const noexcept { return m_message; }
    const char* file() const noexcept { return m_file; }
    int line() const noexcept { return m_line; }

private:
    std::string m_message;
    const char* m_file;
    int m_line;

    static std::string formatMessage(const std::string& message, const char* file, int line) {
        return std::string(file) + ":" + std::to_string(line) + " - " + message;
    }
};

class ResourceException : public GameException {
public:
    ResourceException(const std::string& msg, const char* file, int line)
        : GameException(msg, file, line) {}
};

class WindowException : public GameException {
public:
    using GameException::GameException;
};

class InitializationException : public GameException {
public:
    using GameException::GameException;
};

#define THROW_GAME_EXCEPTION(msg) throw GameException((msg), __FILE__, __LINE__)
#define THROW_RESOURCE_EXCEPTION(msg) throw ResourceException((msg), __FILE__, __LINE__)
#define THROW_WINDOW_EXCEPTION(msg) throw WindowException((msg), __FILE__, __LINE__)
#define THROW_INITIALIZATION_EXCEPTION(msg) throw InitializationException((msg), __FILE__, __LINE__)

