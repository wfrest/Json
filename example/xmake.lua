set_group("example")
set_default(false)

add_cxflags("-fPIE")
add_deps("Json")
set_kind("binary")

function all_examples()
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

for _, example in ipairs(all_examples()) do
target(example[1])
    add_files(example[2])
end

