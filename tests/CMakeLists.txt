if(TEST_INSTALLED)
    find_package(reportportal-client-cpp ${project_version} CONFIG REQUIRED)
endif()

if (CONAN_EXPORTED) # in conan local cache
    enable_testing()

    # standard conan installation, deps will be defined in conanfile.py
    # and not necessary to call conan again, conan is alredy running
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

    find_package(reportportal-client-cpp CONFIG REQUIRED)
endif()

# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
    include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
    include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

find_package(Catch2 MODULE REQUIRED)
find_package(fakeit MODULE REQUIRED)

# This allows for faster build times when iterating on tests.
add_library(catch_main STATIC)
target_sources(catch_main
    PRIVATE
        catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_compile_features(catch_main PUBLIC cxx_std_14)

add_executable(reportportal-client-cpp_tests)
target_sources(reportportal-client-cpp_tests
    PRIVATE
        launch_tests.cpp
        test_item_tests.cpp
        rapidjson_serializer_tests.cpp
        catch.hpp
        utils.h
        utils.cpp)

target_link_libraries(reportportal-client-cpp_tests PRIVATE catch_main Catch2::Catch2 fakeit::fakeit reportportal-client-cpp::reportportal-client-cpp)
target_include_directories(reportportal-client-cpp_tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# FakeIt does not support optimizations -O2 or -O3
if (NOT MSVC)
target_compile_options(reportportal-client-cpp_tests
    BEFORE
    PRIVATE
        -O0)
endif()

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. TEST_PREFIX to whatever you want, or use different
# for different binaries
catch_discover_tests(
    reportportal-client-cpp_tests
    TEST_PREFIX
    ""
    EXTRA_ARGS
    -s
    --reporter=xml
    --out=tests.xml)
