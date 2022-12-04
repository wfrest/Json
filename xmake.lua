set_project("Json")

set_languages("c99", "c++11")
set_version("0.9.0")
set_warnings("allextra")

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
end

add_includedirs("$(projectdir)")

target("Json")
    set_kind("static")
    add_files("*.c")
    add_files("*.cc")
    add_cxflags("-fPIE")
    after_clean(function (target)
        os.rm("$(buildir)")
    end)

includes("test", "example")

