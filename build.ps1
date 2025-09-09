param(
    [string]$QtVersion = "6.9.2",
    [string]$Compiler = "win64_msvc2022_64",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=== OpenModScan Build Script ==="

# Function to install CMake
function Install-CMake {
    Write-Host "Downloading and installing CMake..."
    
    # Download CMake installer
    $cmakeUrl = "https://github.com/Kitware/CMake/releases/download/v3.29.4/cmake-3.29.4-windows-x86_64.msi"
    $installerPath = "$env:TEMP\cmake-installer.msi"
    
    try {
        Invoke-WebRequest -Uri $cmakeUrl -OutFile $installerPath
        Write-Host "Download completed."
    }
    catch {
        Write-Host "Failed to download CMake. Please install manually from: https://cmake.org/download/"
        exit 1
    }
    
    # Install CMake silently
    Write-Host "Installing CMake..."
    $installArgs = @(
        "/i",
        "`"$installerPath`"",
        "/quiet",
        "ADD_CMAKE_TO_PATH=System",
        "ALLUSERS=1"
    )
    
    Start-Process msiexec -ArgumentList $installArgs -Wait
    
    # Clean up
    Remove-Item $installerPath -ErrorAction SilentlyContinue
    
    Write-Host "CMake installed successfully. Please restart your terminal and run this script again."
    exit 0
}

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

# Check if CMake is available
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "CMake not found in PATH. Checking common installation locations..."
    
    # Check common CMake installation paths
    $cmakePaths = @(
        "${env:ProgramFiles}\CMake\bin\cmake.exe",
        "${env:ProgramFiles(x86)}\CMake\bin\cmake.exe",
        "C:\Program Files\CMake\bin\cmake.exe",
        "C:\Program Files (x86)\CMake\bin\cmake.exe"
    )
    
    $cmakeFound = $false
    foreach ($path in $cmakePaths) {
        if (Test-Path $path) {
            $env:PATH = "$(Split-Path $path);$env:PATH"
            Write-Host "Found CMake at: $path"
            $cmakeFound = $true
            break
        }
    }
    
    if (-not $cmakeFound) {
        $choice = Read-Host "Do you want to install CMake automatically? (y/n)"
        if ($choice -eq 'y' -or $choice -eq 'Y') {
            Install-CMake
        } else {
            Write-Host "CMake is required but not found. Please install it from:"
            Write-Host "https://cmake.org/download/"
            Write-Host ""
            Write-Host "After installation, make sure to:"
            Write-Host "1. Run CMake GUI once to complete setup"
            Write-Host "2. Add CMake to your system PATH, or restart your terminal"
            Write-Host "3. Run this script again"
            exit 1
        }
    }
}

Write-Host "CMake version:"
cmake --version

Write-Host "Configuring project with CMake..."

if (-not (Test-Path $BuildDir)) { 
    New-Item -ItemType Directory -Path $BuildDir | Out-Null 
}
Set-Location $BuildDir

$QtBin = Join-Path $QtDir "bin"
$env:PATH = "$QtBin;$env:PATH"

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