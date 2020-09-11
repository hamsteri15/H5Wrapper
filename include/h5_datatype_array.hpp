#pragma once

#include <hdf5.h>
#include <string>
#include <vector>

#include "h5_datatype.hpp"

#include "runtime_assert.hpp"

namespace H5Wrapper {

class H5DatatypeArray : public H5Datatype {

public:
    H5DatatypeArray() = default;

    ///
    ///@brief Construct a new H5DatatypeArray object
    ///
    ///@param basetype Datatype identifier for the array base datatype.
    ///@param rank Rank of the array.
    ///@param dims Size of each array dimension.
    ///
    H5DatatypeArray(const H5Datatype& basetype, size_t rank, const std::vector<size_t>& dims)
        : H5Datatype(array_create(basetype, rank, dims)) {}


    ///
    ///@brief Returns the rank of the array datatype.
    ///
    ///@return size_t rank
    ///
    size_t get_ndims() const {
        int ndims = H5Tget_array_ndims(this->get_handle());
        Utils::runtime_assert(ndims >= 0, "H5DatatypeArray get ndims fails.");
        return size_t(ndims);
    }

    ///
    ///@brief Retrieves sizes of array dimensions.
    ///
    ///@return std::vector<size_t> Sizes of array dimensions.
    ///
    std::vector<size_t> get_dims() const {
        std::vector<hsize_t> i_dims(this->get_ndims());
        int                  err = H5Tget_array_dims(this->get_handle(), i_dims.data());
        Utils::runtime_assert(err >= 0, "H5DatatypeArray get dims fails.");
        return std::vector<size_t>(i_dims.begin(), i_dims.end());
    }

private:
    static hid_t
    array_create(const H5Datatype& basetype, size_t rank, const std::vector<size_t>& dims) {
        std::vector<hsize_t> dims_c(dims.begin(), dims.end());
        hid_t                id = H5Tarray_create(~basetype, static_cast<uint>(rank), dims_c.data());
        Utils::runtime_assert(id >= 0, "H5DatatypeArray array create fails.");
        return id;
    }
};

} // namespace H5Wrapper