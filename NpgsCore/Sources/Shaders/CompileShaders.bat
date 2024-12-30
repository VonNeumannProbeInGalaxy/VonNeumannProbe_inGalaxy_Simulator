@echo off
setlocal

for %%f in (*.vert) do (
    glslc.exe "%%f" -o "%%~nf.vert.spv"
)

for %%f in (*.frag) do (
    glslc.exe "%%f" -o "%%~nf.frag.spv"
)

for %%f in (*.comp) do (
    glslc.exe "%%f" -o "%%~nf.comp.spv"
)

echo Done.
pause
