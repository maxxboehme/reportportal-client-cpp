from conans import ConanFile, CMake, tools
import os
import re


def _is_travis():
    return os.getenv("TRAVIS", False)


def _is_appveyor():
    return os.getenv("APPVEYOR", False)


def _is_conan_reference_defined():
    conan_reference = os.getenv("CONAN_REFERENCE", "")
    return conan_reference == ""


def _get_branch(git):
    if _is_travis():
        return os.getenv("TRAVIS_BRANCH", None)
    elif _is_appveyor():
        return os.getenv("APPVEYOR_REPO_BRANCH", None)
    else:
        return git.get_branch()


def _is_release_branch(branch):
    if re.match(r"^release/\d+\.\d+\.\d+$", branch):
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
        # This is helpful in case someone defines the CONAN_REFERENCE environment variable.
        # The main situation this is for is when using docker containers to package and publish
        # as we run into issues determining revision and branch using git.
        if _is_conan_reference_defined():
            conan_reference = os.getenv("CONAN_REFERENCE", "")
            match = re.match("%s/([^@]+)@.+" % self.name, conan_reference)
            if match:
                self.version = match.group(1)
            else:
                raise ValueError("CONAN_REFERENCE could not be parsed for version")
        else:
            content = tools.load(os.path.join(self.recipe_folder, "../project-meta-info.cmake"))
            major_minor_patch = re.search(r"set\(project_version (\d+\.\d+\.\d+)\)", content).group(1)

            git = tools.Git()
            latest_release_tag = self.run("git rev-list --tags --no-walk --max-count=1")
            if latest_release_tag == 0:
                latest_release_tag += ".."
            revision = self.run("git rev-list %sHEAD --count" % latest_release_tag)
            branch = _get_branch(git)

            if revision == 0 and _is_release_branch(branch):
                # If we are on a release branch and there is no revisions from tag then we should create an
                # official release package.
                self.version = major_minor_patch
            else:
                self.version = "%s-%s+%s" % (major_minor_patch, revision, branch)

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

