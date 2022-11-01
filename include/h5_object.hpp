#pragma once

#include <hdf5.h>
#include <iostream>

#include "runtime_assert.hpp"

namespace H5Wrapper {

class H5Object {

public:
    enum class Type {
        UNINITIALIZED,
        BADOBJECT,
        FILE,
        GROUP,
        DATATYPE,
        DATASPACE,
        DATASET,
        ATTRIBUTE,
        PROPERTY_LIST,
        REFERENCE,
        VIRTUAL_FILE_LAYER,
        PROPERTY_LIST_CLASS,
        ERROR_CLASS,
        ERROR_MESSAGE,
        ERROR_STACK
    };

    enum class Policy { WITH_WARD = 1, WITHOUT_WARD = 2 };

    ///
    ///@brief Default construct. Results in an invalid state.
    ///
    ///
    explicit H5Object()
        : m_handle(0) {}

    ///
    ///@brief Construct from id and policy
    ///
    ///@param id
    ///@param policy
    ///
    explicit H5Object(hid_t id, Policy policy = Policy::WITH_WARD)
        : m_handle(id) {
        Utils::runtime_assert(m_handle >= 0, "H5Object costructor fails. Invalid hid.");

        if (policy == Policy::WITHOUT_WARD) { increment_reference_count(); }
    }

    ///
    ///@brief Copy construct
    ///
    ///@param other a copyable handle
    ///
    H5Object(const H5Object& other)
        : m_handle(other.m_handle) {

        if (is_valid()) { increment_reference_count(); }
    }

    ///
    ///@brief Move constuct
    ///
    ///@param other a movable handle
    ///
    H5Object(H5Object&& other) noexcept
        : m_handle(other.m_handle) {
        other.m_handle = 0;
    }

    ///
    ///@brief Assign
    ///
    ///@param assignable handle
    ///@return H5Object& this
    ///
    H5Object& operator=(const H5Object& other) {
        if (this == &other) { return *this; }

        if (is_valid()) { close(); }

        if (other.is_valid()) { other.increment_reference_count(); }

        m_handle = other.m_handle;

        return *this;
    }

    ///
    ///@brief Move assign
    ///
    ///@param other Movable handle
    ///@return H5Object& this
    ///
    H5Object& operator=(H5Object&& other) noexcept {
        if (this == &other) { return *this; }

        if (is_valid()) { close(); }

        m_handle       = other.m_handle;
        other.m_handle = 0;

        return *this;
    }

    ///
    ///@brief Destroy the H5Object object
    ///
    ///
    ~H5Object() {
        if (is_valid()) { close(); }
    }

    ///
    ///@brief Convert to hid_t
    ///
    ///@return hid_t my handle
    ///
    // explicit operator hid_t() const { return m_handle; }

    auto get_handle() const { return m_handle; }
    auto get_handle() { return m_handle; }

    hid_t operator~() const { return m_handle; }
    hid_t operator~() { return m_handle; }

    ///
    ///@brief Safely close the handle
    ///
    ///
    void close() {

        H5Object::Type oht{Type::UNINITIALIZED};

        oht = get_type();

        herr_t error_code = 0;
        switch (oht) {
        case H5Object::Type::DATASPACE: error_code = H5Sclose(m_handle); break;
        case H5Object::Type::GROUP: error_code = H5Gclose(m_handle); break;
        case H5Object::Type::DATATYPE: error_code = H5Tclose(m_handle); break;
        case H5Object::Type::ATTRIBUTE: error_code = H5Aclose(m_handle); break;
        case H5Object::Type::FILE: error_code = H5Fclose(m_handle); break;
        case H5Object::Type::PROPERTY_LIST: error_code = H5Pclose(m_handle); break;
        case H5Object::Type::PROPERTY_LIST_CLASS: error_code = H5Pclose_class(m_handle); break;
        case H5Object::Type::ERROR_MESSAGE: error_code = H5Eclose_msg(m_handle); break;
        case H5Object::Type::ERROR_STACK: error_code = H5Eclose_stack(m_handle); break;
        case H5Object::Type::ERROR_CLASS: error_code = H5Eunregister_class(m_handle); break;
        default: error_code = H5Oclose(m_handle);
        }

        Utils::runtime_assert(error_code >= 0, "Could not close hid");

        // in any case we have to reset the ID of the obejct
        m_handle = 0;
    }

    ///
    ///@brief Check if handle is in valid state
    ///
    ///@return true in valid state
    ///@return false not in valid state
    ///
    bool is_valid() const {
        // TODO: what the fuck happens here?

        if (m_handle == 0) { return false; }
        htri_t value = H5Iis_valid(m_handle);

        Utils::runtime_assert(value >= 0, "Could not determine validity of handle.");

        return (0 != value);
    }

