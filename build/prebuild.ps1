# prebuild setup for Appveyor CI

param(
[string]$conf = $(throw "conf is required.")
)

$root = "$PSScriptRoot\..\"

# boost
Write-Host -foregroundcolor "Yellow" "Bootstrapping boost.."
Set-Location "$root\external\boost"
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

# vulkan
# https://blog.jourdant.me/3-ways-to-download-files-with-powershell/
Write-Host -foregroundcolor "Yellow" "Downloading Vulkan SDK.."

$url = "https://s3-ap-northeast-1.amazonaws.com/kittikun-podcast/VulkanSDK.zip"
$zipPath = "$root\external\VulkanSDK.zip"
(New-Object System.Net.WebClient).DownloadFile($url, $zipPath)

# http://stackoverflow.com/questions/27768303/how-to-unzip-a-file-in-powershell
Write-Host -foregroundcolor "Yellow" "Extracting archive.."

Add-Type -assembly “system.io.compression.filesystem”

$zipOut = "$root\external"
[io.compression.zipfile]::ExtractToDirectory($zipPath, $zipOut)