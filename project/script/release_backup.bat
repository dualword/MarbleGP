if exist ..\release\MarbleGP.zip (
  copy ..\release\MarbleGP.zip %RELEASE_BACKUP%\MarbleGP_259.zip
) else (
  echo MarbleGP.zip not found.
)

if exist ..\release\MarbleGP_Setup.exe (
  copy ..\release\MarbleGP_Setup.exe %RELEASE_BACKUP%\MarbleGP_Setup_259.exe
) else (
  echo MarbleGP_Setup.exe not found.
)

if exist ..\release\MarbleGP_Steam.zip (
  copy ..\release\MarbleGP_Setup.exe %RELEASE_BACKUP%\MarbleGP_Setup_259.exe
) else (
  echo MarbleGP_Steam.zip not found.
)

if exist ..\MarbleGP_Android\app\release\app-release.apk (
  copy ..\MarbleGP_Android\app\release\app-release.apk %RELEASE_BACKUP%\marblegp_259.apk
) else (
  echo app-release.apk not found.
)
