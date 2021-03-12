@echo off
set "lock=%temp%\wait%random%.lock"
set start=%time%

cd ..

START 9>"%lock%0" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile0.txt 2>&1
START 9>"%lock%1" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile1.txt 2>&1
#START 9>"%lock%2" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile2.txt 2>&1
#START 9>"%lock%3" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile3.txt 2>&1
#START 9>"%lock%4" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile4.txt 2>&1
::					 
::START 9>"%lock%5" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile5.txt 2>&1
::START 9>"%lock%6" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile6.txt 2>&1
::START 9>"%lock%7" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile7.txt 2>&1
::START 9>"%lock%8" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile8.txt 2>&1
::START 9>"%lock%9" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile9.txt 2>&1
::
::START 9>"%lock%10" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile10.txt 2>&1
::START 9>"%lock%11" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile11.txt 2>&1
::START 9>"%lock%12" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile12.txt 2>&1
::START 9>"%lock%13" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile13.txt 2>&1
::START 9>"%lock%14" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile14.txt 2>&1
::						
::START 9>"%lock%15" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile15.txt 2>&1
::START 9>"%lock%16" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile16.txt 2>&1
::START 9>"%lock%17" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile17.txt 2>&1
::START 9>"%lock%18" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile18.txt 2>&1
::START 9>"%lock%19" /B art_of_war.exe -headless -outputtype score -benchmarkmode -trainmode -nothreads	> logs/logfile19.txt 2>&1


:Wait for all processes to finish (wait until lock files are no longer locked)
::1>nul 2>nul ping /n 2 ::1
for %%F in ("%lock%*") do (
  (call ) 9>"%%F" || goto :Wait
) 1>nul 2>nul

::delete the lock files
del "%lock%*"

:: Finish up
set end=%time%

set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%

:: Mission accomplished
set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
echo command took %hours%:%mins%:%secs%.%ms% (%totalsecs%.%ms%s total)
