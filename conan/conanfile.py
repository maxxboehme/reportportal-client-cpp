from conans import ConanFile, CMake, tools
import os
import re


def _is_release_branch(branch):
    if re.match(r"^release-\d+\.\d+\.\d+$", branch):
        return True
    else:
        return False

class ReportportalclientcppConan(ConanFile):
    name = "reportportal-client-cpp"
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


    # Get Major.Minor.Patch version from project-meta-info.cmake and determine revision and metadata
    # from git information.
    def set_version(self):
        content = tools.load(os.path.join(self.recipe_folder, "../project-meta-info.cmake"))
        major_minor_patch = re.search(r"set\(project_version (\d+\.\d+\.\d+)\)", content).group(1)

        git = tools.Git()
        latest_release_tag = git.run("rev-list --tags --no-walk --max-count=1")
        if latest_release_tag:
            latest_release_tag += ".."
        revision = git.run("rev-list %sHEAD --count" % latest_release_tag)
        branch = git.get_branch()
        if git.is_pristine():
            dirty = ""
        else:
            dirty = ".dirty"

        if revision == 0 and _is_release_branch(branch):
            # If we are on a release branch and there is no revisions from tag then we should create an
            # official release package.
            self.version = major_minor_patch
        else:
            self.version = "%s-%s+%s%s" % (major_minor_patch, revision, branch, dirty)

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

