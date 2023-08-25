@ECHO OFF &SETLOCAL

Title LDGLite Windows auto build script

rem This script uses Qt to configure and build LDGLite for Windows.
rem The primary purpose is to automatically build both the 32bit and 64bit
rem LDGLite distributions and package the build contents (exe, doc and
rem resources ) as LPub3D 3rd Party components.
rem --
rem  Trevor SANDY <trevor.sandy@gmail.com>
rem  Last Update: August 29, 2023
rem  Copyright (c) 2017 - 2023 by Trevor SANDY
rem --
rem This script is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CALL :ELAPSED_BUILD_TIME Start

SET PWD=%CD%

IF "%LP3D_QTVERSION%" == "" SET "LP3D_QTVERSION=5.15.2"
IF "%LP3D_VSVERSION%" == "" SET "LP3D_VSVERSION=2019"

IF "%GITHUB%" EQU "True" (
  SET "BUILD_WORKER=True"
  SET "BUILD_WORKER_JOB=%GITHUB_JOB%"
  SET "BUILD_WORKER_REF=%GITHUB_REF%"
  SET "BUILD_WORKER_OS=%RUNNER_OS%"
  SET "BUILD_WORKER_REPO=%GITHUB_REPOSITORY%"
  SET "BUILD_WORKER_HOST=GITHUB CONTINUOUS INTEGRATION SERVICE"
  SET "BUILD_WORKER_IMAGE=Visual Studio %LP3D_VSVERSION%"
  SET "BUILD_WORKSPACE=%GITHUB_WORKSPACE%"
)

IF "%LP3D_CONDA_BUILD%" EQU "True" (
  SET "BUILD_WORKER=True"
  SET "BUILD_WORKER_JOB=%LP3D_CONDA_JOB%"
  SET "BUILD_WORKER_OS=%LP3D_CONDA_RUNNER_OS%"
  SET "BUILD_WORKER_REPO=%LP3D_CONDA_REPOSITORY%"
  SET "BUILD_WORKER_IMAGE=%CMAKE_GENERATOR%"
  SET "BUILD_WORKER_HOST=CONDA BUILD INTEGRATION SERVICE"
  SET "BUILD_WORKSPACE=%LP3D_CONDA_WORKSPACE%"
)

IF "%CONFIGURATION%" == "" SET CONFIGURATION=release
IF "%LP3D_3RD_DIST_DIR%" == "" SET LP3D_3RD_DIST_DIR=lpub3d_windows_3rdparty

IF "%BUILD_WORKER%" EQU "True" (
  IF "%LP3D_DIST_DIR_PATH%" == "" (
    ECHO.
    ECHO  -ERROR - Distribution directory path not defined.
    GOTO :ERROR_END
  )
  PUSHD %LP3D_BUILD_BASE%
  IF NOT EXIST "%LP3D_3RD_DIST_DIR%" (
    IF EXIST "%LP3D_DIST_DIR_PATH%" (
      MKLINK /d %LP3D_3RD_DIST_DIR% %LP3D_DIST_DIR_PATH% 2>&1
    ) ELSE (
      ECHO.
      ECHO - ERROR - %LP3D_DIST_DIR_PATH% path not defined
      GOTO :ERROR_END
    )
  )
  POPD
  SET ABS_WD=%BUILD_WORKSPACE%
  rem DIST_DIR must be relative to App folder in LDGLite repo
  SET DIST_DIR=..\..\%LP3D_3RD_DIST_DIR%
  SET LDRAW_DOWNLOAD_DIR=%USERPROFILE%
  SET LDRAW_DIR=%USERPROFILE%\LDraw
  IF "%LP3D_QT32_MSVC%" == "" (
    SET LP3D_QT32_MSVC=%LP3D_BUILD_BASE%\Qt\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%\bin
  )
  IF "%LP3D_QT64_MSVC%" == "" (
    SET LP3D_QT64_MSVC=%LP3D_BUILD_BASE%\Qt\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%_64\bin
  )
)

