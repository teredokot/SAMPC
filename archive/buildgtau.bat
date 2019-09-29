@echo off
echo CREATING ARCHIVE...
echo.
arctool2 -c filelistgtau.txt build\samp.saa
echo.
echo VERIFYING ARCHIVE...
echo.
arctool2 -v filelistgtau.txt build\samp.saa
echo.
pause