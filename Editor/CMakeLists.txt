add_executable(Photon)

target_sources(Photon PRIVATE
    main.cpp
)

target_link_libraries(Photon
PRIVATE
    PhotonEngine
    Vulkan::Vulkan
)