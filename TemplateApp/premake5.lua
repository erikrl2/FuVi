outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "TemplateApp" -- TODO: Rename
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	defines
	{
		"SFML_STATIC"
	}

	includedirs
	{
		"src",
		"vendor/SFML-2.5.1/include"
	}

	libdirs
	{
		"vendor/SFML-2.5.1/lib"
	}

	links
	{
		"opengl32.lib",
		"winmm.lib",
		"gdi32.lib",
		"freetype.lib",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines { "DEBUG" }

		links
		{
			"sfml-system-s-d.lib",
			"sfml-window-s-d.lib",
			"sfml-graphics-s-d.lib",
			"sfml-audio-s-d.lib",
			"sfml-network-s-d.lib"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		defines { "NDEBUG" }

		links
		{
			"sfml-system-s.lib",
			"sfml-window-s.lib",
			"sfml-graphics-s.lib",
			"sfml-audio-s.lib",
			"sfml-network-s.lib"
		}
