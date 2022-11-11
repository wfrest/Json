target("test")
    set_kind("binary")
    add_files("test_json.cc")
    add_deps("Json")

target("parser_test")
    set_kind("binary")
    add_files("parser_test.cc")
    add_deps("Json")

target("nh")
    set_kind("binary")
    add_files("nh.cc")

target("json_unittest")
    add_files("json_unittest.cc")
    add_deps("Json")
    add_packages("gtest")

target("test1")
    add_files("test1.cc")


