macro(target_public_headers TARGET)
    set_target_properties(${TARGET} PROPERTIES PUBLIC_HEADER "${ARGN}")
endmacro()

function(to_python_bool option return)
    string(REPLACE "ON" "True" return_temp ${option})
    string(REPLACE "OFF" "False" return_temp ${return_temp})
    set(${return} ${return_temp} PARENT_SCOPE)
endfunction()
