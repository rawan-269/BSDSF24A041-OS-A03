
# BSDSF24A041-OS-A03

A custom UNIX-style shell developed in C for the Operating Systems course (Programming Assignment 03).

**Shell Prompt:** `FCIT>`

## Features

| Version | Feature | Description |
|---|---|---|
| v1.0-base | Base Shell | Command reading, tokenization and fork-exec-wait |
| v2 | Built-in Commands | `cd`, `exit`, `help` and `jobs` |
| v3 | Command History | `history` and `!n` re-execution; stores the last 20 commands |
| v4 | GNU Readline | Command-line editing, arrow-key history and Tab completion |
| v5 | I/O Redirection and Pipes | Input/output redirection and multi-stage pipelines |
| v6 | Command Chaining and Background Execution | Sequential commands using `;`, background execution using `&`, and job management |
| v7 | Conditional Execution | Multiline `if-then-else-fi` control structure |
| v8 | Shell Variables | Variable assignment, expansion and display using `set` |

## Requirements

- Linux or Ubuntu (WSL supported)
- GCC compiler
- GNU Make
- GNU Readline development library

Install dependencies on Ubuntu:

```bash
sudo apt update
sudo apt install build-essential libreadline-dev
```

## Build

Clone the repository and enter its directory:

```bash
git clone https://github.com/rawan-269/BSDSF24A041-OS-A03.git
cd BSDSF24A041-OS-A03
```

Compile the project:

```bash
make
```

## Run

Start the shell:

```bash
./bin/myshell
```

## Clean

Remove generated object files and executable:

```bash
make clean
```

## Built-in Commands

| Command | Description |
|---|---|
| `cd <directory>` | Change the current working directory |
| `exit` | Exit the shell |
| `help` | Display available commands and syntax |
| `jobs` | List running background jobs |
| `history` | Display command history |
| `!n` | Re-execute the n-th command from history |
| `set` | Display all defined shell variables |

## Supported Operators and Syntax

### I/O Redirection and Pipes

```bash
ls -l > files.txt
sort < files.txt
cat /etc/passwd | grep root
```

- `>` redirects standard output to a file.
- `<` redirects standard input from a file.
- `|` connects the output of one command to the input of another.

### Command Chaining

```bash
echo "First command"; echo "Second command"
```

Commands separated by `;` execute sequentially.

### Background Execution

```bash
sleep 10 &
jobs
```

The `&` operator starts a command in the background. The `jobs` command lists active background jobs.

### Conditional Execution

```bash
if true
then
    echo "Condition succeeded"
else
    echo "Condition failed"
fi
```

The `then` block executes when the condition command succeeds. The optional `else` block executes when it fails.

### Shell Variables

```bash
MSG=hello
echo $MSG
set
```

Variables can also contain spaces:

```bash
MESSAGE="Hello, World!"
echo $MESSAGE
```

Assigning a variable again updates its value.

## Project Structure

```text
BSDSF24A041-OS-A03/
├── src/
│   ├── main.c
│   ├── shell.c
│   └── execute.c
├── include/
│   └── shell.h
├── bin/
│   └── myshell
├── obj/
├── Makefile
├── README.md
└── .gitignore
```

## Testing

The shell should be tested for:

- External commands such as `ls`, `pwd` and `whoami`.
- Built-in commands such as `cd`, `exit` and `help`.
- Command history and `!n` re-execution.
- Readline arrow-key navigation and Tab completion.
- Input/output redirection and pipelines.
- Sequential command execution using `;`.
- Background execution and job listing using `&` and `jobs`.
- Both successful and unsuccessful conditional execution.
- Variable assignment, expansion and reassignment.

## Git Workflow

The project uses feature branches, commits, annotated tags and GitHub releases.

Feature branches include:

- `feature-built-ins`
- `feature-history`
- `feature-readline`
- `feature-io-redirection`
- `feature-multitasking`
- `feature-if-then-else`
- `feature-variables`

Feature 9 demonstrates collaborative Git workflow using a secondary GitHub account, repository fork, issue creation and pull request review and merge.

## Releases

The project uses version tags from `v1.0-base` through `v8`.

Each feature release should include the corresponding compiled `bin/myshell` executable.

## Author

- Roll Number: BSDSF24A041
- GitHub: [rawan-269](https://github.com/rawan-269)
- Repository: [BSDSF24A041-OS-A03](https://github.com/rawan-269/BSDSF24A041-OS-A03)
