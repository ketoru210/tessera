# deploy.ps1 - build Tessera and bundle the Qt + MinGW runtime DLLs next to the
# executable, so tessera.exe can run standalone (double-click, or copy to a PC
# without Qt installed).
#
# Usage:  ./deploy.ps1
#
# Adjust the paths below if your Qt installation moves.

$ErrorActionPreference = "Stop"

# --- paths ---
# Machine-specific locations: default to this machine's layout, but allow override
# via environment variables so the script stays portable across machines.
#   e.g.  $env:QT_ROOT = "D:/Qt/6.12/mingw_64";  ./deploy.ps1
$QtRoot      = if ($env:QT_ROOT)   { $env:QT_ROOT }   else { "C:/Qt/6.11.1/mingw_64" }
$MingwBin    = if ($env:MINGW_BIN) { $env:MINGW_BIN } else { "C:/Qt/Tools/mingw1310_64/bin" }
$Ninja       = if ($env:NINJA)     { $env:NINJA }     else { "C:/Qt/Tools/Ninja/ninja.exe" }
$SourceDir   = $PSScriptRoot
$BuildDir    = Join-Path $SourceDir "build/vscode"
$WinDeployQt = Join-Path $QtRoot "bin/windeployqt.exe"

# --- configure (only the first time, or after the build dir is wiped) ---
if (-not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
    Write-Host "[deploy] configuring $BuildDir ..." -ForegroundColor Cyan
    cmake -S $SourceDir -B $BuildDir -G Ninja `
        -DCMAKE_PREFIX_PATH="$QtRoot" `
        -DCMAKE_MAKE_PROGRAM="$Ninja" `
        -DCMAKE_C_COMPILER="$MingwBin/gcc.exe" `
        -DCMAKE_CXX_COMPILER="$MingwBin/g++.exe"
}

# --- build ---
Write-Host "[deploy] building ..." -ForegroundColor Cyan
cmake --build $BuildDir

# --- bundle the runtime DLLs next to the exe(s) ---
# windeployqt copies each exe's Qt + MinGW runtime DLLs into its folder. Both exes
# live in $BuildDir, so this makes tessera.exe (GUI) and coretest.exe (console
# harness) runnable standalone - no need to put Qt's bin on PATH.
foreach ($exeName in @("tessera.exe", "coretest.exe")) {
    $Exe = Join-Path $BuildDir $exeName
    if (Test-Path $Exe) {
        Write-Host "[deploy] running windeployqt on $Exe ..." -ForegroundColor Cyan
        & $WinDeployQt --compiler-runtime $Exe
    }
}

Write-Host "[deploy] done. Standalone exes in: $BuildDir" -ForegroundColor Green
