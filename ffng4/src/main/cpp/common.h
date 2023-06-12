#ifndef FISH_FILLETS_COMMON_H
#define FISH_FILLETS_COMMON_H

#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cassert>
#include <stdexcept>
#include <cmath>

#include <string>
#include <vector>
#include <bitset>
#include <deque>
#include <array>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <memory>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <optional>
#include <filesystem>
#include <functional>
#include <atomic>

#include "types.h"
#include "utils.h"
#include "log.h"

using namespace std::string_literals;

#ifndef __cpp_lib_unreachable
namespace std {
    [[noreturn, maybe_unused]] static void unreachable() {
        __builtin_unreachable();
    }
}
#endif

#endif //FISH_FILLETS_COMMON_H
