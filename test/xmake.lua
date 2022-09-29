target("test")
    set_kind("binary")
    add_files("test_json.cc")
    add_deps("Json")
    
target("nh")
    set_kind("binary")
    add_files("nh.cc")

