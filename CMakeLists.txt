cmake_minimum_required(VERSION 3.15)

# Keep project information in separate file for things that my change.
include("${CMAKE_CURRENT_LIST_DIR}/project-meta-info.cmake")

project(reportportal-client-cpp
    VERSION ${project_version}
    DESCRIPTION  ${project_description}
    HOMEPAGE_URL ${project_homepage}
    LANGUAGES C CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/Utilities.cmake)
include(GNUInstallDirs)

# Enable Doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

option(ENABLE_TESTING "Enable building tests" ON)
option(ENABLE_BENCHMARKING "Enable building benchmarks" OFF)

include(cmake/SetupConan.cmake)

if(TEST_INSTALLED)
    message("Testing installed software")

    enable_testing()
    add_subdirectory(tests)
    # When just wanting to test the installed version
    # just return early
    return()
endif()

add_subdirectory(src)

if(ENABLE_TESTING)
    message("Building Tests")
    enable_testing()

    add_subdirectory(tests)
endif()

if(ENABLE_BENCHMARKING)
    message("Building Benchmarks")
    add_subdirectory(benchmarks)
endif()


set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(version_config "${generated_dir}/${PROJECT_NAME}-config-version.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}-config.cmake")
set(targets_export_name "${PROJECT_NAME}-targets")
set(namespace "${PROJECT_NAME}::")
set(config_files_location "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" VERSION ${project_version} COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in" "${project_config}"
    INSTALL_DESTINATION ${config_files_location})

string(REPLACE "-" "_" INSTALLER_PROJECT_NAME "${PROJECT_NAME}")
set(runtime_component "${INSTALLER_PROJECT_NAME}_rt")
set(development_component "${INSTALLER_PROJECT_NAME}_dev")
install(
    TARGETS reportportal-client-cpp
    EXPORT ${targets_export_name}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT ${runtime_component}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT ${runtime_component}
        NAMELINK_COMPONENT ${development_component}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT ${development_component}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/reportportal
        COMPONENT ${development_component})
install(
    EXPORT ${targets_export_name}
    NAMESPACE ${namespace}
    DESTINATION ${config_files_location}
    COMPONENT ${development_component})
install(
    FILES ${project_config} ${version_config}
    DESTINATION ${config_files_location}
    COMPONENT ${development_component})

# Packaging Support
include(cmake/Packaging.cmake)
