@REM @echo off
for /l %%x in (1, 1, 13) do (
   (
    echo %%x
    ) | main.exe
)
examine.exe