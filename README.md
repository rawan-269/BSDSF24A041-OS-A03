# BSDSF24A041-OS-A03

A custom UNIX shell written in C for the Operating Systems course (Programming Assignment 03).

Prompt: `FCIT>`

## Features implemented so far

| Version | Feature |
|---------|---------|
| v1.0-base | Base shell: read, tokenize, fork-exec-wait |
| v2 | Built-in commands: exit, cd, help, jobs |
| v3 | Command history: history command and !n re-execution (last 20 commands) |
| v4 | GNU Readline: line editing, Up/Down arrow history, Tab completion |

## Requirements

- Linux (Ubuntu or WSL)
- gcc and make
- GNU Readline development library

Install on Ubuntu:

    sudo apt install build-essential libreadline-dev

## Build

    make

## Run

    ./bin/myshell

## Clean

    make clean

## Built-in commands

| Command | Description |
|---------|-------------|
| `cd <dir>` | Change the current directory |
| exit | Exit the shell |
| help | Show the list of built-in commands |
| jobs | Placeholder for job control (not yet implemented) |
| history | Show the last 20 commands with numbers |
| !n | Re-run the n-th command from history |

## Project structure

    BSDSF24A041-OS-A03/
    |-- src/
    |   |-- main.c       main loop (readline, history expansion, dispatch)
    |   |-- shell.c      tokenizer, history storage, built-in commands
    |   `-- execute.c    fork-exec-wait for external commands
    |-- include/
    |   `-- shell.h      constants and function prototypes
    |-- Makefile
    `-- README.md

## Releases

Each feature is tagged and published as a GitHub release (v1.0-base, v2, v3, v4, ...). Each release has the compiled `myshell` binary attached.

## Author

- Roll No: BSDSF24A041
- GitHub: rawan-269
