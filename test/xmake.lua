target("test")
    set_kind("binary")
    add_files("test_json.cc")
    add_deps("Json")

target("parser_test")
    set_kind("binary")
    add_files("parser_test.c")
    add_deps("Json")

target("nh")
    set_kind("binary")
    add_files("nh.cc")

