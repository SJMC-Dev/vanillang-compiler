#ifndef VNLC_SESSION_HPP
#define VNLC_SESSION_HPP

#include "config/VnlcConfig.hpp"

class VnlcSession {
private:
    const VnlcConfig config;

public:
    VnlcSession(VnlcConfig&& config);

    void run();
};

#endif // VNLC_SESSION_HPP