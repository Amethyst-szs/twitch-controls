@echo off
SET /A test=%RANDOM% * 100000 / 32768 + 1
SET /A num=0
IF NOT "%1"=="" (set num=1)
IF NOT "%2"=="" (set num=2)
IF NOT "%3"=="" (set num=3)
IF NOT "%4"=="" (set num=4)
IF NOT "%5"=="" (set num=5)
(
  if "%num%"=="1" echo %1
  if "%num%"=="2" echo %1 %2
  if "%num%"=="3" echo %1 %2 %3
  if "%num%"=="4" echo %1 %2 %3 %4
  if "%num%"=="5" echo %1 %2 %3 %4 %5
  echo %test%
) > events.log