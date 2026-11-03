set(EXECUTABLE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/omodscan.desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/omodscan.desktop"
    @ONLY
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES res/omodscan16.png DESTINATION share/icons/hicolor/16x16/apps RENAME omodscan.png)
install(FILES res/omodscan32.png DESTINATION share/icons/hicolor/32x32/apps RENAME omodscan.png)
install(FILES res/omodscan64.png DESTINATION share/icons/hicolor/64x64/apps RENAME omodscan.png)
install(FILES res/omodscan128.png DESTINATION share/icons/hicolor/128x128/apps RENAME omodscan.png)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/omodscan.desktop" DESTINATION share/applications)

install(CODE "execute_process(COMMAND update-desktop-database -q ${CMAKE_INSTALL_PREFIX}/share/applications)")
install(CODE "execute_process(COMMAND gtk-update-icon-cache -q -t -f ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor)")
