call bla.bat
gcc -c splorer.c -I..\include
dlxgen splorer.s2a splorer.o scroll.o
copy splorer.s2a ..\bin\apps /Y
