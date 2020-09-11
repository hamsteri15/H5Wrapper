#pragma once

#include "h5_datatype_creator.hpp"
#include <type_traits>

namespace H5Wrapper {

template <typename T, typename = void> struct is_h5_convertible_helper : public std::false_type {};

template <typename T>
struct is_h5_convertible_helper<T, std::void_t<decltype(H5DatatypeCreator<T>::create())>>
    : public std::true_type {};

template <typename T, typename = void>
struct is_h5_convertible : public is_h5_convertible_helper<T> {};

template <typename T> constexpr bool is_h5_convertible_v = is_h5_convertible<T>::value;

} // namespace H5Wrapper