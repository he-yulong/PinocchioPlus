project "PinocchioJingmaCore"
	kind "StaticLib"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Pinocchio/**.h", "Pinocchio/**.cpp", "./premake5.lua", "../premake5.lua" }
project "PinocchioJingmaSkin"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "AttachWeights/**.h", "AttachWeights/**.cpp" }
	includedirs {
		"../PinocchioPlus/",
	}
	links { 
		"PinocchioPlusCore"
	}
project "PinocchioJingmaDemo"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Demo/**.h", "Demo/**.cpp" }
	filter "system:linux"
		includedirs {
			"../PinocchioPlus/",
		}
		links { 
			"PinocchioJingmaCore",
			"fltk",
			"fltk_gl",
			"GL",
			"GLU",
			"Xrender",
			"Xcursor",
			"Xfixes",
			"Xft",
			"fontconfig",
			"Xinerama",
			"pthread",
			"dl",
			"m",
			"X11",
		}
project "PinocchioJingmaDemoUI"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "DemoUI/**.h", "DemoUI/**.cpp" }
	filter "system:linux"
		includedirs {
			"../PinocchioPlus/",
		}
		links { 
			"PinocchioJingmaCore",
			"fltk",
			"fltk_gl",
			"GL",
			"GLU",
			"Xrender",
			"Xcursor",
			"Xfixes",
			"Xft",
			"fontconfig",
			"Xinerama",
			"pthread",
			"dl",
			"m",
			"X11",
		}