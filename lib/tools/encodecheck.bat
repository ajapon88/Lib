@echo off

set ENCODECHECKER=%~dp0encodecheck.py

cd %~dp0../
for /F %%A in ('dir /s /b *.h *cpp') do (
	%ENCODECHECKER% utf-8 CRLF %%A
)
pause
