#include "catch.hpp"

#include "h5_object.hpp"
#include "h5_dataspace.hpp"
#include "h5_dataspace_scalar.hpp"
#include "h5_dataspace_hyperslab.hpp"
#include "h5_dataset.hpp"
#include "h5_dataspace_all.hpp"
#include "h5_dataspace_hyperslab.hpp"
#include "h5_datatype_array.hpp"
#include "h5_datatype_compound.hpp"
#include "h5_datatype_creator.hpp"
#include "h5_file.hpp"
#include "h5_group.hpp"
#include "h5_property.hpp"
#include "is_h5_convertible.hpp"


/*
#include "io/jada_type_to_h5.hpp"

#include "type_traits/is_h5_convertible.hpp"
*/

TEST_CASE("H5Object constructors"){
    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5Object());
    //REQUIRE_NOTHROW(H5Object(H5T_NATIVE_FLOAT));

    //after default construction both states should be invalid
    H5Object lhs; 
    H5Object rhs(lhs);
    CHECK(lhs.is_valid() == false);
    CHECK(rhs.is_valid() == false);
    
}


TEST_CASE("H5File creation") {

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5File());

    std::string fname = "file_test1.h5";

    auto f1 = H5File::create(fname, H5File::CreationFlag::TRUNCATE);

    CHECK(H5File::exists(fname));

    // Check that throws for existing files
    REQUIRE_THROWS(H5File::create(fname, H5File::CreationFlag::NEW));
    CHECK(f1.is_valid());

    //TODO: I dont like that copies point to the same file. Not calling close on all the files,
            //reults in a crash in open...

    H5File copy(f1);
    CHECK(copy.is_valid());
    copy.close();

    f1.close();
    CHECK(!f1.is_valid());



    {

        H5File another = H5File::open(fname, H5File::AccessFlag::READ);
        CHECK(another.is_valid());

        another.close();
    }




}
TEST_CASE("H5File functionality") {

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5File());

    std::string fname = "file_test2.h5";

    auto f1 = H5File::create(fname, H5File::CreationFlag::TRUNCATE);

    CHECK(H5File::exists(fname));


    CHECK(f1.get_filesize() > 0); // bad check
    CHECK(!f1.read_only());
    f1.close();

    //If the created file is open this fails...
    CHECK(H5File::is_hdf5(fname)); 


    ////////////

    auto f2 = H5File::open(fname, H5File::AccessFlag::READ);
    CHECK(f2.read_only());
    //CHECK(!f2.parallel_access());


    CHECK(f2.get_object_count() == 1);

    auto f2_handle   = f2.get_handle();
    auto f2_handle_2 = f2.get_open_object_ids()[0];
    CHECK(f2_handle == f2_handle_2);

    f2.close();



    ////////////

    auto f3 = H5File::open(fname, H5File::AccessFlag::READANDWRITE);

    auto dt = H5DatatypeCreator<double>::create();
    dt.commit(f3, "some_name");
    CHECK(f3.get_object_count() == 2);

    const auto& f_acc = f3.get_access_property();
    CHECK(f_acc.is_valid());
    const auto& f_cre = f3.get_creation_property();
    CHECK(f_cre.is_valid());

}







