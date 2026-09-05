@echo off
setlocal

set "RELEASE_DIR=%~dp0RELEASE"
set "dosbox_command=c:\Program Files\DOSBox Staging\dosbox.exe"

:parse_arguments
if "%~1"=="" goto arguments_done
if /i "%~1"=="--dosbox" (
    if "%~2"=="" (
        >&2 echo error: --dosbox requires the path to a DOSBox executable
        exit /b 1
    )
    if not exist "%~2" (
        >&2 echo error: DOSBox executable "%~2" was not found
        exit /b 1
    )
    set "dosbox_command=%~2"
    shift
    shift
    goto parse_arguments
)
if /i "%~1"=="--help" (
    echo Usage: %~nx0 [--dosbox "C:\path\to\dosbox.exe"]
    exit /b 0
)
>&2 echo error: unknown option "%~1"
>&2 echo Usage: %~nx0 [--dosbox "C:\path\to\dosbox.exe"]
exit /b 1

:arguments_done
if not exist "%RELEASE_DIR%\ZINE.EXE" (
    >&2 echo error: "%RELEASE_DIR%\ZINE.EXE" not found; prepare a release first
    exit /b 1
)

if not defined dosbox_command (
    if defined DOSBOX (
        set "dosbox_command=%DOSBOX%"
    ) else (
        where /q dosbox-staging.exe
        if not errorlevel 1 (
            set "dosbox_command=dosbox-staging.exe"
        ) else (
            where /q dosbox.exe
            if errorlevel 1 (
                >&2 echo error: DOSBox or DOSBox Staging was not found
                exit /b 1
            )
            set "dosbox_command=dosbox.exe"
        )
    )
)

"%dosbox_command%" -c "mount c \"%RELEASE_DIR%\"" -c "c:" -c "zine.exe"
exit /b %errorlevel%
