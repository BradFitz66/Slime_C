@echo off

SET "SRC_DIR=%~dp0src\Assets"
SET "DEST_DIR=%~dp0build\Assets"

IF NOT EXIST "%DEST_DIR%" mkdir "%DEST_DIR%"

xcopy "%SRC_DIR%\*" "%DEST_DIR%\" /E /Y /I /Q
