set_project("Json")
set_languages("c++11")
set_warnings("all", "error")

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_symbols("hidden")
    set_optimize("fastest")
    set_strip("all")
end

add_requires("gtest")

add_includedirs("./")

target("Json")
    set_kind("static")
    add_files("*.c")
    add_files("*.cc")

add_subdirs('test')
add_subdirs('example')
