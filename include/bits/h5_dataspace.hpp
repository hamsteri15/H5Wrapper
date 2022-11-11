#pragma once

#include <array>
#include <vector>

#include "h5_functions.hpp"
#include "h5_object.hpp"

#include "runtime_assert.hpp"

namespace H5Wrapper {

class H5Dataspace : public H5Object {

public:
    using dims_array = std::vector<size_t>;

    H5Dataspace() = default;

    // this has to be here so that dimensions can be read from file
    explicit H5Dataspace(hid_t id)
        : H5Object(id) {
        //Utils::runtime_assert(get_type() == H5Object::Type::DATASPACE, "Not a dataspace handle.");
    }



    static H5Dataspace create(const std::vector<size_t>& dims) {
        return H5Dataspace(dims.begin(), dims.end());
    }

    static H5Dataspace create(std::initializer_list<size_t> dims) {
        return H5Dataspace::create(std::vector(dims));
    }

    template <size_t N> static H5Dataspace create(const std::array<size_t, N>& dims) {
        return H5Dataspace(dims.begin(), dims.end());
    }


    static H5Dataspace create(hid_t id){
        return H5Dataspace(id);
    }




    dims_array get_dimensions() const { return get_dimensions(this->get_handle()); }

    size_t get_rank() const { return get_rank(this->get_handle()); }

    hid_t clone_handle() const {

        hid_t id = H5Scopy(this->get_handle());
        Utils::runtime_assert(id >= 0, "H5Dataspace::clone_handle fails.");
        return id;
    }

protected:
    static std::vector<hsize_t> cast(const std::vector<size_t>& vector) {
        std::vector<hsize_t> ret(vector.begin(), vector.end());
        return ret;
    }


    
private:

    template <class IT>
    H5Dataspace(const IT begin, const IT end)
        : H5Dataspace(create_simple(begin, end)) {}

    




    template <class IT> static hid_t create_simple(const IT begin, const IT end) {
        std::vector<hsize_t> dims(begin, end);
        return H5Screate_simple(int(dims.size()), dims.data(), NULL);
    }

    

    static size_t get_rank(hid_t id) {
        int n_dims = H5Sget_simple_extent_ndims(id);
        Utils::runtime_assert(n_dims >= 0, "H5Dataspace get_rank fails.");
        return size_t(n_dims);
    }

    static dims_array get_dimensions(hid_t id) {
        std::vector<hsize_t> dims(get_rank(id));
        std::vector<hsize_t> max_dims(get_rank(id));
        auto                 err = H5Sget_simple_extent_dims(id, dims.data(), max_dims.data());
        Utils::runtime_assert(err >= 0, "H5Dataspace get_dimensions fails.");
        dims_array ret(dims.begin(), dims.end());
        return ret;
    }
};

} // namespace H5Wrapper
