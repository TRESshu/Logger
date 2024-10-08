add_rules("mode.debug", "mode.release")
--添加spdlog依赖
--dd_requires("spdlog", {configs = {header_only = true}})
-- 设置 C++20 标准
set_languages("cxx20")
-- Logger 动态链接库
target("Logger")
    set_kind("shared")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_includedirs("spdlog-1.14.1/include")
    add_defines("BUILDING_DLL")
    set_default(false)  -- 默认不构建

-- test 可执行文件
target("test")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_includedirs("spdlog-1.14.1/include")
    add_defines("BUILDING_EXE")
    set_default(false)  -- 默认不构建
    --add_packages("spdlog")  

-- 自定义任务来构建 DLL
task("build_dll")
on_run(function ()
    os.exec("xmake build Logger")
end)
set_menu {
    usage = "xmake build_dll",
    description = "Build the dynamic library",
}

-- 自定义任务来构建可执行文件
task("build_exe")
on_run(function ()
    os.exec("xmake build test")
end)
set_menu {
    usage = "xmake build_exe",
    description = "Build the executable",
}
--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

