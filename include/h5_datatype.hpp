#pragma once

#include <hdf5.h>
#include <string>

#include "h5_location.hpp"
#include "h5_object.hpp"
#include "h5_property.hpp"

#include "runtime_assert.hpp"

namespace H5Wrapper {

/*
enum class TypeClass : std::underlying_type<H5T_class_t>::type
{
  NONE = H5T_NO_CLASS,         //!< indicates a non-type
  INTEGER = H5T_INTEGER,       //!< indicates an integer type
  FLOAT = H5T_FLOAT,           //!< indicates a float type
  TIME = H5T_TIME,             //!< indicates a time type
  STRING = H5T_STRING,         //!< indicates a string type
  COMPOUND = H5T_COMPOUND,     //!< indicates a compound type
  ARRAY = H5T_ARRAY,            //!< indicates an array type
  BITFIELD = H5T_BITFIELD,     //!< indicates a bitfield type
  OPAQUE = H5T_OPAQUE,         //!< indicates an opaque type
  REFERENCE = H5T_REFERENCE,   //!< indicates a reference type
  ENUM = H5T_ENUM,             //!< indicates an enumeration type
  VARLENGTH = H5T_VLEN        //!< indicates a variable length type

};
*/

class H5Datatype : public H5Object {

public:
    H5Datatype() = default;

    // TODO: make this protected and have some static factory method
    explicit H5Datatype(hid_t id)
        : H5Object(id) {}

    ///
    ///@brief Opens a committed (named) datatype.
    ///
    ///@param loc Location identifier
    ///@param datatype_name Name given to committed datatype
    ///@param ac Datatype access property list
    ///@return H5Datatype datatype handle
    ///
    static H5Datatype open(const H5Location&               loc,
                           const std::string&              datatype_name,
                           const H5DatatypeAccessProperty& ac = H5DatatypeAccessProperty()) {
        // TODO: create a factory method to return a correct type
        hid_t id = H5Topen(~loc, datatype_name.c_str(), ~ac);
        Utils::runtime_assert(id >= 0, "Datype open fails.");
        return H5Datatype(id);
    }


    ///
    ///@brief Copy an existing datatype.
    ///
    ///@param other datatype handle to copy 
    ///@return H5Datatype a datatype identifier if successful.
    ///
    static H5Datatype copy(hid_t other) {
        hid_t id = H5Tcopy(other);
        Utils::runtime_assert(id >= 0, "Dataype copy fails.");
        return H5Datatype(id);
    }


    ///
    ///@brief Copies an existing datatype.
    ///
    ///@param other Identifier of datatype to copy. Can be a datatype identifier, a predefined
    /// datatype (defined in H5Tpublic.h), or a dataset identifier.
    ///@return H5Datatype a datatype identifier if successful
    ///
    static H5Datatype copy(const H5Datatype& other) {

        return copy(~other);
    }

    ///
    ///@brief Copies this object
    ///
    ///@return H5Datatype a datatype identifier if successful
    ///
    H5Datatype copy() const { return H5Datatype::copy(*this); }

    ///
    ///@brief Commits a transient datatype, linking it into the file and creating a new committed
    /// datatype.
    ///
    ///@param loc Location identifier
    ///@param datatype_name Name given to committed datatype
    ///@param lc Link creation property list
    ///@param dc Datatype creation property list
    ///@param da Datatype access property list
    ///
    void commit(const H5Location&               loc,
                const std::string&              datatype_name,
                const H5LinkCreateProperty&     lc = H5LinkCreateProperty(),
                const H5DatatypeCreateProperty& dc = H5DatatypeCreateProperty(),
                const H5DatatypeAccessProperty& da = H5DatatypeAccessProperty()) {
        herr_t err = H5Tcommit(~loc, datatype_name.c_str(), this->get_handle(), ~lc, ~dc, ~da);
        Utils::runtime_assert(err >= 0, "Datatype commit fails.");
    }

    ///
    ///@brief Determines whether a datatype is a committed type or a transient type.
    ///
    ///@return true if the datatype has been committed.
    ///@return false if the datatype has not been committed.
    ///
    bool commited() const {
        htri_t query = H5Tcommitted(this->get_handle());
        Utils::runtime_assert(query >= 0, "Datatype commited fails.");
        if (query > 0) { return true; }
        return false;
    }

