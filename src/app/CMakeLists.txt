qt_add_executable(lensflare
    main.cpp
        AppController.cpp
        AppController.h
        ImageProvider.cpp
        ImageProvider.h
)

set_source_files_properties(
        qml/common/Constants.qml
        PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

function(add_lensflare_qml_file source_file)
    get_filename_component(file_name "${source_file}" NAME)

    set_source_files_properties(
            "${source_file}"
            PROPERTIES
            QT_RESOURCE_ALIAS "${file_name}"
    )

    list(APPEND LENSFLARE_QML_FILES "${source_file}")

    set(LENSFLARE_QML_FILES
            "${LENSFLARE_QML_FILES}"
            PARENT_SCOPE
    )
endfunction() #helper function to make adding qml files easier.


add_lensflare_qml_file(qml/Main.qml)
add_lensflare_qml_file(qml/miscPanels/LFFileDialogue.qml)
add_lensflare_qml_file(qml/common/LFToolTip.qml)
add_lensflare_qml_file(qml/common/LFButton.qml)
add_lensflare_qml_file(qml/common/LFMenu.qml)
add_lensflare_qml_file(qml/common/LFMenuItem.qml)
add_lensflare_qml_file(qml/panels/MainMenuBar.qml)
add_lensflare_qml_file(qml/panels/FolderViewPanel.qml)
add_lensflare_qml_file(qml/panels/MainViewPanel.qml)
add_lensflare_qml_file(qml/panels/EditingPanel.qml)
add_lensflare_qml_file(qml/panels/FilmStripPanel.qml)

qt_add_qml_module(lensflare
        URI Lensflare
        VERSION 1.0
        QML_FILES
        qml/Main.qml
        qml/miscPanels/LFFileDialogue.qml
        qml/common/Constants.qml
        qml/common/LFToolTip.qml
        qml/common/LFButton.qml
        qml/common/LFMenu.qml
        qml/common/LFMenuItem.qml
        qml/panels/MainMenuBar.qml
        qml/panels/FolderViewPanel.qml
        qml/panels/MainViewPanel.qml
        qml/panels/EditingPanel.qml
        qml/panels/FilmStripPanel.qml
)

target_link_libraries(lensflare
    PRIVATE
        lensflare_core
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
