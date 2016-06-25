@rmdir /s /q ipch
@del /f /q /s *.user
rem @del /f /q /s *.suo
@del /f /q /s *.sdf
@del /f /q /s *.ncb
@del /f /q /s *.ilk
@del /f /q /s *.pdb
@del /f /q _Bin\win32\*.exe
@del /f /q _Bin\x64\*.exe

@rmdir /s /q mygl\win32
@rmdir /s /q mygl\x64

@rmdir /s /q "abc mygl"\win32
@rmdir /s /q "abc mygl"\x64

@rmdir /s /q "Shadow Mapping"\win32
@rmdir /s /q "Shadow Mapping"\x64

@rmdir /s /q "Shadow Volumes"\win32
@rmdir /s /q "Shadow Volumes"\x64

@rmdir /s /q "Bullet Demo"\win32
@rmdir /s /q "Bullet Demo"\x64