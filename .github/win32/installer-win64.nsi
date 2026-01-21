#--------------------------------
# Includes

  !include "MUI2.nsh"
  !include "UAC.nsh"
  !include "LogicLib.nsh"
  !include "FileFunc.nsh"

#--------------------------------
# Custom defines
  !define MUI_ICON ${ICON_FILE}
  !define NAME "Open ModScan"
  !define PUBLISHER "Alexandr Ananev"
  !define APPFILE "omodscan.exe"
  !define ICOFILE "omodscan.exe"
  !define PACKAGENAME "omodscan_${VERSION}_x64"
  !define SLUG "${NAME} v${VERSION}"
  !define UPDATEURL "https://github.com/sanny32/OpenModScan/releases"

  !define MUI_FINISHPAGE_TEXT "${NAME} v${VERSION} has been installed on your computer."
  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_TEXT "Launch ${NAME}"
  !define MUI_FINISHPAGE_RUN_FUNCTION RunApp
  !define MUI_FINISHPAGE_RUN_CHECKED

Function RunApp
  !insertmacro UAC_AsUser_ExecShell "$INSTDIR\${APPFILE}"
FunctionEnd

#--------------------------------
# Variables

  Var RebootRequired

#--------------------------------
# General

  Name "${NAME} v${VERSION}"
  OutFile "${OUTPUT_FILE}"
  InstallDir "$PROGRAMFILES64\${NAME}"
  RequestExecutionLevel admin

#--------------------------------
# Pages
  
  # Installer pages
  !insertmacro MUI_PAGE_LICENSE ${LICENSE_FILE}
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  # Uninstaller pages
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
  # Set UI language
  !insertmacro MUI_LANGUAGE "English"

#-------------------------------- 
# Installer Sections

  Section
    SetOutPath $INSTDIR
    File /r "${BUILD_PATH}\*"
    WriteUninstaller $INSTDIR\uninstall.exe
  SectionEnd

  Section "Write Registry Information"
    # Registry information for add/remove programs
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "DisplayName" "${NAME}"
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "DisplayIcon" "$\"$INSTDIR\${ICOFILE}$\""
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "Publisher" "${PUBLISHER}"
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "URLUpdateInfo" "${UPDATEURL}"
    WriteRegStr HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "DisplayVersion" "${VERSION}"
    WriteRegDWORD HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "VersionMajor" ${VERSIONMAJOR}
    WriteRegDWORD HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "VersionMinor" ${VERSIONMINOR}
    # There is no option for modifying or repairing the install
    WriteRegDWORD HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "NoModify" 1
    WriteRegDWORD HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "NoRepair" 1
    # Set the estimated eize so Add/Remove Programs can accurately report the size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
	  WriteRegDWORD HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "EstimatedSize" "$0"
  SectionEnd

Section "Visual Studio Runtime"
    SetOutPath "$INSTDIR"
    ExecWait '"$INSTDIR\vc_redist.x64.exe" /install /quiet /norestart' $0

    ; 3010 = reboot required
    ${If} $0 == 3010
        StrCpy $RebootRequired "1"
    ${EndIf}
SectionEnd

#--------------------------------
# Section - Shortcut

  Section "Desktop Shortcut" DeskShort
      CreateShortCut "$DESKTOP\${NAME}.lnk" "$INSTDIR\${APPFILE}" "" "$INSTDIR\${ICOFILE}"
  SectionEnd

  Section "Start Menu"
      CreateDirectory "$SMPROGRAMS\${NAME}"
	    CreateShortCut "$SMPROGRAMS\${NAME}\${NAME}.lnk" "$INSTDIR\${APPFILE}" "" "$INSTDIR\${ICOFILE}"
  SectionEnd
#--------------------------------

#--------------------------------
# Post-install section: ask about reboot only once
Section -Post
    ; Check if VC Redist indicated reboot is required
    ${If} $RebootRequired == "1"
        MessageBox MB_ICONEXCLAMATION|MB_YESNO \
            "A system restart is required to complete the installation.$\r$\nRestart now?" \
            IDYES DoReboot

        ; If user clicked "No", continue silently
        Goto EndPost

        DoReboot:
            Reboot
    ${EndIf}

    EndPost:
SectionEnd

# Remove empty parent directories

  Function un.RMDirUP
    !define RMDirUP '!insertmacro RMDirUPCall'

    !macro RMDirUPCall _PATH
          push '${_PATH}'
          Call un.RMDirUP
    !macroend

    # $0 - current folder
    ClearErrors

    Exch $0
    # DetailPrint "ASDF - $0\.."
    RMDir "$0\.."

    IfErrors Skip
    ${RMDirUP} "$0\.."
    Skip:

    Pop $0

  FunctionEnd

#--------------------------------
# Section - Uninstaller

Section "Uninstall"
  # Attempt to delete the exe directly
  ClearErrors
  Delete "$INSTDIR\${APPFILE}"
  IfErrors ProcessStillRunning ProcessNotFound

ProcessStillRunning:
  MessageBox MB_ICONEXCLAMATION|MB_OK "The application is still running. Uninstallation cannot continue."
  Quit 

ProcessNotFound:
  # Delete Shortcut
  Delete "$DESKTOP\${NAME}.lnk"

# Delete Start Menu Folder
  RMDir /r "$SMPROGRAMS\${NAME}"

  # Delete Uninstall
  Delete "$INSTDIR\uninstall.exe"

  # Delete Folder
  RMDir /r "$INSTDIR"
  ${RMDirUP} "$INSTDIR"

  # Remove uninstaller information from the registry
	DeleteRegKey HKLM64 "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
  DeleteRegKey HKCU64 "Software\${NAME}"
SectionEnd