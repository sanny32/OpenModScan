param(
    [switch]$qt5,
    [switch]$qt6,
    [string]$BuildType = "Release"
)

if ($qt5 -and $qt6) {
    Write-Error "Can't use -qt5 and -qt6 parameters together."
    exit 1
}

if ($qt5) {
    $QtVersion   = "5.15.2"
    $Arch        = "win64"
    $Compiler    = "msvc2019_64"
    $CMakeGenerator = "Visual Studio 16 2019"
}
else {
    $QtVersion   = "6.10.1"
    $Arch        = "win64"
    $Compiler    = "msvc2022_64"
    $CMakeGenerator = "Visual Studio 17 2022"
}

$QtMajorVersion = ($QtVersion -replace '^(\d+)\..*$', '$1')

$ErrorActionPreference = "Stop"

Write-Host "================================"
Write-Host "=== OpenModScan Build Script ==="
Write-Host "================================"
Write-Host "Qt Version: $QtVersion"
Write-Host "Compiler:   $Compiler"
Write-Host "Generator:  $CMakeGenerator"
Write-Host "BuildType:  $BuildType"
Write-Host "================================"

# Check Windows version and architecture
Write-Host ""
Write-Host "Checking system requirements..."
$os = Get-CimInstance -ClassName Win32_OperatingSystem
$windowsVersion = [System.Environment]::OSVersion.Version
$architecture = $env:PROCESSOR_ARCHITECTURE

Write-Host "Windows Version: $($os.Caption)"
Write-Host "Build Number: $($os.BuildNumber)"
Write-Host "Architecture: $architecture"

# Check minimum Windows version (Windows 10 or later)
if ($windowsVersion.Major -lt 10) {
    Write-Error "This script requires Windows 10 or later."
    exit 1
}

# Check if running on 64-bit system for 64-bit builds
if ($Arch -eq "win64" -and $architecture -ne "AMD64") {
    Write-Error "64-bit build requested but running on 32-bit system. Architecture: $architecture"
    exit 1
}

# Function to install Python
function Install-Python {
    $pythonUrl = "https://www.python.org/ftp/python/3.11.9/python-3.11.9-amd64.exe"
    $installerPath = "$env:TEMP\python-installer.exe"
    
    try {
        Write-Host "Downloading Python 3.11.9..."
        Invoke-WebRequest -Uri $pythonUrl -OutFile $installerPath
        Write-Host "Download completed."
    }
    catch {
        Write-Host "Failed to download Python. Please install manually from: https://www.python.org/downloads/"
        exit 1
    }
    
   Write-Host "Installing Python (this may take a few minutes)..."
    $installArgs = @(
        "/quiet",
        "InstallAllUsers=1",
        "PrependPath=1",
        "Include_test=0",
        "Include_doc=0",
        "Include_tcltk=0",
        "Include_launcher=1"
    )
    
    $process = Start-Process -FilePath $installerPath -ArgumentList $installArgs -Wait -PassThru
    
    if ($process.ExitCode -eq 0) {
        Write-Host "Python installed successfully."
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
        
        # Verify installation
        Start-Sleep -Seconds 2
        if (Get-Command python -ErrorAction SilentlyContinue) {
            $pythonVersion = python --version 2>&1
            Write-Host "Python version: $pythonVersion"
            return $true
        }
    }
    
    Write-Host "Python installation may have completed, but verification failed."
    Write-Host "Please restart your terminal and run the script again, or install Python manually."
    return $false
}

