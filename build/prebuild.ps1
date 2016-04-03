#prebuild setup for Appveyor CI

param(
[string]$conf = $(throw "conf is required.")
)

$root = "$PSScriptRoot\..\"

# boost
Write-Host -foregroundcolor "Yellow" "Bootstrapping boost.."
Set-Location "$root\src\external\boost"
.\bootstrap.bat

Write-Host -foregroundcolor "Yellow" "Generating boost headers.."
.\b2 headers -d0

if ($conf -eq "Debug") {
    Write-Host -foregroundcolor "Yellow" "Building boost for Universal Windows.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=debug link=static threading=multi runtime-link=shared windows-api=desktop --stagedir=winrt --with-log
    Write-Host -foregroundcolor "Yellow" "Building boost for Win32 unit tests.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=debug link=static threading=multi runtime-link=shared --stagedir=win --with-log --with-program_options --with-filesystem
} elseif ($conf -eq "Release") {
    Write-Host -foregroundcolor "Yellow" "Building boost for Universal Windows.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=release link=static threading=multi runtime-link=shared windows-api=desktop --stagedir=winrt --with-thread --with-date_time
    Write-Host -foregroundcolor "Yellow" "Building boost for Win32 unit tests.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=release link=static threading=multi runtime-link=shared --stagedir=win --with-program_options --with-filesystem --with-thread --with-date_time
}

# shader compiler
Write-Host -foregroundcolor "Yellow" "Building Okonomiyaki Shader Compiler.."

$buildCmd = "C:\Program Files (x86)\MSBuild\14.0\bin\msbuild.exe"
$sln = "$root\src\external\okonomi-shadercompiler\build\ShaderCompiler.sln"

$buildArgs = @(
        $sln,
        "/l:C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll",
        "/p:Configuration=$conf",
        "/p:Platform=x64",
        "/verbosity:minimal")

& $buildCmd $buildArgs

# vulkan
# https://blog.jourdant.me/3-ways-to-download-files-with-powershell/
Write-Host -foregroundcolor "Yellow" "Downloading Vulkan SDK.."

$url = "https://s3-ap-northeast-1.amazonaws.com/kittikun-podcast/VulkanSDK.zip"
$output = "$root\src\external"
(New-Object System.Net.WebClient).DownloadFile($url, $output)

# http://www.howtogeek.com/tips/how-to-extract-zip-files-using-powershell/
Write-Host -foregroundcolor "Yellow" "Extracting archive.."

$shell = new-object -com shell.application
Expand-Archive "VulkanSDK.zip"