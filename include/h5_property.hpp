#pragma once

#include <hdf5.h>
#include <mpi.h>

#include "h5_object.hpp"
#include "runtime_assert.hpp"

namespace H5Wrapper {


enum class PropertyType {
    ATTRIBUTE_CREATE,
    DATASET_ACCESS,
    DATASET_CREATE,
    DATASET_XFER,
    DATATYPE_ACCESS,
    DATATYPE_CREATE,
    FILE_ACCESS,
    FILE_CREATE,
    FILE_MOUNT,
    GROUP_ACCESS,
    GROUP_CREATE,
    LINK_ACCESS,
    LINK_CREATE,
    OBJECT_COPY,
    OBJECT_CREATE,
    STRING_CREATE
};

namespace detail {


static inline hid_t to_h5_handle(PropertyType type) {
    switch (type) {
    case PropertyType::ATTRIBUTE_CREATE: return H5P_ATTRIBUTE_CREATE;

    case PropertyType::DATASET_ACCESS: return H5P_DATASET_ACCESS;

    case PropertyType::DATASET_CREATE: return H5P_DATASET_CREATE;

    case PropertyType::DATASET_XFER: return H5P_DATASET_XFER;

    case PropertyType::DATATYPE_ACCESS: return H5P_DATATYPE_ACCESS;

    case PropertyType::DATATYPE_CREATE: return H5P_DATATYPE_CREATE;

    case PropertyType::FILE_ACCESS: return H5P_FILE_ACCESS;

    case PropertyType::FILE_CREATE: return H5P_FILE_CREATE;

    case PropertyType::FILE_MOUNT: return H5P_FILE_MOUNT;

    case PropertyType::GROUP_ACCESS: return H5P_GROUP_ACCESS;

    case PropertyType::GROUP_CREATE: return H5P_GROUP_CREATE;

    case PropertyType::LINK_ACCESS: return H5P_LINK_ACCESS;

    case PropertyType::LINK_CREATE: return H5P_LINK_CREATE;

    case PropertyType::OBJECT_COPY: return H5P_OBJECT_COPY;

    case PropertyType::OBJECT_CREATE: return H5P_OBJECT_CREATE;

    case PropertyType::STRING_CREATE: return H5P_STRING_CREATE;

    default: throw std::runtime_error("Invalid property type.");
    }
}


static inline PropertyType to_type(hid_t handle) {

    if (bool(H5Pequal(handle, H5P_ATTRIBUTE_CREATE))) { return PropertyType::ATTRIBUTE_CREATE; }

    if (bool(H5Pequal(handle, H5P_DATASET_ACCESS))) { return PropertyType::DATASET_ACCESS; }

    if (bool(H5Pequal(handle, H5P_DATASET_CREATE))) { return PropertyType::DATASET_CREATE; }

    if (bool(H5Pequal(handle, H5P_DATASET_XFER))) { return PropertyType::DATASET_XFER; }

    if (bool(H5Pequal(handle, H5P_DATATYPE_ACCESS))) { return PropertyType::DATATYPE_ACCESS; }

    if (bool(H5Pequal(handle, H5P_DATATYPE_CREATE))) { return PropertyType::DATATYPE_CREATE; }

    if (bool(H5Pequal(handle, H5P_FILE_ACCESS))) { return PropertyType::FILE_ACCESS; }

    if (bool(H5Pequal(handle, H5P_FILE_CREATE))) { return PropertyType::FILE_CREATE; }

    if (bool(H5Pequal(handle, H5P_FILE_MOUNT))) { return PropertyType::FILE_MOUNT; }

    if (bool(H5Pequal(handle, H5P_GROUP_ACCESS))) { return PropertyType::GROUP_ACCESS; }

    if (bool(H5Pequal(handle, H5P_GROUP_CREATE))) { return PropertyType::GROUP_CREATE; }

    if (bool(H5Pequal(handle, H5P_LINK_ACCESS))) { return PropertyType::LINK_ACCESS; }

    if (bool(H5Pequal(handle, H5P_LINK_CREATE))) { return PropertyType::LINK_CREATE; }

    if (bool(H5Pequal(handle, H5P_OBJECT_COPY))) { return PropertyType::OBJECT_COPY; }

    if (bool(H5Pequal(handle, H5P_OBJECT_CREATE))) { return PropertyType::OBJECT_CREATE; }

    if (bool(H5Pequal(handle, H5P_STRING_CREATE))) { return PropertyType::STRING_CREATE; }

    throw std::runtime_error("Not a Property type handle");
}


template <PropertyType T> struct H5Property : public H5Object {

    // static constexpr my_type = T;

    H5Property()
        : H5Object(H5Pcreate(detail::to_h5_handle(T))) {}

    explicit H5Property(hid_t id)
        : H5Object(id) {
        Utils::runtime_assert(detail::to_type(id) == T, "Invalid property type handle.");
    }
};

} // namespace detail

struct H5AttributeCreateProperty : public detail::H5Property<PropertyType::ATTRIBUTE_CREATE> {};

struct H5DatasetAccessProperty : public detail::H5Property<PropertyType::DATASET_ACCESS> {};

struct H5DatasetCreateProperty : public detail::H5Property<PropertyType::DATASET_CREATE> {};

struct H5DatasetTransferProperty : public detail::H5Property<PropertyType::DATASET_XFER> {

