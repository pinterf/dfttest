CMAKE_MINIMUM_REQUIRED( VERSION 3.8.2 )

set(PluginName "dfttest")

if (NOT WIN32)
  string(TOLOWER "${PluginName}" PluginName)
endif()

set(ProjectName "${PluginName}")
project(${ProjectName} LANGUAGES CXX)

# Core sources (always compiled)
set(dfttest_Sources
  dfttest.cpp
  PlanarFrame.cpp
)

# Intel SIMD-specific sources (only compiled when INTEL_INTRINSICS is enabled)
if(ENABLE_INTEL_SIMD)
  list(APPEND dfttest_Sources
    dfttest_avx.cpp
    dfttest_avx2.cpp
  )
endif()

add_library(${PluginName} SHARED ${dfttest_Sources})

set_target_properties(${PluginName} PROPERTIES "OUTPUT_NAME" "${PluginName}")
if (MINGW)
  set_target_properties(${PluginName} PROPERTIES PREFIX "")
  set_target_properties(${PluginName} PROPERTIES IMPORT_PREFIX "")
endif()

# Automatically group source files according to directory structure
foreach(FILE ${dfttest_Sources})
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  string(REGEX REPLACE "(\\./)" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  if ("${FILE}" MATCHES ".*\\.cpp")
    set(GROUP "Source Files\\${GROUP}")
  elseif("${FILE}" MATCHES ".*\\.h")
    set(GROUP "Header Files\\${GROUP}")
  endif()

  source_group("${GROUP}" FILES "${FILE}")
endforeach()


if(ENABLE_INTEL_SIMD)
  if (MSVC_IDE)
    IF(CLANG_IN_VS STREQUAL "1" OR IntelLLVM_IN_VS STREQUAL "1")
        # special AVX option for source files with *_avx.cpp pattern
        file(GLOB_RECURSE SRCS_AVX "*_avx.cpp")
        set_source_files_properties(${SRCS_AVX} PROPERTIES COMPILE_FLAGS " -mavx ")

        # special AVX2 option for source files with *_avx2.cpp pattern
        file(GLOB_RECURSE SRCS_AVX2 "*_avx2.cpp")
        set_source_files_properties(${SRCS_AVX2} PROPERTIES COMPILE_FLAGS " -mavx2 -mfma ")
    ELSE()
        # special AVX option for source files with *_avx.cpp pattern
        file(GLOB_RECURSE SRCS_AVX "*_avx.cpp")
        set_source_files_properties(${SRCS_AVX} PROPERTIES COMPILE_FLAGS " /arch:AVX ")

        # special AVX2 option for source files with *_avx2.cpp pattern
        file(GLOB_RECURSE SRCS_AVX2 "*_avx2.cpp")
        set_source_files_properties(${SRCS_AVX2} PROPERTIES COMPILE_FLAGS " /arch:AVX2 ")
    ENDIF()
  else()
    # special AVX option for source files with *_avx.cpp pattern
    file(GLOB_RECURSE SRCS_AVX "*_avx.cpp")
    set_source_files_properties(${SRCS_AVX} PROPERTIES COMPILE_FLAGS " -mavx ")

    # special AVX2 option for source files with *_avx2.cpp pattern
    file(GLOB_RECURSE SRCS_AVX2 "*_avx2.cpp")
    set_source_files_properties(${SRCS_AVX2} PROPERTIES COMPILE_FLAGS " -mavx2 -mfma ")
  endif()
endif()


# Specify include directories
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Haiku")
    set(SYSLIB "root")
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
    set(SYSLIB "pthread")
elseif(MSVC OR MINGW)
    set(SYSLIB "uuid" "winmm" "vfw32" "msacm32" "gdi32" "user32" "advapi32" "ole32" "imagehlp")
else()
    # dl is needed for dlopen (loading libfftw3f at runtime)
    set(SYSLIB "pthread" "dl" "m")
endif()

if(MINGW)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    list(APPEND SYSLIB "pthread")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND SYSLIB "stdc++")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        list(APPEND SYSLIB "stdc++fs")
    endif()
elseif(NOT MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND SYSLIB "c++")
endif()

target_link_libraries(${ProjectName} ${SYSLIB})

include(GNUInstallDirs)

INSTALL(TARGETS ${ProjectName}
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/avisynth")
