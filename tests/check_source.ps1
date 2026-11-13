$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

function Assert-True([bool]$condition, [string]$message) {
    if (-not $condition) {
        throw "FAIL: $message"
    }
    Write-Output "PASS: $message"
}

$config = Get-Content (Join-Path $root "include\config.h") -Raw
$pinValues = [regex]::Matches($config, 'constexpr uint8_t \w+_PIN = (\d+);') |
    ForEach-Object { [int]$_.Groups[1].Value }
Assert-True ($pinValues.Count -eq 12) "all 12 assigned GPIOs are declared"
Assert-True (($pinValues | Select-Object -Unique).Count -eq $pinValues.Count) `
    "assigned GPIOs do not conflict"
Assert-True ($config -match 'RELAY_SIMULATION_MODE = true') "relay simulation defaults on"
Assert-True ($config -match 'BUZZER_SIMULATION_MODE = true') "buzzer simulation defaults on"
Assert-True ($config -match 'SENSOR_INTERVAL_MS = 2000') "sensor poll interval is two seconds"

$sourceFiles = Get-ChildItem (Join-Path $root "src") -Filter "*.cpp"
$allSource = ($sourceFiles | ForEach-Object { Get-Content $_.FullName -Raw }) -join "`n"
$relayAndBuzzer = (Get-Content (Join-Path $root "src\relay_manager.cpp") -Raw) +
    (Get-Content (Join-Path $root "src\buzzer_manager.cpp") -Raw)
Assert-True ($relayAndBuzzer -notmatch '\b(pinMode|digitalWrite|tone)\s*\(') `
    "relay and buzzer managers do not drive physical outputs"
Assert-True ($allSource -match 'INPUT_PULLUP') "buttons use internal pull-ups"
Assert-True ($allSource -match 'BUTTON_DEBOUNCE_MS') "button debounce interval is applied"
Assert-True ($allSource -match 'isfinite') "sensor readings reject invalid numeric values"
Assert-True ($allSource -match 'HTTP_GET' -and $allSource -notmatch 'HTTP_POST') `
    "dashboard registers read-only GET routes only"
Assert-True ((Get-Content (Join-Path $root ".gitignore") -Raw) -match 'include/secrets\.h') `
    "local Wi-Fi credentials are ignored by git"

Write-Output "Source contract checks passed. These checks do not replace firmware compilation or hardware tests."
