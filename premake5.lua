workspace "PinocchioPlus"
    configurations { "Debug", "Release", "Dist" }
    platforms "x64"
    architecture "x86_64"
    startproject "PinocchioJingma"
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


    include "PinocchioJingma"