IF "%APPVEYOR%" EQU "True" (
  IF "%LP3D_DIST_DIR_PATH%" == "" (
    ECHO.
    ECHO  -ERROR: Distribution directory path not defined.
    GOTO :ERROR_END
  )
  SET APPVEYOR_BUILD_WORKER_IMAGE=Visual Studio %LP3D_VSVERSION%
  rem DIST_DIR must be relative to App folder in LDGLite repo
  SET DIST_DIR=..\..\%LP3D_3RD_DIST_DIR%
  SET LDRAW_DOWNLOAD_DIR=%APPVEYOR_BUILD_FOLDER%
  SET LDRAW_DIR=%APPVEYOR_BUILD_FOLDER%\LDraw
  IF "%LP3D_QT32_MSVC%" == "" (
    SET LP3D_QT32_MSVC=C:\Qt\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%\bin
  )
  IF "%LP3D_QT64_MSVC%" == "" (
    SET LP3D_QT64_MSVC=C:\Qt\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%_64\bin
  )
)

IF "%BUILD_WORKER%" NEQ "True" (
  IF "%APPVEYOR%" NEQ "True" (
    rem DIST_DIR must be relative to App folder in LDGLite repo
    SET DIST_DIR=..\..\lpub3d_windows_3rdparty
    SET LDRAW_DOWNLOAD_DIR=%USERPROFILE%
    SET LDRAW_DIR=%USERPROFILE%\LDraw
    IF "%LP3D_QT32_MSVC%" == "" (
      SET LP3D_QT32_MSVC=C:\Qt\IDE\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%\bin
    )
    IF "%LP3D_QT64_MSVC%" == "" (
      SET LP3D_QT64_MSVC=C:\Qt\IDE\%LP3D_QTVERSION%\msvc%LP3D_VSVERSION%_64\bin
    )
  )
)

IF "%LP3D_CONDA_BUILD%" NEQ "True" (
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\%LP3D_VSVERSION%\Professional\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files ^(x86^)\Microsoft Visual Studio\%LP3D_VSVERSION%\Professional\VC\Auxiliary\Build
  )  
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\%LP3D_VSVERSION%\Community\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files ^(x86^)\Microsoft Visual Studio\%LP3D_VSVERSION%\Community\VC\Auxiliary\Build
  )
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\%LP3D_VSVERSION%\BuildTools\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files ^(x86^)\Microsoft Visual Studio\%LP3D_VSVERSION%\BuildTools\VC\Auxiliary\Build
  )
  IF EXIST "C:\Program Files (x86)\Microsoft Visual Studio\%LP3D_VSVERSION%\Enterprise\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files ^(x86^)\Microsoft Visual Studio\%LP3D_VSVERSION%\Enterprise\VC\Auxiliary\Build
  )
)
IF NOT EXIST "%LP3D_VCVARSALL_DIR%" (
  ECHO.
  ECHO  -ERROR - Microsoft Visual Studio C++ environment not defined.
  GOTO :ERROR_END
)

rem Visual C++ 2012 -vcvars_ver=11.0 version 11.0  _MSC_VER 1700
rem Visual C++ 2013 -vcvars_ver=12.0 version 12.0  _MSC_VER 1800
rem Visual C++ 2015 -vcvars_ver=14.0 version 14.0  _MSC_VER 1900
rem Visual C++ 2017 -vcvars_ver=14.1 version 15.9  _MSC_VER 1916
rem Visual C++ 2019 -vcvars_ver=14.2 version 16.11 _MSC_VER 1929
rem Visual C++ 2022 -vcvars_ver=14.2 version 17.3  _MSC_VER 1933
IF "%LP3D_MSC_VER%" == "" SET LP3D_MSC_VER=1900
IF "%LP3D_VCSDKVER%" == "" SET LP3D_VCSDKVER=8.1
IF "%LP3D_VCTOOLSET%" == "" SET LP3D_VCTOOLSET=v140
IF "%LP3D_VCVARSALL_VER%" == "" SET LP3D_VCVARSALL_VER=-vcvars_ver=14.0

IF "%LP3D_VALID_7ZIP%" =="" SET LP3D_VALID_7ZIP=0
IF "%LP3D_7ZIP_WIN64%" == "" SET "LP3D_7ZIP_WIN64=%ProgramFiles%\7-zip\7z.exe"

SET LP3D_SYS_DIR=%WINDIR%\System32
SET OfficialCONTENT=complete.zip

SET PACKAGE=LDGLite
SET VERSION=1.3.6

