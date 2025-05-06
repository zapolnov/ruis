cmake_minimum_required(VERSION 3.10)

include(GNUInstallDirs)

set(name ruis)

project(${name})

if(NOT TARGET rasterimage::rasterimage)
find_package(rasterimage CONFIG REQUIRED)
endif()

if(NOT TARGET r4::r4)
find_package(r4 CONFIG REQUIRED)
endif()

if(NOT TARGET papki::papki)
find_package(papki CONFIG REQUIRED)
endif()

if(NOT TARGET tml::tml)
find_package(tml CONFIG REQUIRED)
endif()

if(NOT TARGET utki::utki)
find_package(utki CONFIG REQUIRED)
endif()

if(NOT TARGET veg::veg)
find_package(veg CONFIG REQUIRED)
endif()

if(NOT TARGET svgren::svgren)
find_package(svgren CONFIG REQUIRED)
endif()

find_package(Freetype REQUIRED)

set(srcdir "${CMAKE_CURRENT_SOURCE_DIR}/../../src/${name}")
file(GLOB_RECURSE src RELATIVE "${srcdir}" CONFIGURE_DEPENDS "${srcdir}/*.cpp" "${srcdir}/*.hpp")

set(srcs)
foreach(file ${src})
    get_filename_component(path "${file}" DIRECTORY)
    string(REPLACE "/" "\\" path "Source Files/${path}")
    source_group("${path}" FILES "${srcdir}/${file}")
    list(APPEND srcs "${srcdir}/${file}")
endforeach()

add_library(
    ${name}
    STATIC
        ${srcs}
)

target_compile_features(${name} PUBLIC cxx_std_20)
set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(${name} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(
    ${name}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    ${name}
    PUBLIC
        r4::r4
        papki::papki
        utki::utki
        rasterimage::rasterimage
        svgren::svgren
        veg::veg
        tml::tml
        Freetype::Freetype
)

if(NOT RUIS_DISABLE_INSTALL)

# install library header files preserving directory hierarchy
install(
    DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/../../src/${name}"
    DESTINATION
        "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN
        "*.hpp"
)

install(
    TARGETS
        ${name}
    EXPORT # generate cmake configs
        ${name}-config
)

# install cmake configs
install(
    EXPORT
        ${name}-config
    FILE
        ${name}-config.cmake
    DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${name}
    NAMESPACE
        ${name}::
)

endif()
