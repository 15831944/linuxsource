@echo off

rem Modified by Gu Zhnehua 2007/06/26

echo      ==============================================
echo      =      make win32 release license tools      =
echo      ==============================================

del /F /Q Release\*.*
nmake /NOLOGO /A /B /f LicenseKey.mak CFG="LicenseKey - Win32 Release" /x nmakeError.log > ..\..\..\..\10-common\version\compileInfo\licensekey.txt
if not exist "..\..\..\..\10-common\version\release\win32\tools/" mkdir "..\..\..\..\10-common\version\release\win32\tools" 
if not exist "..\..\..\..\10-common\version\release\win32\tools\LicenseKey/" mkdir "..\..\..\..\10-common\version\release\win32\tools\LicenseKey" 

copy release\LicenseKey.exe ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\LicenseKey.exe

rem ������ҪINI�����ļ�
rem copy ..\licenseError.ini ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\licenseError.ini
rem copy ..\USBKey.ini ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\USBKey.ini

rem ����Ҫ��3��DLL

if not exist "..\..\..\..\10-common\lib\release\win32\usbkey.dll" (
	copy .\usbkey.dll ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\usbkey.dll
) else (
	copy ..\..\..\..\10-common\lib\release\win32\usbkey.dll ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\usbkey.dll
)


copy ..\..\..\..\b0-thirdparty\haikey\dll\HKAPI.dll ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\HKAPI.dll
copy ..\..\..\..\b0-thirdparty\haikey\dll\HKCommand.dll ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\HKCommand.dll

rem ��copy˵���ļ�
rem copy ..\ʹ��˵��.txt ..\..\..\..\10-common\version\release\win32\tools\LicenseKey\ʹ��˵��.txt

cd ..

@echo on
