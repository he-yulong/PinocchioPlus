project "PinocchioPlusCore"
    kind "StaticLib"
    language "c++"
    cppdialect "c++17"
    staticruntime "On"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    files { "Pinocchio/**.h", "Pinocchio/**.cpp" }
project "PinocchioPlusSkin"
    kind "StaticLib"
    language "c++"
    cppdialect "c++17"
    staticruntime "On"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    files { "AttachWeights/**.h", "AttachWeights/**.cpp" }
project "PinocchioPlusDemo"
    kind "ConsoleApp"
    language "c++"
    cppdialect "c++17"
    staticruntime "On"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    files { "Demo/**.h", "Demo/**.cpp" }
    filter "system:linux"
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
project "PinocchioPlusDemoUI"
    kind "ConsoleApp"
    language "c++"
    cppdialect "c++17"
    staticruntime "On"
    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    files { "DemoUI/**.h", "DemoUI/**.cpp" }
    filter "system:linux"
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