add_library(lensflare_core STATIC
    Core.cpp
    Core.h
)

add_library(lensflare::core ALIAS lensflare_core)

target_link_libraries(lensflare_core
    PUBLIC
        Qt6::Core
        Qt6::Gui
        PkgConfig::LibRaw
        PkgConfig::LCMS2
        Exiv2::exiv2lib
        nlohmann_json::nlohmann_json
)
