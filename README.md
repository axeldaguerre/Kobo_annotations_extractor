# Kobo annotation extractor

This is a program permitting to extract your Kobo's annotations.
You can extract them has html or text.


## How to use

Using the `exe` from `build` folder or by running the `build.bat` from your computer:

`kobo_extractor_rm.exe path_to_KoboReader.sqlite output_path "full"` 
 
`"full"` is for the maximum column values, if you don't set it it will do the light version

If you don't add any parameter, it will print a message a small helper 

## Build the exe

You will have to run the `build.bat` but you need msvc from Visual Studio or by using the https://github.com/Data-Oriented-House/PortableBuildTools (recommended).
If you want to use MSVC from Visual Studio, you will need to start from the native cmd or using the `shell.bat` from this repository, tweak the path from yours inside it.

## Output example 

HTML output : https://github.com/axeldaguerre/Kobo_annotations_extractor/blob/main/data/exemple.html

## Tested

It was tested only for Kobo Libra 2. But Any Kobo should works just fine.

In fact if a program is using `SQLite` as its annotation saving, you will be able to tweak the tool quite easily, you will have to change the column name though (using a tool like DB Browser for Sqlite). 


## Highly influenced
This is influenced by https://github.com/EpicGamesExt/raddebugger. Currently a significant portion of the code is from this very well written source code.