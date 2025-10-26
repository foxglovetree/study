
.Prebuild:

    cmake --preset=default

.Build
    cmake --build build

.Debug with VSCode.

..open vscode within Powershell Devloper for VS 2022:
..lauch.json:
    ====
    {
        "configurations": [
            {
                "name": "Launch Ogre Project (via CMake Tools)",
                "type": "cppvsdbg",
                "request": "launch",
                "program": "${command:cmake.launchTargetPath}",
                "args": [],
                "stopAtEntry": false,
                "cwd": "${workspaceFolder}",
                "environment": [],
                "console": "internalConsole"
            }
        ]
    }        
    ====




