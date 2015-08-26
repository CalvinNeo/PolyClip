@echo off
md "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\scarlet"
copy "D:\code\c&cpplib\scarlet\*.h"    "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\scarlet" /y
copy "D:\code\c&cpplib\scarlet\*.cpp"  "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\scarlet" /y
pause