add_executable(https_client_example
    main.cpp
)

target_link_libraries(https_client_example
    PRIVATE
        https_client
)


if(WIN32)
add_custom_command(TARGET https_client_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:https_client>"
        $<TARGET_FILE_DIR:https_client_example>
)
endif()