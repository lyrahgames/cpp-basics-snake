# C++ Basics: The Game Snake

## Authors
- Johanna Vielemeyer
- Markus Pawellek (markuspawellek@gmail.com)

## Requirements
- C++17
- SFML Graphics Module

## Getting Started

```
mkdir cpp-basics
cd cpp-basics
git clone https://github.com/lyrahgames/cpp-basics-snake.git snake
cd snake
```

Create the persistent configuration out-of-source and set appropiate compiler flags.

    bdep init -C @gcc cc config.cxx=g++ "config.cxx.coptions=-O3 -march=native"

Build the executable.

    b

Run the executable.

    snake/snake

Clean build output.

    b clean

From here on, open your editor inside the directory and look at the code and start with further developments.

## Install the Application

Create persistent configuration for all packages in the new directory `cpp-basics-packages` with appropriate compiler flags and installation options.

    bpkg create -d cpp-basics-packages cc \
        config.cxx=clang++ \
        "config.cxx.coptions=-O3 -march=native" \
        config.install.root=/usr/local \
        config.install.sudo=sudo

Download and build this packages inside the configuration folder.

    bpkg build snake@https://github.com/lyrahgames/cpp-basics-snake.git#master

Install the built executable with its documentation.

    bpkg install snake

Run the application in any directory.

    snake

Uninstall the executable while in 'cpp-basics-packages'.

    bpkg uninstall snake