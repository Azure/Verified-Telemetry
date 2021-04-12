# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: MIT
#
# Cmake module used by any project's target for creating a code coverage target for it.
#

if(VT_CODE_COVERAGE AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CodeCoverage)
endif()

macro(add_code_coverage_target code_cov_target)
    if(VT_CODE_COVERAGE AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        append_coverage_compiler_flags()

        # Basic coverage using lcov (gcc integrated)
        setup_target_for_coverage_lcov(
            NAME ${code_cov_target}_cov
            EXECUTABLE ${code_cov_target}_test
 #                                   EXCLUDE ${COV_EXCLUDE}
 #                                   LCOV_ARGS --rc lcov_branch_coverage=1
        )

        # HTML and XML - Coverage using gcovr (Needs to be installed into system)
#        setup_target_for_coverage_gcovr_html(NAME ${code_cov_target}_cov_html EXECUTABLE ${code_cov_target}_test)
#        setup_target_for_coverage_gcovr_xml(NAME ${code_cov_target}_cov_xml EXECUTABLE ${code_cov_target}_test)

        # add project to coverage projects for printing
#        file(APPEND ${CMAKE_BINARY_DIR}/coverage_targets.txt " ${code_cov_target}_cov_xml")
    endif()
endmacro()