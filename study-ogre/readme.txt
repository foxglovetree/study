.prepare soft tools
.. windows os(win11 verified at present)
.. install MS VS tools 
    make sure the components for c++ development are selected.
    including vcpkg,
    including cmake,
    including ninja(if cmake is selected)

.set env vars:
..set ENV var:VCPKG_HOME=<folder path to vcpkg command>
..set PATH for ninja command.

.build from command line:
..open Developer PowerShell for VS 2022.
..cd this folder(the folder containing this readme file).
    PS > cd <this>
..Generate makefiles:
    PS > cmake --preset=default
...Build :
    PS > cd build
    PS > ninja

.dev with vscode
..install Addons:

..start vscode from Developer PowerShell for VS 2022.
..Add configurion in lauch.json:
    
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