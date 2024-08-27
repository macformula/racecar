# Blake Freer
# April 13, 2024

function(directory_exists output dir)
    # Check if a directory exists
    if((EXISTS "${dir}" AND IS_DIRECTORY "${dir}"))
        message(STATUS "The directory \"${dir}\" exists.")
        set(${output} TRUE PARENT_SCOPE)
    else()
        message(STATUS "\"${dir}\" does not exist or is not a directory.")
        set(${output} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(file_exists output file)
    # Check if a file exists
    if((EXISTS "${file}" AND NOT IS_DIRECTORY "${file}"))
        message(STATUS "The file \"${file}\" exists.")
        set(${output} TRUE PARENT_SCOPE)
    else()
        message(STATUS "\"${file}\" does not exist or is not a file.")
        set(${output} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(assert_exists)
    # Exit if a path does not exist.
    set(options DIR FILE)
    set(one_value_args PATH)
    set(multi_value_args START PASS FAIL)

    cmake_parse_arguments(
        ASSERT_EXISTS
        "${options}"
        "${one_value_args}"
        "${multi_value_args}"
        ${ARGN}
    )

    message(CHECK_START ${ASSERT_EXISTS_START})
    
    if(${ASSERT_EXISTS_FILE})
        file_exists(out ${ASSERT_EXISTS_PATH})
    elseif(${ASSERT_EXISTS_DIR})
        directory_exists(out ${ASSERT_EXISTS_PATH})
    else()
        message(FATAL_ERROR "mode must be FILE or DIR")
    endif()

    if(${out})
        message(CHECK_PASS ${ASSERT_EXISTS_PASS})
    else()
        message(CHECK_PASS ${ASSERT_EXISTS_FAIL})
        message(FATAL_ERROR "Exiting due to an error.")
    endif()
endfunction()