# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

# CMakeList.txt : CMake project for CMakeProject2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 99)

option(UNIT_TESTING "Build unit test projects" ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

project(vt LANGUAGES C)
enable_testing ()




# default for Unit testing with cmocka is OFF, however, this will be ON on CI and tests must
# pass before committing changes
if (UNIT_TESTING)
  # make generate step fail if cmocka dependency is not found
  find_package(cmocka CONFIG REQUIRED)

  # When using VCPKG, cmocka lib name is set different than when it is globally installed
  if(DEFINED ENV{VCPKG_ROOT} OR DEFINED ENV{VCPKG_INSTALLATION_ROOT})
    set(CMOCKA_LIB ${CMOCKA_LIBRARIES})
  else()
    set(CMOCKA_LIB cmocka::cmocka)
  endif()

  # for gcc, we need to add no-clobbered compile opt to avoid warning about set-jump function
  set(NO_CLOBBERED_WARNING "")
  if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(NO_CLOBBERED_WARNING "-Wno-clobbered")
  endif()

  add_subdirectory(tests/core)

  set(VT_DEVICE "CUSTOM_DEVICE")

endif()


add_subdirectory(src/platform)
add_subdirectory(src/core)