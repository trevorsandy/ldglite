@ECHO OFF &SETLOCAL

Title Download and extract LDraw library - used for testing

IF "%APPVEYOR%" EQU "True" (
  SET LDRAW_DOWNLOAD_DIR=%APPVEYOR_BUILD_FOLDER%
  SET LDRAW_DIR=%APPVEYOR_BUILD_FOLDER%\LDraw
) ELSE (
  SET LDRAW_DOWNLOAD_DIR=%USERPROFILE%
  SET LDRAW_DIR=%USERPROFILE%\LDraw
)
SET zipWin64=C:\program files\7-zip
SET OfficialCONTENT=complete.zip

ECHO.
ECHO -Check for LDraw library...
IF NOT EXIST "%LDRAW_DIR%\parts" (
  REM SET CHECK=0
  IF NOT EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    ECHO.
    ECHO -LDraw directory %LDRAW_DIR% does not exist - Downloading...

    CALL :DOWNLOAD_LDRAW_LIBS
  )
  IF EXIST "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" (
    IF EXIST "%zipWin64%" (
      ECHO.
      ECHO -7zip exectutable found at "%zipWin64%"
      ECHO.
      ECHO -Extracting %OfficialCONTENT%...
      ECHO.
      "%zipWin64%\7z.exe" x -o"%LDRAW_DOWNLOAD_DIR%\" "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%" | findstr /i /r /c:"^Extracting\>" /c:"^Everything\>"
      IF EXIST "%LDRAW_DIR%\parts" (
        ECHO.
        ECHO -LDraw directory %LDRAW_DIR% extracted.
        ECHO.
        ECHO -Cleanup %OfficialCONTENT%...
        DEL /Q "%LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%"
      )
    ) ELSE (
      ECHO [WARNING] Could not find zip executable.
      GOTO :END
    )
  ) ELSE (
    ECHO.
    ECHO -[WARNING] Could not find %LDRAW_DOWNLOAD_DIR%\%OfficialCONTENT%.
    GOTO :END
  )
) ELSE (
  ECHO.
  ECHO -LDraw directory %LDRAW_DIR% exist.
)
GOTO :END

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
>>%t%   WScript.Echo " The file may be in use by another process.", vbLF
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

SET LibraryOPTION=Official
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
ECHO.
ECHO - LDraw archive library download finshed.
EXIT /b

:END
ECHO.
ECHO Finished.
ENDLOCAL
EXIT /b
