project "PinocchioPlusCore"
	kind "StaticLib"
	language "c++"
	cppdialect "c++14"
	staticruntime "Off"
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	files { "Pinocchio/**.h", "Pinocchio/**.cpp", "./premake5.lua", "../premake5.lua" }
	defines { "PINOCCHIO_STATIC" }
project "PinocchioPlusSkin"
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
			"../PinocchioPlus/",
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
			"../PinocchioPlus/",
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
			"../PinocchioPlus/",
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
			"../PinocchioPlus/",
			"%{IncludeDir.fltk}"
		}
		libdirs { 
			"%{LibDir.fltk}",
		}
		links { 
			"PinocchioPlusCore",
			--"fltk.lib",
			--"fltkforms.lib",
			--"fltkgl.lib",
			--"fltkimages.lib",
			--"fltkjpeg.lib",
			--"fltkpng.lib",
			--"zlib.lib",
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
			"../PinocchioPlus/",
		}
		links { 
			"PinocchioPlusCore"
		}
	filter "system:windows"
		includedirs {
			"../PinocchioPlus/",
		}
		links { 
			"PinocchioPlusCore"
		}