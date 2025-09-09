param(
    [string]$QtVersion = "6.9.2",
    [string]$Compiler = "win64_msvc2022_64",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=== OpenModScan Build Script ==="

if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Python not found. Install from https://www.python.org/downloads/"
    exit 1
}

try {
    python -m aqt --version > $null 2>&1
} catch {
    if (-not (pip show aqtinstall 2>$null)) {
        Write-Host "Installing aqtinstall from GitHub..."
        python -m pip install --upgrade pip
        python -m pip install git+https://github.com/miurahr/aqtinstall.git
    }
}

$BuildDir = "build-omodscan-Qt_${QtVersion}_MSVC2022_64bit-$BuildType"

$QtDir = Join-Path $PWD ".qt\$QtVersion\$Compiler"

if (-not (Test-Path $QtDir)) {
    Write-Host "Downloading Qt $QtVersion ($Compiler)..."
    python -m aqt install-qt windows desktop $QtVersion $Compiler --outputdir $QtDir
    if ($LASTEXITCODE -ne 0) {
        Write-Error "ERROR: aqtinstall failed. Exiting."
        exit 1
    }
}

if (-not (Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir | Out-Null }
Set-Location $BuildDir

$QtBin = Join-Path $QtDir "bin"
$env:PATH = "$QtBin;$env:PATH"

cmake ../omodscan -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="$QtDir" -DCMAKE_BUILD_TYPE=$BuildType
cmake --build . --config $BuildType

Write-Host "=== Build finished successfully ==="
