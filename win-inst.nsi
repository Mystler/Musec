!include MUI2.nsh

BrandingText            "Musec"
CRCCheck                on
InstallDir              "$PROGRAMFILES\Musec"
InstallDirRegKey        HKCU "Software\Mystler\Musec" ""
OutFile                 "Musec.exe"
RequestExecutionLevel   user

Name                "Musec"
VIAddVersionKey     "CompanyName"       "Mystler"
VIAddVersionKey     "FileDescription"   "Musec"
VIAddVersionKey     "FileVersion"       "1.1.0"
VIAddVersionKey     "LegalCopyright"    "Florian Meißner"
VIAddVersionKey     "ProductName"       "Musec"
VIProductVersion    "1.1.0.0"

!define MUI_ABORTWARNING
!define MUI_ICON                        "res\musec.ico"
!define MUI_FINISHPAGE_RUN              "$INSTDIR\Musec.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE                   "COPYING.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!define MUI_LANGDLL_ALLLANGUAGES
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Section "Files"
    SetOutPath  "$INSTDIR"
    File        "dist\Musec.exe"
    File        "dist\icudt52.dll"
    File        "dist\icuin52.dll"
    File        "dist\icuuc52.dll"
    File        "dist\Qt5Core.dll"
    File        "dist\Qt5Gui.dll"
    File        "dist\Qt5Multimedia.dll"
    File        "dist\Qt5Network.dll"
    File        "dist\Qt5Widgets.dll"

    SetOutPath  "$INSTDIR\imageformats"
    File        "dist\imageformats\qico.dll"

    SetOutPath  "$INSTDIR\locales"
    File        "dist\locales\musec_de.qm"
    File        "dist\locales\musec_fr.qm"

    SetOutPath  "$INSTDIR\mediaservice"
    File        "dist\mediaservice\qtmedia_audioengine.dll"
    File        "dist\mediaservice\wmfengine.dll"

    SetOutPath  "$INSTDIR\platforms"
    File        "dist\platforms\qwindows.dll"

    SetOutPath  "$INSTDIR\playlistformats"
    File        "dist\playlistformats\qtmultimedia_m3u.dll"

    SetOutPath  "$INSTDIR"
    File        "dist\vcredist_x86.exe"
    ExecWait    "$INSTDIR\vcredist_x86.exe /q /norestart"

    WriteRegStr HKCU "Software\Mystler\Musec" "" $INSTDIR
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    CreateShortCut "$DESKTOP\Musec.lnk" "$INSTDIR\Musec.exe"
    CreateDirectory "$SMPROGRAMS\Musec"
    CreateShortCut  "$SMPROGRAMS\Musec\Musec.lnk" "$INSTDIR\Musec.exe"
    CreateShortCut  "$SMPROGRAMS\Musec\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Uninstall"
    RMDir /r "$SMPROGRAMS\Musec"
    Delete "$INSTDIR\Uninstall.exe"
    RMDir /r "$INSTDIR"
    DeleteRegKey /ifempty HKCU "Software\Mystler\Musec"
SectionEnd
