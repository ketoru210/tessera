# run.ps1 - one entry point to build, bundle, and run Tessera.
#
# Usage:
#   ./run.ps1           run the GUI (tessera.exe)   [default]
#   ./run.ps1 --gui     run the GUI (tessera.exe)
#   ./run.ps1 --test    run the console harness (coretest.exe)
#
# Pipeline: deploy.ps1 (cmake configure if needed + cmake build + bundle Qt/MinGW
# DLLs via windeployqt) -> launch the chosen exe. The bundle step is what lets the
# exes run by themselves, without putting Qt's bin on PATH.
$ErrorActionPreference = "Stop"

$ScriptDir = $PSScriptRoot
$BuildDir  = Join-Path $ScriptDir "build/vscode"

# --- parse args ---
$opt = if ($args.Count -ge 1) { $args[0] } else { "--gui" }
switch ($opt) {
    "--test" { $target = "test" }
    "--gui"  { $target = "gui"  }
    default  {
        Write-Host "Unknown option: $opt" -ForegroundColor Red
        Write-Host "Usage: ./run.ps1 [--gui|--test]"
        exit 1
    }
}

# --- build + bundle DLLs (deploy.ps1 does cmake configure/build then windeployqt) ---
Write-Host "[run] deploy: cmake build + bundle Qt DLLs ..." -ForegroundColor Cyan
& (Join-Path $ScriptDir "deploy.ps1")

# --- pick + launch the exe ---
$Exe = if ($target -eq "test") {
    Join-Path $BuildDir "coretest.exe"
} else {
    Join-Path $BuildDir "tessera.exe"
}

if (-not (Test-Path $Exe)) {
    Write-Host "Executable not found: $Exe" -ForegroundColor Red
    exit 1
}

Write-Host "[run] launching $(Split-Path $Exe -Leaf) ..." -ForegroundColor Cyan
& $Exe
