#pragma once

#include <hdf5.h>
#include <string>

#include "h5_datatype.hpp"

#include "runtime_assert.hpp"

namespace H5Wrapper {

class H5DatatypeCompound : public H5Datatype {

public:

    H5DatatypeCompound() = default;


    ///
    ///@brief Retrieves the number of elements in a compound.
    ///
    ///@return size_t Returns the number of elements if successful.
    ///
    size_t get_nmembers() const{
        int nmembers = H5Tget_nmembers(this->get_handle());
        Utils::runtime_assert(nmembers >= 0, "H5DatatypeCompound get nmembers fails.");
        return size_t(nmembers);
    }
    

    ///
    ///@brief Returns datatype class of compound.
    ///
    ///@param member_no Compound object member number.
    ///@return H5T_class_t Returns the datatype class if successful.
    ///
    H5T_class_t get_member_class(size_t member_no) const{
        auto class_t = H5Tget_member_class(this->get_handle(), unsigned(member_no));
        Utils::runtime_assert(class_t >= 0, "H5DatatypeCompound get member class fails.");
        return class_t;
    }

    ///
    ///@brief Retrieves the name of a compound or enumeration datatype member. 
    ///
    ///@param Zero-based index of the field or element whose name is to be retrieved.
    ///@return std::string the member name if succesful.
    ///
    std::string get_member_name(size_t field_idx) const{
        const char* name = H5Tget_member_name(this->get_handle(), unsigned(field_idx));
        Utils::runtime_assert(name != nullptr, "H5DatatypeCompound get member name fails.");
        return std::string(name);
    }

    ///
    ///@brief Retrieves the index of a compound or enumeration datatype member.
    ///
    ///@param member_name Name of the field or member whose index is to be retrieved.
    ///@return size_t Returns a valid field or member index if successful.
    ///
    size_t get_member_index(const std::string& member_name) const{
        int idx = H5Tget_member_index(this->get_handle(), member_name.c_str());
        Utils::runtime_assert(idx >= 0, "H5DatatypeCompound get member index fails.");
        return size_t(idx);
    }

    ///
    ///@brief Retrieves the offset of a field of a compound datatype. 
    ///
    ///@param member_no Number of the field whose offset is requested.
    ///@return size_t Returns the byte offset of the field if successful.
    ///
    size_t get_member_offset(size_t member_no) const{
        return H5Tget_member_offset(this->get_handle(), unsigned(member_no));
    }

    ///
    ///@brief Returns the datatype of the specified member.
    ///
    ///@param field_idx Field index (0-based) of the field type to retrieve.
    ///@return H5Datatype Returns the identifier of a copy of the datatype of the field if successful.
    ///
    H5Datatype get_member_type(size_t field_idx) const{
        //TODO: ensure it makes sense to return a H5Datatype, or should a factory method be used.
        hid_t id = H5Tget_member_type(this->get_handle(), unsigned(field_idx));
        Utils::runtime_assert(id >= 0, "H5DatatypeCompound get member type fails.");
        return H5Datatype(id);
    }

    
    ///
    ///@brief Adds a new member to a compound datatype if successful. 
    ///
    ///@param type Datatype identifier of the field to insert.
    ///@param name Name of the field to insert.
    ///@param offset Offset in memory structure of the field to insert.
    ///
    void insert(const H5Datatype& type, const std::string& name, size_t offset ){
        herr_t err = H5Tinsert(this->get_handle(), name.c_str(), offset, ~type);
        Utils::runtime_assert(err >= 0, "H5DatatypeCompound insert fails.");
    }
    
    ///
    ///@brief Recursively removes padding from within a compound datatype to make it more efficient (space-wise) to store that data. 
    ///
    ///
    void pack(){
        herr_t err = H5Tpack(this->get_handle());
        Utils::runtime_assert(err >= 0, "H5DatatypeCompound pack fails.");
    }



};

} // namespace H5Wrapper