#!/bin/bash
# Program: Setup.ps1
# Author: Colton Franklin
# Usage: ./setup.ps1
# Description: Configures a new acapi template project.


function checkMsbuild ()
{
    try { msbuild | Out-Null }
    catch [System.Management.Automation.CommandNotFoundException] 
    {
        Write-Output "ERROR: Msbuild is not available, please ensure msbuild.exe is installed and added to the PATH Environment Variable."
        Write-Output "msbuild.exe is included in Visual Studio installations, but also can be downloaded seperately if using VSCode."
        Write-Output "The needed package if using seperately is 'Desktop development with c++' (Windows 10 SDK required)"
        Write-Output "With either case, the location of msbuild.exe needs to be added to the PC's 'Environment Variables'"
        exit
    }
}

function checkGit () 
{
    try { git | Out-Null }
    catch [System.Management.Automation.CommandNotFoundException] 
    {
        Write-Output "ERROR: Git is not installed. Please install git to download the JHP libraries."
        exit
    }
}

if ((Test-Path "${PSScriptRoot}\project_name.vcxproj") -eq 1)
{
    $newProjectName = Read-Host -Prompt "Enter the new project name to rename the template project or 'exit' to quit:"
}
else 
{
    Write-Output "ERROR: Template .vcxproj doesn't exist in this directory, ensure the full template is copied and retry."
    exit
}

if ($newProjectName -eq "exit") { exit }
$RegEx = '[^-\w\.]'
if ($newProjectName -eq "" -or $newProjectName -match $RegEx)
{
    Write-Output "ERROR: Invalid file name, exiting..."
}

if ((Test-Path "C:\Program Files\GRAPHISOFT\API Development Kit 23.3006\Support") -eq 1)
{
    $supportPath23 = "C:\Program Files\GRAPHISOFT\API Development Kit 23.3006\Support"
    Write-Output "Archicad 23 Support path found: ${supportPath23}"
}
else 
{
    Write-Output "Support path not found, please ensure API Dev Kit 23.3006 is installed to C:\Program Files\GRAPHISOFT\ ."
    exit
}

if ((Test-Path "C:\Program Files\GRAPHISOFT\API Development Kit 22.3004\Support") -eq 1)
{
    $supportPath22 = "C:\Program Files\GRAPHISOFT\API Development Kit 23.3006\Support"
    Write-Output "Archicad 22 Support path found: ${supportPath22}"
}
else 
{
    Write-Output "Archicad 22 Support path not found, please ensure API Dev Kit 22.3004 is installed to C:\Program Files\GRAPHISOFT\ ."
    exit
}

#checkMsbuild only need to check this if planning on using vscode...
checkGit

$files =    ".gitignore",
            "project_name.vcxproj",
            "Src/project_name.cpp",
            "Src/project_nameResIDs.hpp",
            "RINT/project_name.grc",
            "RFIX.win/project_name.rc2",
            "RFIX/project_nameFix.grc",
            ".vscode/c_cpp_properties.json",
            ".vscode/settings.json",
            ".vscode/tasks.json"

foreach ($file in $files)
{
    if ((Test-Path $file) -eq 1)
    {
        
        ((Get-Content -Path $file -Raw).Replace('project_name', $newProjectName)) | Set-Content -Path $file
        ((Get-Content -Path $file -Raw).Replace('PROJECT_NAME', $newProjectName.ToUpper())) | Set-Content -Path $file
    } 
}




Write-Output "Opening Archicad Developer page now..."
Invoke-Expression "start https://archicadapi.graphisoft.com/profile/developer"
$developerID = Read-Host -Prompt "Please copy 9-digit ID and enter"

if ($developerID -match '[0-9]' -and $developerID.Length -eq 9) 
{
    ((Get-Content -Path "RFIX/project_nameFix.grc" -Raw) -replace 'xxxxxxxxx', $developerID) | Set-Content -Path "RFIX/project_nameFix.grc"
}
else 
{
    Write-Output "ERROR: Developer ID is invalid, skipping replace. Please replace 'xxxxxxxxx' in 'RFIX/project_nameFix.grc' with a valid Developer ID before building..."
}

Write-Output "Opening AddOnAdmin.exe to retrieve Add-On ID..."
& "${supportPath22}\Tools\Win\AddOn Management\AddOnAdmin.exe"
$addOnID = Read-Host -Prompt "Please create and enter 10-digit Add-On ID"


if ($addOnID -match '[0-9]' -and $addOnID.Length -eq 10) 
{
    ((Get-Content -Path "RFIX/project_nameFix.grc" -Raw) -replace 'yyyyyyyyyy', $addOnID) | Set-Content -Path "RFIX/project_nameFix.grc"
}
else 
{
    Write-Output "ERROR: Add-On ID is invalid, skipping replace. Please replace 'yyyyyyyyyy' in 'RFIX/project_nameFix.grc' with a valid Add-On ID before building..."
}

Rename-Item -Path "project_name.vcxproj"                -NewName "${newProjectName}.vcxproj"
Rename-Item -Path "Src/project_name.cpp"                -NewName "${newProjectName}.cpp"
Rename-Item -Path "Src/project_nameResIDs.hpp"          -NewName "${newProjectName}ResIDs.hpp"
Rename-Item -Path "RINT/project_name.grc"               -NewName "${newProjectName}.grc"
Rename-Item -Path "RFIX.win/project_name.rc2"           -NewName "${newProjectName}.rc2"
Rename-Item -Path "RFIX/project_nameFix.grc"            -NewName "${newProjectName}Fix.grc"

Write-Output "Successful setup complete!"