SET THIRD_INSTALL=unknown
SET INSTALL_32BIT=unknown
SET INSTALL_64BIT=unknown
SET PLATFORM_ARCH=unknown
SET LDCONFIG_FILE=unknown
SET CHECK=unknown

ECHO.
ECHO -Start %PACKAGE% %~nx0 with commandline args: [%*].

rem Verify 1st input flag options
IF NOT [%1]==[] (
  IF NOT "%1"=="x86" (
    IF NOT "%1"=="x86_64" (
      IF NOT "%1"=="-all" (
        IF NOT "%1"=="-help" GOTO :PLATFORM_ERROR
      )
    )
  )
)

rem Parse platform input flags
IF [%1]==[] (
  SET PLATFORM_ARCH=-all
  GOTO :SET_CONFIGURATION
)
IF /I "%1"=="x86" (
  SET PLATFORM_ARCH=%1
  GOTO :SET_CONFIGURATION
)
IF /I "%1"=="x86_64" (
  SET PLATFORM_ARCH=%1
  GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-all" (
  SET PLATFORM_ARCH=-all
  GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-help" (
  GOTO :USAGE
)
rem If we get here display invalid command message.
GOTO :COMMAND_ERROR

:SET_CONFIGURATION
rem Verify 2nd input flag options
IF NOT [%2]==[] (
  IF NOT "%2"=="-ins" (
    IF NOT "%2"=="-chk" GOTO :CONFIGURATION_ERROR
  )
)

rem Verify 3rd input flag options
IF NOT [%3]==[] (
  IF NOT "%3"=="-chk" GOTO :CONFIGURATION_ERROR
)

rem Set third party install as default behaviour
IF [%2]==[] (
  SET THIRD_INSTALL=1
  GOTO :BUILD
)

IF /I "%2"=="-ins" (
  SET THIRD_INSTALL=1
  GOTO :BUILD
)

rem Set build check flag
IF /I "%2"=="-chk" (
  SET CHECK=1
  GOTO :BUILD
)

:BUILD
rem Display build settings
ECHO.
IF "%BUILD_WORKER%" EQU "True" (
  ECHO   BUILD_HOST.............[%BUILD_WORKER_HOST%]
  ECHO   BUILD_WORKER_IMAGE.....[%BUILD_WORKER_IMAGE%]
  ECHO   BUILD_WORKER_JOB.......[%BUILD_WORKER_JOB%]
  ECHO   BUILD_WORKER_REF.......[%BUILD_WORKER_REF%]
  ECHO   BUILD_WORKER_OS....... [%BUILD_WORKER_OS%]
  ECHO   PROJECT REPOSITORY.....[%BUILD_WORKER_REPO%]
)
IF "%APPVEYOR%" EQU "True" (
  ECHO   BUILD_HOST.............[APPVEYOR CONTINUOUS INTEGRATION SERVICE]
  ECHO   BUILD_ID...............[%APPVEYOR_BUILD_ID%]
  ECHO   BUILD_BRANCH...........[%APPVEYOR_REPO_BRANCH%]
  ECHO   PROJECT_NAME...........[%APPVEYOR_PROJECT_NAME%]
  ECHO   REPOSITORY_NAME........[%APPVEYOR_REPO_NAME%]
  ECHO   REPO_PROVIDER..........[%APPVEYOR_REPO_PROVIDER%]
)
ECHO   PACKAGE................[%PACKAGE%]
ECHO   VERSION................[%VERSION%]
ECHO   WORKING_DIRECTORY......[%PWD%]
ECHO   DIST_DIRECTORY.........[%DIST_DIR:/=\%]
ECHO   LDRAW_DIRECTORY........[%LDRAW_DIR%]
ECHO.  LDRAW_DOWNLOAD_DIR.....[%LDRAW_DOWNLOAD_DIR%]

rem Perform build check
IF /I "%3"=="-chk" (
  SET CHECK=1
)

rem Check if build all platforms
IF /I "%PLATFORM_ARCH%"=="-all" (
  GOTO :BUILD_ALL
)

