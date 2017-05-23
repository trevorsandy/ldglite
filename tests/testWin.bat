@ECHO OFF
ECHO START----------------------------------------------> testWin.log
ECHO.>> testWin.log

SET CMD_LDCONFIG=1

SET LDSEARCHDIRS=C:\Users\Trevor\LDraw\Unofficial\customParts^|C:\Users\Trevor\LDraw\Unofficial\fade^|C:\Users\Trevor\LDraw\Unofficial\testParts
SET ARGS=-l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -w1 -l
REM SET LDCONFIG=^=ldconfigtest\\LDConfigCustomTest01.ldr
SET LDCONFIG=^=LDConfigCustom01.ldr
SET OUTFILE=-mFTestResult_1.3.3_Foo2.png 
SET INFILE=Foo2.ldr
SET LDRAWDIR=C:\Users\Trevor\LDraw

IF %CMD_LDCONFIG%==1 (
	SET CMDLINEARGS=%ARGS% %LDCONFIG% %OUTFILE% %INFILE%
) ELSE (
	SET CMDLINEARGS=%ARGS% %OUTFILE% %INFILE%
)

ECHO VARIABLES:>> testWin.log
ECHO LDRAWDIR     [%LDRAWDIR%]>> testWin.log
ECHO ARGS         [%ARGS%]>> testWin.log
IF NOT CMD_LDCONFIG==[] ECHO LDCONFIG     [%LDCONFIG%]>> testWin.log
ECHO OUTFILE      [%OUTFILE%]
ECHO INFILE       [%INFILE%]
ECHO CMDLINEARGS  [ldglite.exe %CMDLINEARGS%]>> testWin.log

REM 64bit Release
COPY /b/v/y ..\..\build-ldglite-Desktop_Qt_5_7_1_MinGW_64bit_Msys64-Release\release\ldglite.exe ldglite.exe
REM 32bit Debug
REM COPY "..\..\build-ldglite-Desktop_Qt_5_7_1_MinGW_32bit-Debug\release\ldglite.exe" ldglite.exe

ECHO.>> testWin.log
ECHO NORMAL PART TEST----------------------------------->> testWin.log
ldglite.exe %CMDLINEARGS%>> testWin.log
ECHO.>> testWin.log
ECHO END------------------------------------------------>> testWin.log