Implementation of Linux Shell in C

Linux Shell
===========

This is implementation of Linux Shell using C Programming.

Project Description:
    
    C program that implements a shell called utdash.
    
    linux commands will be executed using fork/exec.
    
    Shell will print a prompt of your choice to /dev/tty, and wait for input from /dev/tty.
    
    It allows command line which contain whitespace delimited arguments. These arguments will be passed to the command when it is executed. 
    
    It accepts redirection commands in one of three formats: 

	 *	command arguments > filename 

	 *	command arguments >> filename 

	 *	command arguments < filename 

Project Files:

  utdash.c : source file of the project

  Makefile : Makefile to compile the project

Instructions to run the project: 

  $make

  $./utdash
  
Example:

Utdash$ls -l

total 31

-rw------- 1 jxk131030 sn   137 Aug 10 20:43 Makefile

-rw------- 1 jxk131030 sn   703 Aug 10 20:50 README.md

-rwx--x--x 1 jxk131030 sn 18741 Aug 10 20:54 utdash

-rw------- 1 jxk131030 sn  8552 Aug 10 20:44 utdash.c

Utdash$

	 