ECHO.
ECHO -Building %PLATFORM_ARCH% platform, %CONFIGURATION% configuration...
rem If build Win32, set to vs2017 for WinXP compat
CALL :CONFIGURE_VCTOOLS %PLATFORM_ARCH%
rem Configure buid arguments and set environment variables
CALL :CONFIGURE_BUILD_ENV
CD /D %PWD%
rem Display QMake version
ECHO.
qmake -v & ECHO.
rem Configure makefiles
qmake %LDGLITE_CONFIG_ARGS%
rem Perform build
nmake.exe %LDGLITE_MAKE_ARGS%
rem Check build status
IF %PLATFORM_ARCH%==x86 (SET EXE=app\32bit_%CONFIGURATION%\%PACKAGE%.exe)
IF %PLATFORM_ARCH%==x86_64 (SET EXE=app\64bit_%CONFIGURATION%\%PACKAGE%.exe)
IF NOT EXIST "%EXE%" (
  ECHO.
  ECHO "-ERROR - %EXE% was not successfully built."
  GOTO :ERROR_END
)
rem Perform build check if specified
IF %CHECK%==1 (CALL :CHECK_BUILD %PLATFORM_ARCH%)
rem Package 3rd party install content
IF %THIRD_INSTALL%==1 (CALL :3RD_PARTY_INSTALL)
GOTO :END

:BUILD_ALL
rem Launch qmake/make across all platform builds
ECHO.
ECHO -Build x86 and x86_64 platforms...
FOR %%P IN ( x86, x86_64 ) DO (
  ECHO.
  ECHO  -Building %%P platform, %CONFIGURATION% configuration...
  SET PLATFORM_ARCH=%%P
  CALL :CONFIGURE_VCTOOLS %%P
  CALL :CONFIGURE_BUILD_ENV
  CD /D %PWD%
  ECHO.
  qmake -v & ECHO.
  SETLOCAL ENABLEDELAYEDEXPANSION
  qmake !LDGLITE_CONFIG_ARGS! & nmake.exe !LDGLITE_MAKE_ARGS!
  IF %%P==x86 (SET EXE=app\32bit_%CONFIGURATION%\%PACKAGE%.exe)
  IF %%P==x86_64 (SET EXE=app\64bit_%CONFIGURATION%\%PACKAGE%.exe)
  IF NOT EXIST "!EXE!" (
    ECHO.
    ECHO "-ERROR - !EXE! was not successfully built."
    GOTO :ERROR_END
  )
  ENDLOCAL
  IF %CHECK%==1 (CALL :CHECK_BUILD %%P)
  IF %THIRD_INSTALL%==1 (CALL :3RD_PARTY_INSTALL)
  rem Reset PATH_PREPENDED
  SET PATH_PREPENDED=False
)
GOTO :END

:CONFIGURE_VCTOOLS
ECHO.
ECHO -Set MSBuild platform toolset...
IF %1==x86_64 (
  IF "%LP3D_CONDA_BUILD%" NEQ "True" (
    SET LP3D_MSC_VER=1929
    SET LP3D_VCSDKVER=10.0
    SET LP3D_VCTOOLSET=v142
    SET LP3D_VCVARSALL_VER=-vcvars_ver=14.2
  )
) ELSE (
  SET LP3D_VCSDKVER=8.1
  SET LP3D_VCTOOLSET=v140
  SET LP3D_VCVARSALL_VER=-vcvars_ver=14.0
)
ECHO.
ECHO   PLATFORM_ARCHITECTURE..[%1]
ECHO   MSVS_VERSION...........[%LP3D_VSVERSION%]
ECHO   MSVC_SDK_VERSION.......[%LP3D_VCSDKVER%]
ECHO   MSVC_TOOLSET...........[%LP3D_VCTOOLSET%]
IF "%LP3D_CONDA_BUILD%" NEQ "True" (
  IF %1==x86 (ECHO   LP3D_QT32_MSVC.........[%LP3D_QT32_MSVC%])
)
IF %1==x86_64 (ECHO   LP3D_QT64_MSVC.........[%LP3D_QT64_MSVC%])
ECHO   MSVC_VCVARSALL_VER.....[%LP3D_VCVARSALL_VER%]
ECHO   MSVC_VCVARSALL_DIR.....[%LP3D_VCVARSALL_DIR%]
EXIT /b

