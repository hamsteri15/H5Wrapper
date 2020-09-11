#pragma once

#include <hdf5.h>

#include "h5_object.hpp"
//#include "h5_data.hpp"


namespace H5Wrapper {



class H5Location : public H5Object{

public:

    H5Location() = default;


protected:

    explicit H5Location(hid_t id)
    : H5Object(id) {

        auto type = this->get_type();

        if ((type != H5Object::Type::FILE) && (type != H5Object::Type::GROUP) ) {
            throw "Invalid type.";
        }
    }



};

} // namespace H5Wrapper