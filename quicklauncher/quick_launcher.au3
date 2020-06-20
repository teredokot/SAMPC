
#NoTrayIcon
#RequireAdmin ; registry functions requires admin mode

#include <GUIConstantsEx.au3>
#include <ColorConstantS.au3>
#include <EditConstants.au3>
#include <WinAPI.au3>
#include <Memory.au3>

Local Const $g_sDefaultKey = "HKCU\Software\SAMP"
Local Const $MAX_PATH = 256

Local $g_sHost = RegRead($g_sDefaultKey, "Host")
If @error Then $g_sHost = "127.0.0.1:7777"

Local $g_sNickname = RegRead($g_sDefaultKey, "Nickname")
If @error Then $g_sNickname = "UnknownPlayer"

Local $g_sGamePath = RegRead($g_sDefaultKey, "GamePath")
If @error Then $g_sGamePath = ""

GUICreate("San Andreas Multiplayer - Quick Launcher", 510, 280)
GUISetBkColor($COLOR_BLACK)

Local $idInput_Host = GUICtrlCreateInput($g_sHost, 45, 40, 420, 20)
GUICtrlCreateLabel("Host:", 45, 20, 70, 20)
GUICtrlSetColor(-1, $COLOR_WHITE)

Local $idInput_Nickname = GUICtrlCreateInput($g_sNickname, 45, 90, 420, 20)
GUICtrlCreateLabel("Nickname:", 45, 70, 100,  20)
GUICtrlSetColor(-1, $COLOR_WHITE)

Local $idInput_Password = GUICtrlCreateInput("", 45, 140, 420, 20, $ES_PASSWORD)
GUICtrlCreateLabel("Password:", 45, 120, 70, 20)
GUICtrlSetColor(-1, $COLOR_WHITE)

Local $idInput_GamePath = GUICtrlCreateInput($g_sGamePath, 45, 190, 325, 20)
$idButton_Browse = GUICtrlCreateButton("Browse", 375, 190, 90,  20)
GUICtrlCreateLabel("Game Path:", 45, 170, 70, 20)
GUICtrlSetColor(-1, $COLOR_WHITE)

Local $idCheckBox_DebugMode = GUICtrlCreateCheckbox("Launch as debug mode", 45, 230)
DllCall("UxTheme.dll", "int", "SetWindowTheme", "hwnd", GUICtrlGetHandle($idCheckBox_DebugMode), "wstr", 0, "wstr", 0)
GUICtrlSetColor($idCheckBox_DebugMode, $COLOR_WHITE)

Local $idButton_Launch = GUICtrlCreateButton("Launch", 280, 230, 90, 25)
Local $idButton_Exit = GUICtrlCreateButton("Exit", 375, 230, 90, 25)

GUISetIcon("gta_sa.ico")

GUISetState(@SW_SHOW)

While 1
   Switch GUIGetMsg()
   Case $GUI_EVENT_CLOSE, $idButton_Exit
	  Save()
	  ExitLoop
   Case $idButton_Browse
	  LocateGame()
   Case $idButton_Launch
	  LaunchMod()
   EndSwitch
WEnd

GUIDelete()

Func LocateGame()
   While 1
	  Local $sPath = FileSelectFolder("Please locate your GTA San Andreas installation...", "")
	  If @error = 1 Then
		 ExitLoop
	  ElseIf Not ($sPath == "") And FileExists($sPath & "\gta_sa.exe") Then
		 RegWrite($g_sDefaultKey, "GamePath", "REG_SZ", $sPath)
		 GUICtrlSetData($idInput_GamePath, $sPath)
		 ExitLoop
	  Else
		 If MsgBox(BitOR($MB_ICONERROR, $MB_RETRYCANCEL), "SA-MP Error", "GTA San Andreas executable (gta_sa.exe) not found.") = $IDCANCEL Then ExitLoop
	  EndIf
   WEnd
EndFunc

