workspace "Function-Visualizer"
	architecture "x86_64"
	configurations { "Debug", "Release", }
	startproject "Function-Visualizer"

group "Dependencies"
	include "Function-Visualizer/vendor/imgui-sfml"
group ""

include "Function-Visualizer"
