@ECHO ON &SETLOCAL

Title LDGLite Windows auto build script Debug

rem This script uses Qt to configure and build LDGLite for Windows.
rem The primary purpose is to automatically build both the 32bit and 64bit
rem LDGLite distributions and package the build contents (exe, doc and
rem resources ) as LPub3D 3rd Party components.
rem --
rem  Trevor SANDY <trevor.sandy@gmail.com>
rem  Last Update: July 20, 2025
rem  Copyright (c) 2019 - 2025 by Trevor SANDY
rem --
rem This script is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CALL :ELAPSED_BUILD_TIME Start

SET PWD=%CD%

IF "%LP3D_VSVERSION%" == "" SET LP3D_VSVERSION=2022
IF "%LP3D_QT32VERSION%" == "" SET LP3D_QT32VERSION=5.15.2
IF "%LP3D_QT64VERSION%" == "" SET LP3D_QT64VERSION=6.9.1
IF "%LP3D_QT32VCVERSION%" == "" SET LP3D_QT32VCVERSION=2019
IF "%LP3D_QT64VCVERSION%" == "" SET LP3D_QT64VCVERSION=2022

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
IF %CONFIGURATION% EQU debug SET d=d

IF "%LP3D_CONDA_BUILD%" NEQ "True" (
  IF EXIST "C:\Program Files\Microsoft Visual Studio\%LP3D_VSVERSION%\Community\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files\Microsoft Visual Studio\%LP3D_VSVERSION%\Community\VC\Auxiliary\Build
  )
  IF EXIST "C:\Program Files\Microsoft Visual Studio\%LP3D_VSVERSION%\Enterprise\VC\Auxiliary\Build" (
    SET LP3D_VCVARSALL_DIR=C:\Program Files\Microsoft Visual Studio\%LP3D_VSVERSION%\Enterprise\VC\Auxiliary\Build
  )
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
  ECHO  -ERROR - Microsoft Visual Studio C++ environment [%LP3D_VCVARSALL_DIR%] not defined.
  GOTO :ERROR_END
)

rem https://learn.microsoft.com/en-us/cpp/overview/compiler-versions
rem Visual C++ 2012 -vcvars_ver=11.0 Toolset v110 VSVersion 11.0    _MSC_VER 1700
rem Visual C++ 2013 -vcvars_ver=12.0 Toolset v120 VSVersion 12.0    _MSC_VER 1800
rem Visual C++ 2015 -vcvars_ver=14.0 Toolset v140 VSVersion 14.0    _MSC_VER 1900
rem Visual C++ 2017 -vcvars_ver=14.1 Toolset v141 VSVersion 15.9    _MSC_VER 1916
rem Visual C++ 2019 -vcvars_ver=14.2 Toolset v142 VSVersion 16.11.3 _MSC_VER 1929
rem Visual C++ 2022 -vcvars_ver=14.4 Toolset v143 VSVersion 17.14.0 _MSC_VER 1944
IF "%LP3D_MSC32_VER%" == "" SET LP3D_MSC32_VER=1929
IF "%LP3D_VC32SDKVER%" == "" SET LP3D_VC32SDKVER=8.1
IF "%LP3D_VC32TOOLSET%" == "" SET LP3D_VC32TOOLSET=v141
IF "%LP3D_VC32VARSALL_VER%" == "" SET LP3D_VC32VARSALL_VER=-vcvars_ver=14.1

IF "%LP3D_MSC64_VER%" == "" SET LP3D_MSC64_VER=1944
IF "%LP3D_VC64SDKVER%" == "" SET LP3D_VC64SDKVER=10.0
IF "%LP3D_VC64TOOLSET%" == "" SET LP3D_VC64TOOLSET=v143
IF "%LP3D_VC64VARSALL_VER%" == "" SET LP3D_VC64VARSALL_VER=-vcvars_ver=14.4

IF "%LP3D_MSCARM64_VER%" == "" SET LP3D_MSCARM64_VER=1944
IF "%LP3D_VCARM64SDKVER%" == "" SET LP3D_VCARM64SDKVER=10.0
IF "%LP3D_VCARM64TOOLSET%" == "" SET LP3D_VCARM64TOOLSET=v143
IF "%LP3D_VCARM64VARSALL_VER%" == "" SET LP3D_VCARM64VARSALL_VER=-vcvars_ver=14.4

