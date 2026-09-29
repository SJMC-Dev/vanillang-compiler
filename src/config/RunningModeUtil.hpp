#ifndef VNLC_RUNNING_MODE_UTIL_HPP
#define VNLC_RUNNING_MODE_UTIL_HPP

#include "config/RunningMode.hpp"
#include <string_view>

namespace vnlc {
    namespace RunningModeUtil {
        [[nodiscard]] RunningMode getRunningMode(std::string_view modeLiteral);
    }
} // namespace vnlc

#endif // VNLC_RUNNING_MODE_UTIL_HPP
