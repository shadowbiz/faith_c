@echo off

REM
REM  To run this at startup, use this as your shortcut target:
REM  %windir%\system32\cmd.exe /k f:\gamedev\faith\misc\shell.bat
REM

cd /d d:\gamedev\faith\code

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
set path=d:\gamedev\faith\misc;%path%
