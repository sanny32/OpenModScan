param(
    [string]$VcpkgRoot = "$PWD\.vcpkg",
    [string]$Triplet = "x64-windows",
    [string]$Generator = "Visual Studio 17 2022"
)

$ErrorActionPreference = "Stop"

Write-Host "=== OpenModScan Build Script ==="

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "CMake not found in PATH"
    exit 1
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "Git not found in PATH"
    exit 1
}

if (-not (Test-Path $VcpkgRoot)) {
    Write-Host "Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg $VcpkgRoot
    cmd /c "$VcpkgRoot\bootstrap-vcpkg.bat"
}

Write-Host "Updating vcpkg..."
cd $VcpkgRoot
git pull
cmd /c ".\bootstrap-vcpkg.bat"
cd ..

Write-Host "Installing prebuilt Qt packages..."
& "$VcpkgRoot\vcpkg.exe" install qtbase:$Triplet --recurse
& "$VcpkgRoot\vcpkg.exe" install qttools:$Triplet --recurse
& "$VcpkgRoot\vcpkg.exe" install qtsvg:$Triplet --recurse
& "$VcpkgRoot\vcpkg.exe" install qtserialport:$Triplet --recurse
& "$VcpkgRoot\vcpkg.exe" install qtserialbus:$Triplet --recurse

$QtVersion = (& "$VcpkgRoot\vcpkg.exe" list qtbase | ForEach-Object {
    if ($_ -match "qtbase:$Triplet\s+([0-9]+\.[0-9]+\.[0-9]+)") { $matches[1] }
})[0]

if (-not $QtVersion) {
    Write-Error "Failed to detect Qt version from vcpkg"
    exit 1
}

$BuildDir = "build-omodscan-Qt_${QtVersion}_MSVC2022_64bit-Release"

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}
Set-Location $BuildDir

cmake ../omodscan -G "$Generator" -DCMAKE_TOOLCHAIN_FILE="$VcpkgRoot\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=$Triplet -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

Write-Host "=== Build finished successfully ==="
