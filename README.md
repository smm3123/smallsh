# smallsh
smallsh is a program written in C that implements a subset of features from well-known shells, such as bash.

## Compiling
Navigate to the src directory and use the following command to compile the project: `gcc --std=gnu99 -o smallsh main.c`

Afterwards, type the following to run smallsh: `./smallsh`

## Using smallsh
### 1. The Command Prompt
Starting the program will prompt the user with a `:`, after which the user can input a command.
The general syntax of the command line is:

`command [arg1 arg2 ...] [< input_file] [> output_file] [&]`

Items within the square brackets are optional. If the `&` character appears at the end of the command, execution will take place in the background. If it appears anywhere else, it will be treated as normal text. 

The `>` or `<` keywords followed by a filename indicates standard input or output needs to be redirected. These should appear after all the arguments. Input redirection can appear before or after output redirection.

The shell does not support quoting, piping, or syntax error checking.

### 2. Comments & Blank Lines
The shell allows for blank lines and comments. Any line that begins with `#` is considered a comment and will be ignored. Midline comments are not supported.

### 3. Variable Expansion
Any instance of the characters `$$` within a command get expanded into the process ID of the smallsh instance itself. This is the only variable expansion performed by the shell.

### 4. Built-In Commands
The shell supports three built-in commands: `exit`, `cd`, and `status`. These are the only commands that the shell handles itself, and the rest are passed on to a forked off child process. These commands do not have exit statuses and can only be used in the foreground. 

### 5. Other Commands
The shell allows the execution of any commands that aren't built-in. These commands utilize `fork()`, `execvp()`, and `waitpid()` to run the command. These commands can be run in the background and each child process will terminate after the command is run.

### 6. Signals
The shell has custom signal handlers for SIGINT and SIGTSTP.

SIGINT is sent through the ctrl+C command from the keyboard. This signal will terminate any child process running as a foreground process. Any children running in the background will ignore the SIGINT signal.

SIGTSTP is sent through the ctrl+Z command from the keyboard. Any child processes running in either the foreground or the background will ignore the default behavior of the SIGTSTP process. Instead, when the parent process receives the SIGTSTP signal, it will enter a foreground only mode. In this state, the shell will simply ignore the `&` symbol and run the command in the foreground instead of the background. Entering ctrl+Z to send the SIGTSTP signal again will exit foreground only mode.

## Sample Execution
Here is an example of how smallsh is run:

```$ smallsh
: ls
junk   smallsh    smallsh.c
: ls > junk
: status
exit value 0
: cat junk
junk
smallsh
smallsh.c
: wc < junk > junk2
: wc < junk
       3       3      23
: test -f badfile
: status
exit value 1
: wc < badfile
cannot open badfile for input
: status
exit value 1
: badfile
badfile: no such file or directory
: sleep 5
^Cterminated by signal 2
: status &
terminated by signal 2
: sleep 15 &
background pid is 4923
: ps
  PID TTY          TIME CMD
 4923 pts/0    00:00:00 sleep
 4564 pts/0    00:00:03 bash
 4867 pts/0    00:01:32 smallsh
 4927 pts/0    00:00:00 ps
:
: # that was a blank command line, this is a comment line
:
background pid 4923 is done: exit value 0
: # the background sleep finally finished
: sleep 30 &
background pid is 4941
: kill -15 4941
background pid 4941 is done: terminated by signal 15
: pwd
/nfs/stak/users/chaudhrn/CS344/prog3
: cd
: pwd
/nfs/stak/users/mahdisy
: cd CS344
: pwd
/nfs/stak/users/mahdisy/CS344
: echo 4867
4867
: echo $$
4867
: ^C^Z
Entering foreground-only mode (& is now ignored)
: date
 Mon Jan  2 11:24:33 PST 2017
: sleep 5 &
: date
 Mon Jan  2 11:24:38 PST 2017
: ^Z
Exiting foreground-only mode
: date
 Mon Jan  2 11:24:39 PST 2017
: sleep 5 &
background pid is 4963
: date
 Mon Jan 2 11:24:39 PST 2017
: exit
$```
