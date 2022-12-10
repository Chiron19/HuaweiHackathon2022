@REM @echo off
for /l %%x in (1, 1, 10) do (
   (
    echo %%x
    ) | main0.exe
)
examine.exe