qt_add_executable(lensflare
    main.cpp
)

set_source_files_properties(
        Constants.qml
        PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(lensflare
    URI Lensflare
    VERSION 1.0
    QML_FILES
        Constants.qml
        Main.qml
        LSToolTip.qml
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
