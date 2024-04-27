@echo off

if not exist build mkdir build
pushd build
@REM Windows grow significantly their pdb size over time
del *.pdb > NUL 2> NUL

set CommonCompilerFlags= /I..\src\ /nologo /FC 
set CommonLinkerFlags= /INCREMENTAL:NO
set warn_disabled=-wd4201 -wd4505 -wd4100
cl /I..\src\ -Zi -W4  %warn_disabled% -nologo ..\src\kobo\kobo_main.c -Fekobo_extractor_dm.exe /link %CommonLinkerFlags%
cl %CommonCompilerFlags% %warn_disabled% -nologo ..\src\kobo\kobo_main.c -Fekobo_extractor_rm.exe /link %CommonLinkerFlags%

popd
