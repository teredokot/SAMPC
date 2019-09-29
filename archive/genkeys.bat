@echo off
echo WARNING WARNING WARNING
echo.
echo READ THIS CAREFULLY
echo.
echo Archive Tool: 
echo     If you recreate keys, arctool2's verification will fail stating that the
echo     files were not found in the archive. You need to rebuild arctool2.exe to
echo     get verification working again.
echo.
echo SAMP Client: 
echo     You need to rebuild samp.dll again if new keys are made.
echo.
echo For the mentally challenged, there's a backup copy of the keys in .\keybackup
echo.
echo Press CTRL+C to cancel, or if you want to continue
pause
echo.
arctool2 -gk
pause