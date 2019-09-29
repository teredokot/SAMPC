;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Include Font Stuff

  !include FontReg.nsh
  !include FontName.nsh

;--------------------------------
;General

!define VERSION "0.2X-u1_1"

Name "San Andreas Multiplayer ${VERSION}"
OutFile "sa-mp-${VERSION}-install.exe"
AutoCloseWindow true
DirText "Please select your Grand Theft Auto: San Andreas directory:"
InstallDir "$PROGRAMFILES\Rockstar Games\GTA San Andreas\"
InstallDirRegKey HKLM "Software\Rockstar Games\GTA San Andreas\Installation" ExePath

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages


  !define MUI_WELCOMEPAGE_TITLE "Welcome!"
  !define MUI_FINISHPAGE_TITLE "Installation Complete."

  !insertmacro MUI_PAGE_LICENSE "samp-license.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Functions

Function .onVerifyInstDir
	IfFileExists $INSTDIR\gta_sa.exe GoodGood
		Abort
	GoodGood:
FunctionEnd

;--------------------------------
;Installer Sections

Section "" 
	SetOutPath $INSTDIR
	File samp.exe
	File samp.dll
	File samp.saa
	File samp_debug.exe
	File rcon.exe
	File dxutgui.png
	File "samp-license.txt"

	SetOutPath $SYSDIR
	File "c:\windows\system32\d3dx9_25.dll"

	SetOutPath $INSTDIR
	WriteUninstaller SAMPUninstall.exe
	
	CreateDirectory "$SMPROGRAMS\San Andreas Multiplayer"
	CreateShortcut "$SMPROGRAMS\San Andreas Multiplayer\San Andreas Multiplayer.lnk" "$INSTDIR\samp.exe"
	CreateShortcut "$SMPROGRAMS\San Andreas Multiplayer\Uninstall.lnk" "$INSTDIR\SAMPUninstall.exe"
SectionEnd

Section "Fonts"
	StrCpy $FONT_DIR $FONTS
	!insertmacro InstallTTFFont 'gtaweap3.ttf'
        SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000
SectionEnd

Section "Uninstall"
	Delete $INSTDIR\samp.exe
	Delete $INSTDIR\samp.dll
	Delete $INSTDIR\samp.saa
	Delete $INSTDIR\samp_debug.exe
	Delete $INSTDIR\SAMPUninstall.exe
	
	Delete "$SMPROGRAMS\San Andreas Multiplayer\San Andreas Multiplayer.lnk"
	Delete "$SMPROGRAMS\San Andreas Multiplayer\Uninstall.lnk"
	RMDir "$SMPROGRAMS\San Andreas Multiplayer"
SectionEnd
