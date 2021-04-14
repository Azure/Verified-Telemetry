# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: MIT
#
# Cmake module used by any project's target for creating a code coverage target for it.
#

if(VT_CODE_COVERAGE AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    include(CodeCoverage)
endif()

macro(add_code_coverage_target code_cov_target)
    if(VT_CODE_COVERAGE AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
        append_coverage_compiler_flags()

        setup_target_for_coverage_gcovr_xml(
            NAME ${code_cov_target}_coverage
            EXCLUDE ${COV_EXCLUDE}
            EXECUTABLE ${code_cov_target}
        )
        setup_target_for_coverage_gcovr_html(
            NAME ${code_cov_target}_coverage_html 
            EXCLUDE ${COV_EXCLUDE}
            EXECUTABLE ${code_cov_target}
        )
    endif()
endmacro()