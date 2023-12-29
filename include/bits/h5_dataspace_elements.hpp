#pragma once

#include <algorithm> //std::transform
#include <vector>

#include "h5_dataspace.hpp"

namespace H5Wrapper {

class H5Elements : public H5Dataspace {

public:

    using dims_array = std::vector<size_t>;

    H5Elements() = default;


    static H5Elements select(const H5Dataspace& parent, dims_array indices)
    {
        return H5Elements::select(parent, indices.size(), indices);
    }


    static H5Elements select(const H5Dataspace& parent, size_t count, dims_array indices)
    {
        return H5Elements(parent, count, indices);
    }



private:

    H5Elements(const H5Dataspace& parent, size_t count, dims_array indices)
    : H5Dataspace(select_elements(parent, count, indices))
    {}


    static hid_t select_elements(const H5Dataspace& parent, size_t count, dims_array indices)
    {
        hid_t id = parent.clone_handle();


        if (count == 0 && indices.size() == 0){
            auto err = H5Sselect_none(id);
            Utils::runtime_assert(err >= 0, "HDF5 none type element selection fails.");
            return id;
        }

        auto err = H5Sselect_elements
        (
            id,
            H5S_SELECT_SET,
            count,
            cast(indices).data()
        );
        Utils::runtime_assert(err >= 0, "HDF5 element selection fails.");

        return id;
    }

};

} // namespace H5Wrapper