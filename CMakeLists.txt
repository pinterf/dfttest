# Tested generators:
# Command line makefile generator
# "MinGW Makefiles": MSYS2/Mingw32 GCC 8.3, 10.3 build
# IDE project file generators
# "Visual Studio 15 2017" optional platform generator Win32 and x64
# "Visual Studio 16 2019" optional platform generator Win32 and x64. optional toolset "v141_xp"
# "Visual Studio 17 2022" optional platform generator Win32 and x64. optional toolset "v141_xp"
# "Visual Studio 17 2022" optional platform generator Win32 and x64. optional toolset "clangcl"

# Intel C++ Compilers
# Minimum Cmake version for Intel C++ Compiler 2023: 3.22.3 (Win) 3.20 (Linux)
# Howto: https://www.intel.com/content/www/us/en/developer/articles/technical/using-oneapi-compilers-with-cmake-in-visual-studio.html
# "Intel(R) oneAPI DPC++ Compiler", two flavours. DPCPP is not compatible with Avisynth.
# - Intel® NextGen Compiler (in base kit, LLVM based): TOOLSET = "Intel C++ Compiler 2023", COMPILER EXE NAME = icx.exe
# - Intel® Classic Compiler (in extra HPC kit): TOOLSET = "Intel C++ Compiler 19.2", COMPILER EXE NAME = icl.exe
# CMake support files.
# - Info from: c:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\IntelDPCPP\ReadMe.txt
# - copy c:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\IntelDPCPP\IntelDPCPPConfig.cmake to c:\Program Files\CMake\share\cmake-3.20\Modules\
# CMake GUI:
# - Generator: "Visual Studio 17 2022"
# - Optional toolset to use (-T option):
#   For LLVM based icx: Intel C++ Compiler 2023 (or earlier 2022, 2021)
#   For classic icl: Intel C++ Compiler 19.2
# - Specify native compilers: browse for the appropriate compiler executable path.
#   icx: C:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\bin\icx.exe
#   icl: C:\Program Files (x86)\Intel\oneAPI\compiler\latest\windows\bin\intel64\icl.exe

CMAKE_MINIMUM_REQUIRED( VERSION 3.8.2 )
# VS2019: 3.14.1
# Intel 2023: 3.22.3 (Win) 3.20 (Linux)
# VS2022: 3.21

project("dfttest" LANGUAGES CXX)
  include(GNUInstallDirs)

# Avoid uselessly linking to unused libraries
set(CMAKE_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)

# We require C++17 or higher.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

