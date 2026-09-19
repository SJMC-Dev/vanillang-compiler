#ifndef VNLC_ERROR_HPP
#define VNLC_ERROR_HPP

#include <exception>
#include <string>
#include <string_view>

namespace vnlc {
    class Error : public std::exception {
    private:
        std::string message;

    public:
        Error(std::string_view message) : message(message) {}

        const char* what() const noexcept override {
            return message.c_str();
        }
    };
} // namespace vnlc

#endif // VNLC_ERROR_HPP