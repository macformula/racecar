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
