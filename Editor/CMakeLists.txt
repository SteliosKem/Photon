add_executable(Photon)

target_sources(Photon PRIVATE
    main.cpp
)

target_link_libraries(Photon
PRIVATE
    PhotonEngine
)

add_custom_command(TARGET Photon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Shaders
        $<TARGET_FILE_DIR:Photon>/Shaders
)