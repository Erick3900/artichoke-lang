local function getStem(filepath)
    local filename = vim.fn.fnamemodify(tostring(filepath), ":t")
    local stem = filename:match(".*%.()")

    if stem == nil then
        return filename
    end

    return filename:sub(0, stem - 2)
end

local function getTestName(file)
    local fs = require('plenary.path')
    local file_path = fs:new(file)

    local parent_stem = getStem(file_path:parent())
    local file_stem = getStem(file_path)

    if (parent_stem == "tests") then
        return file_stem .. "_test"
    else
        return parent_stem .. "." .. file_stem .. "_test"
    end
end

return {
    {
        name = "run",
        program = ws.build_dir .. dirsep .. "arti-lang",
        args = {
            "tests/input/primes.arti",
            "> out"
        },
        cwd = workspace_folder,
        depends = {
            "build"
        },
        default = true
    },
    {
        name = "test",
        program = "ctest",
        args = {
            "--force-new-ctest-process",
            "--stop-on-failure"
        },
        cwd = ws.build_dir,
        depends = {
            "build"
        }
    },
    {
        name = "test_current",
        program = ws.build_dir .. dirsep .. getTestName(file),
        args = {},
        cwd = ws.build_dir,
        depends = {
            "build"
        }
    }
}
