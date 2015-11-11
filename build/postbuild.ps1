param (
    [string]$src = $(throw "src is required."),
    [string]$dst = $(throw "dst is required.")
)

Write-Host "Running post-build..."
Write-Host "Copying files from $src to $dst"
Copy-Item $src $dst -recurse -force