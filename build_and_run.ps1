Write-Host "Building RaceCar project..." -ForegroundColor Green
Set-Location build

# Clean the build first
Write-Host "Cleaning previous build..." -ForegroundColor Yellow
cmake --build . --config Release --target clean

# Build the project
cmake --build . --config Release

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful! Running RaceCar..." -ForegroundColor Green
    Set-Location Release
    & .\RaceCar.exe
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to continue"
}

Set-Location ..
Set-Location ..