:CONFIGURE_BUILD_ENV
CD /D %PWD%
ECHO.
ECHO -Configure %PACKAGE% %PLATFORM_ARCH% build environment...
ECHO.
ECHO -Cleanup previous %PACKAGE% qmake config files - if any...
FOR /R %%I IN (
  ".qmake.stash"
  "Makefile*"
  "ldrawini\Makefile*"
  "mui\Makefile*"
  "app\Makefile*"
) DO DEL /S /Q "%%~I" >NUL 2>&1
ECHO.
SET LDGLITE_CONFIG_ARGS=CONFIG+=3RD_PARTY_INSTALL=%DIST_DIR% CONFIG+=%CONFIGURATION% CONFIG-=debug_and_release
ECHO   LDGLITE_CONFIG_ARGS.....[%LDGLITE_CONFIG_ARGS%]
rem /c flag suppresses the copyright
SET LDGLITE_MAKE_ARGS=/c /f Makefile
ECHO   LDGLITE_MAKE_ARGS.......[%LDGLITE_MAKE_ARGS%]
rem Set vcvars for AppVeyor or local build except conda-build
IF "%PATH_PREPENDED%" NEQ "True" (
  IF "%LP3D_CONDA_BUILD%" EQU "True" (
    SET "PATH=%PATH%"
  ) ELSE (
    SET "PATH=%LP3D_SYS_DIR%;%LP3D_WIN_GIT%"
    IF %PLATFORM_ARCH% EQU x86 (
      ECHO.
      SET WINDOWS_TARGET_PLATFORM_VERSION=%LP3D_VCSDKVER%
      IF EXIST "%LP3D_QT32_MSVC%\qtenv2.bat" (
        CALL "%LP3D_QT32_MSVC%\qtenv2.bat"
      ) ELSE (
        SETLOCAL ENABLEDELAYEDEXPANSION
        SET PATH=%LP3D_QT32_MSVC%;!PATH!
        ENDLOCAL
      )
      IF EXIST "%LP3D_VCVARSALL_DIR%\vcvars32.bat" (
        CALL "%LP3D_VCVARSALL_DIR%\vcvars32.bat" %LP3D_VCVARSALL_VER%
      ) ELSE (
        ECHO -ERROR: vcvars32.bat not found.
        GOTO :ERROR_END
      )
    ) ELSE (
      ECHO.
      IF EXIST "%LP3D_QT64_MSVC%\qtenv2.bat" (
        CALL "%LP3D_QT64_MSVC%\qtenv2.bat"
      ) ELSE (
        SETLOCAL ENABLEDELAYEDEXPANSION
        SET PATH=%LP3D_QT64_MSVC%;!PATH!
        ENDLOCAL
      )
      IF EXIST "%LP3D_VCVARSALL_DIR%\vcvars64.bat" (
        CALL "%LP3D_VCVARSALL_DIR%\vcvars64.bat" %LP3D_VCVARSALL_VER%
      ) ELSE (
        ECHO -ERROR: vcvars64.bat not found.
        GOTO :ERROR_END
      )
    )
  )
  ECHO.
  SET PATH_PREPENDED=True
  SETLOCAL ENABLEDELAYEDEXPANSION
  ECHO(   PATH_PREPEND............[!PATH!]
    ENDLOCAL
  )
  rem Display MSVC Compiler settings
  ECHO.
  ECHO.%LP3D_MSC_VER% > %TEMP%\settings.c
  cl.exe -Bv -EP %TEMP%\settings.c >NUL
  ECHO.
) ELSE (
  ECHO   PATH_ALREADY_PREPENDED..[%PATH%]
)
EXIT /b

