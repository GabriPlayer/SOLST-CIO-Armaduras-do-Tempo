@echo off
chcp 65001 >nul
setlocal
cd /d "%~dp0"
set "UE=C:\Program Files\Epic Games\UE_5.8\Engine"
set "PROJ=%~dp0unreal\Solsticio.uproject"

if not exist "%UE%\Binaries\Win64\UnrealEditor.exe" (
  echo Unreal Engine 5.8 nao encontrado em "%UE%".
  echo Instala-o pelo Epic Games Launcher ou edita a variavel UE neste ficheiro.
  pause & exit /b 1
)

:menu
cls
echo ==========================================
echo   SOLSTICIO: Ecos do Tempo - Launcher
echo ==========================================
echo   1. Jogar (versao 3D)
echo   2. Abrir no Unreal Editor
echo   3. Jogar versao web (navegador)
echo   4. Compilar o projeto
echo   0. Sair
echo.
set /p op=Escolhe uma opcao: 
if "%op%"=="1" goto play
if "%op%"=="2" goto editor
if "%op%"=="3" goto web
if "%op%"=="4" goto build
if "%op%"=="0" exit /b 0
goto menu

:build
call "%UE%\Build\BatchFiles\Build.bat" SolsticioEditor Win64 Development "-Project=%PROJ%" -WaitMutex
if errorlevel 1 ( echo. & echo A compilacao falhou. & pause & goto menu )
exit /b 0

:play
call :build_quiet || goto menu
start "" "%UE%\Binaries\Win64\UnrealEditor.exe" "%PROJ%" /Engine/Maps/Entry -game -windowed -ResX=1600 -ResY=900 -log
exit /b 0

:editor
call :build_quiet || goto menu
start "" "%UE%\Binaries\Win64\UnrealEditor.exe" "%PROJ%"
exit /b 0

:web
start "" "%~dp0game\index.html"
exit /b 0

:build_quiet
echo A compilar (so demora na primeira vez)...
call "%UE%\Build\BatchFiles\Build.bat" SolsticioEditor Win64 Development "-Project=%PROJ%" -WaitMutex >"%TEMP%\solsticio_build.log" 2>&1
if errorlevel 1 (
  echo A compilacao falhou. Ver "%TEMP%\solsticio_build.log"
  pause
  exit /b 1
)
exit /b 0