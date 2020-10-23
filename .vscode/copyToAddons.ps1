Write-Output "Copying JHP_DevTools.apx to Archicad 23 Add-On location..."
$BuildLocation = "${PSScriptRoot}\..\Build\x64\Release\JHP_DevTools.apx"
$Destination = "C:\Program Files\GRAPHISOFT\ARCHICAD 23\Add-Ons\JHP\JHP_DevTools.apx"

Copy-Item -path $BuildLocation -Destination $Destination -Force

