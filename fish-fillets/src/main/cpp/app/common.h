#ifndef FISH_FILLETS_COMMON_H
#define FISH_FILLETS_COMMON_H

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cassert>
#include <stdexcept>
#include <cmath>
#include <version>

#include <string>
#include <vector>
#include <bitset>
#include <deque>
#include <array>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <initializer_list>
#include <memory>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>
#include <filesystem>
#include <functional>
#include <atomic>
#include <limits>

#ifdef __cpp_lib_math_constants
#include <numbers>
#else
namespace std::numbers {
    template<typename T>
    inline constexpr T pi_v = (T)3.141592653589793238462643383279502884L;
}
#endif

#include "types.h"
#include "log.h"

using namespace std::string_literals;
using namespace std::chrono_literals;

#ifndef __cpp_lib_unreachable
namespace std {
    [[noreturn, maybe_unused]] static void unreachable() {
        __builtin_unreachable();
    }
}
#endif

#endif //FISH_FILLETS_COMMON_H
