<img src="./logo/best_logo_ever_alpha.png" width=25%>

# Wolf Programming Language
A simple, lightweight language made in Nim

[![license](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](./LICENSE)
[![build](https://img.shields.io/travis/Ralakus/wolf-lang.svg?style=flat-square)](https://travis-ci.org/Ralakus/wolf-lang)
[![issues](https://img.shields.io/github/issues/Ralakus/wolf-lang.svg?style=flat-square)](https://github.com/Ralakus/wolf-lang/issues)
[![repo size](https://img.shields.io/github/repo-size/Ralakus/wolf-lang.svg?style=flat-square)](https://github.com/Ralakus/wolf-lang)
[![code size](https://img.shields.io/github/languages/code-size/Ralakus/wolf-lang.svg?style=flat-square)](https://github.com/Ralakus/wolf-lang)


## Build requirements
* Nim compiler
* A C compiler ( i.e gcc, clang )

## How to build ( linux )
0. Run `make config=release` or `make config=debug` or just `make`
1. Output will be under `build/bin`

## How to build ( Windows )
0. Run `nimble build -d:debug --verbosity:2 --checks:on --opt:none --debugger:native --stackTrace:on --lineTrace:on` for debug build or `nimble build -d:release --verbosity:2 --checks:off --opt:speed --debugger:native --stackTrace:off --lineTrace:off` for a release build
1. Output will be under `build/bin`

## What is Wolf?
Wolf is a fast, simple programming language that aims to be user friendly and safe yet give control to the user  
Wolf is the successor to [LabyriLanguage](https://gitlab.com/Ralakus/LabyriLanguage)

## How can follow the progress?
* Follow it on Discord! There are also some other cool projects on this Discord like FlukeWM, ModEngine, Lir Language, and more https://discord.gg/yhsWwxm