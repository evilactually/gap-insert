---
title: "Envar - A better way to manage evironment variables on windows"
description: ""
layout: post
tags: programming scheme
mymeta: "NOW THAT IS AWESOME"
---

### What Is It?

A console application for managing Windows environment variables

### Why Make This?

If you are a developer and you install lots of tools that you need to have
on your PATH, you probably have faced name conflicts, general clutter or
even ran out of space to keep all your paths. On Linux it's not much of  a
problem since you can neatly keep all your variables in bash scripts.  On
Windows however there isn't any easy way to persistently change them without
relying on awkward user interface or tweaking deeply hidden  registry keys.

Envar provides a simple script syntax for setting, creating and removing
environment variables in either system or user scopes. It can be used
directly from command line or as interpreter for script files. All changes
are made on the registry so they are visible between applications and
persist after reboots.

Envar doesn't enforce any particular work-flow, it just merely provides an
interpreter for a script syntax. How you use it is up to you.

### Compiling
    
Requirements: Chicken Scheme MinGW Additional eggs: defstruct, test (only if
you want to run tests)

If you have working chicken scheme under MinGW Msys just run make in bash
shell:

    make 
OR
    make build

This will produce a non portable executable that will depend on  chicken
runtime

To compile a portable executable with chicken runtime included:

    make build-portable

To build and package into archive:

    make package

Other targets:

    make clean

    make tests

### Binary Distribution

[https://dl.dropboxusercontent.com/u/23611520/envar-1.0.tar.gz](https://dl.dropboxusercontent.com/u/23611520/envar-1.0.tar.gz)

### Source Code

[https://github.com/echolot-1/envar](https://github.com/echolot-1/envar)

### Usage

Envar is meant to be used from normal Windows cmd shell, not from Linux-like
bash shell that comes with MinGW(some syntax clashes with bash: $() and
";").

Before you do any damage to your environment variables, save them to file:

    envar -e my_vars_when_my_computer_was_still_working.env

To load them back:

    envar -i my_vars_when_my_computer_was_still_working.env

(Note that the author himself is testing this tutorial from the safety of
(virtual machine)

A very common task is to add some directory to PATH variable. Here's how you
would add "C:\Java\bin" to current user's PATH:

    envar PATH : [$(PATH);C:\Java\bin];

Next, to print a current value of a variable:

    envar PATH;

Your scripts can contain more than one statement. For instance you can
assign and print from the same script:

    envar PATH : [$(PATH);C:\Java\bin]; PATH;

If variable doesn't exists it's automatically created, so don't worry about
causing errors:

    envar MYVAR : [TEST VALUE];

To remove variable from registry, use "-" operator.

    envar MYVAR -;

You can also create an empty variable, with + operator:

    envar MYVAR +;

As you've seen previously $() blocks are used to get values of other
variables and insert them into some other values. It's also safe to
reference a variable in the same script, just like  in any other imperative
language:

    envar A : [Amethist]; B : [ring]; C : [$(A) $(B)];

Windows OS distinguishes between two types of environment variables, user
scope and system scope variables. Until now, you've been working only with
user scope variables. If you wanted to reference system scope, prepend
variable name with "@".

This will print value of system variable PATH:

    envar @PATH;

Same convention is used in $() blocks:

    envar @PATH : [$(@PATH);C:\Java\bin];

If you want to set many variables at the same time it might be convinient to
store your scripts in files and then interpret them with -i option:

    envar.exe -i myvars.env

It's possible to dump all current variables into a script, as was shown
before:

    envar.exe -e myvars.env

This will represent each variable as an assignment statement of appropriate
scope, it should  be safe to use as a back-up and restore tool or as basis
for another script.

If file option is absent Envar will work with standard input/output pipe,
which allows for some interesting possibilities (some of which weren't
actually planned by the author and were only later discovered while writing
this tutorial):

Print exported script directly into terminal:

    envar.exe -e

Pipe out to file:

    envar.exe -e > myvars.env

Pipe out and append to file:

    envar.exe -e >> myvars.env
   
Type script interactively in terminal 
(press Ctrl-Z and Enter to print EOF):

    envar.exe -i

Pipe output from other commands:
    echo "MYVAR : [VALUE];" | envar -i

Pipe in from file:

    envar -i < myscript.env

There's probably more things you can do.

Assignment statement has a little syntax sugar to help you organize complex
values in larger scripts. For instance a typical PATH variable has many
entries and is very convoluted, so it's useful to break-up them into lines:

    @Path : [%SystemRoot%\system32;]
            [%SystemRoot%;]
            [%SystemRoot%\System32\Wbem;]
            [C:\chicken\bin;]
            [C:\Program Files\Git\cmd;];

Lastly, your scripts can contain comments. Anything after # to the end of a
line or string,  which ever happens first is stripped out from a script and
doesn't affect it, in previous example we could comment out some paths for
instance:

    @Path : [%SystemRoot%\system32;]
            [%SystemRoot%;]
           #[%SystemRoot%\System32\Wbem;]
            [C:\chicken\bin;] # chicken 4.8.0.3 
           #[C:\Program Files\Git\cmd;];

Another feature of $() blocks, is that they can be used to execute any any
command in shell and  insert their output into values:

    envar A : [Amethist $(shell echo ring)];

Those are the registry keys affected by Envar, if you need to know:

    User variables: 
        HKEY_CURRENT_USER/Environment

    System variables: 
        SYSTEM_SCOPE/SYSTEM/CurrentControlSet/Control/Session Manager/Environment