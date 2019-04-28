
local is_64_bit = true

local coverage_test = false

local linux, windows

if package.config:sub(1,1) == '\\' then
    windows = true
else 
    linux = true
end

workspace "wolf-lang"
    language "C"
    cdialect "C11"

    location "build"

    if is_64_bit then
        architecture "x86_64"
    else
        architecture "x86"
    end

    configurations { "Debug", "Release" }

    filter { "configurations:Debug" }
        symbols "On"
        optimize "Off"

    filter { "configurations:Release" }
        symbols "Off"
        optimize "Speed"
        linkoptions  { "-flto" }
        
    filter { }

    targetdir "build/bin/"
    objdir    "build/obj/"

project "wolf"
    kind "ConsoleApp"

    includedirs "src"

    files {
        "src/main.c"
    }

    if is_64_bit then
        defines { "WOLF_64_BIT" }
    else 
        defines { "WOLF_32_BIT" }
    end

    filter "configurations:Debug"
        defines { "DEBUG" }

        if build_os == linux and coverage_test then 
            buildoptions { "-fprofile-arcs", "-ftest-coverage" }
            linkoptions  { "-fprofile-arcs", "-ftest-coverage" }
        end

    filter "configurations:Release"
        defines { "NDEBUG" }

    filter {}