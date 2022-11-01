#pragma once

#include <hdf5.h>

#include "h5_object.hpp"
#include "h5_property.hpp"
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

public:

    ///
    ///@brief Gets the number of links (child nodes) in a group
    ///
    ///@return size_t number of links
    ///
    size_t nlinks() const {
        H5G_info_t info;
        auto       err = H5Gget_info(~(*this), &info);
        Utils::runtime_assert(err >= 0, "Failed to get group info.");
        return info.nlinks;
    }

    ///
    ///@brief Returns the names of all the links in the current location.
    ///
    ///@return std::vector<std::string> Names of the links in this location.
    ///
    std::vector<std::string> link_names() const {

        std::vector<std::string> names;

        for (size_t i = 0; i < nlinks(); i++) {

            ssize_t size =
                1 + H5Lget_name_by_idx(
                        ~(*this), ".", H5_INDEX_NAME, H5_ITER_INC, i, NULL, 0, H5P_DEFAULT);

            char* name = new char[size];

            size = H5Lget_name_by_idx(
                ~(*this), ".", H5_INDEX_NAME, H5_ITER_INC, i, name, size_t(size), H5P_DEFAULT);

            names.push_back(std::string(name));

            delete[] name;
        }
        return names;
    }

    ///
    ///@brief Returns the names of all the datasets in the current location.
    ///
    ///@return std::vector<std::string> Names of the datasets.
    ///
    std::vector<std::string> dataset_names() const {
        std::vector<std::string> links = link_names();
        std::vector<std::string> ret;
        for (auto l : links){
            
            H5Object obj(
                H5Oopen(~(*this), l.c_str(), ~H5LinkAccessProperty())
            );
            
            if (obj.get_type() == H5Object::Type::DATASET){
                ret.push_back(l);
            }

        }
        return ret;
    }    


};

} // namespace H5Wrapper