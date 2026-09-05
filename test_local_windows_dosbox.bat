@echo off
setlocal

set "ROOT_DIR=%~dp0."
set "dosbox_command="

:parse_arguments
if "%~1"=="" goto arguments_done
if /i "%~1"=="--dosbox" (
    if "%~2"=="" (
        >&2 echo error: --dosbox requires the path to a DOSBox executable
        pause
        exit /b 1
    )
    if not exist "%~2" (
        >&2 echo error: DOSBox executable "%~2" was not found
        pause
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
pause
exit /b 1

:arguments_done
if not exist "%ROOT_DIR%\ZINE.EXE" (
    >&2 echo error: "%ROOT_DIR%\ZINE.EXE" not found; restore it or run build_dos-zine_bin.sh
    pause
    exit /b 1
)

if not defined dosbox_command (
    if defined DOSBOX (
        set "dosbox_command=%DOSBOX%"
    ) else if exist "C:\Program Files\DOSBox Staging\dosbox.exe" (
        set "dosbox_command=C:\Program Files\DOSBox Staging\dosbox.exe"
    ) else (
        where /q dosbox-staging.exe
        if not errorlevel 1 (
            set "dosbox_command=dosbox-staging.exe"
        ) else (
            where /q dosbox.exe
            if errorlevel 1 (
                >&2 echo error: DOSBox or DOSBox Staging was not found
                pause
                exit /b 1
            )
            set "dosbox_command=dosbox.exe"
        )
    )
)

echo Testing the canonical local executable: ZINE.EXE
"%dosbox_command%" -c "mount c \"%ROOT_DIR%\"" -c "c:" -c "zine.exe"
set "dosbox_exit_code=%errorlevel%"
if not "%dosbox_exit_code%"=="0" (
    >&2 echo error: DOSBox exited with code %dosbox_exit_code%
    pause
)
exit /b %dosbox_exit_code%
