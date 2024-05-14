# Kobo annotation extractor

This is a program permitting to extract your Kobo's annotations.
You can extract them has html or text.

## How to use

Using the `exe` from `build` folder or by running the `build.bat` from your computer:

`kobo_extractor_rm.exe path_to_KoboReader.sqlite output_path "full"` 
 
`"full"` is for the maximum column values, if you don't set it it will do the light version

If you don't add any parameter, it will print a message a small helper 

## Tested

It was tested only for Kobo Libra 2. But Any Kobo should works just fine.

In fact if a program is using `SQLite` as its annotation saving, you will be able to tweak the tool quite easily, you will have to change the column name though (using a tool like DB Browser for Sqlite). 


## Highly influenced
This is influenced by https://github.com/EpicGamesExt/raddebugger. Currently a significant portion of the code is from this very well written source code.