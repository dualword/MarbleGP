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
copy ..\MarbleGP_Android\app\release\app-release.apk ..\release\marblegp.apk
setup.iss
release_backup.bat
set password=
getVersionInfo.bat
pause