IF "%LP3D_VALID_TAR%" == "" SET LP3D_VALID_TAR=0
IF "%LP3D_SYS_DIR%" == "" (
  SET LP3D_SYS_DIR=%WINDIR%\System32
)
IF "%LP3D_WIN_TAR%" == "" (
  SET LP3D_WIN_TAR=%LP3D_SYS_DIR%\Tar.exe
)
IF NOT EXIST "%LP3D_WIN_TAR%" (
  SET LP3D_WIN_TAR=
  SET LP3D_WIN_TAR_MSG=Not Found
) ELSE (
  SET LP3D_VALID_TAR=1 
  SET LP3D_WIN_TAR_MSG=%LP3D_WIN_TAR%
)

SET OfficialCONTENT=complete.zip
SET LP3D_AMD64_ARM64_CROSS=0

SET PACKAGE=LDGLite
SET VERSION=1.3.8

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
  IF /I NOT "%1"=="x86" (
    IF /I NOT "%1"=="x86_64" (
      IF /I NOT "%1"=="arm64" (
        IF /I NOT "%1"=="-all_amd" (
          IF /I NOT "%1"=="-help" GOTO :PLATFORM_ERROR
        )
      )
    )
  )
)

rem Parse platform input flags
IF [%1]==[] (
  SET PLATFORM_ARCH=-all_amd
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
IF /I "%1"=="arm64" (
  SET PLATFORM_ARCH=ARM64
  GOTO :SET_CONFIGURATION
)
IF /I "%1"=="-all_amd" (
  SET PLATFORM_ARCH=-all_amd
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
  IF /I NOT "%2"=="-ins" (
    IF /I NOT "%2"=="-chk" GOTO :CONFIGURATION_ERROR
  )
)

rem Verify 3rd input flag options
IF NOT [%3]==[] (
  IF /I NOT "%3"=="-chk" GOTO :CONFIGURATION_ERROR
)

rem Setup library options and set ARM64 cross compilation
IF /I "%PLATFORM_ARCH%" == "ARM64" (
  IF /I "%COMMANDPROMPTTYPE%" == "Cross" (
    IF /I "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
      SET LP3D_AMD64_ARM64_CROSS=1
    )
  )
  SET LP3D_QTARCH=arm64
) ELSE (
  SET LP3D_QTARCH=64
)
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
    SET LP3D_QT32_MSVC=%LP3D_BUILD_BASE%\Qt\%LP3D_QT32VERSION%\msvc%LP3D_QT32VCVERSION%\bin
  )
  IF "%LP3D_QT64_MSVC%" == "" (
    SET LP3D_QT64_MSVC=%LP3D_BUILD_BASE%\Qt\%LP3D_QT64VERSION%\msvc%LP3D_QT64VCVERSION%_%LP3D_QTARCH%\bin
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
    SET LP3D_QT32_MSVC=C:\Qt\%LP3D_QT32VERSION%\msvc%LP3D_QT32VCVERSION%\bin
  )
  IF "%LP3D_QT64_MSVC%" == "" (
    SET LP3D_QT64_MSVC=C:\Qt\%LP3D_QT64VERSION%\msvc%LP3D_QT64VCVERSION%_%LP3D_QTARCH%\bin
  )
)

