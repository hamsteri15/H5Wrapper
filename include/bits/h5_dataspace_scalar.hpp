#pragma once

#include "h5_dataspace.hpp"

namespace H5Wrapper{


class H5DataspaceScalar : public H5Dataspace {

public:
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
    H5DataspaceScalar() : H5Dataspace(H5S_SCALAR) {}
    #pragma GCC diagnostic pop
    

};
 
}