![Bingo logo](https://raw.githubusercontent.com/SuperDelphi/bingo/master/bingo.png)

# bingo
Your idea notebook, right in the terminal.

## Compatibility

At the moment, Bingo only supports Windows.

## Set up

1. Download the latest release [here](https://github.com/SuperDelphi/bingo/releases).
2. (Optional) Add the executable path to the PATH variable for it to be used from anywhere.
3. The first time, Bingo will ask you to provide an **absolute** path to a file where it will store your ideas.
It can be an existing file as well as a file that it will automatically create. We suggest you to choose a path you can access yourself, so that you can read the text file as you would do when writing down ideas manually.

## How to use

### Saving an idea

Run ``bingo "<message>"`` (quotes are optional but recommended).

### Listing your ideas

Run ``bingo`` without parameters.

## How to compile yourself

### Prerequisites

In order to compile this code, you will need:

- A C compiler that supports Makefiles
- CMake

### On Windows

1. Place yourself into the source folder.
2. If you want a Makefile for compilation, run the command ``cmake -G "Unix Makefiles"`` to generate one. Then, run ``make``.

### On Linux

*Note: Bingo hasn't been tested for Linux yet!*

1. Place yourself into the source folder.
2. Run the command ``cmake .`` then run ``make``.
