#pragma once
#include <algorithm> //std::find
#include <hdf5.h>
#include <mpi.h>
#include <sys/stat.h> //stat buffer
#include <vector>

#include "is_parallel.hpp"
#include "h5_location.hpp"
#include "h5_property.hpp"

#include "runtime_assert.hpp"

namespace H5Wrapper {


//TODO: consider requiring an explicit close call instead of calling it in the destructor
class H5File : public H5Location {

private:
    H5FileCreateProperty m_create_p;
    H5FileAccessProperty m_access_p;

public:
    enum class AccessFlag {
        READ, // Existing file is opened with read-only access. If file does not exist, H5Fopen
              // fails.
        READANDWRITE // Existing file is opened with read-write access. If file does not exist,
                     // H5Fopen fails.
    };

    enum class CreationFlag {
        NEW, // If file already exists, H5Fcreate fails. If file does not exist, it is created and
             // opened with read-write access.
        TRUNCATE // If file already exists,
                 // file is opened with read-write access and new data overwrites existing data,
                 // destroying all prior content, i.e., file content is truncated upon opening.
                 // If file does not exist, it is created and opened with read-write access.
    };

    static unsigned convert_flag(AccessFlag flag) {
        if (flag == AccessFlag::READ) { return H5F_ACC_RDONLY; }
        return H5F_ACC_RDWR; // read and write
    }

    static unsigned convert_flag(CreationFlag flag) {
        if (flag == CreationFlag::NEW) { return H5F_ACC_EXCL; }
        return H5F_ACC_TRUNC; // truncate
    }

public:
    H5File() = default;

    ///
    ///@brief Createas a new H5File
    ///
    ///@param name specifies the name of the file to be created.
    ///@param flag parameter specifies the creation mode.
    ///@param access_property specifies the file access property list. Use of H5P_DEFAULT specifies
    /// that default I/O access properties are to be used
    ///@return H5File a file identifier for the open file
    ///
    static H5File create(const std::string&          name,
                         CreationFlag                flag,
                         const H5FileCreateProperty& creation_property = H5FileCreateProperty(),
                         const H5FileAccessProperty& access_property   = H5FileAccessProperty()) {

        return H5File(name, flag, creation_property, access_property);
    }

    ///
    ///@brief Opens an existing HDF5 file.
    ///
    ///@param name specifies the name of the file to be opened.
    ///@param flag parameter specifies whether the file will be opened in APPEND or READ-only mode
    ///@param access_property specifies the file access property list. Use of H5P_DEFAULT specifies
    /// that default I/O access properties are to be used
    ///@return H5File a file identifier for the open file
    ///
    static H5File open(const std::string&          name,
                       AccessFlag                  flag,
                       const H5FileAccessProperty& access_property = H5FileAccessProperty()) {

        Utils::runtime_assert(exists(name), "File does not exist");
        
        //TODO: Make less insane
        if (is_parallel()){
            access_property.store_mpi_info(MPI_COMM_WORLD, MPI_INFO_NULL);
        }

        hid_t id = H5Fopen(name.c_str(), convert_flag(flag), ~access_property);
        Utils::runtime_assert(id >= 0, "H5File open fails.");

        return H5File(id);
    }

    /*
    // TODO: This is closer to a clone operation
    ///
    ///@brief Returns a new identifier for a previously-opened HDF5 file.
    ///
    ///@param file Identifier of a file for which an additional identifier is required.
    ///@return H5File Returns a new file identifier if successful.
    ///
    static H5File reopen(const H5File& file) {
        hid_t id = H5Freopen(~file);
        Utils::runtime_assert(id >= 0, "H5File reopen fails.");
        return H5File(id);
    }
    */

    ///
    ///@brief Terminates access to an HDF5 file.
    ///
    ///@param file Identifier of a file to terminate access to.
    ///
    void close() {
        hid_t id = H5Fclose(this->get_handle());
        Utils::runtime_assert(id >= 0, "H5File close fails.");
    }

    ///
    ///@brief Determines whether a file is in the HDF5 format.
    ///
    ///@param name File name to check format. The file must exist.
    ///@return true  if the file name is an HDF5 file.
    ///@return if the file name is not an HDF5 file.
    ///
    static bool is_hdf5(const std::string& name) {

        
        htri_t query = H5Fis_hdf5(name.c_str());
        Utils::runtime_assert(query >= 0, "H5File is hdf5 fails.");
        if (query > 0) { return true; }
        return false;
        
    }

