@echo off
setlocal

if "%Platform%" neq "x64" (
    echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt or alternatively call vcvarsall.
    exit /b 1
)

set "root_dir=%cd%"
set "build_dir=%root_dir%\build"
set "src_dir=%root_dir%\src"
set "run_dir=%root_dir%\run_path"

set "common_comp_options=/GS- /Oi /W4 /wd4201 -I%root_dir%"
set "common_link_options=/INCREMENTAL:no /opt:icf /opt:ref libvcruntime.lib kernel32.lib %temp_link_libs%"

if "%1"=="debug" (
	set "comp_options=%common_comp_options% /DMU_DEBUG=1 /Od /Zo /Z7 /MTd /RTC1 /wd4100"
	set "link_options=%common_link_options% libucrtd.lib libvcruntimed.lib"
) else if "%1"=="release" (
	set "comp_options=%common_comp_options% /O2"
	set "link_options=%common_link_options%"
) else (
	echo Illegal first argument^, must be one of ^<debug^|release^>
	goto end
)

if "%2"=="game" (
	set /A build_engine=0
	set /A build_game=1
) else if "%2"=="engine" (
	set /A build_engine=1
	set /A build_game=0
) else if "%2"=="all" (
	set /A build_engine=1
	set /A build_game=1
) else (
	echo Illegal second argument^, must be one of ^<game^|engine^|all^>
	goto end
)

if "%3" neq "" (
	echo Illegal number of arguments^, expected^: build ^<debug^|release^>
	goto end
)

if not exist %build_dir% mkdir %build_dir%
if not exist %run_dir%   mkdir %run_dir%
cd %build_dir%

if /I "%build_engine%" EQU "1" (
 	cl /nologo /diagnostics:caret %comp_options% -I%root_dir%\vendor\Win32GL %src_dir%\mu_win32.c /link %link_options% /fixed /SUBSYSTEM:windows /out:engine.exe /pdb:engine.pdb user32.lib opengl32.lib gdi32.lib winmm.lib
	copy /Y %build_dir%\engine.exe %run_dir% >nul
	copy /Y %build_dir%\engine.pdb %run_dir% >nul
)

if /I "%build_game%" EQU "1" (
	cl /nologo /diagnostics:caret %comp_options% %src_dir%\mu.c /LD /link %link_options% /out:game.dll /EXPORT:Tick
	copy /Y %build_dir%\game.dll %run_dir% >nul
	copy /Y %build_dir%\game.pdb %run_dir% >nul
)
:end
endlocal
