#pragma once

#include <hdf5.h>

#include "h5_location.hpp"
#include "h5_property.hpp"

namespace H5Wrapper {

class H5Group : public H5Location {

public:
    H5Group() = default;

    ///
    ///@brief Creates a new group and links it into the location.
    ///
    ///@param loc File or group identifier
    ///@param name Absolute or relative name of the link to the new group
    ///@param create_intermediate Allow group ceation /with/many/groups
    ///@param group_cr_prop Group creation property list identifier
    ///@param group_ac_prop Group access property list identifier
    ///@return H5Group Returns a group identifier if successful
    ///
    static H5Group create(const H5Location&            loc,
                          const std::string&           name,
                          bool                         create_intermediate = true,
                          const H5GroupCreateProperty& group_cr_prop = H5GroupCreateProperty(),
                          const H5GroupAccessProperty& group_ac_prop = H5GroupAccessProperty()) {

        H5LinkCreateProperty link_prop;
        if (create_intermediate) { link_prop.set_create_intermediate_groups(); }
        return H5Group(loc, name, link_prop, group_cr_prop, group_ac_prop);
    }

    ///
    ///@brief Opens an existing group with a group access property list.
    ///
    ///@param loc  File or group identifier specifying the location of the group to be opened
    ///@param name Name of the group to open
    ///@param group_ac_prop Group access property list identifier
    ///@return H5Group Returns a group identifier if successful
    ///
    static H5Group open(const H5Location&     loc,
                        const std::string&    name,
                        H5GroupAccessProperty group_ac_prop = H5GroupAccessProperty()) {
        hid_t id = H5Gopen(~loc, name.c_str(), ~group_ac_prop);
        Utils::runtime_assert(id >= 0, "H5Group open fails.");
        return H5Group(id);
    }

    ///
    ///@brief Queries if a group exists
    ///
    ///@param loc  File or group identifier specifying the location of the group to be queried
    ///@param name Name of the group to query
    ///@return bool true if exists, false otherwise
    ///
    static bool exists(const H5Location& loc, std::string name) {

        return H5Lexists_safe(~loc, name.c_str()) == 1;
    }

    ///
    ///@brief Closes the specified group.
    ///
    ///
    void close() {
        herr_t err = H5Gclose(this->get_handle());
        Utils::runtime_assert(err >= 0, "H5Group close fails.");
    }

private:
    H5Group(const H5Location&            loc,
            const std::string&           path,
            const H5LinkCreateProperty&  link_p,
            const H5GroupCreateProperty& group_cr_p,
            const H5GroupAccessProperty& group_ac_p)
        : H5Location(group_create(loc, path, link_p, group_cr_p, group_ac_p)) {}

    explicit H5Group(hid_t id)
        : H5Location(id) {}

    static hid_t group_create(const H5Location&            loc,
                              const std::string&           path,
                              const H5LinkCreateProperty&  link_p,
                              const H5GroupCreateProperty& group_cr_p,
                              const H5GroupAccessProperty& group_ac_p) {

        auto id = H5Gcreate(~loc, path.c_str(), ~link_p, ~group_cr_p, ~group_ac_p);
        Utils::runtime_assert(id >= 0, "Group create fails.");
        return id;
    }

    ///
    ///@brief Extends the H5Lexists to work with paths
    ///
    ///@param base base location, typically a file id
    ///@param path the path to check existence of
    ///@return int 1 if whole path exists 0 otherwise
    ///
    static int H5Lexists_safe(hid_t base, const char* path) {
        hid_t last = base, next;
        char* pch;
        char  pathc[2048];
        strcpy(pathc, path);
        pch = strtok(pathc, "/");
        while (pch != NULL) {
            int exists = H5Lexists(last, pch, H5P_DEFAULT);
            if (!exists) {
                if (last != base) H5Gclose(last);
                return 0;
            } else {
                next = H5Gopen(last, pch, H5P_DEFAULT);
                if (last != base) H5Gclose(last);
                last = next;
            }
            pch = strtok(NULL, "/");
        }
        if (last != base) H5Gclose(last);
        return 1;
    }
};

} // namespace H5Wrapper
