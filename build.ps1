param(
    [string]$QtVersion = "6.9.2",
    [string]$Compiler = "win64_msvc2022_64",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "=== OpenModScan Build Script ==="

# Function to download and setup portable CMake
function Install-CMakePortable {
    Write-Host "Downloading portable CMake..."
    
    $cmakeZipUrl = "https://github.com/Kitware/CMake/releases/download/v3.29.4/cmake-3.29.4-windows-x86_64.zip"
    $zipPath = "$env:TEMP\cmake.zip"
    $extractPath = "$PWD\.tools\cmake"
    
    try {
        # Download CMake
        Write-Host "Downloading CMake from GitHub..."
        Invoke-WebRequest -Uri $cmakeZipUrl -OutFile $zipPath
        Write-Host "Download completed."
    }
    catch {
        Write-Host "Failed to download CMake. Please install manually from: https://cmake.org/download/"
        exit 1
    }
    
    # Extract CMake
    Write-Host "Extracting CMake..."
    if (-not (Test-Path $extractPath)) {
        New-Item -ItemType Directory -Path $extractPath -Force | Out-Null
    }
    
    try {
        Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force
    }
    catch {
        Write-Host "Failed to extract CMake. Trying alternative method..."
        # Alternative extraction method
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        [System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $extractPath)
    }
    
    # Clean up
    Remove-Item $zipPath -ErrorAction SilentlyContinue
    
    # Find cmake.exe (it's usually in bin folder)
    $cmakeExe = Get-ChildItem -Path $extractPath -Recurse -Filter "cmake.exe" | Select-Object -First 1
    
    if ($cmakeExe) {
        $cmakeDir = $cmakeExe.DirectoryName
        Write-Host "CMake found at: $cmakeDir"
        return $cmakeDir
    }
    
    Write-Host "Failed to find cmake.exe in the extracted files."
    Write-Host "Please install CMake manually from: https://cmake.org/download/"
    exit 1
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
    Write-Host "Installing aqtinstall..."
    python -m pip install --upgrade pip
    python -m pip install aqtinstall
    
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

# Check if CMake is available
$cmakePath = $null
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    $cmakePath = (Get-Command cmake).Source
    Write-Host "Found system CMake: $cmakePath"
} else {
    Write-Host "CMake not found in PATH, using portable version..."
    $cmakeDir = Install-CMakePortable
    $env:PATH = "$cmakeDir;$env:PATH"
    $cmakePath = "$cmakeDir\cmake.exe"
}

Write-Host "CMake version:"
& $cmakePath --version

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
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null 
}
Set-Location $BuildDir

$QtBin = Join-Path $QtDir "bin"
$env:PATH = "$QtBin;$env:PATH"

Write-Host "Configuring project with CMake..."
& $cmakePath ../omodscan -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="$QtDir" -DCMAKE_BUILD_TYPE=$BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    exit 1
}

Write-Host "Building project..."
& $cmakePath --build . --config $BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    exit 1
}

Write-Host "=== Build finished successfully ==="