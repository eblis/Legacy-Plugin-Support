for /F "tokens=6-10* delims=. " %%i in (docs\LPS_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -a -c2 "LPS %1.%2.%3.%4.zip" @files_release.txt
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "LPS %1.%2.%3.%4.zip" docs\*.* *.caca
call "pack symbols.bat" LPS Bridge %1.%2.%3.%4
exit

error:
echo "Error packing LPS"
