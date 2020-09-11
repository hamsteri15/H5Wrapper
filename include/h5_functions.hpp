#pragma once

#include <hdf5.h>

#include "runtime_assert.hpp"
#include "array_cast.hpp"


/*
NOTE! These may be handy:
https://github.com/BlueBrain/HighFive
https://github.com/steven-varga/h5cpp
*/

namespace H5Wrapper{

struct H5{

    static bool is_valid(hid_t obj)  {

        auto ret = H5Iis_valid(obj);
        Utils::runtime_assert(ret >= 0, "H5Is_valid fails.");
        return ret > 0;

    }


    static hid_t type_copy(hid_t type_id) {

        auto ret = H5Tcopy(type_id);
        Utils::runtime_assert(ret >= 0, "H5 type copy fails.");
        return ret;
    } 


    static void type_close(hid_t type_id) {
        auto err = H5Tclose(type_id);
        Utils::runtime_assert(err >= 0, "H5 type close fails.");
    }


    static void dataspace_close(hid_t dataspace_id) {
        auto err = H5Sclose(dataspace_id);
        Utils::runtime_assert(err >= 0, "H5 dataspace close fails.");
    }

    template<size_t N>
    static hid_t create_simple_dataspace(const std::array<size_t, N>& current_dims, 
                                         const std::array<size_t, N>& max_dims) {

        

        auto cdims = Utils::cast_to_ulonglongarray(current_dims);
        auto mdims = Utils::cast_to_ulonglongarray(max_dims);
        auto ret = H5Screate_simple(int(N), cdims.cbegin(), mdims.cbegin());
        //auto ret = H5Screate(N, cdims.cbegin(), mdims.cbegin());
        Utils::runtime_assert(ret >= 0, "H5 create simple dataspace fails.");
        return ret;
    }
    


};



} //namespace H5Wrapper