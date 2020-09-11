#pragma once

#include "h5_datatype.hpp"

namespace H5Wrapper {

/*
template <class T = void> struct H5DatatypeCreator {

    static H5Datatype create() { return H5Datatype(); }
};
*/

template<class T>
struct H5DatatypeCreator{

    //static_assert(false, "No conversion to H5Datatype available.");
};


template <> struct H5DatatypeCreator<signed char> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_CHAR); }
};

template <> struct H5DatatypeCreator<unsigned char> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_UCHAR); }
};

template <> struct H5DatatypeCreator<short> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_SHORT); }
};

template <> struct H5DatatypeCreator<unsigned short> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_USHORT); }
};

template <> struct H5DatatypeCreator<int> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_INT); }
};

template <> struct H5DatatypeCreator<unsigned> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_UINT); }
};

template <> struct H5DatatypeCreator<long> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_LONG); }
};

template <> struct H5DatatypeCreator<unsigned long> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_ULONG); }
};

template <> struct H5DatatypeCreator<long long> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_LLONG); }
};

template <> struct H5DatatypeCreator<unsigned long long> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_ULLONG); }
};

template <> struct H5DatatypeCreator<float> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_FLOAT); }
};

template <> struct H5DatatypeCreator<double> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_DOUBLE); }
};

template <> struct H5DatatypeCreator<bool> {

    static H5Datatype create() { return H5Datatype::copy(H5T_NATIVE_HBOOL); }
};

} // namespace H5Wrapper