TEST_CASE("Group tests") {

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5Group());

    std::string fname  = "group_test1.h5";
    auto hf      = H5File::create(fname, H5File::CreationFlag::TRUNCATE);

    SECTION("Group creation and existence"){
        std::string gname1 = "group1";
        std::string gname2 = "asd/asdf2/ss";
        std::string gname3 = "asd/somethingelse";

        auto group1  = H5Group::create(hf, gname1);
        auto group1b = H5Group::open(hf, gname1);
        auto group2  = H5Group::create(hf, gname2);
        auto group3  = H5Group::create(hf, gname3);
        auto group3b = H5Group::create(group3, gname3); // group to group

        REQUIRE_THROWS(H5Group::create(hf, gname2, false));
        
        CHECK(H5Group::exists(hf, gname1));
        CHECK(H5Group::exists(hf, gname2));
        CHECK(H5Group::exists(hf, gname3));
        CHECK(H5Group::exists(hf, "some") == false);
        CHECK(H5Group::exists(hf, "/some") == false);
        CHECK(H5Group::exists(hf, "some/other") == false);
        CHECK(H5Group::exists(hf, "some/other/longer/asd/some") == false);
        CHECK(H5Group::exists(hf, "some/other///longer2/asd/some") == false);
        
        // H5GroupCreateProperty(), H5GroupAccessProperty()

        CHECK(group1.is_valid());
        CHECK(group1b.is_valid());
        CHECK(group2.is_valid());
        CHECK(group3.is_valid());
        CHECK(group3b.is_valid());

    }

    SECTION("link_names/dataset_names"){

        auto link_group = H5Group::create(hf, "link_names_test");
        auto datatype = H5DatatypeCreator<int>::create();
        auto dataspace = H5Dataspace::create({1});
        auto ds1 = H5Dataset::create(link_group, "obj1", datatype, dataspace);
        auto ds2 = H5Dataset::create(link_group, "obj2", datatype, dataspace);
        auto ds3 = H5Dataset::create(link_group, "obj3", datatype, dataspace);
        H5Group::create(link_group, "other_group");
        
        CHECK(link_group.link_names() == std::vector<std::string>{"obj1", "obj2", "obj3", "other_group"});
        CHECK(link_group.dataset_names() == std::vector<std::string>{"obj1", "obj2", "obj3"});
    }

}





TEST_CASE("H5Datatype constructors") {

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5DatatypeArray());
    REQUIRE_NOTHROW(H5DatatypeCompound());
}

TEST_CASE("H5DatatypeCreator create") {

    using namespace H5Wrapper;

    auto dt1 = H5DatatypeCreator<signed char>::create();
    auto dt2 = H5DatatypeCreator<bool>::create();
    auto dt3 = H5DatatypeCreator<double>::create();

    CHECK(dt1.is_valid());
    CHECK(dt2.is_valid());
    CHECK(dt3.is_valid());


    CHECK(is_h5_convertible_v<double>);




}
/*

TEST_CASE("H5DatatypeCreator create jada types") {

    using namespace H5Wrapper;

    H5Datatype dt1 = H5DatatypeCreator<Math::StaticVector<double, 3>>::create();
    H5Datatype dt2 = H5DatatypeCreator<Math::Vec2d>::create();

    CHECK(dt1.is_valid());
    CHECK(dt2.is_valid());
}
*/


TEST_CASE("H5Datatype test") {

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5Datatype());

    H5Datatype native = H5DatatypeCreator<double>::create();

    {
        H5Datatype copy = native;
        CHECK(copy.is_valid());
    }

    CHECK(native.is_valid());

    std::string type_name = "ASD";
    std::string fname     = "datatype_file.h5";

    {

        auto file = H5File::create(fname, H5File::CreationFlag::TRUNCATE);

        auto dt = H5DatatypeCreator<double>::create();

        CHECK(dt.is_valid());

        CHECK(!dt.commited());

        dt.commit(file, type_name);

        CHECK(dt.commited());

        CHECK(dt.belongs_to_file() == fname);
    }

    {
        auto file = H5File::open(fname, H5File::AccessFlag::READ);

        auto dt2 = H5Datatype::open(file, type_name);

        CHECK(dt2.is_valid());

        auto dt3 = dt2.copy();
        CHECK(!dt3.commited());

        CHECK(dt3.get_class() == H5T_FLOAT);

        CHECK(dt3.detect_class(H5T_FLOAT));
        CHECK(dt2.detect_class(H5T_INTEGER) == false);
        CHECK(dt3.get_precision() == 64);
    }
}









