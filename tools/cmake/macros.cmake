function(logged_set option value)
    set(${option}
        ${value}
        PARENT_SCOPE
    )
    message("-- ${option} = ${value}")
endfunction()

macro(add_option option help)
    option(${option} ${help})

    if(${option})
        message(STATUS "${option}: ON")
        add_compile_definitions(${option}=1)
    else()
        message(STATUS "${option}: OFF")
    endif()
endmacro()