:CHECK_BUILD
ECHO.
ECHO -Perform build check...
CALL :CHECK_LDRAW_DIR
IF %1==x86 SET PL=32
IF %1==x86_64 SET PL=64
SET "LPUB3D_DATA=%LOCALAPPDATA%\LPub3D Software\LPub3D"
SET "LDRAW_UNOFFICIAL=%LDRAW_DIR%\Unofficial"
REM SET "LDSEARCHDIRS=%LPUB3D_DATA%\fade^|%LDRAW_UNOFFICIAL%\customParts^|%LDRAW_UNOFFICIAL%\fade^|%LDRAW_UNOFFICIAL%\testParts"
SET ARGS=-l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1 -l
SET LDCONFIG_FILE=tests\LDConfigCustom01.ldr
SET IN_FILE=tests\Foo2.ldr
SET OUT_FILE=tests\%PL%bit_%CONFIGURATION%-TestOK_1.3.5_Foo2.png
SET PACKAGE_PATH=app\%PL%bit_%CONFIGURATION%\%PACKAGE%.exe
SET COMMAND_LINE_ARGS=%ARGS% -ldcF%LDCONFIG_FILE% -mF%OUT_FILE% %IN_FILE%
SET COMMAND=%PACKAGE_PATH% %COMMAND_LINE_ARGS%
IF %CHECK%==1 (
  ECHO.
  ECHO   PACKAGE................[%PACKAGE%]
  ECHO   PACKAGE_PATH...........[%PACKAGE_PATH%]
  ECHO   ARGUMENTS..............[%ARGS%]
  ECHO   LDCONFIG_FILE..........[%LDCONFIG_FILE%]
  ECHO   OUT_FILE...............[%OUT_FILE%]
  ECHO   IN_FILE................[%IN_FILE%]
  ECHO   LDRAWDIR.^(ENV VAR^).....[%LDRAWDIR%]
  ECHO   LDRAW_DIRECTORY........[%LDRAW_DIR%]
  REM ECHO   LDRAW_SEARCH_DIRS......[%LDSEARCHDIRS%]
  ECHO   COMMAND................[%COMMAND%]
  IF EXIST "%OUT_FILE%" (
    DEL /Q "%OUT_FILE%"
  )
  %COMMAND% > Check.out
  IF EXIST "Check.out" (
    FOR %%R IN (Check.out) DO IF NOT %%~zR LSS 1 ECHO. & TYPE "Check.out"
    DEL /Q "Check.out"
  )
  IF EXIST "%OUT_FILE%" (
    ECHO.
    ECHO -Build Check, create %OUT_FILE% from %IN_FILE% - Test successful!
  )
) ELSE (
  ECHO -Check is not possible
)
EXIT /b

:3RD_PARTY_INSTALL
ECHO.
ECHO -Installing 3rd party distribution files to [%DIST_DIR%]...
ECHO.
rem Configure makefiles and perform build
nmake.exe %LDGLITE_MAKE_ARGS% install
EXIT /b

:CHECK_LDRAW_DIR
ECHO.
ECHO -%PACKAGE% - Check for LDraw library...
IF %LP3D_VALID_7ZIP% == 0 (
  "%LP3D_7ZIP_WIN64%" > %TEMP%\output.tmp 2>&1
  FOR /f "usebackq eol= delims=" %%a IN (%TEMP%\output.tmp) DO (
    ECHO.%%a | findstr /C:"7-Zip">NUL && (
      SET LP3D_VALID_7ZIP=1
      ECHO.
      ECHO -7zip exectutable found at "%LP3D_7ZIP_WIN64%"
    ) || (
      ECHO.
      ECHO [WARNING] Could not find 7zip executable at %LP3D_7ZIP_WIN64%.
    )
    GOTO :END_7ZIP_LOOP
  )
)
:END_7ZIP_LOOP
IF NOT EXIST "%LDRAW_DIR%\parts" (
  REM SET CHECK=0
  IF NOT EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    ECHO.
    ECHO -LDraw directory %LDRAW_DIR% does not exist - Downloading...

    CALL :DOWNLOAD_LDRAW_LIBS
  )
  IF EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    IF %LP3D_VALID_7ZIP% == 1 (
      ECHO.
      ECHO -Extracting %OfficialCONTENT%...
      ECHO.
      "%LP3D_7ZIP_WIN64%" x -o"%LDRAW_DOWNLOAD_DIR%\" "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" | findstr /i /r /c:"^Extracting\>" /c:"^Everything\>"
      IF EXIST "%LDRAW_DIR%\parts" (
        ECHO.
        ECHO -LDraw directory %LDRAW_DIR% extracted.
        ECHO.
        ECHO -Cleanup %OfficialCONTENT%...
        DEL /Q "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%"
        ECHO.
        ECHO -Set LDRAWDIR to %LDRAW_DIR%.
        SET LDRAWDIR=%LDRAW_DIR%
      ) ELSE (
        ECHO -[WARNING] LDRAWDIR is not set, %LDRAW_DIR%\parts does not exist.
        SET CHECK=0
      )
    )
  ) ELSE (
    ECHO.
    ECHO -[WARNING] Could not find %LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%.
    SET CHECK=0
  )
) ELSE (
  ECHO.
  ECHO -LDraw directory exist at [%LDRAW_DIR%].
  ECHO.
  ECHO -Set LDRAWDIR to %LDRAW_DIR%.
  SET LDRAWDIR=%LDRAW_DIR%
)
:END_7ZIP_LOOP
EXIT /b

