set_project("Json")
set_languages("c99", "c++11")

set_version("0.9.0")

add_rules("mode.release", "mode.debug", "mode.check")

add_includedirs("$(projectdir)")

target("Json")
    set_kind("static")
    add_files("*.c")
    add_files("*.cc")
    add_cxflags("-fPIE")
    after_clean(function (target)
        os.rm("$(buildir)")
    end)

includes("test", "example", "bench")

