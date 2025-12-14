#!/usr/bin/env pwsh
param(
    [string]$BuildDir = "Code/build",
    [string]$Generator = "MinGW Makefiles"
)

Write-Host "Configuring project..."
cmake -S Code -B $BuildDir -G "$Generator" -DCMAKE_BUILD_TYPE=Release
if($LASTEXITCODE -ne 0){ throw "CMake configure failed" }

Write-Host "Building..."
cmake --build $BuildDir --config Release -- -j 4
if($LASTEXITCODE -ne 0){ throw "Build failed" }

Write-Host "Packaging..."
cmake --build $BuildDir --config Release --target package
if($LASTEXITCODE -ne 0){ throw "Package failed" }

$out = Get-ChildItem -Path (Join-Path $BuildDir "*") -Filter "supersearch-*.zip" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
if($null -eq $out){ Write-Host "No package found"; exit 1 }

New-Item -ItemType Directory -Path dist -Force | Out-Null
Copy-Item $out.FullName -Destination dist -Force
Write-Host "Package created: dist\$(Split-Path $out.FullName -Leaf)"
