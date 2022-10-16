workspace "FuVi"
	architecture "x86_64"
	configurations { "Debug", "Release", }
	startproject "FuVi"

group "Dependencies"
	include "FuVi/vendor/imgui-sfml"
group ""

include "FuVi"
