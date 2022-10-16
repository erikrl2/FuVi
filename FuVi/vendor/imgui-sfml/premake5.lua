outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "ImGui-SFML"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"*.cpp",
		"*.h"
	}

	defines
	{
		"SFML_STATIC"
	}

	includedirs
	{
		"../SFML-2.5.1/include"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines { "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		defines { "NDEBUG" }
