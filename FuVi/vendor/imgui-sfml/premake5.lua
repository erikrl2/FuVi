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

	filter "system:windows"
		systemversion "latest"

        defines
        {
            "SFML_STATIC"
        }

        includedirs
        {
            "../SFML-2.5.1/include"
        }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines { "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		defines { "NDEBUG" }
