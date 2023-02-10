@echo off echo *** Enter code signing certificate password for %1
rem set /P password=Password: 
SignTool sign /f "%PFX_FILE%" /p "%password%" /t http://timestamp.digicert.com /fd SHA256 %1
rem set password=