    ///
    ///@brief Checks whether a file exists or not.
    ///
    ///@param name File name to query existance.
    ///@return true if exists
    ///@return false else
    ///
    static bool exists(const std::string& name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    ///
    ///@brief returns the size of the HDF5 file.
    ///
    ///@return size_t size of the filen in bytes.
    ///
    size_t get_filesize() const {
        hsize_t size;
        herr_t  err = H5Fget_filesize(this->get_handle(), &size);
        Utils::runtime_assert(err >= 0, "H5File get filesize fails.");
        return size_t(size);
    }

    ///
    ///@brief Returns a file creation property list identifier.
    ///
    ///@return const H5FileCreateProperty& creation property
    ///
    const H5FileCreateProperty& get_creation_property() const { return m_create_p; }

    ///
    ///@brief Get the access property list identifier.
    ///
    ///@return const H5FileAccessProperty& access property
    ///
    const H5FileAccessProperty& get_access_property() const { return m_access_p; }

    ///
    ///@brief Checks if a file was opened with read only intent.
    ///
    ///@return true if read only
    ///@return false if write access
    ///
    bool read_only() const {
        auto intent = get_intent();
        if (intent == H5F_ACC_RDONLY || intent == H5F_ACC_SWMR_READ) { return true; }
        return false;
    }

    ///
    ///@brief Checks if file has parallel access.
    ///
    ///@return true if has parallel access.
    ///@return false no parallel access available.
    ///
    bool parallel_access() const {
        auto intent = get_intent();
        if (intent == H5F_ACC_SWMR_READ || intent == H5F_ACC_SWMR_WRITE) { return true; }
        return false;
    }

    ///
    ///@brief Returns the number of open object identifiers for an open file.
    ///       Note! Counts *this as an open object also.
    ///
    ///@return size_t Returns the number of open objects if successful.
    ///
    size_t get_object_count() const {
        auto size = H5Fget_obj_count(this->get_handle(), H5F_OBJ_ALL);
        Utils::runtime_assert(size >= 0, "H5File get object count fails.");
        return size_t(size);
    }

    ///
    ///@brief Returns a list of open object identifiers currently open.
    ///
    ///@return std::vector<hid_t> list of open object identifiers.
    ///
    std::vector<hid_t> get_open_object_ids() const {

        size_t             count = get_object_count();
        std::vector<hid_t> ids(count);
        auto size = H5Fget_obj_ids(this->get_handle(), H5F_OBJ_ALL, count, ids.data());
        Utils::runtime_assert(size >= 0, "get open object ids fails.");
        return ids;
    }

private:
    H5File(const std::string&          name,
           CreationFlag                flag,
           const H5FileCreateProperty& creation_property,
           const H5FileAccessProperty& access_property)
        : H5Location(file_create(name, flag, creation_property, access_property))
        , m_create_p(creation_property)
        , m_access_p(access_property) {}

    explicit H5File(hid_t id)
        : H5Location(id) {}

    ///
    ///@brief Gets the intent of the file which was flagged on creation.
    ///
    ///@return The value of the flag is returned in intent. Valid values are as follows:
    ///         H5F_ACC_RDWR  	File was opened with read/write access.
    ///         H5F_ACC_RDONLY  	File was opened with read-only access.
    ///         H5F_ACC_SWMR_WRITE    	File was opened with read/write access for a
    ///         single-writer/multiple-reader (SWMR) scenario. Note that the writer process must
    ///         also open the file with the H5F_ACC_RDWR flag. H5F_ACC_SWMR_READ  	File was opened
    ///         with read-only access for a single-writer/multiple-reader (SWMR) scenario. Note that
    ///         the reader process must also open the file with the H5F_ACC_RDONLY flag.
    ///
    unsigned get_intent() const {
        unsigned intent;
        herr_t   err = H5Fget_intent(this->get_handle(), &intent);
        Utils::runtime_assert(err >= 0, "H5File get intent fails");
        return intent;
    }

    ///
    ///@brief Createas a new H5File
    ///
    ///@param name specifies the name of the file to be created.
    ///@param flag parameter specifies the creation mode.
    ///@param access_property specifies the file access property list. Use of H5P_DEFAULT specifies
    /// that default I/O access properties are to be used
    ///@return hid_t a file identifier handle for the created file if successful
    ///
    hid_t static file_create(const std::string&          name,
                             CreationFlag                flag,
                             const H5FileCreateProperty& creation_property,
                             const H5FileAccessProperty& access_property) {

        //TODO: Make less insane
        if (is_parallel()){
            access_property.store_mpi_info(MPI_COMM_WORLD, MPI_INFO_NULL);
        }

        hid_t id =
            H5Fcreate(name.c_str(), convert_flag(flag), ~creation_property, ~access_property);
        Utils::runtime_assert(id >= 0, "H5File file create fails.");
        return id;
    }
};

} // namespace H5Wrapper
