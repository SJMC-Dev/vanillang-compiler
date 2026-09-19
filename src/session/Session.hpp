#ifndef VNLC_SESSION_HPP
#define VNLC_SESSION_HPP

#include "config/Config.hpp"

namespace vnlc {
    class Session {
    private:
        const Config config;

    public:
        Session(Config&& config);

        void run();
    };
} // namespace vnlc

#endif // VNLC_SESSION_HPP