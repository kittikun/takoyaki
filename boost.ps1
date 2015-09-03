#http://blogs.technet.com/b/heyscriptingguy/archive/2015/03/11/use-powershell-to-extract-zipped-files.aspx
Add-Type -assembly "system.io.compression.filesystem"

pwd
Invoke-WebRequest "http://bit.ly/1JPHkL3" -OutFile "boost_1_59_0.zip"
[io.compression.zipfile]::ExtractToDirectory(".\boost_1_59_0.zip", ".")
Set-Location ".\boost_1_59_0"
& ".\bootstrap.bat"
& .\b2 address-model=64 toolset=msvc-14.0 windows-api=desktop