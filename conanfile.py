from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
import shutil
import os

import git

class CryptoHFTradingBot(ConanFile):
    name = "crypto-hft-trading-bot"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    options = {
        "use_mold": [True, False],
        "enable_static_analysis": [True, False],
        "enable_msan": [True, False],
        "enable_lsan": [True, False],
        "enable_unit_tests": [True, False],
        "enable_e2e_tests": [True, False],
        "enable_benchmarks": [True, False],
    }

    default_options = {
        "use_mold": False,
        "enable_static_analysis": False,
        "enable_msan": False,
        "enable_lsan": False,
        "enable_unit_tests": False,
        "enable_e2e_tests": False,
        "enable_benchmarks": False,
    }

    def requirements(self):
        self.requires("spdlog/1.14.1")
        
    def build_requirements(self):
        if self.options.enable_unit_tests or self.options.enable_e2e_tests:
            self.test_requires("gtest/1.14.0")

        if self.options.enable_benchmarks:
            self.test_requires("benchmark/1.8.4")

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["CMAKE_BUILD_TYPE"] = self.settings.build_type
        tc.cache_variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
        tc.cache_variables["ENABLE_STATIC_ANALYSIS"] = self.options.enable_static_analysis
        tc.cache_variables["ENABLE_UNIT_TESTING"] = self.options.enable_unit_tests
        tc.cache_variables["ENABLE_E2E_TESTING"] = self.options.enable_e2e_tests
        tc.cache_variables["ENABLE_BENCHMARKS"] = self.options.enable_benchmarks
        tc.cache_variables["ENABLE_MSAN"] = self.options.enable_msan
        tc.cache_variables["ENABLE_LSAN"] = self.options.enable_lsan
        
        if self.options.use_mold:
            tc.cache_variables["CMAKE_SHARED_LINKER_FLAGS"] = "-fuse-ld=mold"
            tc.cache_variables["CMAKE_EXE_LINKER_FLAGS"] = "-fuse-ld=mold"

        repo = git.Repo(search_parent_directories=True)
        tc.cache_variables["GIT_HASH"] = repo.head.object.hexsha

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        self.copy_compile_commands()

    def copy_compile_commands(self):
        build_folder = self.build_folder
        source_file = os.path.join(build_folder, "compile_commands.json")
        destination_file = os.path.join(self.source_folder, "compile_commands.json")
        
        if os.path.isfile(source_file):
            shutil.copy(source_file, destination_file)
            self.output.info(f"Copied compile_commands.json to {destination_file}")
        else:
            self.output.warning("compile_commands.json not found!")