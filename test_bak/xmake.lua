add_requires("gtest")

set_group("test")
set_default(false)

add_cxflags("-fPIE")
add_packages("gtest")
add_deps("Json")
set_kind("binary")

function all_tests()
    local res = {}
    for _, x in ipairs(os.files("**.cc")) do
        local item = {}
        local s = path.filename(x)
        table.insert(item, s:sub(1, #s - 3))       -- target
        table.insert(item, path.relative(x, "."))  -- source
        table.insert(res, item)
    end
    return res
end

for _, test in ipairs(all_tests()) do
target(test[1])
    add_files(test[2])
end

