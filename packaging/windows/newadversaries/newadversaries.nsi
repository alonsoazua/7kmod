; newadversaries.nsi
; Installer of New Adversaries, a mod of Seven Kingdoms: Ancient Adversaries.
; Based on ../install.nsi of 7kaa. Licensed under the GPL.
;
; Built by make_installer.sh, which prepares the files in a staging folder and
; runs makensis there:
;   makensis -DVERSION=0.3.0 -DSTAGE=<staging folder> -DOUTFILE=<setup.exe> newadversaries.nsi

!ifndef VERSION
  !define VERSION "0.3.0"
!endif
!ifndef STAGE
  !define STAGE "stage"
!endif
!ifndef OUTFILE
  !define OUTFILE "NewAdversaries-${VERSION}-setup.exe"
!endif

!define APPNAME "New Adversaries"
!define REGKEY  "Software\NewAdversaries"
!define UNINSTKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\NewAdversaries"

Unicode true
!include "MUI2.nsh"

Name "${APPNAME} ${VERSION}"
OutFile "${OUTFILE}"
InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKLM "${REGKEY}" "Install_Dir"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

VIProductVersion "${VERSION}.0"
VIAddVersionKey /LANG=1033 "ProductName" "${APPNAME}"
VIAddVersionKey /LANG=1033 "FileDescription" "${APPNAME} ${VERSION} setup, a mod of Seven Kingdoms: Ancient Adversaries"
VIAddVersionKey /LANG=1033 "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=1033 "LegalCopyright" "GPL; music by Bjorn Lynne and Enlight Software (see COPYING-Music.txt)"
VIAddVersionKey /LANG=1033 "LegalTrademarks" "Seven Kingdoms is a trademark of Enlight Software"

Var StartMenuFolder

;--------------------------------
; Interface

!define MUI_ICON   "${STAGE}\installer.ico"
!define MUI_UNICON "${STAGE}\installer.ico"
!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${STAGE}\Licencias\COPYING.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${REGKEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${APPNAME}"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\7k-newAdversaries.exe"
!define MUI_FINISHPAGE_RUN_TEXT "$(RunGame)"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\$(ReadmeFile)"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "$(ShowReadme)"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages (the first one is the default)

!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

; remember the language for the uninstaller
!define MUI_LANGDLL_REGISTRY_ROOT "HKLM"
!define MUI_LANGDLL_REGISTRY_KEY "${REGKEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

LangString ReadmeFile ${LANG_SPANISH} "LEEME.txt"
LangString ReadmeFile ${LANG_ENGLISH} "README.txt"
LangString RunGame    ${LANG_SPANISH} "Jugar New Adversaries"
LangString RunGame    ${LANG_ENGLISH} "Play New Adversaries"
LangString ShowReadme ${LANG_SPANISH} "Ver el LEEME"
LangString ShowReadme ${LANG_ENGLISH} "Show the README"
LangString Uninstall  ${LANG_SPANISH} "Desinstalar"
LangString Uninstall  ${LANG_ENGLISH} "Uninstall"

LangString SecGame    ${LANG_SPANISH} "New Adversaries y Classic (requerido)"
LangString SecGame    ${LANG_ENGLISH} "New Adversaries and Classic (required)"
LangString SecMusic   ${LANG_SPANISH} "Música"
LangString SecMusic   ${LANG_ENGLISH} "Music"
LangString SecDesktop ${LANG_SPANISH} "Acceso directo en el escritorio"
LangString SecDesktop ${LANG_ENGLISH} "Desktop shortcut"

LangString DescGame    ${LANG_SPANISH} "Los dos ejecutables, los datos del juego, la documentación y las licencias."
LangString DescGame    ${LANG_ENGLISH} "Both executables, the game data, the documentation and the licenses."
LangString DescMusic   ${LANG_SPANISH} "La música original de Bjorn Lynne (unos 48 MB). Tiene su propia licencia."
LangString DescMusic   ${LANG_ENGLISH} "The original music by Bjorn Lynne (about 48 MB). It has its own license."
LangString DescDesktop ${LANG_SPANISH} "Un acceso directo a New Adversaries en el escritorio."
LangString DescDesktop ${LANG_ENGLISH} "A shortcut to New Adversaries on the desktop."

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

;--------------------------------
; Sections

Section "$(SecGame)" SecGameId
  SectionIn RO

  SetOutPath "$INSTDIR"
  File "${STAGE}\7k-newAdversaries.exe"
  File "${STAGE}\7k-newAdversaries-Classic.exe"
  File "${STAGE}\LEEME.txt"
  File "${STAGE}\README.txt"
  File "${STAGE}\7kaa-manual.pdf"
  File "${STAGE}\SDL2.dll"
  File "${STAGE}\OpenAL32.dll"
  File "${STAGE}\libcurl-4.dll"
  File /r "${STAGE}\ENCYC"
  File /r "${STAGE}\ENCYC2"
  File /r "${STAGE}\IMAGE"
  File /r "${STAGE}\RESOURCE"
  File /r "${STAGE}\SCENARI2"
  File /r "${STAGE}\SCENARIO"
  File /r "${STAGE}\SOUND"
  File /r "${STAGE}\SPRITE"
  File /r "${STAGE}\TUTORIAL"
  File /r "${STAGE}\locale"
  File /r "${STAGE}\MODSOUND"
  File /r "${STAGE}\Changelog"
  File /r "${STAGE}\Docs"
  File /r "${STAGE}\Licencias"

  WriteRegStr HKLM "${REGKEY}" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayName" "${APPNAME} ${VERSION}"
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINSTKEY}" "DisplayIcon" "$INSTDIR\7k-newAdversaries.exe"
  WriteRegStr HKLM "${UNINSTKEY}" "Publisher" "alonsoazua (7kmod)"
  WriteRegStr HKLM "${UNINSTKEY}" "URLInfoAbout" "https://github.com/alonsoazua/7kmod"
  WriteRegStr HKLM "${UNINSTKEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${UNINSTKEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINSTKEY}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\New Adversaries.lnk" "$INSTDIR\7k-newAdversaries.exe" "" "$INSTDIR\7k-newAdversaries.exe" 0
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\New Adversaries Classic.lnk" "$INSTDIR\7k-newAdversaries-Classic.exe" "" "$INSTDIR\7k-newAdversaries-Classic.exe" 0
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\LEEME.lnk" "$INSTDIR\LEEME.txt"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\README.lnk" "$INSTDIR\README.txt"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Changelog.lnk" "$INSTDIR\Changelog"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\$(Uninstall).lnk" "$INSTDIR\uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

