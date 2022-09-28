add_deps("Json")

target("test")
    set_kind("binary")
    add_files("test_json.cc")
