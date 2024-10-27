@echo off
echo !!! Make Sure to run updateversion.bat and rebuild the Android version before preparing the release
echo *** Enter code signing certificate password
set /P password=Password: 
rd /S /Q temp_android
rd /S /Q temp_es
rd /S /Q temp_gl
python.exe createDataFiles.py
del ..\..\MarbleGP\MarbleGP_Windows.exe
copy ..\MarbleGP_Windows\x64\Release\MarbleGP_Windows.exe ..\..\MarbleGP
echo *** Signing MarbleGP_Windows.exe
SignTool.exe sign /f "%PFX_FILE%" /p "%password%" /t http://timestamp.digicert.com /fd SHA256 ..\..\MarbleGP\MarbleGP_Windows.exe
7z.exe a -r -tzip ..\release\MarbleGP.zip ..\..\MarbleGP\*.*
del ..\..\MarbleGP_Steam\MarbleGP_Steam.exe
copy ..\MarbleGP_Steam\x64\Release\MarbleGP_Steam.exe ..\..\MarbleGP_Steam
echo *** Signing MarbleGP_Steam.exe
SignTool.exe sign /f "%PFX_FILE%" /p "%password%" /t http://timestamp.digicert.com /fd SHA256 ..\..\MarbleGP_Steam\MarbleGP_Steam.exe
7z.exe a -r -tzip ..\release\MarbleGP_Steam.zip ..\..\MarbleGP_Steam\*.*
copy ..\MarbleGP_Android\app\release\app-release.aab ..\release\marblegp.aab^
setup.iss
release_backup.bat
set password=
getVersionInfo.bat
pause