; Music files: not GPL, see COPYING-Music.txt (the user can leave them out)
Section "$(SecMusic)" SecMusicId
  SetOutPath "$INSTDIR"
  File /r "${STAGE}\MUSIC"
SectionEnd

Section "$(SecDesktop)" SecDesktopId
  SetShellVarContext all
  CreateShortcut "$DESKTOP\New Adversaries.lnk" "$INSTDIR\7k-newAdversaries.exe" "" "$INSTDIR\7k-newAdversaries.exe" 0
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGameId} $(DescGame)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMusicId} $(DescMusic)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopId} $(DescDesktop)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Uninstaller: removes only what the installer put there. The saved games in
; %APPDATA%\7kfans.com\ are kept.

Section "Uninstall"
  SetShellVarContext all

  Delete "$INSTDIR\7k-newAdversaries.exe"
  Delete "$INSTDIR\7k-newAdversaries-Classic.exe"
  Delete "$INSTDIR\LEEME.txt"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\7kaa-manual.pdf"
  Delete "$INSTDIR\SDL2.dll"
  Delete "$INSTDIR\OpenAL32.dll"
  Delete "$INSTDIR\libcurl-4.dll"
  RMDir /r "$INSTDIR\ENCYC"
  RMDir /r "$INSTDIR\ENCYC2"
  RMDir /r "$INSTDIR\IMAGE"
  RMDir /r "$INSTDIR\RESOURCE"
  RMDir /r "$INSTDIR\SCENARI2"
  RMDir /r "$INSTDIR\SCENARIO"
  RMDir /r "$INSTDIR\SOUND"
  RMDir /r "$INSTDIR\SPRITE"
  RMDir /r "$INSTDIR\TUTORIAL"
  RMDir /r "$INSTDIR\locale"
  RMDir /r "$INSTDIR\MODSOUND"
  RMDir /r "$INSTDIR\MUSIC"
  RMDir /r "$INSTDIR\Changelog"
  RMDir /r "$INSTDIR\Docs"
  RMDir /r "$INSTDIR\Licencias"
  Delete "$INSTDIR\uninstall.exe"
  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\*.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  Delete "$DESKTOP\New Adversaries.lnk"

  DeleteRegKey HKLM "${UNINSTKEY}"
  DeleteRegKey HKLM "${REGKEY}"
SectionEnd
