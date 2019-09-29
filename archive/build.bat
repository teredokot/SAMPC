@echo off
echo CREATING ARCHIVE...
echo.
arctool2 -c filelist.txt build\samp.saa
echo.
echo VERIFYING ARCHIVE...
echo.
arctool2 -v filelist.txt build\samp.saa
echo.
pause