# txtdiff
A tool to compare and verify text files

##
### Purpose
The Unix 'diff' tool was too confusing for me and I needed a simpler tool to use.
I made this as a alternitive to the diff tool.

### How to use
The program takes in 2 arguments
[FILE_1] and [FILE_2]

A sample use of the program would be
```
txtdiff FILE_1 FILE_2
```
It will find what has changed between FILE_1 and FILE_2. 
So if you added a line to FILE_2 that wasn't in FILE_1 then it will show up as a line that was added
If there was a line that you deleted in FILE_2 that was orignally in FILE_1 it will show up as a line that was deleted

##### How do you know if a line was added or deleted?
A line that is added will be indicated with a '+' whereas a line deleted will apear as a '-'
Lines that have not changed will have no character in that place

## Flag Arguments
#### --help
Will print a help message to help you know how the program works
#### --stdout
Will function normally but without any colors
#### --lines
Do not show line numbers
#### --verify
Do not do a in-depth file diff evaluation but instead simply check if the files are the same or not.
The program will return -1 (or 255) if the files are not the same, and 1 if the files are the same
#### --compare
NOT COMPLETED YET
Will show the two files side by side showing what was added and removed.

## How to Compile
### Windows
This program will NOT compile on Windows. It uses linux functions to determine the Window size and to print colors. If I get enough requests for a windows version, I will make one. But for now it is un-needed.
### Linux / Unix
Simply type in 
```
make
```
If you would like a debug version (there is not much debug info) you can type in
```
make debug
```
If you would like the executable to have a different file name then do
```
make BUILD=[your name here]
```

