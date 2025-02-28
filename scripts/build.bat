@echo off
pushd %~dp0\..\
call Libraries\premake\premake5.exe vs2022
popd
PAUSE