Func LaunchMod()
   Local $sHost = GUICtrlRead($idInput_Host)
   If $sHost == "" Then
	  MsgBox($MB_ICONERROR, "SA-MP Error", "No host specified!")
	  Return
   EndIf

   Local $sName = GUICtrlRead($idInput_Nickname)
   If $sName == "" Then
	  MsgBox($MB_ICONERROR, "SA-MP Error", "No nick specified!")
	  Return
   EndIf

   $nPID = ProcessExists("gta_sa.exe")
   If $nPID <> 0 Then
	  If MsgBox(BitOR($MB_ICONERROR, $MB_YESNO), "SA-MP Error", "An instance of GTA: San Andreas already running. Do you want to terminate that process?") = $IDNO Then Return

	  If ProcessClose($nPID) = 0 Then
		 MsgBox($MB_ICONERROR, "SA-MP Error", "Failed to terminate GTA: San Andreas.")
		 Return
	  EndIf
   EndIf

   Local $sPath = GUICtrlRead($idInput_GamePath)
   If $sPath == "" Or Not FileExists($sPath & "\gta_sa.exe") Then
	  MsgBox($MB_ICONERROR, "SA-MP Error", "No game path specified or gta_sa.exe not found in given path location.")
	  Return
   EndIf

   Local $aRequiredFiles[] = ["samp.dll", "samp.saa", "dxutgui.png"]
   For $i = 0 To UBound($aRequiredFiles) - 1
	  If Not FileExists(@WorkingDir & "\\" & $aRequiredFiles[$i]) Then
		 MsgBox($MB_SYSTEMMODAL, "SA-MP Error", $aRequiredFiles[$i] & " not found. Please check your local directory.")
		 Return
	  EndIf
   Next

   Local $tSTARTUPINFO = DllStructCreate($tagSTARTUPINFO)
   Local $tPROCESS_INFORMATION = DllStructCreate($tagPROCESS_INFORMATION)

   DllStructSetData($tSTARTUPINFO, "Size", DllStructGetSize($tSTARTUPINFO))

   Local $sCommandLine = '"' & $sPath & '\gta_sa.exe" '

   If IsDebugModeOn() Then
	  $sCommandLine &= '-d '
   Else
	  $sCommandLine &= '-c '
   EndIf

   $sCommandLine &= '-n "' & $sName & '" -h '

   Local $aHostSplitted = StringSplit($sHost, ":")
   If @error = 1 Then ; If no ":" delimiter found
	  $sCommandLine &= $sHost & " -p 7777"
   Else
	  $sCommandLine &= $aHostSplitted[1] & " -p " & $aHostSplitted[2]
   EndIf

   Local $sPassword = GUICtrlRead($idInput_Password)
   If Not ($sPassword == "") Then
	  $sCommandLine &= " -z " & $sPassword
   EndIf

   Local $bResult = _WinAPI_CreateProcess(Null, $sCommandLine, Null, Null, False, _
	  $CREATE_SUSPENDED, Null, $sPath, $tSTARTUPINFO, $tPROCESS_INFORMATION)
   If Not $bResult Then
	  MsgBox($MB_ICONERROR, "SA-MP Error", "Unable to execute GTA: San Andreas. Error code: " & _WinAPI_GetLastError())
	  Return
   EndIf

   Local $pLibRemote = _MemVirtualAllocEx(DllStructGetData($tPROCESS_INFORMATION, "hProcess"), Null, $MAX_PATH, $MEM_COMMIT, $PAGE_READWRITE)

   Local $tBuffer = DllStructCreate("char[200]")
   Local $nWritten = 0, $pThreadID = Null
   DllStructSetData($tBuffer, 1, @WorkingDir & "\samp.dll")

   Local $bSuccess = _WinAPI_WriteProcessMemory(DllStructGetData($tPROCESS_INFORMATION, "hProcess"), $pLibRemote, DllStructGetPtr($tBuffer), DllStructGetSize($tBuffer), $nWritten)

   Local $pLoadLibraryA = _WinAPI_GetProcAddress(_WinAPI_GetModuleHandle("kernel32"), "LoadLibraryA")
   Local $hThread = _WinAPI_CreateRemoteThread(DllStructGetData($tPROCESS_INFORMATION, "hProcess"), Null, 0, _
		$pLoadLibraryA, $pLibRemote, 0, $pThreadID)

   _WinAPI_WaitForSingleObject($hThread, 2000)
   _WinAPI_CloseHandle($hThread)
   _MemVirtualFreeEx(DllStructGetData($tPROCESS_INFORMATION, "hProcess"), $pLibRemote, $MAX_PATH, $MEM_RELEASE)
   _WinAPI_ResumeThread(DllStructGetData($tPROCESS_INFORMATION, "hThread"))
EndFunc

Func Save()
   Local $sHost = GUICtrlRead($idInput_Host)
   If Not ($sHost == "") Then RegWrite($g_sDefaultKey, "Host", "REG_SZ", $sHost)

   Local $sNick = GUICtrlRead($idInput_Nickname)
   If Not ($sNick == "") Then RegWrite($g_sDefaultKey, "Nickname", "REG_SZ", $sNick)

   Local $sPath = GUICtrlRead($idInput_GamePath)
   If Not ($sPath == "") Then RegWrite($g_sDefaultKey, "GamePath", "REG_SZ", $sPath)
EndFunc

Func IsDebugModeOn()
    Return BitAND(GUICtrlRead($idCheckBox_DebugMode), $GUI_CHECKED) = $GUI_CHECKED
EndFunc

Func IsValidNick($sNickname)
   ; TODO: Invalid character and length check?
   Return True;
EndFunc

Func _WinAPI_ResumeThread($hThread)
   Local $aResult = DllCall('kernel32.dll', 'DWORD', 'ResumeThread', 'HANDLE', $hThread)
   If @error Or (_WinAPI_DWordToInt($aResult[0]) = -1) Then Return SetError(1, 0, -1)
   Return $aResult[0]
EndFunc

Func _WinAPI_CreateRemoteThread($hProcess,$tSecurityAttr,$iStackSize,$pStartAddr,$pParameter,$iFlags, ByRef $pThreadID)
   Local $aResult = DllCall("kernel32.dll", "HANDLE", "CreateRemoteThread", "HANDLE", $hProcess, "struct*", $tSecurityAttr, "ulong_ptr", $iStackSize, _
	  "ptr", $pStartAddr, "ptr", $pParameter, "dword", $iFlags, "dword*", 0)
   If @error Then Return SetError(@error, @extended, False)
   $pThreadID = $aResult[7]
   Return $aResult[0]
EndFunc
