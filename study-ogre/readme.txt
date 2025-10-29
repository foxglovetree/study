.prepare soft tools
.. windows os(win11 verified at present)
.. install MS VS tools 
    make sure the components for c++ development are selected.
.. install vcpkg
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat

.. install cmake.

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
    PS > cmake --build build

.Configure Plugin
..cd C:/Users/<your-account-name>/Documents/StudyOgre/
..copy <project-home>/cfg/plugins.cfg ./
..copy <soft-install-dir>/vcpkg/packages/ogre_x64-windows/debug/plugins/ogre/*.dll ./

..copy dll dependencies for Plugin_DotScene_d.dll
...cd <project>/build/Debug
...copy <soft>/vcpkg/packages/pugixml_x64-windows/debug/bin/pugixml.dll ./
...copy <soft/vcpkg/packages/ogre_x64-windows/debug/bin/OgrePaing_d.dll ./
...copy <soft/vcpkg/packages/ogre_x64-windows/debug/bin/OgreTerrain_d.dll ./

.Configure Resource
..copy cfg/resources.cfg C:/Users/<your-account-name>/Documents/StudyOgre/


.resources.cfg

.dev with vscode
..install Addons:

..start vscode from Developer PowerShell for VS 2022.
..RUN/Add Configuration/C++(Windows)
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
.Dependency Tree:
..Plugin_DotScene.dll(place this file in PluginFolder)
...pugixml.dll(place this file in PATH)
...OgreTerrain.dll(place this file in PATH)
...OgrePaing.dll (place this file in PATH)
..
.Configuration files:
..plugins.cfg(For windows you need copy to %PATH% or your Docuemnt/StudyOgre folder or other valid folder).

..resources.cfg(same as plugins.cfg)

.TroubleShooting:
