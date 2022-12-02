add_cxflags("-fPIE")
target("json_test")
    add_files("json_test.cc")
    add_deps("Json")
    add_packages("gtest")

target("json_obj_test")
    add_files("json_obj_test.cc")
    add_deps("Json")
    add_packages("gtest")

target("json_arr_test")
    add_files("json_arr_test.cc")
    add_deps("Json")
    add_packages("gtest")

target("iter_test")
    add_files("iter_test.cc")
    add_deps("Json")
    add_packages("gtest")


