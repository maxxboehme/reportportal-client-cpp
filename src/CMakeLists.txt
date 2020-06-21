find_package(stduuid MODULE REQUIRED)
find_package(RapidJSON MODULE REQUIRED)
find_package(CURL MODULE REQUIRED)

add_library(reportportal-client-cpp)
add_library(${PROJECT_NAME}::reportportal-client-cpp ALIAS reportportal-client-cpp)
target_sources(reportportal-client-cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/attribute_map.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/service.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/launch.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/test_item.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/issue.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/external_system_issue.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/itest_item_parent.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/iservice.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/ijson_serializer.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/rapidjson_serializer.hpp>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/reportportal/log.hpp>
    PRIVATE
        service.cpp
        launch.cpp
        test_item.cpp
        rapidjson_serializer.cpp
        log.cpp)
# This seems redundant since we declare the headers PUBLIC in sources but
# We need to call this to tell cmake what is the public headers so installing happens automatically.
target_public_headers(reportportal-client-cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/attribute_map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/service.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/launch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/test_item.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/issue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/external_system_issue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/itest_item_parent.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/iservice.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/ijson_serializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/rapidjson_serializer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reportportal/log.hpp)
target_include_directories(reportportal-client-cpp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(reportportal-client-cpp
    PUBLIC
        stduuid::stduuid
    PRIVATE
        RapidJSON::RapidJSON
        CURL::CURL)
target_compile_features(reportportal-client-cpp PUBLIC cxx_std_17)
# Needed on Windows platforms as WinDef.h has macros for min/max that interfere with std::min/max
target_compile_definitions(reportportal-client-cpp
    PRIVATE
      NOMINMAX)