:DOWNLOAD_LDRAW_LIBS
ECHO.
ECHO - Download LDraw archive libraries...

SET OutputPATH=%LDRAW_DOWNLOAD_DIR%

ECHO.
ECHO - Prepare BATCH to VBS to Web Content Downloader...

IF NOT EXIST "%TEMP%\$" (
  MD "%TEMP%\$"
)

SET vbs=WebContentDownload.vbs
SET t=%TEMP%\$\%vbs% ECHO

IF EXIST %TEMP%\$\%vbs% (
 DEL %TEMP%\$\%vbs%
)

:WEB CONTENT SAVE-AS Download-- VBS
>%t% Option Explicit
>>%t% On Error Resume Next
>>%t%.
>>%t% Dim args, http, fileSystem, adoStream, url, target, status
>>%t%.
>>%t% Set args = Wscript.Arguments
>>%t% Set http = CreateObject("WinHttp.WinHttpRequest.5.1")
>>%t% url = args(0)
>>%t% target = args(1)
>>%t% WScript.Echo "- Getting '" ^& target ^& "' from '" ^& url ^& "'...", vbLF
>>%t%.
>>%t% http.Open "GET", url, False
>>%t% http.Send
>>%t% status = http.Status
>>%t%.
>>%t% If status ^<^> 200 Then
>>%t% WScript.Echo "- FAILED to download: HTTP Status " ^& status, vbLF
>>%t% WScript.Quit 1
>>%t% End If
>>%t%.
>>%t% Set adoStream = CreateObject("ADODB.Stream")
>>%t% adoStream.Open
>>%t% adoStream.Type = 1
>>%t% adoStream.Write http.ResponseBody
>>%t% adoStream.Position = 0
>>%t%.
>>%t% Set fileSystem = CreateObject("Scripting.FileSystemObject")
>>%t% If fileSystem.FileExists(target) Then fileSystem.DeleteFile target
>>%t% If Err.Number ^<^> 0 Then
>>%t%   WScript.Echo "- Error - CANNOT DELETE: '" ^& target ^& "', " ^& Err.Description
>>%t%   WScript.Echo "  The file may be in use by another process.", vbLF
>>%t%   adoStream.Close
>>%t%   Err.Clear
>>%t% Else
>>%t%  adoStream.SaveToFile target
>>%t%  adoStream.Close
>>%t%  WScript.Echo "- Download successful!"
>>%t% End If
>>%t%.
>>%t% 'WebContentDownload.vbs
>>%t% 'Title: BATCH to VBS to Web Content Downloader
>>%t% 'CMD ^> cscript //Nologo %TEMP%\$\%vbs% WebNAME WebCONTENT
>>%t% 'VBS Created on %date% at %time%
>>%t%.

ECHO.
ECHO - VBS file "%vbs%" is done compiling
ECHO.
ECHO - LDraw archive library download path: %OutputPATH%

SET WebCONTENT="%OutputPATH%\%OfficialCONTENT%"
SET WebNAME=http://www.ldraw.org/library/updates/complete.zip

ECHO.
ECHO - Download archive file: %WebCONTENT%...

IF EXIST %WebCONTENT% (
 DEL %WebCONTENT%
)

ECHO.
cscript //Nologo %TEMP%\$\%vbs% %WebNAME% %WebCONTENT% && @ECHO off

IF EXIST %OfficialCONTENT% (
  ECHO.
  ECHO - LDraw archive library %OfficialCONTENT% downloaded
)
EXIT /b

:PLATFORM_ERROR
ECHO.
CALL :USAGE
ECHO.
ECHO -01. (FLAG ERROR) Platform or usage flag is invalid. Use x86, x86_64 or -all [%~nx0 %*].
ECHO      See Usage.
GOTO :ERROR_END

