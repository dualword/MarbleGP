@echo off
@cd ..\release

echo *** Remove old temp data
del MarbleGP_Android.apk
rmdir /s /q MarbleGP_Android
rmdir /s /q compiled_resources
mkdir compiled_resources
echo *** Copy compile APK (release mode)
copy ..\MarbleGP_Android\MarbleGP_Android\MarbleGP_Android.Packaging\ARM64\Release\MarbleGP_Android.apk .
echo *** Extract APK
7z.exe x MarbleGP_Android.apk -oMarbleGP_Android
echo *** Copy Manifest
copy ..\MarbleGP_Android\MarbleGP_Android\MarbleGP_Android.Packaging\AndroidManifest.xml .
echo *** Compile Resources
%ANDROID_AAPT2% compile MarbleGP_Android\res\drawable\marblegp_logo.png -o compiled_resources\
mkdir res\values
copy ..\..\MarbleGP_Android\MarbleGP_Android\MarbleGP_Android.Packaging\res\values\*.* MarbleGP_Android\res\values
%ANDROID_AAPT2% compile MarbleGP_Android\res\values\strings.xml -o compiled_resources
cd MarbleGP_Android
echo *** Create temp.apk
%ANDROID_AAPT2% link --proto-format -o temp.apk -I "C:\Microsoft\AndroidSDK\25\platforms\android-30\android.jar" --manifest AndroidManifest.xml -R ..\compiled_resources\drawable_marblegp_logo.png.flat -R ..\compiled_resources\values_strings.arsc.flat --java gen --auto-add-overlay
echo *** Extract temp.apk
mkdir temp
7z.exe x temp.apk -otemp
echo *** Create temp.zip
mkdir temp\manifest
move temp\AndroidManifest.xml temp\manifest
mkdir temp\lib
mkdir temp\lib\arm64-v8a
copy lib\arm64-v8a\libMarbleGP_Android.so temp\lib\arm64-v8a
mkdir temp\assets
xcopy assets\*.* temp\assets /s /e
7z a temp.zip .\temp\*
echo *** Create Android App Bundle
java -jar %ANDROID_BUNDLETOOL% build-bundle --modules=temp.zip --output=marblegp.aab

rem java -jar %ANDROID_BUNDLETOOL% build-apks --bundle=marblegp.aab --output=apk\universal.apks --mode=universal
rem %ANDROID_ADB% install universal.apk

cd ..

cd ..\script
pause