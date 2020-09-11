#pragma once

#include <hdf5.h>
#include <string>

#include "runtime_assert.hpp"

#include "h5_dataspace.hpp"
#include "h5_dataspace_all.hpp"
#include "h5_datatype.hpp"
#include "h5_location.hpp"
#include "h5_object.hpp"
#include "h5_property.hpp"

namespace H5Wrapper {

class H5Dataset : public H5Object {

public:
    H5Dataset() = default;

    ///
    ///@brief Creates a new dataset and links it into the file.
    ///
    ///@param loc Location identifier
    ///@param name Dataset name
    ///@param type Datatype identifier
    ///@param file_dataspace Dataspace identifier
    ///@param link_prop Link creation property list
    ///@param creat_prop Dataset creation property list
    ///@param acc_prop  Dataset access property list
    ///@return H5Dataset Returns a dataset identifier if successful
    ///
    static H5Dataset create(const H5Location&              loc,
                            const std::string&             name,
                            const H5Datatype&              type,
                            const H5Dataspace&             file_dataspace,
                            const H5LinkCreateProperty&    link_prop  = H5LinkCreateProperty(),
                            const H5DatasetCreateProperty& creat_prop = H5DatasetCreateProperty(),
                            const H5DatasetAccessProperty& acc_prop   = H5DatasetAccessProperty()) {

        return H5Dataset(loc, name, type, file_dataspace, link_prop, creat_prop, acc_prop);
    }

    ///
    ///@brief Get the dataspace of the dataset
    ///
    ///@return H5Dataspace dataspace used in creation of the dataset.
    ///
    H5Dataspace get_dataspace() const {
        hid_t id = H5Dget_space(this->get_handle());
        Utils::runtime_assert(id >= 0, "H5Dataset get_dataspace fails.");
        return H5Dataspace(id);
    }

    ///
    ///@brief Get the datatype of the dataset.
    ///
    ///@return H5Datatype datatype used in the creation of the dataset.
    ///
    H5Datatype get_datatype() const {
        hid_t id = H5Dget_type(this->get_handle());
        Utils::runtime_assert(id >= 0, "H5Dataspace get_datatype fails.");
        return H5Datatype(id);
    }

    ///
    ///@brief Opens an existing dataset.
    ///
    ///@param loc Location identifier
    ///@param name Dataset name
    ///@param acc_prop Dataset access property list
    ///@return H5Dataset Returns a dataset identifier if successful
    ///
    static H5Dataset open(const H5Location&              loc,
                          const std::string&             name,
                          const H5DatasetAccessProperty& acc_prop = H5DatasetAccessProperty()) {
        hid_t id = H5Dopen(~loc, name.c_str(), ~acc_prop);
        Utils::runtime_assert(id >= 0, "H5Dataset open fails.");
        return H5Dataset(id);
    }

    ///
    ///@brief Closes an open dataset.
    ///
    ///
    void close() {
        auto err = H5Dclose(this->get_handle());
        Utils::runtime_assert(err >= 0, "H5Dataset close fails.");
    }

    template <class T>
    void write(const T*                         buffer,
               const H5Dataspace&               memory_dataspace = H5DataspaceAll(),
               const H5DatasetTransferProperty& transfer_prop = H5DatasetTransferProperty()) const {

        H5Dataspace file_space = this->get_dataspace();
        H5Datatype  file_dtype = this->get_datatype();

        herr_t err = H5Dwrite(this->get_handle(),
                              ~file_dtype,
                              ~memory_dataspace,
                              ~file_space,
                              ~transfer_prop,
                              buffer);

        Utils::runtime_assert(err >= 0, "H5Dataset write fails.");
    }

    template <class T>
    void write(const T*                         buffer,
               const H5Dataspace&               memory_dataspace,
               const H5Dataspace&               file_dataspace,
               const H5DatasetTransferProperty& transfer_prop = H5DatasetTransferProperty()) const {

        H5Datatype file_dtype = this->get_datatype();

        herr_t err = H5Dwrite(this->get_handle(),
                              ~file_dtype,
                              ~memory_dataspace,
                              ~file_dataspace,
                              ~transfer_prop,
                              buffer);

        Utils::runtime_assert(err >= 0, "H5Dataset write fails.");
    }

    template <class T>
    void read(T*                               buffer,
              const H5Dataspace&               memory_dataspace = H5DataspaceAll(),
              const H5DatasetTransferProperty& transfer_prop    = H5DatasetTransferProperty()) {

        H5Dataspace file_space = this->get_dataspace();
        H5Datatype  file_dtype = this->get_datatype();

        herr_t err = H5Dread(this->get_handle(),
                             ~file_dtype,
                             ~memory_dataspace,
                             ~file_space,
                             ~transfer_prop,
                             buffer);
        Utils::runtime_assert(err >= 0, "H5Dataset read fails.");
    }

    template <class T>
    void read(T*                               buffer,
              const H5Dataspace&               memory_dataspace,
              const H5Dataspace&               file_dataspace,
              const H5DatasetTransferProperty& transfer_prop = H5DatasetTransferProperty()) {

        H5Datatype file_dtype = this->get_datatype();

        herr_t err = H5Dread(this->get_handle(),
                             ~file_dtype,
                             ~memory_dataspace,
                             ~file_dataspace,
                             ~transfer_prop,
                             buffer);
        Utils::runtime_assert(err >= 0, "H5Dataset read fails.");
    }

private:
    hid_t static dataset_create(const H5Location&              loc,
                                const std::string&             name,
                                const H5Datatype&              type,
                                const H5Dataspace&             file_dataspace,
                                const H5LinkCreateProperty&    link_prop,
                                const H5DatasetCreateProperty& creat_prop,
                                const H5DatasetAccessProperty& acc_prop) {

        hid_t id = H5Dcreate(
            ~loc, name.c_str(), ~type, ~file_dataspace, ~link_prop, ~creat_prop, ~acc_prop);
        Utils::runtime_assert(id >= 0, "H5Dataset dataset_create fails.");
        return id;
    }

    H5Dataset(const H5Location&              loc,
              const std::string&             name,
              const H5Datatype&              type,
              const H5Dataspace&             file_dataspace,
              const H5LinkCreateProperty&    link_prop,
              const H5DatasetCreateProperty& creat_prop,
              const H5DatasetAccessProperty& acc_prop)
        : H5Object(
              dataset_create(loc, name, type, file_dataspace, link_prop, creat_prop, acc_prop)) {}

    explicit H5Dataset(hid_t id)
        : H5Object(id){};
};

} // namespace H5Wrapper