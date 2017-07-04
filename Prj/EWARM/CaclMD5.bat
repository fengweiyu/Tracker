    @echo off
    (
    echo [Path]
    echo ResPath="%~dp0Debug\Exe\JT808.bin"
    echo DesPath="%~dp0Debug\Exe\updateMD5.bin"
    )>CalcMD5.ini
    
    echo wscript.sleep 1000>sleep.vbs
    @cscript sleep.vbs >nul
    start CalcMD5.exe
    del /f /s /q sleep.vbs

    echo wscript.sleep 1000>sleep.vbs
    @cscript sleep.vbs >nul
    taskkill /f /im CalcMD5.exe
    del /f /s /q sleep.vbs

    del  /f /s /q CalcMD5.ini
    