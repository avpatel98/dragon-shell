# dragonshell

Ayushi Patel, CMPUT 379

## Features

1. Support for built-in commands
    * Since built-in commands do not need a new process to run, first the
    program searches to see if the command is a built-in command.
    * cd
        * chdir()
        * Passes in user issued parameters to change directories
        * Tested by moving backwards through the directories and forward again.
        To make sure that the directory changed, used ls and pwd to check
        contents and see the directory name.
    * pwd
        * getcwd()
        * Obtain string containing current working directory from getcwd() and
        send it to stdout using printf().
        * Tested by moving through directories and seeing if the current working
        directory changes.
    * $PATH
        * Use an array to keep track of path tokens and then print path tokens
        with ':' in between.
        * Tested by adding to path and overwriting path, and printing path to
        see if changes were made.
    * a2path
        * Check the beginning of the string to check if path is to be appended
        to or overwritten.
        * Tested by adding to path and overwriting path, running programs to
        see if some could execute and some could not.
    * exit
        * wait(), kill(), \_exit()
        * Kill the last background process in case it is active, call wait till
        no child processes are left, and exit using exit system call.
        * Tested by typing in exit and Ctrl-D to see if shell exits gracefully.
        Also check to see if there are zombie processes on exit.


2. Launching external programs
    * fork(), getcwd(), execve()
    * Fork to create a child process and then execute the command using execve.
    Execve will be tested with the absolute path if passed in or with the
    defined path and the current working directory to see if it executes. If
    not, error message is printed.
    * Tested by using many standard commands and by running the dragonshell
    through dragonshell


3. Running multiple commands in a single newline
    * After receiving the input, it is split into tokens at ';' and looped over
    and processed individually.
    * Tested by using a list of commands separated by ';' to see if they will
    all execute


4. Running programs in the background
    * close()
    * The parent process does not wait for the child process to terminate and
    suppresses the output to stdout and stderr by closing these streams
    * Tested by running commands that produce output to stdout.


5. Support redirecting the output from one program to a file
    * open(), dup(), dup2(), close()
    * Opens the file, redirects output to the file, and then redirects output
    back to stdout.
    * Tested by running commands that normally produce output to stdout and
    seeing if file is created/overwritten.


6. Support piping the output of one program to another program
    * pipe(), dup(), dup2(), close()
    * Opens the pipe and forks within the child process. The child runs the
    first command while the grandchild runs the second command. The child closes
    the read end of the pipe while the grandchild closes the write end of the
    pipe. If the first command redirects the output to a file, the second
    command receives nothing from the first.
    * Tested by running commands that produce output that can be used as the
    input in another commands.


7. Handling signals
    * signal(), kill()
    * The signal() system call is used to register signal handlers that use the
    kill() system call to forward SIGINT and SIGTSTP signals to the child
    processes
    * Tested by running a background process and a foreground process, and
    issuing Ctrl-C and Ctrl-Z. The ps command was used to see the status of the
    processes.


## Test Cases

In addition to the test cases described in the assignment description, below are
some other test cases that were used:
* find ./ > in.txt | sort
* find ./ | sort > out.txt
* cd .. ; pwd
* sleep 50 * ; sleep 10 ^C
* ls -l -a -F | sort ; find ./ -name "\*.c" > result.out ; ls -lFa
* a2path

## Sources

The lecture notes, the lab slides, the man pages and the TAs' guidance were used
to complete this assignment.