# Detect Intel processors and turn Intel SIMD on or off automatically.
message("-- Detected target processor as: ${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCHID)
if( ("${ARCHID}" STREQUAL "x86") OR
    ("${ARCHID}" STREQUAL "x64") OR
    ("${ARCHID}" STREQUAL "i686") OR
    ("${ARCHID}" STREQUAL "amd64") OR
    ("${ARCHID}" STREQUAL "x86_64") )
  set(INTEL_SIMD "ON")
else()
  set(INTEL_SIMD "OFF")
endif()

option(ENABLE_INTEL_SIMD "Enable SIMD intrinsics for Intel processors" "${INTEL_SIMD}")

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

IF( MSVC )  # Check for Visual Studio
  #1910-1919 = VS 15.0 (v141 toolset) Visual Studio 2017
  #1920-1929 = VS 16.0 (v142 toolset) Visual Studio 2019
  #1930-1939 = VS 17.0 (v143 toolset) Visual Studio 2022

  # Use from a separate "build" directory in order not to overwrite "official" .sln and vcxproj files

  IF( MSVC_VERSION VERSION_LESS 1910 )
    MESSAGE(FATAL_ERROR "Visual C++ 2017 or newer required. For non XP builds use VS 2022")
  ENDIF()

  IF(MSVC_IDE)
      message("MSVC_IDE support found, reported CMAKE_GENERATOR_TOOLSET is: ${CMAKE_GENERATOR_TOOLSET}")
      string( TOLOWER "${CMAKE_GENERATOR_TOOLSET}" cmake_gentoolset_lower)

      IF(cmake_gentoolset_lower STREQUAL "intel c++ compiler 2021" OR
      cmake_gentoolset_lower STREQUAL "intel c++ compiler 2022" OR
      cmake_gentoolset_lower STREQUAL "intel c++ compiler 2023" OR
      cmake_gentoolset_lower STREQUAL "intel c++ compiler 2024" OR
      cmake_gentoolset_lower STREQUAL "intel c++ compiler 2025")
        # IntelLLVM
        set(IntelLLVM_IN_VS "1")
      ELSEIF(cmake_gentoolset_lower STREQUAL "intel c++ compiler 19.2")
        # Intel Classic
        set(IntelClassic_IN_VS "1")
      ELSEIF(cmake_gentoolset_lower MATCHES "intel")
        MESSAGE(FATAL_ERROR "Possibly unknown or unsupported Intel compiler") # version update needed in the above lines
      ELSEIF(cmake_gentoolset_lower STREQUAL "llvm" OR cmake_gentoolset_lower STREQUAL "clangcl")
        if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")  # hope: always
          message("LLVM toolset was specified via -T. Compiler ID is: ${CMAKE_CXX_COMPILER_ID}; CMAKE_CXX_COMPILER_VERSION is: ${CMAKE_CXX_COMPILER_VERSION}")
          if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.1 )
            MESSAGE(FATAL_ERROR "Clang 7.0.1 or newer required")
          endif()
        endif()
        set(CLANG_IN_VS "1")
      ELSEIF(cmake_gentoolset_lower STREQUAL "v141_clang_c2")
        message("v141_clang_c2 toolset was specified via -T. Reported MSVC_VERSION is: ${MSVC_VERSION}")
        message("May not work, try clangcl or LLVM")
        set(CLANG_IN_VS "1")
      ENDIF()

      option(WINXP_SUPPORT "Make binaries compatible with Windows XP and Vista" OFF)
      if(WINXP_SUPPORT)
        IF(NOT CLANG_IN_VS STREQUAL "1")
          set(CMAKE_GENERATOR_TOOLSET "v141_xp" CACHE STRING "The compiler toolset to use for Visual Studio." FORCE)
          message("CMAKE_GENERATOR_TOOLSET is forced to: ${CMAKE_GENERATOR_TOOLSET}")
          add_definitions("/Zc:threadSafeInit-")
        ENDIF()
      endif()
  ENDIF()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        set(CLANG_IN_VS "1")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        set(IntelLLVM_IN_VS "1")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        set(IntelClassic_IN_VS "1")
    endif()

    IF(CLANG_IN_VS STREQUAL "1")
        STRING( REPLACE "/EHsc" "/EHa" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        STRING( REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-inconsistent-missing-override")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
    ELSEIF(IntelLLVM_IN_VS STREQUAL "1")
        message("IntelLLVM in VS environment chosen, setting additional flags")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-inconsistent-missing-override")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHa")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise")
    ELSEIF(IntelClassic_IN_VS STREQUAL "1")
        message("Intel Classic chosen, setting additional flags")
        set(DELETE_THIS "/std:c++17")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CompilerFlags
                CMAKE_CXX_FLAGS
                CMAKE_CXX_FLAGS_DEBUG
                CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL
                CMAKE_CXX_FLAGS_RELWITHDEBINFO
                CMAKE_C_FLAGS
                CMAKE_C_FLAGS_DEBUG
                CMAKE_C_FLAGS_RELEASE
                CMAKE_C_FLAGS_MINSIZEREL
                CMAKE_C_FLAGS_RELWITHDEBINFO
                )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
            message("MSVC flags: ${CompilerFlag}:${${CompilerFlag}}")
        endforeach()
    ELSE()
        # MSVC
        STRING( REPLACE "/EHsc" "/EHa" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        STRING( REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        if (NOT (MSVC_VERSION LESS 1930)) # at least VS2022
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:contract")
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:contract")
        endif()
    ENDIF()
    # Prevent VC++ from complaining about not using MS-specific functions
    add_definitions("/D _CRT_SECURE_NO_WARNINGS /D _SECURE_SCL=0")

    # Enable CRT heap debugging - only effective in debug builds
    add_definitions("/D _CRTDBG_MAP_ALLOC")

    set(MSVC_CPU_ARCH "SSE2" CACHE STRING "Set MSVC architecture optimization level (default: SSE2)")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:${MSVC_CPU_ARCH}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:${MSVC_CPU_ARCH}")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      if("${MSVC_CPU_ARCH}" MATCHES "(SSE2)")
        set(DELETE_THIS "/arch:${MSVC_CPU_ARCH}")
        message("MSVC doesn't need x86-64 builds to define /arch:${MSVC_CPU_ARCH}. Setting will be ignored.")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
      elseif("${MSVC_CPU_ARCH}" MATCHES "(IA32|SSE)")
        set(DELETE_THIS "/arch:${MSVC_CPU_ARCH}")
        message("MSVC doesn't allow x86-64 builds to define /arch:${MSVC_CPU_ARCH}. Setting will be ignored.")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        STRING( REPLACE "${DELETE_THIS}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
      endif()
    endif()

    IF(CLANG_IN_VS STREQUAL "1" OR IntelLLVM_IN_VS STREQUAL "1")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-reorder")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-value")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-value")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gcc-compat")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-gcc-compat")
    ENDIF()

    if (NOT WINXP_SUPPORT)
      if (NOT (MSVC_VERSION LESS 1910))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /permissive-")
      endif()
    endif()

    if(ENABLE_INTEL_SIMD)
      add_definitions("/D INTEL_INTRINSICS")
    endif()

ELSE()

    # not MS Visual Studio IDE

    if( ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")   AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)) OR
        ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5)) )
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z" )
    endif()

    if(ENABLE_INTEL_SIMD)
      SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -msse2 -DINTEL_INTRINSICS" )
    endif()

    if(WIN32)
      SET( CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-stdcall-fixup" )
      SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D__CRT__NO_INLINE=1" )
    elseif(APPLE)
      SET( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error" )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      SET( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
    endif()

ENDIF()

IF(ENABLE_INTEL_SIMD)
  message("Intel SIMD enabled")
ELSE()
  message("Intel SIMD disabled")
ENDIF()

add_subdirectory("dfttest")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