:CONFIGURATION_ERROR
ECHO.
CALL :USAGE
ECHO.
ECHO -02. (FLAG ERROR) Configuration flag is invalid [%~nx0 %*].
ECHO      See Usage.
GOTO :ERROR_END

:COMMAND_ERROR
ECHO.
CALL :USAGE
ECHO.
ECHO -03. (COMMAND ERROR) Invalid command string [%~nx0 %*].
ECHO      See Usage.
GOTO :ERROR_END

:USAGE
ECHO ----------------------------------------------------------------
ECHO.
ECHO LDGLite Windows auto build script.
ECHO.
ECHO ----------------------------------------------------------------
ECHO Usage:
ECHO  build [ -help]
ECHO  build [ x86 ^| x86_64 ^| -all ] [ -ins ^| -chk ] [ -chk ]
ECHO.
ECHO ----------------------------------------------------------------
ECHO Build 64bit, Release and perform build check
ECHO build x86_64 -chk
ECHO.
ECHO Build 64bit, Release and perform install and build check
ECHO build x86_64 -ins -chk
ECHO.
ECHO Build 32bit, Release and perform build check
ECHO build x86 -chk
ECHO.
ECHO Build 64bit and32bit, Release and perform build check
ECHO build -all -chk
ECHO.
ECHO Build 64bit and32bit, Release, perform install and build check
ECHO build -all -ins -chk
ECHO.
ECHO Flags:
ECHO ----------------------------------------------------------------
ECHO ^| Flag    ^| Pos ^| Type             ^| Description
ECHO ----------------------------------------------------------------
ECHO  -help......1......Useage flag         [Default=Off] Display useage.
ECHO  x86........1......Platform flag       [Default=Off] Build 32bit architecture.
ECHO  x86_64.....1......Platform flag       [Default=Off] Build 64bit architecture.
ECHO  -all.......1......Configuraiton flag  [Default=On ] Build both  32bit and 64bit architectures
ECHO  -ins.......2......Project flag        [Default=Off] Install distribution as LPub3D 3rd party installation
ECHO  -chk.......2,3....Project flag        [Default=On ] Perform a quick image redering check using command line ini file
ECHO.
ECHO Be sure the set your LDraw directory in the variables section above if you expect to use the '-chk' option.
ECHO.
ECHO Flags are case sensitive, use lowere case.
ECHO.
ECHO If no flag is supplied, 64bit platform, Release Configuration built by default.
ECHO ----------------------------------------------------------------
EXIT /b

:ELAPSED_BUILD_TIME
IF [%1] EQU [] (SET start=%build_start%) ELSE (
  IF "%1"=="Start" (
    SET build_start=%time%
    EXIT /b
  ) ELSE (
    SET start=%1
  )
)
ECHO.
ECHO -%~nx0 finished.
SET end=%time%
SET options="tokens=1-4 delims=:.,"
FOR /f %options% %%a IN ("%start%") DO SET start_h=%%a&SET /a start_m=100%%b %% 100&SET /a start_s=100%%c %% 100&SET /a start_ms=100%%d %% 100
FOR /f %options% %%a IN ("%end%") DO SET end_h=%%a&SET /a end_m=100%%b %% 100&SET /a end_s=100%%c %% 100&SET /a end_ms=100%%d %% 100

SET /a hours=%end_h%-%start_h%
SET /a mins=%end_m%-%start_m%
SET /a secs=%end_s%-%start_s%
SET /a ms=%end_ms%-%start_ms%
IF %ms% lss 0 SET /a secs = %secs% - 1 & SET /a ms = 100%ms%
IF %secs% lss 0 SET /a mins = %mins% - 1 & SET /a secs = 60%secs%
IF %mins% lss 0 SET /a hours = %hours% - 1 & SET /a mins = 60%mins%
IF %hours% lss 0 SET /a hours = 24%hours%
IF 1%ms% lss 100 SET ms=0%ms%
ECHO  Elapsed build time %hours%:%mins%:%secs%.%ms%
ENDLOCAL
EXIT /b

:ERROR_END
ECHO -%~nx0 will terminate!
CALL :ELAPSED_BUILD_TIME
EXIT /b 3

:END
CALL :ELAPSED_BUILD_TIME
EXIT /b
