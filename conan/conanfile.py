from conans import ConanFile, CMake, tools


class ReportportalclientcppConan(ConanFile):
    name = "reportportal-client-cpp"
    version = "0.1"
    license = "Apache-2.0"
    author = "Maxx Boehme"
    url = "https://github.com/maxxboehme/reportportal-client-cpp"
    description = "C++ client for ReportPortal.io"
    topics = ("reportportal")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "enable_testing": [True, False], "enable_benchmarking": [True, False]}
    default_options = {"shared": False, "enable_testing": False, "enable_benchmarking": False}
    exports_sources = [
        "../*",
        "!*build/*",
        "!*tags",
        "!*.swp",
    ]
    generators = "cmake", "cmake_paths", "cmake_find_package"
    requires = "libcurl/7.69.1", "stduuid/1.0", "rapidjson/1.1.0"


    def build_requirements(self):
        if self.options.enable_testing:
            self.build_requires("catch2/2.9.2") # Needed for tests (locked to this version for now because of https://github.com/eranpeer/FakeIt/issues/197)
            self.build_requires("FakeIt/2.0.5@gasuketsu/testing") # Needed for mocks
            self.options["FakeIt"].integration = "catch"

        if self.options.enable_benchmarking:
            self.build_requires("benchmark/1.5.0")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["ENABLE_TESTING"] = self.options.enable_testing
        cmake.definitions["ENABLE_BENCHMARKING"] = self.options.enable_benchmarking
        cmake.configure()
        cmake.build()

        if self.options.enable_testing:
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

