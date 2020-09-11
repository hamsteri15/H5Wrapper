#pragma once

#include "h5_dataspace.hpp"

namespace H5Wrapper{




class H5DataspaceAll : public H5Dataspace {

public:
    
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
    H5DataspaceAll() : H5Dataspace(H5S_ALL) {}
    #pragma GCC diagnostic pop
    

};
 
}