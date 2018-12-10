@echo off

IF NOT DEFINED clset (call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64)
SET clset=64

set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -DFAITH_INTERNAL=1 -DFAITH_SLOW=1 -DFAITH_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL

cl %CommonCompilerFlags% ..\code\faith.cpp -Fmfaith.map -LD /link -incremental:no -opt:ref -PDB:faith_%random%.pdb -EXPORT:GameUpdate
cl %CommonCompilerFlags% ..\code\win32_faith.cpp -Fmwin32_faith.map /link %CommonLinkerFlags%
popd