IF "%BUILD_WORKER%" NEQ "True" (
  IF "%APPVEYOR%" NEQ "True" (
    rem DIST_DIR must be relative to App folder in LDGLite repo
    SET DIST_DIR=..\..\lpub3d_windows_3rdparty
    SET LDRAW_DOWNLOAD_DIR=%USERPROFILE%
    SET LDRAW_DIR=%USERPROFILE%\LDraw
    IF "%LP3D_QT32_MSVC%" == "" (
      SET LP3D_QT32_MSVC=C:\Qt\IDE\%LP3D_QT32VERSION%\msvc%LP3D_QT32VCVERSION%\bin
    )
    IF "%LP3D_QT64_MSVC%" == "" (
      SET LP3D_QT64_MSVC=C:\Qt\IDE\%LP3D_QT64VERSION%\msvc%LP3D_QT64VCVERSION%_%LP3D_QTARCH%\bin
    )
  )
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
ECHO   LP3D_WIN_TAR...........[%LP3D_WIN_TAR_MSG%]
IF %LP3D_AMD64_ARM64_CROSS% EQU 1 (
  ECHO   COMPILATION............[ARM64 on AMD64 host]
)

rem Perform build check
IF /I "%3"=="-chk" (
  SET CHECK=1
)

rem Check if build all platforms
IF /I "%PLATFORM_ARCH%"=="-all_amd" (
  GOTO :BUILD_ALL_AMD
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
IF %PLATFORM_ARCH%==x86 (SET EXE=app\32bit_%CONFIGURATION%\%PACKAGE%%d%.exe)
IF %PLATFORM_ARCH%==x86_64 (SET EXE=app\64bit_%CONFIGURATION%\%PACKAGE%%d%.exe)
IF %PLATFORM_ARCH%==ARM64 (SET EXE=app\64bit_%CONFIGURATION%\%PACKAGE%%d%.exe)
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

:BUILD_ALL_AMD
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
  IF %%P==x86 (SET EXE=app\32bit_%CONFIGURATION%\%PACKAGE%%d%.exe)
  IF %%P==x86_64 (SET EXE=app\64bit_%CONFIGURATION%\%PACKAGE%%d%.exe)
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
    SET LP3D_MSC_VER=1944
    SET LP3D_VCSDKVER=%LP3D_VC64SDKVER%
    SET LP3D_VCTOOLSET=%LP3D_VC64TOOLSET%
    SET LP3D_VCVARSALL_VER%LP3D_VC64VARSALL_VER%
  )
) ELSE (
  IF %1==ARM64 (
    SET LP3D_MSC_VER=%LP3D_MSCARM64_VER%
    SET LP3D_VCSDKVER=%LP3D_VCARM64SDKVER%
    SET LP3D_VCTOOLSET=%LP3D_VCARM64TOOLSET%
    SET LP3D_VCVARSALL_VER=%LP3D_VCARM64VARSALL_VER%
  ) ELSE (
    SET LP3D_MSC_VER=%LP3D_MSC32_VER%
    SET LP3D_VCSDKVER=%LP3D_VC32SDKVER%
    SET LP3D_VCTOOLSET=%LP3D_VC32TOOLSET%
    SET LP3D_VCVARSALL_VER=%LP3D_VC32VARSALL_VER%
  )
)
ECHO.
ECHO   PLATFORM_ARCHITECTURE..[%1]
ECHO   MSVS_VERSION...........[%LP3D_VSVERSION%]
IF %1==-all_amd (
  ECHO   MSVC_QT32_VERSION......[%LP3D_QT32VCVERSION%]
  ECHO   MSVC_QT64_VERSION......[%LP3D_QT64VCVERSION%]
) ELSE (
  IF %1==x86 (
    ECHO   MSVC_QT32_VERSION......[%LP3D_QT32VCVERSION%]
  )
  IF %1==x86_64 (
    ECHO   MSVC_QT64_VERSION......[%LP3D_QT64VCVERSION%]
  )
  IF %1==ARM64 (
    ECHO   MSVC_QT64_VERSION......[%LP3D_QT64VCVERSION%]
  )
)
ECHO   MSVC_MSC_VERSION.......[%LP3D_MSC_VER%]
ECHO   MSVC_SDK_VERSION.......[%LP3D_VCSDKVER%]
ECHO   MSVC_TOOLSET...........[%LP3D_VCTOOLSET%]
IF "%LP3D_CONDA_BUILD%" NEQ "True" (
  IF %1==x86 (ECHO   LP3D_QT32_MSVC.........[%LP3D_QT32_MSVC%])
)
IF %1==x86_64 (ECHO   LP3D_QT64_MSVC.........[%LP3D_QT64_MSVC%])
IF %1==ARM64 (ECHO   LP3D_QT64_MSVC.........[%LP3D_QT64_MSVC%])
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
rem /c flag suppresses the copyright
SET LDGLITE_MAKE_ARGS=/c /f Makefile
rem Set vcvars for AppVeyor or local build environments
IF %PLATFORM_ARCH% EQU x86_64 (
  SET LP3D_VCVARS=vcvars64.bat
)
IF %PLATFORM_ARCH% EQU ARM64 (
  SET LP3D_VCVARS=vcvarsamd64_arm64.bat
  SET LDGLITE_CONFIG_ARGS=-config %CONFIGURATION% %LDGLITE_CONFIG_ARGS%
)
ECHO   LDGLITE_CONFIG_ARGS............[%LDGLITE_CONFIG_ARGS%]
ECHO   LDGLITE_MAKE_ARGS..............[%LDGLITE_MAKE_ARGS%]
ECHO.
IF "%PATH_PREPENDED%" EQU "True" (
  ECHO   "PATH_ALREADY_PREPENDED..[%PATH%]"
  EXIT /b
)
IF "%LP3D_CONDA_BUILD%" EQU "True" (
  GOTO :COMPILER_SETTINGS
)
SET "PATH_ADDITIONS=%LP3D_SYS_DIR%"
IF "%LP3D_WIN_GIT%" NEQ "" (
  SET "PATH_ADDITIONS=%PATH_ADDITIONS%;%LP3D_WIN_GIT%"
)
SET "PATH=%PATH_ADDITIONS%;%PATH%"
IF %PLATFORM_ARCH% EQU x86 (
  IF EXIST "%LP3D_VCVARSALL_DIR%\vcvars32.bat" (
	SET "LP3D_VCVARSALL_BAT=%LP3D_VCVARSALL_DIR%\vcvars32.bat"
  ) ELSE (
    ECHO.
    ECHO -ERROR - vcvars32.bat not found.
    GOTO :ERROR_END
  )
  IF EXIST "%LP3D_QT32_MSVC%\qtenv2.bat" (
	SET "LP3D_QTENV_BAT=%LP3D_QT32_MSVC%\qtenv2.bat"
  ) ELSE (
    SET "LP3D_QT_MSVC_PATH=%LP3D_QT32_MSVC%"
  )
) ELSE (
  IF EXIST "%LP3D_VCVARSALL_DIR%\%LP3D_VCVARS%" (
	SET "LP3D_VCVARSALL_BAT=%LP3D_VCVARSALL_DIR%\%LP3D_VCVARS%"
  ) ELSE (
    ECHO.
    ECHO -ERROR - %LP3D_VCVARS% not found.
    GOTO :ERROR_END
  )
  IF EXIST "%LP3D_QT64_MSVC%\qtenv2.bat" (
	SET "LP3D_QTENV_BAT=%LP3D_QT64_MSVC%\qtenv2.bat"
  ) ELSE (
    SET "LP3D_QT_MSVC_PATH=%LP3D_QT64_MSVC%"
  )
)
SET PATH_PREPENDED=True
CALL "%LP3D_VCVARSALL_BAT%" %LP3D_VCVARSALL_VER%
ECHO.
IF "%LP3D_QTENV_BAT%" NEQ "" (
  CALL "%LP3D_QTENV_BAT%"
  ECHO(   PATH_PREPEND............["%PATH%"])
) ELSE (
  IF "%LP3D_QT_MSVC_PATH%" EQU "" (
    GOTO :COMPILER_SETTINGS
  )
)
IF "%QTDIR%" EQU "" (FOR %%I IN ("%LP3D_QT_MSVC_PATH%") DO SET QTDIR=%%~dpI)
IF "%QTDIR:~-1%" EQU "\" (SET QTDIR=%QTDIR:~0,-1%)
SET "PATH=%LP3D_QT_MSVC_PATH%;%PATH%"
ECHO(   PATH_PREPEND............[%PATH%])

