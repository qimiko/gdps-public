# assumes game is in $ROOT/windows/game/ and that gd was built to $ROOT/build
param([switch] $includeDebug = $false) 

$gameFolder = "$PSScriptRoot\game\"
$baseDir = "$PSScriptRoot\..\"

Copy-Item "$PSScriptRoot\steam_appid.txt" "$gameFolder"

# setup geode path
Copy-Item "$env:GEODE_SDK\bin\nightly\Geode.dll" "$gameFolder"
Copy-Item "$env:GEODE_SDK\bin\nightly\GeodeUpdater.exe" "$gameFolder"

if ($includeDebug) {
	Copy-Item "$env:GEODE_SDK\bin\nightly\Geode.pdb" "$gameFolder"
}

if (Test-Path "$gameFolder\Resources\geode.loader") {
	Remove-Item "$gameFolder\Resources\geode.loader" -Recurse
}

Copy-Item "$baseDir\android\app\src\main\assets\geode.loader" "$gameFolder\Resources" -Recurse

# setup mod
Copy-Item "$baseDir\build\aurav2.dll" "$gameFolder"

if ($includeDebug) {
	Copy-Item "$baseDir\build\aurav2.pdb" "$gameFolder"
}

if (Test-Path "$gameFolder\Resources\zmx.aura") {
	Remove-Item "$gameFolder\Resources\zmx.aura" -Recurse
}
New-Item "$gameFolder\Resources\zmx.aura" -ItemType "Directory"
Copy-Item "$baseDir\build\resources\*" "$gameFolder\Resources\zmx.aura"

iscc "$PSScriptRoot\gdps.iss"

$zipOut = "$PSScriptRoot\Output\gdps.zip"
Compress-Archive "$gameFolder\*" $zipOut -Force
Write-Output "Archive written to $zipOut"
