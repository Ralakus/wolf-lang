# Wolf Programming Language
A simple, lightweight language made in C11

[![license](http://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

## Build requirements
* Premake 5
* A C compiler that supports the C11 standard ( i.e gcc, clang )

## How to build ( linux )
0. Run `premake5 gmake2 && make -C build`
1. Output will be under `build/bin`

## How to build ( Windows )
0. Run `premake5 vs2017` or whatever Visual Studio version you have
1. Open generated solution file

## What is Wolf?
Wolf is a fast, simple programming language that aims to be user friendly and safe yet give control to the user
Wolf is the predecessor to [LabyriLanguage](https://gitlab.com/Ralakus/LabyriLanguage)