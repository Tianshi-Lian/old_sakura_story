workspace "sakura_story"
    startproject "sakura_story_client"
    architecture "x64"

    configurations
    {
        "debug",
        "release"
    }

tdir = "bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}"
odir = "bin-int/%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}"

include("sita")

project "sakura_story"
	location "sakura_story"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

	links
	{
		"sita"
	}

    targetdir(tdir)
    objdir(odir)

    files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/**.c",
        "%{prj.name}/vendor/**.cpp",
        "%{prj.name}/vendor/**.h",
    }

    sysincludedirs
    {
        "vendor/src",
        "vendor/include",
        "%{prj.name}/vendor",
    }

    includedirs
    {
        "%{prj.name}/include",
		"sita/sita/include",
    }

	postbuildcommands
	{
		"{COPY} %{prj.name}.build.meta ../" .. tdir,
		"{COPY} ../data/ ../" .. tdir .. "/data/",
		"python ../tools/_postbuild.py %{prj.name}",
	}

    filter "system:windows"
        systemversion "latest"

        files
        {
            "%{prj.name}/vendor/**.natvis",
        }

        links
        {
            "vendor/lib/spdlog/spdlog",
            "vendor/lib/SDL2/SDL2",
            "vendor/lib/SDL2/SDL2main",
            "vendor/lib/SDL2_ttf/SDL2_ttf",
            "vendor/lib/SDL2_gpu/SDL2_gpu",
        }

        defines
        {
            "SITA_WIN"
        }

        postbuildcommands
        {
            "{COPY} ../vendor/bin/*.dll ../" .. tdir,
        }

        debugdir(tdir)

    filter "system:linux"
        links
        {
            "spdlog",
        }

    filter "configurations:debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:release"
        runtime "Release"
        symbols "off"
        optimize "on"
