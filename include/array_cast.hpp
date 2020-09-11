#pragma once
#include <array>

namespace H5Wrapper::Utils{

// TODO Check if there's a better way
template <size_t N>
constexpr std::array<unsigned long long, N> cast_to_ulonglongarray(const std::array<size_t, N> arr) {

    std::array<unsigned long long, N> ret{};
    std::copy(arr.begin(), arr.end(), ret.begin());
    return ret;
}


}