    void set_collective_mpi_io() {
        herr_t err = H5Pset_dxpl_mpio(this->get_handle(), H5FD_MPIO_COLLECTIVE);
        Utils::runtime_assert(err >= 0, "set_collective_mpi_io fails.");
    }
};

struct H5DatatypeAccessProperty : public detail::H5Property<PropertyType::DATATYPE_ACCESS> {};

struct H5DatatypeCreateProperty : public detail::H5Property<PropertyType::DATATYPE_CREATE> {};

struct H5FileAccessProperty : public detail::H5Property<PropertyType::FILE_ACCESS> {

    H5FileAccessProperty() = default;

    explicit H5FileAccessProperty(hid_t id) : detail::H5Property<PropertyType::FILE_ACCESS>(id) {}

    void store_mpi_info(MPI_Comm comm, MPI_Info info) const{

        herr_t err = H5Pset_fapl_mpio(this->get_handle(), comm, info);
        Utils::runtime_assert(err >= 0, "store_mpi_info fails.");
    }
};

struct H5FileCreateProperty : public detail::H5Property<PropertyType::FILE_CREATE> {

    H5FileCreateProperty() = default;

    explicit H5FileCreateProperty(hid_t id) : detail::H5Property<PropertyType::FILE_CREATE>(id) {}

};

struct H5FileMountProperty : public detail::H5Property<PropertyType::FILE_MOUNT> {};

struct H5GroupAccessProperty : public detail::H5Property<PropertyType::GROUP_ACCESS> {};

struct H5GroupCreateProperty : public detail::H5Property<PropertyType::GROUP_CREATE> {};

struct H5LinkAccessProperty : public detail::H5Property<PropertyType::LINK_ACCESS> {};

struct H5LinkCreateProperty : public detail::H5Property<PropertyType::LINK_CREATE> {

    void set_create_intermediate_groups() {
        auto err = H5Pset_create_intermediate_group(this->get_handle(), 1);
        Utils::runtime_assert(err >= 0, "Set create_intermediate_groups fails.");
    }
};

struct H5ObjectCopyProperty : public detail::H5Property<PropertyType::OBJECT_COPY> {};

struct H5ObjectCreateProperty : public detail::H5Property<PropertyType::OBJECT_CREATE> {};

struct H5StringCreateProperty : public detail::H5Property<PropertyType::STRING_CREATE> {};

} // namespace H5Wrapper
