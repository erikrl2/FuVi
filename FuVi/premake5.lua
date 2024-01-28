outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "FuVi"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"vendor/imgui-sfml",
		"vendor/exprtk"
	}

	links
	{
		"ImGui-SFML",
	}

	filter "system:windows"
		systemversion "latest"

        buildoptions { "/bigobj" }

        includedirs
        {
            "vendor/SFML-2.5.1/include",
        }

        libdirs
        {
            "vendor/SFML-2.5.1/lib"
        }

        defines
        {
            "SFML_STATIC"
        }

        links
        {
            "opengl32.lib",
            "winmm.lib",
            "gdi32.lib",
            "freetype.lib"
        }

    filter { "system:windows", "configurations:Debug" }

		links
		{
			"sfml-system-s-d.lib",
			"sfml-window-s-d.lib",
			"sfml-graphics-s-d.lib",
			"sfml-audio-s-d.lib",
			"sfml-network-s-d.lib"
		}

    filter { "system:windows", "configurations:Release" }
		kind "WindowedApp"

		links
		{
			"sfml-system-s.lib",
			"sfml-window-s.lib",
			"sfml-graphics-s.lib",
			"sfml-audio-s.lib",
			"sfml-network-s.lib"
		}

	filter "system:linux"

        links
        {
            "sfml-system",
            "sfml-window",
            "sfml-graphics",
            "OpenGL",
        }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines { "DEBUG" }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		defines { "NDEBUG" }