:COMPILER_SETTINGS
rem Display MSVC Compiler settings
ECHO.
ECHO -Display _MSC_VER %LP3D_MSC_VER% compiler settings
ECHO.
ECHO.%LP3D_MSC_VER% > %TEMP%\settings.c
cl.exe -Bv -EP %TEMP%\settings.c >NUL
EXIT /b

:CHECK_BUILD
ECHO.
ECHO -Perform build check...
CALL :CHECK_LDRAW_DIR
IF %1==x86 SET PL=32
IF %1==x86_64 SET PL=64
IF %1==ARM64 SET PL=64
SET "LPUB3D_DATA=%LOCALAPPDATA%\LPub3D Software\LPub3D"
SET "LDRAW_UNOFFICIAL=%LDRAW_DIR%\Unofficial"
REM SET "LDSEARCHDIRS=%LPUB3D_DATA%\fade^|%LDRAW_UNOFFICIAL%\customParts^|%LDRAW_UNOFFICIAL%\fade^|%LDRAW_UNOFFICIAL%\testParts"
SET ARGS=-l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -2g,2x -w1
SET LDCONFIG_FILE=tests\LDConfigCustom01.ldr
SET IN_FILE=tests\Foo2.ldr
SET OUT_FILE=tests\%PL%bit_%CONFIGURATION%-TestOK_%VERSION%_Foo2.png
SET PACKAGE_PATH=app\%PL%bit_%CONFIGURATION%\%PACKAGE%%d%.exe
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
  %COMMAND% > Check.out 2>&1
  IF EXIST "Check.out" (
    FOR %%R IN (Check.out) DO IF NOT %%~zR LSS 1 ECHO. & TYPE "Check.out"
    DEL /Q "Check.out"
  )
  IF EXIST "%OUT_FILE%.%PACKAGE%.log" (
    ECHO.
    TYPE "%OUT_FILE%.%PACKAGE%.log"
    DEL /Q "%OUT_FILE%.%PACKAGE%.log"
  )
  IF EXIST "%OUT_FILE%" (
    ECHO.
    ECHO -Build Check, create %OUT_FILE% from %IN_FILE% - Test successful!
    DEL /Q "%OUT_FILE%"
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
IF NOT EXIST "%LDRAW_DIR%\parts" (
  REM SET CHECK=0
  IF NOT EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    ECHO.
    ECHO -LDraw directory %LDRAW_DIR% does not exist - Downloading...

    CALL :DOWNLOAD_LDRAW_LIBS
  )
  IF EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    IF %LP3D_VALID_TAR% == 1 (
      ECHO.
      ECHO -Extracting %OfficialCONTENT%...
      ECHO.
      "%LP3D_WIN_TAR%" x -o"%LDRAW_DOWNLOAD_DIR%\" "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" | findstr /i /r /c:"^Extracting\>" /c:"^Everything\>"
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
SET WebNAME=https://library.ldraw.org/library/updates/complete.zip

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
ECHO -01. (PLATFORM_ERROR) Platform or usage flag is invalid. Use x86, x86_64, arm64 or -all_amd [%~nx0 %*].
ECHO      See Usage.
GOTO :ERROR_END

:CONFIGURATION_ERROR
ECHO.
CALL :USAGE
ECHO.
ECHO -02. (CONFIGURATION_ERROR) Configuration flag is invalid [%~nx0 %*].
ECHO      See Usage.
GOTO :ERROR_END

:COMMAND_ERROR
ECHO.
CALL :USAGE
ECHO.
ECHO -03. (COMMAND_ERROR) Invalid command string [%~nx0 %*].
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
ECHO  build [ x86 ^| x86_64 ^| arm64 ^| -all_amd ] [ -ins ^| -chk ] [ -chk ]
ECHO.
ECHO ----------------------------------------------------------------
ECHO Build AMD 64bit, Release and perform build check
ECHO build x86_64 -chk
ECHO.
ECHO Build ARM 64bit, Release and perform install and build check
ECHO build arm64 -ins -chk
ECHO.
ECHO Build AMD 32bit, Release and perform build check
ECHO build x86 -chk
ECHO.
ECHO Build AMD 64bit and32bit, Release and perform build check
ECHO build -all_amd -chk
ECHO.
ECHO Build AMD 64bit and32bit, Release, perform install and build check
ECHO build -all_amd -ins -chk
ECHO.
ECHO Flags:
ECHO ----------------------------------------------------------------
ECHO ^| Flag    ^| Pos ^| Type                ^| Description
ECHO ----------------------------------------------------------------
ECHO  -help......1......Useage flag         [Default=Off] Display useage.
ECHO  x86........1......Platform flag       [Default=Off] Build AMD 32bit architecture.
ECHO  x86_64.....1......Platform flag       [Default=Off] Build AMD 64bit architecture.
ECHO  arm64......1......Platform flag       [Default=Off] Build ARM 64bit architecture.
ECHO  -all_amd...1......Configuraiton flag  [Default=On ] Build both AMD 32bit and 64bit architectures
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