# Function to install CMake system-wide
function Install-CMakeSystem {

    if (-not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()
        ).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
            Write-Error "You need to run script as an Administrator to install CMake!"
            exit 1
        }

    $cmakeUrl = "https://cmake.org/files/v3.31/cmake-3.31.8-windows-x86_64.msi"
    $installerPath = "$env:TEMP\cmake-installer.msi"
    
    try {
        Write-Host "Downloading CMake from cmake.org..."
        Invoke-WebRequest -Uri $cmakeUrl -OutFile $installerPath
        Write-Host "Download completed."
    }
    catch {
        Write-Host "Failed to download CMake. Please install manually from: https://cmake.org/download/"
        exit 1
    }
    
    Write-Host "Installing CMake system-wide..."
    $installArgs = @(
        "/i",
        "`"$installerPath`"",
        "/quiet",
        "/norestart",
        "ADD_CMAKE_TO_PATH=System"
    )
    
    $process = Start-Process -FilePath "msiexec" -ArgumentList $installArgs -Wait -PassThru
    
    if ($process.ExitCode -eq 0) {
        Write-Host "CMake installed successfully."
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
        
        # Verify installation
        Start-Sleep -Seconds 2
        if (Get-Command cmake -ErrorAction SilentlyContinue) {
            $cmakeVersion = cmake --version
            Write-Host "CMake version: $cmakeVersion"
            return $true
        }
    }
    
    Write-Host "CMake installation completed with exit code: $($process.ExitCode)"
    return $false
}

# Test MSVC is installed
function Test-MsvcCompiler {
    if ($QtMajorVersion -eq "5") {
        $msvcPaths = @(
            "${env:ProgramFiles}\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC",
            "${env:ProgramFiles}\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC",
            "${env:ProgramFiles}\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC",
            "${env:ProgramFiles}\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC"
        )
    }
    else {
        $msvcPaths = @(
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC",
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC", 
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC",
            "${env:ProgramFiles}\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC",
            "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC"
        )
    }

    
    foreach ($path in $msvcPaths) {
        if (Test-Path $path) {
            $versions = Get-ChildItem $path -Directory | Sort-Object Name -Descending
            if ($versions) {
                $latestVersion = $versions[0].Name
                $compilerPath = Join-Path $path "$latestVersion\bin\Hostx64\x64\cl.exe"
                if (Test-Path $compilerPath) {
                    return $compilerPath
                }
            }
        }
    }
    return $null
}

# Function to install Visual Studio Build Tools
function Install-VisualStudioBuildTools {   
    if ($QtMajorVersion -eq "5") {
        $vsInstallerUrl = "https://aka.ms/vs/16/release/vs_buildtools.exe"  # VS2019
        $installerPath = "$env:TEMP\vs2019_buildtools.exe"
    } else {
        $vsInstallerUrl = "https://aka.ms/vs/17/release/vs_buildtools.exe"  # VS2022
        $installerPath = "$env:TEMP\vs2022_buildtools.exe"
    }
    
    try {
        Write-Host "Downloading Visual Studio Build Tools..."
        Invoke-WebRequest -Uri $vsInstallerUrl -OutFile $installerPath
        Write-Host "Download completed."
    }
    catch {
        Write-Host "Failed to download Visual Studio Build Tools. Please install manually from: https://visualstudio.microsoft.com/downloads/"
        exit 1
    }
    
    Write-Host "Installing Visual Studio Build Tools (this may take a while)..."
    $installArgs = @(
        "--quiet",
        "--norestart",
        "--add", "Microsoft.VisualStudio.Workload.VCTools",
        "--add", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
        "--add", "Microsoft.VisualStudio.Component.Windows10SDK.19041"
    )
    
    $process = Start-Process -FilePath $installerPath -ArgumentList $installArgs -Wait -PassThru
    
    if ($process.ExitCode -eq 0) {
        Write-Host "Visual Studio Build Tools installed successfully."
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "User")
        
        return $true
    } else {
        Write-Host "Visual Studio Build Tools installation failed with exit code: $($process.ExitCode)"
        return $false
    }
}

# Check if Python is available
Write-Host ""
Write-Host "Checking for Python..."
try {
    $pythonVersion = & python --version 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "Python not found"
    }
    
    Write-Host "Python found: $pythonVersion"
    
    # Check if Python version is compatible (3.6 or newer)
    if ($pythonVersion -match "Python (\d+)\.(\d+)") {
        $major = [int]$matches[1]
        $minor = [int]$matches[2]
        
        if ($major -lt 3 -or ($major -eq 3 -and $minor -lt 6)) {
            Write-Host "Python version $major.$minor is too old. aqtinstall requires Python 3.6+."
            $choice = Read-Host "Do you want to install a newer Python version? (y/n)"
            if ($choice -eq 'y' -or $choice -eq 'Y') {
                $success = Install-Python
                if (-not $success) {
                    Write-Error "Python installation failed."
                    exit 1
                }
            } else {
                Write-Error "Please install Python 3.6 or newer manually."
                exit 1
            }
        }
    }
}
catch {
    Write-Host "Python not found."
    $choice = Read-Host "Do you want to download and install Python 3.11? (y/n)"
    if ($choice -eq 'y' -or $choice -eq 'Y') {
        $success = Install-Python
        if (-not $success) {
            Write-Error "Python installation failed or requires terminal restart."
            exit 1
        }
    } else {
        Write-Host "Please install Python manually from: https://www.python.org/downloads/"
        Write-Host "Make sure to check 'Add Python to PATH' during installation."
        exit 1
    }
}

# Check if MSVC is available
Write-Host ""
Write-Host "Checking for MSVC compiler..."
$msvcPath = Test-MsvcCompiler
if (!$msvcPath) {
    Write-Host "MSVC compiler not found."
    $choice = Read-Host "Do you want to install Visual Studio Build Tools? (y/n)"
    if ($choice -eq 'y' -or $choice -eq 'Y') {
        $success = Install-VisualStudioBuildTools
        if ($success) {
            # Check again after installation
            $msvcPath = Test-MsvcCompiler
            if (!$msvcPath) {
                Write-Error "MSVC compiler still not found after installation. Please restart your terminal and run the script again."
                exit 1
            }
        } else {
            Write-Error "Visual Studio Build Tools installation failed. Please install manually."
            exit 1
        }
    } else {
        Write-Host "Please install Visual Studio 2022 with C++ support from: https://visualstudio.microsoft.com/downloads/"
        exit 1
    }
}
else{
    Write-Host "MSVC compiler found: $msvcPath"
}

# Check if aqtinstall is available by trying to import it
Write-Host ""
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

python -m aqt version

# Check if CMake is available
Write-Host ""
Write-Host "Checking for CMake..."
$cmakePath = $null
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    $cmakePath = (Get-Command cmake).Source
    Write-Host "Found system CMake: $cmakePath"
} else {
    Write-Host "CMake not found in PATH."
    $choice = Read-Host "Do you want to install CMake system-wide? (y/n)"
    if ($choice -eq 'y' -or $choice -eq 'Y') {
        $success = Install-CMakeSystem
        if ($success) {
            $cmakePath = (Get-Command cmake).Source
            Write-Host "CMake installed: $cmakePath"
        } else {
            Write-Error "CMake installation failed. Please install manually from: https://cmake.org/download/"
            exit 1
        }
    } else {
        Write-Host "Please install CMake manually from: https://cmake.org/download/"
        Write-Host "Make sure to add it to PATH during installation."
        exit 1
    }
}

& $cmakePath --version

# Check if Qt is available
Write-Host ""
Write-Host "Checking for Qt..."
$QtDir = "C:\Qt\$QtVersion\$Compiler"
if (-not (Test-Path $QtDir)) {
    Write-Host "Downloading Qt $QtVersion ($Compiler)..."

    $modules = if ($QtMajorVersion -eq "6") { 
        "qtserialbus", "qtserialport", "qt5compat", "qtpdf" 
    }

	python -m aqt install-qt windows desktop $QtVersion ${Arch}_${Compiler} --outputdir C:\Qt -m $modules

    if ($LASTEXITCODE -ne 0) {
        Remove-Item $QtDir -Recurse -Force -ErrorAction SilentlyContinue
        Write-Error "ERROR: aqtinstall failed. Exiting."
        exit 1
    }
}
Write-Host "Found Qt: $QtDir"

# Create Build dir
$BuildDir = "build-omodscan-Qt_${QtVersion}_$($Compiler.ToUpper())bit-$BuildType"
if (-not (Test-Path $BuildDir)) { 
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null 
}
Set-Location $BuildDir

$QtBin = Join-Path $QtDir "bin"
$env:PATH = "$QtBin;$env:PATH"

$QtDir = $QtDir -replace '\\', '/'

Write-Host ""
Write-Host "Configuring project with CMake..."
$cmakeArgs = @(
    "../src",
    "-G", $CMakeGenerator,
    "-DCMAKE_PREFIX_PATH=`"$QtDir`""
)

