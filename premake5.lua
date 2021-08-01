workspace "project"
    startproject "project"
    architecture "x64"

    configurations
    {
        "debug",
        "release"
    }

tdir = "bin/%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}"
odir = "bin-int/%{cfg.system}-%{cfg.buildcfg}-%{cfg.architecture}/%{prj.name}"

project "project"
    location "project"
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

    filter "system:windows"
        systemversion "latest"

        links
        {
            "vendor/lib/spdlog/spdlog"
        }

        defines
        {
            "PS_WIN"
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
            "PS_LINUX"
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
            "PS_DEBUG"
        }
		
		postbuildcommands
		{
			"{COPY} ../build.meta ../" .. tdir
		}

    filter "configurations:release"
        runtime "Release"
        symbols "off"
        optimize "on"
