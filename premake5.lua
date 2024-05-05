workspace "PinocchioPlus"
	configurations { "Debug", "Release", "Dist" }
	platforms "x64"
	architecture "x86_64"
	startproject "PinocchioPlusCore"
	location "project-files"
	filter "configurations:Debug"
		defines {"PP_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"PP_RELEASE"}
		runtime "Release"
		optimize "on"
		symbols "off"
	filter "configurations:Dist"
		defines {"PP_DIST"}
		runtime "Release"
		optimize "on"
		symbols "off"
	filter {}
	filter "system:windows"
		systemversion "latest"
		defines {"PP_WINDOWS"}
	filter "system:linux"
		defines {"PP_LINUX"}
	filter {}
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	IncludeDir = {}
	IncludeDir["fltk"] = "../vendor/fltk-release-1.3.9"
	LibDir = {}
	LibDir["fltk"] = "../vendor/fltk-release-1.3.9/lib/"
	group "Dependencies"
		include "./vendor/fltk-release-1.3.9"
	group ""
	include "PinocchioJingma"
	include "PinocchioPlus"

