set_group("bench")
set_default(false)

add_deps("Json")
set_kind("binary")

add_requires("nlohmann_json", {system = false})
add_requires("benchmark", {system = false})

add_packages("benchmark", "pthread")

target("nlohmann_json_accept_bench")
    add_files("nlohmann_json_accept_bench.cc")
    add_packages("nlohmann_json")

target("wfrest_parse_bench")
    add_files("wfrest_parse_bench.cc")
	add_deps("Json")
