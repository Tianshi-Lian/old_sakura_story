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

project "sakura_story"
    location "sakura_story"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir(tdir)
    objdir(odir)

    files
    {
        "%{prj.name}/include/**.h",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    sysincludedirs
    {
        "vendor/src",
        "vendor/include"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/include"
    }

    links
    {
    }

    filter "system:windows"
        systemversion "latest"

        links
        {
            "vendor/lib/spdlog/spdlog"
        }

        defines
        {
            "S_WIN"
        }

        postbuildcommands
        {
            "{COPY} ../vendor/bin/*.dll ../" .. tdir,
            "{COPY} ../data/ ../" .. tdir .. "/data/"
        }

        debugdir(tdir)

    filter "system:linux"    
        links
        {
            "spdlog"
        }

        defines
        {
            "S_LINUX"
        }
        
        postbuildcommands
        {
            "{COPY} ../data/* ../" .. tdir .. "/data/"
        }

    filter "configurations:debug"
        runtime "Debug"
        symbols "on"
        defines
        {
            "S_DEBUG"
        }
		
		postbuildcommands
		{
			"{COPY} ../build.meta ../" .. tdir
		}

    filter "configurations:release"
        runtime "Release"
        symbols "off"
        optimize "on"
