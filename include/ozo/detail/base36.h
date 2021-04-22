#pragma once

#include <boost/hana/string.hpp>
#include <boost/hana/if.hpp>
#include <string>
#include <cmath>

namespace ozo::detail {

template<unsigned long u>
constexpr auto ultob36() {
    constexpr int base = 36;
    constexpr int d = u % base;
    constexpr char sym = boost::hana::if_(d < 10, '0' + d, 'A' + d - 10);
    constexpr auto str = boost::hana::string_c<sym>;

    if constexpr (u / base > 0) {
        return ultob36<u / base>() + str;
    } else {
        return str;
    }
}

template<long i>
constexpr auto ltob36() {
    return ultob36<static_cast<unsigned long>(i)>();
}

inline void reverse(std::string& str) {
    const std::size_t size = str.size();
    const std::size_t times = std::ceil(size / 2);
    for (std::size_t count = 0; count < times; count++) {
        std::swap(str[count], str[size - count - 1]);
    }
}

inline std::string ltob36(long i) {
    auto u = static_cast<unsigned long>(i);
    std::string out ;
    constexpr auto base = 36;
    do {
        const char d = u % base;
        out.push_back(d < 10 ? '0' + d : 'A' + d - 10);
        u /= base;
    } while (u > 0);

    reverse(out);

    return out;
}

} // namespace ozo::detail
