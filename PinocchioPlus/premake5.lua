project "PinocchioPlusCore"
	kind "StaticLib"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Pinocchio/**.h", "Pinocchio/**.cpp", "./premake5.lua", "../premake5.lua" }
	defines { "PINOCCHIO_STATIC" }
	includedirs {
		"vendor/spdlog-1.14.1/include/",
	}
project "PinocchioPlusSkin"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "AttachWeights/**.h", "AttachWeights/**.cpp" }
	includedirs {
		"Pinocchio/",
	}
	links { 
		"PinocchioPlusCore"
	}
project "PinocchioPlusDemo"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Demo/**.h", "Demo/**.cpp" }
	filter "system:linux"
		includedirs {
			"../PinocchioPlus/Pinocchio/",
		}
		links { 
			"PinocchioPlusCore",
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
	filter "system:windows"
		includedirs {
			"Pinocchio/",
			"%{IncludeDir.fltk}"
		}
		libdirs { 
			"%{LibDir.fltk}",
		}
		links { 
			"PinocchioPlusCore",
			"fltk.lib",
			"fltk_forms.lib",
			"fltk_gl.lib",
			"fltk_images.lib",
			"fltk_jpeg.lib",
			"fltk_png.lib",
			"fltk_z.lib",
			"opengl32.lib"
		}
project "PinocchioPlusDemoUI"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "DemoUI/**.h", "DemoUI/**.cpp" }
	filter "system:linux"
		includedirs {
			"Pinocchio/",
		}
		links { 
			"PinocchioPlusCore",
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
	filter "system:windows"
		includedirs {
			"Pinocchio/",
			"%{IncludeDir.fltk}"
		}
		libdirs { 
			"%{LibDir.fltk}",
		}
		links { 
			"PinocchioPlusCore",
			"fltk.lib",
			"fltk_forms.lib",
			"fltk_gl.lib",
			"fltk_images.lib",
			"fltk_jpeg.lib",
			"fltk_png.lib",
			"fltk_z.lib",
			"opengl32.lib"
		}

project "PinocchioPlusTest"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Test/**.h", "Test/**.cpp" }
	filter "system:linux"
		includedirs {
			"Pinocchio/",
		}
		links { 
			"PinocchioPlusCore"
		}
	filter "system:windows"
		includedirs {
			"Pinocchio/",
		}
		links { 
			"PinocchioPlusCore"
		}
		
project "PinocchioPlusExperiment"
	kind "ConsoleApp"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { 
		"Experiment/**.h", 
		"Experiment/**.cpp", 
		"../experiment.json",
	}
	filter "system:linux"
		includedirs {
			"Pinocchio/",
			"vendor/spdlog-1.14.1/include/",
			"../vendor/json/include",
		}
		links { 
			"PinocchioPlusCore",
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
	filter "system:windows"
		includedirs {
			"Pinocchio/",
			"vendor/spdlog-1.14.1/include/",
			"%{IncludeDir.fltk}",
			"../vendor/json/include",
		}
		libdirs { 
			"%{LibDir.fltk}",
		}
		links { 
			"PinocchioPlusCore",
			"fltk.lib",
			"fltk_forms.lib",
			"fltk_gl.lib",
			"fltk_images.lib",
			"fltk_jpeg.lib",
			"fltk_png.lib",
			"fltk_z.lib",
			"opengl32.lib"
		}