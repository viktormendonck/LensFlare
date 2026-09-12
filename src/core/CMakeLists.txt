add_library(lensflare_core STATIC
    Core.cpp
    Core.h
    raw/RawDecoder.cpp
    raw/RawDecoder.h
        helpers.cpp
        helpers.h
)

target_include_directories(lensflare_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lensflare_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        PkgConfig::LibRaw
        PkgConfig::LCMS2
        Exiv2::exiv2lib
        nlohmann_json::nlohmann_json
)
