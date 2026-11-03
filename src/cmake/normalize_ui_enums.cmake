if(NOT DEFINED UI_FILE_LIST)
    message(FATAL_ERROR "UI_FILE_LIST is not set")
endif()

if(NOT EXISTS "${UI_FILE_LIST}")
    message(FATAL_ERROR "UI file list does not exist: ${UI_FILE_LIST}")
endif()

file(READ "${UI_FILE_LIST}" ui_file_list_content)
string(REPLACE "\r\n" "\n" ui_file_list_content "${ui_file_list_content}")
string(REPLACE "\r" "\n" ui_file_list_content "${ui_file_list_content}")
string(REPLACE "\n" ";" ui_file_lines "${ui_file_list_content}")

set(ui_files)
foreach(ui_file_line IN LISTS ui_file_lines)
    string(STRIP "${ui_file_line}" ui_file)
    if(NOT ui_file STREQUAL "")
        list(APPEND ui_files "${ui_file}")
    endif()
endforeach()

set(enum_scope_replacements
    "Qt::AlignmentFlag::=Qt::"
    "Qt::Orientation::=Qt::"
    "Qt::FocusPolicy::=Qt::"
    "Qt::ContextMenuPolicy::=Qt::"
    "Qt::ScrollBarPolicy::=Qt::"
    "Qt::ArrowType::=Qt::"
    "QFrame::Shape::=QFrame::"
    "QFrame::Shadow::=QFrame::"
    "QSizePolicy::Policy::=QSizePolicy::"
    "QDialogButtonBox::StandardButton::=QDialogButtonBox::"
    "QAbstractItemView::EditTrigger::=QAbstractItemView::"
    "QAbstractItemView::SelectionMode::=QAbstractItemView::"
    "QAbstractItemView::SelectionBehavior::=QAbstractItemView::"
    "QAbstractScrollArea::SizeAdjustPolicy::=QAbstractScrollArea::"
    "QComboBox::SizeAdjustPolicy::=QComboBox::"
    "QTabWidget::TabPosition::=QTabWidget::"
    "QToolButton::ToolButtonPopupMode::=QToolButton::"
)

set(changed_files)

foreach(ui_file IN LISTS ui_files)
    if(NOT EXISTS "${ui_file}")
        message(FATAL_ERROR "UI file does not exist: ${ui_file}")
    endif()

    file(READ "${ui_file}" content)
    set(normalized_content "${content}")

    foreach(replacement IN LISTS enum_scope_replacements)
        string(REPLACE "=" ";" replacement_pair "${replacement}")
        list(GET replacement_pair 0 from)
        list(GET replacement_pair 1 to)
        string(REPLACE "${from}" "${to}" normalized_content "${normalized_content}")
    endforeach()

    if(NOT content STREQUAL normalized_content)
        file(WRITE "${ui_file}" "${normalized_content}")
        list(APPEND changed_files "${ui_file}")
    endif()
endforeach()

if(changed_files)
    list(LENGTH changed_files changed_count)
    message(STATUS "Normalized Qt5-compatible enum names in ${changed_count} UI file(s)")
    foreach(changed_file IN LISTS changed_files)
        message(STATUS "  ${changed_file}")
    endforeach()
endif()
