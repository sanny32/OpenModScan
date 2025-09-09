param(
    [string]$QtVersion = "6.9.2",
    [string]$Compiler = "win64_msvc2022_64",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=== OpenModScan Build Script ==="

# Check if Python is available
if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Python not found. Install from https://www.python.org/downloads/"
    exit 1
}

# Check if aqtinstall is available by trying to import it
Write-Host "Checking for aqtinstall..."
try {
    $null = python -c "import aqt; print('aqtinstall found')" 2>$null
    $aqtInstalled = $true
}
catch {
    $aqtInstalled = $false
}

if (-not $aqtInstalled) {
    Write-Host "Installing aqtinstall from GitHub..."
    python -m pip install --upgrade pip
    python -m pip install git+https://github.com/miurahr/aqtinstall.git
    
    # Verify installation
    try {
        $null = python -c "import aqt; print('aqtinstall installed successfully')" 2>$null
        Write-Host "aqtinstall installed successfully."
    }
    catch {
        Write-Error "Failed to install aqtinstall. Please install manually: pip install aqtinstall"
        exit 1
    }
}

Write-Host "Using aqtinstall version:"
python -m aqt version

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

if (-not (Test-Path $BuildDir)) { 
    New-Item -ItemType Directory -Path $BuildDir | Out-Null 
}
Set-Location $BuildDir

$QtBin = Join-Path $QtDir "bin"
$env:PATH = "$QtBin;$env:PATH"

# Check if CMake is available
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "CMake not found. Please install CMake and add it to PATH"
    exit 1
}

Write-Host "Configuring project with CMake..."
cmake ../omodscan -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="$QtDir" -DCMAKE_BUILD_TYPE=$BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    exit 1
}

Write-Host "Building project..."
cmake --build . --config $BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    exit 1
}

Write-Host "=== Build finished successfully ==="