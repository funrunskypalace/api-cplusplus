for /f "tokens=* usebackq" %%f IN (`"%programfiles(x86)%\Microsoft Visual Studio\Installer\vswhere" -latest -property installationPath`) do (
    set VSLATESTPATH=%%f
)
rem for /f "usebackq delims=#" %a in (OPS) do %a\Common7\Tools\VsDevCmd.bat
call "%VSLATESTPATH%\Common7\Tools\VsDevCmd.bat"
