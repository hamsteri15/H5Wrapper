#pragma once

#include "h5_object.hpp"

namespace H5Wrapper{

template <class T> class H5DatatypeBase : public H5Object {

public:

    H5DatatypeBase() : H5Object( static_cast<T*>(this)->create_handle() ) {}


};

}