TEST_CASE("H5Dataspace creation"){
    using namespace H5Wrapper;
    
    REQUIRE_NOTHROW(H5Dataspace());
    
    std::vector<size_t> dims{10, 10, 10}; 

    auto ds1 = H5Dataspace::create(dims);
    CHECK(ds1.is_valid());
    CHECK(ds1.get_rank() == 3);
    CHECK(ds1.get_dimensions() == dims);


    auto ds2 = H5DataspaceAll();
    auto ds3 = H5DataspaceScalar();

    //CHECK(ds2.is_valid()); //TODO: It appears that H5S_ALL is not valid... WTF?? 

    
}




TEST_CASE("H5Hyberslab creation"){
    using namespace H5Wrapper;
    
    //REQUIRE_NOTHROW(H5Dataspace());
    
    std::vector<size_t> dims{10, 10, 10}; 

    auto ds1 = H5Dataspace::create(dims);
    CHECK(ds1.is_valid());

    {
        auto hs1 = H5Hyperslab::select(ds1, {1,1,1}, {5,5,5});
        CHECK(hs1.is_valid());
        //REQUIRE_THROWS(H5Hyperslab::select(ds1, {0,0,0}, {11,11,11}));
    }


    
}





TEST_CASE("H5Dataset creation"){

    using namespace H5Wrapper;
    
    REQUIRE_NOTHROW(H5Dataset());

    std::string fname = "dataset_test1.h5";
    std::string dsetname1 = "first";


    auto hf = H5File::create(fname, H5File::CreationFlag::TRUNCATE);
    auto dt = H5DatatypeCreator<int>::create();
    auto file_dataspace = H5Dataspace::create({10, 10, 10});
    
    auto ds1 = H5Dataset::create(hf, dsetname1, dt, file_dataspace);

    CHECK(ds1.is_valid());
    ds1.close();
    CHECK(!ds1.is_valid());

    auto ds1_b = H5Dataset::open(hf, dsetname1);

    CHECK(ds1_b.is_valid());

    

}



TEST_CASE("H5Dataset read and write all"){

    using namespace H5Wrapper;
    

    std::string fname = "dataset_test2.h5";
    std::string dsetname1 = "first";

    size_t n_elements = 100;
    std::vector<size_t> buffer_dims{n_elements};
    

    //write
    {
        std::vector<int> buffer(n_elements, 50);
    
        CHECK(buffer[0] == 50);
    
        auto hf = H5File::create(fname, H5File::CreationFlag::TRUNCATE);
        auto dt = H5DatatypeCreator<int>::create();
        auto file_dataspace = H5Dataspace::create(buffer_dims);
        
        auto ds1 = H5Dataset::create(hf, dsetname1, dt, file_dataspace);
        ds1.write(buffer.data());

        
    }

    //read
    {
        std::vector<int> buffer(n_elements, 1);
        CHECK(buffer[0] == 1);

        auto hf = H5File::open(fname, H5File::AccessFlag::READ);
        auto ds1 = H5Dataset::open(hf, dsetname1);
        CHECK(ds1.is_valid());
        ds1.read(buffer.data());
        CHECK(buffer[0] == 50);
        CHECK(buffer[3] == 50);
        CHECK(ds1.get_dataspace().get_dimensions() == buffer_dims);

    }
    
    

}

TEST_CASE("H5Dataset read and write hyperslab"){

    using namespace H5Wrapper;
    

    std::string fname = "dataset_test3.h5";
    std::string dsetname1 = "first";

    size_t n_elements = 100;
    std::vector<size_t> buffer_dims{n_elements};
    std::vector<size_t> slab_start = {0};
    std::vector<size_t> slab_extent = {5};
    
    //write
    {
        std::vector<int> buffer(n_elements, 50);
    
        CHECK(buffer[0] == 50);
    
        auto hf = H5File::create(fname, H5File::CreationFlag::TRUNCATE);
        auto dt = H5DatatypeCreator<int>::create();
        
        auto file_dataspace = H5Dataspace::create(slab_extent);
        
        auto ds1 = H5Dataset::create(hf, dsetname1, dt, file_dataspace);
        ds1.write(buffer.data());

    }

    
    //read
    {
        std::vector<int> buffer(n_elements, 1);
        CHECK(buffer[0] == 1);

        auto hf = H5File::open(fname, H5File::AccessFlag::READ);
        auto ds1 = H5Dataset::open(hf, dsetname1);

        auto file_dataspace = ds1.get_dataspace();
        auto slab = H5Hyperslab::select(file_dataspace, slab_start, slab_extent);

        ds1.read(buffer.data(), slab);

        for (size_t i = slab_start[0]; i < slab_extent[0]; ++i){
            CHECK(buffer[i] == 50);
        }

        for (size_t i = slab_extent[0]; i < n_elements; ++i){
            CHECK(buffer[i] == 1);
        }


    }
    
    

}