if ($QtMajorVersion -eq "5") {
    $cmakeArgs += "-DCMAKE_DISABLE_FIND_PACKAGE_Qt6=TRUE"
} 

& $cmakePath @cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Set-Location ..
    Write-Error "CMake configuration failed"   
    exit 1
}

Write-Host "Building project..."
& $cmakePath --build . --config $BuildType
if ($LASTEXITCODE -ne 0) {
    Set-Location ..
    Write-Error "Build failed"
    exit 1
}

# Find the built executable
$exePath = Join-Path $PWD "$BuildType\omodscan.exe"
if (-not (Test-Path $exePath)) {
    # Try alternative path
    $exePath = Join-Path $PWD "omodscan.exe"
}

if (Test-Path $exePath) {
    Write-Host ""
    Write-Host "Build successful! Executable found: $exePath"
    
    # Find windeployqt to copy required DLLs
    $windeployqtPath = Join-Path $QtDir "bin\windeployqt.exe"
    
    if (Test-Path $windeployqtPath) {
        Write-Host ""
        Write-Host "Running windeployqt to deploy Qt dependencies..."
        
        # Change to directory containing the executable
        $exeDir = Split-Path $exePath -Parent
        $currentDir = Get-Location
        Set-Location $exeDir
        
         # Common windeployqt arguments
        $windeployArgs = @(
            "--release"
            "--plugindir", ".\plugins"
            "--no-compiler-runtime"
            "--no-opengl-sw"
        )

        if ($QtMajorVersion -eq "6") {
            # Qt 6 specific options
            $windeployArgs += @(
                "--skip-plugin-types", "help,generic,networkinformation,qmltooling,tls"
                "--exclude-plugins", "qsqlibase,qsqlmimer,qsqloci,qsqlodbc,qsqlpsql"
            )
        }
        elseif ($QtMajorVersion -eq "5") {
            # Qt 5 specific options
            $windeployArgs += "--no-system-d3d-compiler"
        }

        # Executable
        $windeployArgs += "omodscan.exe"

        # Run windeployqt
        & $windeployqtPath @windeployArgs | Out-Null
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "windeployqt completed successfully"
            
        } else {
            Write-Warning "windeployqt failed, but build completed successfully"
        }
        
        # Return to original directory
        Set-Location $currentDir
    } else {
        Write-Warning "windeployqt.exe not found at: $windeployqtPath"
        Write-Host "You may need to manually copy Qt DLLs to the executable directory"
    }
}

Set-Location ..
Write-Host ""
Write-Host "=== Build finished successfully ==="