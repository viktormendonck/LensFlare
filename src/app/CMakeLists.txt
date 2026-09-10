qt_add_executable(lensflare
    main.cpp
)

qt_add_qml_module(lensflare
    URI Lensflare
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(lensflare
    PRIVATE
        lensflare::core
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

install(TARGETS lensflare
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