TEST_CASE("H5Dataset write hyperslab hyperslab"){

    using namespace H5Wrapper;
    

    std::string fname = "dataset_test4.h5";
    std::string dsetname1 = "first";

    

    std::vector<size_t>   global_dims = {10, 10};

    std::vector<size_t>   barriers = {2,2};
    std::vector<size_t>   local_interior_dims = {5,5};
    std::vector<size_t>   local_dims = { 2 * barriers[0] + local_interior_dims[0],
                                         2 * barriers[1] + local_interior_dims[1]}; 
    

    //start writing/reading to/from this index in the global array 
    std::vector<size_t> start_idx_in_global_array = {0,5}; 


    //write
    {

        size_t local_buffer_size = local_dims[0] * local_dims[1];

        std::vector<int> buffer(local_buffer_size, 1);

        CHECK(buffer[0] == 1);

        
    
        auto hf = H5File::create(fname, H5File::CreationFlag::TRUNCATE);
        auto dt = H5DatatypeCreator<int>::create();
        
        auto file_dataspace = H5Dataspace::create(global_dims);
        auto file_dataspace2 = H5Hyperslab::select(file_dataspace, start_idx_in_global_array, local_interior_dims);
        

        auto memory_dataspace = H5Dataspace::create(local_dims);
        auto memory_dataspace2 = H5Hyperslab::select(memory_dataspace, barriers, local_interior_dims);

        auto ds1 = H5Dataset::create(hf, dsetname1, dt, file_dataspace2);
        ds1.write(buffer.data(), memory_dataspace2, file_dataspace2);

    }

    
    //read
    {

        size_t local_buffer_size = local_dims[0] * local_dims[1];

        std::vector<int> buffer(local_buffer_size, 2);

        CHECK(buffer[0] == 2);

    
        auto hf = H5File::open(fname, H5File::AccessFlag::READ);
        auto ds1 = H5Dataset::open(hf, dsetname1);

        auto file_dataspace = ds1.get_dataspace();
        auto file_dataspace2 = H5Hyperslab::select(file_dataspace, start_idx_in_global_array, local_interior_dims);

        auto memory_dataspace = H5Dataspace::create(local_dims);
        auto memory_dataspace2 = H5Hyperslab::select(memory_dataspace, barriers, local_interior_dims);


        ds1.read(buffer.data(), memory_dataspace2, file_dataspace2);


        size_t ni = local_interior_dims[0];
        size_t nj = local_interior_dims[1];

        size_t ngc_i = barriers[0];
        size_t ngc_j = barriers[1]; 

        size_t NI = ni + 2*ngc_i;
        size_t NJ = nj + 2*ngc_j;

        for (size_t j = ngc_j; j < NJ - ngc_j; ++j){
        for (size_t i = ngc_i; i < NI - ngc_i; ++i){

            size_t idx = i + NI * j;

            CHECK(buffer[idx] == 1);

        }}

        for (size_t j = 0; j < ngc_j; ++j){
        for (size_t i = 0; i < ngc_i; ++i){

            size_t idx = i + NI * j;

            CHECK(buffer[idx] == 2);

        }}



    }
    

}



TEST_CASE("H5Property constructors"){

    using namespace H5Wrapper;

    REQUIRE_NOTHROW(H5DatasetAccessProperty());
    REQUIRE_NOTHROW(H5DatasetAccessProperty());
    H5DatasetAccessProperty t;

    CHECK(t.is_valid());
   
}

