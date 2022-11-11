#pragma once

#include <algorithm> //std::transform
#include <vector>

#include "h5_dataspace.hpp"

namespace H5Wrapper {

// TODO: Make this class fully static!
class H5Hyperslab : public H5Dataspace {

public:
    using dims_array = typename H5Dataspace::dims_array;

    H5Hyperslab() = default;

    static H5Hyperslab select(const H5Dataspace& parent,
                              const dims_array&  start,
                              const dims_array&  extent,
                              const dims_array&  stride = dims_array{},
                              const dims_array&  block  = dims_array{}) {
        return H5Hyperslab(parent, start, extent, stride, block);
    }

    template <size_t N>
    static H5Hyperslab select(const H5Dataspace&           parent,
                              const std::array<size_t, N>& start,
                              const std::array<size_t, N>& extent) {

        return H5Hyperslab(parent,
                           dims_array(start.begin(), start.end()),
                           dims_array(extent.begin(), extent.end()),
                           dims_array(),
                           dims_array());
    }

    ///
    ///@brief Returns the start index of the hyperslab w.r.t parent dataspace
    ///
    ///@return std::vector<size_t> start indices
    ///
    std::vector<size_t> start() const {
        std::vector<hsize_t> start(this->get_rank());
        std::vector<hsize_t> end(this->get_rank());
        auto err = H5Sget_select_bounds(this->get_handle(), start.data(), end.data());
        Utils::runtime_assert(err >= 0, "H5Hyperslab start fails");
        return std::vector<size_t>(start.begin(), start.end());
    }

    ///
    ///@brief Returns the end indices of the hyperslab w.r.t parent dataspace.
    ///
    ///@return std::vector<size_t> end indices
    ///
    std::vector<size_t> end() const {
        std::vector<hsize_t> start(this->get_rank());
        std::vector<hsize_t> end(this->get_rank());
        auto err = H5Sget_select_bounds(this->get_handle(), start.data(), end.data());
        Utils::runtime_assert(err >= 0, "H5Hyperslab end fails");

        std::vector<size_t> ret(end.begin(), end.end());

        // increment by one because the end returned by select bound is the actual index of the
        // upper bound
        for (auto& e : ret) { e += 1; }

        return ret;
    }

private:
    static bool valid_hyperslab(const H5Dataspace& parent,
                                const dims_array&  start,
                                const dims_array&  extent,
                                const dims_array&  stride,
                                const dims_array&  block) {

        dims_array max_hslab_dims(start.size());
        std::transform(start.begin(),
                       start.end(),
                       extent.begin(),
                       max_hslab_dims.begin(),
                       std::plus<size_t>());

        for (size_t i = 0; i < max_hslab_dims.size(); ++i) {
            if (max_hslab_dims[i] > parent.get_dimensions()[i]) { return false; }
        }

        auto prank = parent.get_rank();

        if (start.size() != prank) { return false; }

        if (extent.size() != prank) { return false; }

        if (stride.size() != prank && stride.size() != 0) { return false; }

        if (block.size() != prank && block.size() != 0) { return false; }

        return true;
    }

    static hid_t select_hyperslab(const H5Dataspace& parent,
                                  const dims_array&  start,
                                  const dims_array&  extent,
                                  const dims_array&  stride,
                                  const dims_array&  block) {

        Utils::runtime_assert(valid_hyperslab(parent, start, extent, stride, block),
                              "Invalid hyperslab dimensions.");

        hid_t id = parent.clone_handle();

        auto err = H5Sselect_hyperslab(id,
                                       H5S_SELECT_SET,
                                       cast(start).data(),
                                       cast(stride).data(),
                                       cast(extent).data(),
                                       cast(block).data());

        Utils::runtime_assert(err >= 0, "Select hyperslab fails");

        return id;
    }

    H5Hyperslab(const H5Dataspace& parent,
                const dims_array&  start,
                const dims_array&  extent,
                const dims_array&  stride,
                const dims_array&  block)
        : H5Dataspace(select_hyperslab(parent, start, extent, stride, block)) {}
};

} // namespace H5Wrapper