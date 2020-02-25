# About pkgconfig
In case of you have build and install opencv, zbar, quirc, you may need this file.  Of couse you need instakk pkg-config tools first before build and install those three library.
You can move opencv4.pc to /usr/local/opencv4/lib/pkgconfig and use "pkg-config --cflags --libs opencv4" to test if it is ok. 
You can move zbar.pc and quirc.pc to /usr/local /lib/pkgconfig and test those in "pkg-config --cflags --libs zbar quirc" to test.

## step1 
Build opencv fellowing the tutorial on opencv.org website
Then install them

## step2
Build zbar like "http://zbar.sourceforge.net/" have told to you.
Install it.

## step3
Buold quirc like "https://github.com/dlbeer/quirc/" have told to you.
Install it.

## Check and repair
To check  if every pkg-config have been installed on your disk. If not use the files here to repair it.
And you an also modify the path in *.pc to the right path you have installed.
