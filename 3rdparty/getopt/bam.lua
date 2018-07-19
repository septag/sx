--[[
    a getopt.
   version 0.1, march, 2012

   Copyright (C) 2012- Fredrik Kihlander

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.

   Fredrik Kihlander
--]]

BUILD_PATH = "local"

function get_config()
    local config = ScriptArgs["config"]
    if config == nil then
        return "debug"
    end
    return config
end

function get_platform()
    local platform = ScriptArgs["platform"]
    if platform == nil then
        if family == "windows" then
            platform = "winx64"
        else
            platform = "linux_x86_64"
        end
    end
    return platform
end

function get_base_settings()
    local settings = {}

    settings._is_settingsobject = true
    settings.invoke_count = 0
    settings.debug = 0
    settings.optimize = 0
    SetCommonSettings(settings)

    -- add all tools
    for _, tool in pairs(_bam_tools) do
        tool(settings)
    end

    return settings
end

function set_compiler( settings, config )
    if family == "windows" then
        compiler = "msvc"
    else
        compiler = ScriptArgs["compiler"]
        if compiler == nil then
            compiler = "gcc"
        end
    end

    InitCommonCCompiler(settings)
    if compiler == "msvc" then
        SetDriversCL( settings )
        if config == "release" then
            settings.cc.flags:Add( "/Ox" )
            settings.cc.flags:Add( "/TP" ) -- forcing c++ compile on windows =/
        end
    elseif compiler == "gcc" then
        SetDriversGCC( settings )
        settings.cc.flags:Add( "-Wconversion", "-Wextra", "-Wall", "-Werror", "-Wstrict-aliasing=2" )
        if config == "release" then
            settings.cc.flags:Add( "-O2" )
        end
    elseif compiler == "clang" then
        SetDriversClang( settings )
        settings.cc.flags:Add( "-Wconversion", "-Wextra", "-Wall", "-Werror", "-Wstrict-aliasing=2" )
        if config == "release" then
            settings.cc.flags:Add( "-O2" )
        end
    end
end

config   = get_config()
platform = get_platform()
settings = get_base_settings()
set_compiler( settings, config )
TableLock( settings )

local output_path = PathJoin( BUILD_PATH, PathJoin( platform, config ) )
local output_func = function(settings, path) return PathJoin(output_path, PathFilename(PathBase(path)) .. settings.config_ext) end
settings.cc.Output = output_func
settings.lib.Output = output_func
settings.link.Output = output_func

settings.cc.includes:Add( 'include' )

local objs  = Compile( settings, 'src/getopt.c' )
local lib   = StaticLibrary( settings, 'getopt', objs )

local example = Link( settings, 'example', Compile( settings, 'example/example.cpp' ), lib )

local test_objs  = Compile( settings, 'test/getopt_tests.cpp' )
local tests      = Link( settings, 'getopt_tests', test_objs, lib )
