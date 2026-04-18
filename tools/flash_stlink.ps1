param(
    [Parameter(Mandatory = $true)]
    [string]$WorkspaceFolder
)

$ErrorActionPreference = 'Stop'

$bundleRoot = 'C:/Users/Toyio/AppData/Local/stm32cube/bundles/programmer'
if (-not (Test-Path $bundleRoot)) {
    throw "Programmer bundle path not found: $bundleRoot"
}

$programmerDir = Get-ChildItem -Path $bundleRoot -Directory |
    Sort-Object Name -Descending |
    Select-Object -First 1

if (-not $programmerDir) {
    throw 'No STM32 programmer version folder found.'
}

$cli = Join-Path $programmerDir.FullName 'bin/STM32_Programmer_CLI.exe'
if (-not (Test-Path $cli)) {
    throw "STM32_Programmer_CLI.exe not found: $cli"
}

$hexDir = Join-Path $WorkspaceFolder 'build/Debug'
if (-not (Test-Path $hexDir)) {
    throw "Build output folder not found: $hexDir"
}

$hexFile = Get-ChildItem -Path $hexDir -Filter *.hex -File |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1

if (-not $hexFile) {
    throw "No .hex file found in $hexDir"
}

Write-Host "Using programmer: $cli"
Write-Host "Using firmware: $($hexFile.FullName)"

function Release-StLinkOccupancy {
    $processNames = @('ST-LINK_gdbserver', 'STLinkServer', 'arm-none-eabi-gdb')
    foreach ($name in $processNames) {
        $procs = Get-Process -Name $name -ErrorAction SilentlyContinue
        if ($procs) {
            Write-Host "Stopping process occupying ST-Link: $name"
            $procs | Stop-Process -Force -ErrorAction SilentlyContinue
        }
    }
}

$flashArgs = @(
    '-c', 'port=SWD', 'mode=UR', 'reset=HWrst', 'freq=1000',
    '-w', $hexFile.FullName,
    '-v',
    '-rst'
)

$flashArgsLowFreq = @(
    '-c', 'port=SWD', 'mode=UR', 'reset=HWrst', 'freq=400',
    '-w', $hexFile.FullName,
    '-v',
    '-rst'
)

$maxAttempts = 3
for ($attempt = 1; $attempt -le $maxAttempts; $attempt++) {
    Write-Host "Flash attempt $attempt/$maxAttempts"
    if ($attempt -eq $maxAttempts) {
        Write-Host 'Using low-frequency fallback (400kHz).'
        & $cli @flashArgsLowFreq
    }
    else {
        & $cli @flashArgs
    }
    if ($LASTEXITCODE -eq 0) {
        Write-Host 'Flash completed successfully.'
        exit 0
    }

    if ($attempt -lt $maxAttempts) {
        Write-Host 'Flash failed, trying to release ST-Link occupancy before retry...'
        Release-StLinkOccupancy
    }
}

throw 'Flash failed after retries. Check cable, power, and whether debug session is occupying ST-Link.'