    ///
    ///@brief Returns the datatype class identifier.
    ///
    ///@return H5T_class_t  H5T_INTEGER, H5T_FLOAT, H5T_STRING, H5T_BITFIELD, H5T_OPAQUE,
    /// H5T_COMPOUND, H5T_REFERENCE, H5T_ENUM, H5T_VLEN, H5T_ARRAY or otherwise H5T_NO_CLASS
    ///
    H5T_class_t get_class() const { return H5Tget_class(this->get_handle()); }

    ///
    ///@brief Returns the datatype class identifier.
    ///
    ///@param id The datatype handle to query the class
    ///@return H5T_class_t  H5T_INTEGER, H5T_FLOAT, H5T_STRING, H5T_BITFIELD, H5T_OPAQUE,
    /// H5T_COMPOUND, H5T_REFERENCE, H5T_ENUM, H5T_VLEN, H5T_ARRAY or otherwise H5T_NO_CLASS
    ///
    static H5T_class_t get_class(hid_t id) { return H5Tget_class(id); }

    ///
    ///@brief sets the total size in bytes, size, for a datatype.
    ///
    ///@param size New datatype size in bytes
    ///
    void set_size(size_t size) {
        herr_t err = H5Tset_size(this->get_handle(), size);
        Utils::runtime_assert(err >= 0, "Datatype set size fails.");
    }

    ///
    ///@brief Returns the size of a datatype.
    ///
    ///@return size_t size of a datatype in bytes.
    ///
    size_t get_size() const { return H5Tget_size(this->get_handle()); }

    ///
    ///@brief Returns the native datatype of a specified datatype.
    ///
    ///@param type Datatype identifier.
    ///@param direction Direction of search. (H5T_DIR_ASCEND, H5T_DIR_DESCEND)
    ///@return H5Datatype Returns the native datatype identifier for the specified dataset datatype
    /// if successful
    ///
    H5Datatype get_native_type(H5T_direction_t direction) const {
        hid_t id = H5Tget_native_type(this->get_handle(), direction);
        Utils::runtime_assert(id >= 0, "Datatype get native type fails.");
        return H5Datatype(id);
    }

    ///
    ///@brief Determines whether a datatype contains any datatypes of the given datatype class.
    ///
    ///@param type_class Datatype class.
    ///@return true if contains any datatypes of type_class
    ///@return false if does not contain any datatype of a type class
    ///
    bool detect_class(H5T_class_t type_class) const {
        htri_t query = H5Tdetect_class(this->get_handle(), type_class);
        Utils::runtime_assert(query >= 0, "Datatype detect class fails.");
        if (query > 0) { return true; }
        return false;
    }

    ///
    ///@brief Returns the precision of an atomic datatype.
    ///
    ///@return size_t Returns the number of significant bits if successful; otherwise 0.
    ///
    size_t get_precision() const { return H5Tget_precision(this->get_handle()); }

    ///
    ///@brief Sets the precision of an atomic datatype.
    ///
    ///@param precision Number of bits of precision for datatype.
    ///
    void set_precision(size_t precision) const {
        herr_t err = H5Tset_precision(this->get_handle(), precision);
        Utils::runtime_assert(err >= 0, "Datatype set precision fails.");
    }

    ///
    ///@brief Retrieves the bit offset of the first significant bit.
    ///
    ///@return size_t Returns an offset value if successful.
    ///
    size_t get_offset() const {
        int offset = H5Tget_offset(this->get_handle());
        Utils::runtime_assert(offset >= 0, "Datatype get offset fails.");
        return size_t(offset);
    }

    ///
    ///@brief Sets the bit offset of the first significant bit.
    ///
    ///@param offset Offset of first significant bit.
    ///
    void set_offset(size_t offset) {
        herr_t err = H5Tset_offset(this->get_handle(), offset);
        Utils::runtime_assert(err >= 0, "Datatype set offset fails.");
    }



    ///
    ///@brief Retrieves the name of the file to which the object obj_id belongs. The object can be a
    ///group, dataset, attribute, or named datatype.
    ///
    ///@param obj Identifier of the object for which the associated filename is sought.
    ///@return std::string containing the returned filename
    ///
    std::string belongs_to_file() const{

        size_t some_temp_size = 1; // this should be arbitrary...
        auto   size           = H5Fget_name(this->get_handle(), NULL, some_temp_size);

        Utils::runtime_assert(size >= 0, "H5File belongs to file fails.");
        std::string name(size_t(size), 'x');
        size = H5Fget_name(this->get_handle(), name.data(), hsize_t(size + 1)); // queried size + the null character
        Utils::runtime_assert(size >= 0, "H5File belongs to file fails.");

        return name;
    }






};

} // namespace H5Wrapper