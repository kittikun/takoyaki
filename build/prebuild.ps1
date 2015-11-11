param(
[string]$conf = $(throw "conf is required."),
)

$root = "$PSScriptRoot\..\"

Set-Location "$root\src\external\boost"

Write-Host -foregroundcolor "Yellow" "Bootstrapping boost.."
.\bootstrap.bat

Write-Host -foregroundcolor "Yellow" "Generating boost headers.."
.\b2 headers -d0

if ($conf -eq "Debug") {
    Write-Host -foregroundcolor "Yellow" "Building boost for WinRT app.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=debug link=static threading=multi runtime-link=shared windows-api=desktop --stagedir=winrt --with-log
    Write-Host -foregroundcolor "Yellow" "Building boost for Win32 unit tests.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=debug link=static threading=multi runtime-link=shared --stagedir=win --with-log --with-program_options --with-filesystem
} elseif ($conf -eq "Release") {
    Write-Host -foregroundcolor "Yellow" "Building boost for WinRT app.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=release link=static threading=multi runtime-link=shared windows-api=desktop --stagedir=winrt --with-thread --with-date_time
    Write-Host -foregroundcolor "Yellow" "Building boost for Win32 unit tests.."
    ./b2 -d0 address-model=64 toolset=msvc-14.0 variant=release link=static threading=multi runtime-link=shared --stagedir=win --with-program_options --with-filesystem --with-thread --with-date_time
}