    ///
    ///@brief Get the type of the handle
    ///
    ///@return H5Object::Type type
    ///
    H5Object::Type get_type() const {
        H5I_type_t type = H5Iget_type(m_handle);

        switch (type) {
        case H5I_UNINIT: return H5Object::Type::UNINITIALIZED;
        case H5I_BADID: return H5Object::Type::BADOBJECT;
        case H5I_FILE: return H5Object::Type::FILE;
        case H5I_GROUP: return H5Object::Type::GROUP;
        case H5I_DATATYPE: return H5Object::Type::DATATYPE;
        case H5I_DATASPACE: return H5Object::Type::DATASPACE;
        case H5I_DATASET: return H5Object::Type::DATASET;
        case H5I_ATTR: return H5Object::Type::ATTRIBUTE;
        case H5I_REFERENCE: return H5Object::Type::REFERENCE;
        case H5I_VFL: return H5Object::Type::VIRTUAL_FILE_LAYER;
        case H5I_GENPROP_CLS: return H5Object::Type::PROPERTY_LIST_CLASS;
        case H5I_GENPROP_LST: return H5Object::Type::PROPERTY_LIST;
        case H5I_ERROR_CLASS: return H5Object::Type::ERROR_CLASS;
        case H5I_ERROR_MSG: return H5Object::Type::ERROR_MESSAGE;
        case H5I_ERROR_STACK: return H5Object::Type::ERROR_STACK;
        default: Utils::runtime_assert(1 == 2, "H5Object get_type fails.");
        };
        return {};
    }

    ///
    ///@brief Get the number of references to the handle
    ///
    ///@return int number of references
    ///
    int get_reference_count() const {
        int ref_cnt = H5Iget_ref(m_handle);
        Utils::runtime_assert(ref_cnt >= 0, "H5Object: could not get reference counter");

        return ref_cnt;
    }

private:
    hid_t m_handle;

    ///
    ///@brief Safely increment the reference count
    ///
    ///
    void increment_reference_count() const {
        auto err = H5Iinc_ref(m_handle);
        Utils::runtime_assert(err >= 0, "H5Object reference count increment fails.");
    }

    ///
    ///@brief Safely decrement the reference count
    ///
    ///
    void decrement_reference_count() const {

        auto err = H5Idec_ref(m_handle);
        Utils::runtime_assert(err >= 0, "H5Object reference count decrement fails.");
    }
};

///
///@brief Equality comparison.
///
///@param lhs lhs comparison object
///@param rhs rhs comparison object
///@return true when the hid_t of both objects are equal
///@return false when the hid_t of both objects are not equal
///
static inline bool operator==(const H5Object& lhs, const H5Object& rhs) {
    // if one of the object is not valid they are considered as in not-equal
    if ((!rhs.is_valid()) || (!lhs.is_valid())) { return false; }
    return ~lhs == ~rhs;
}

///
///@brief Opposite of the non-equality comparison.
///
///@param lhs lhs comparison object
///@param rhs rhs comparison object
///@return true when the hid_t of both objects are not equal
///@return false when the hid_t of both objects are equal
///
static inline bool operator!=(const H5Object& lhs, const H5Object& rhs) { return !(lhs == rhs); }

///
///@brief Output operator for checking the type.
///
///@param stream stream to output into
///@param type type info to output
///@return std::ostream& modified stream
///
static inline std::ostream& operator<<(std::ostream& stream, const H5Object::Type& type) {

    switch (type) {
    case H5Object::Type::UNINITIALIZED: stream << "UNINITIALIZED"; break;
    case H5Object::Type::BADOBJECT: stream << "BADOBJECT"; break;
    case H5Object::Type::FILE: stream << "FILE"; break;
    case H5Object::Type::GROUP: stream << "GROUP"; break;
    case H5Object::Type::DATATYPE: stream << "DATATYPE"; break;
    case H5Object::Type::DATASPACE: stream << "DATASPACE"; break;
    case H5Object::Type::DATASET: stream << "DATASET"; break;
    case H5Object::Type::ATTRIBUTE: stream << "ATTRIBUTE"; break;
    case H5Object::Type::PROPERTY_LIST: stream << "PROPERTY_LIST"; break;
    case H5Object::Type::REFERENCE: stream << "REFERENCE"; break;
    case H5Object::Type::VIRTUAL_FILE_LAYER: stream << "VIRTUAL_FILE_LAYER"; break;
    case H5Object::Type::PROPERTY_LIST_CLASS: stream << "PROPERTY_LIST_CLASS"; break;
    case H5Object::Type::ERROR_CLASS: stream << "ERROR_CLASS"; break;
    case H5Object::Type::ERROR_MESSAGE: stream << "ERROR_MESSAGE"; break;
    case H5Object::Type::ERROR_STACK: stream << "ERROR_STACK"; break;
    default: stream << "unknown";
    };

    return stream;
}

} // namespace H5Wrapper