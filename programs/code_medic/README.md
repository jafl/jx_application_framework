Installing
==========

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (medic) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.


Requirements
============

On OS X, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


Notes
=====

Code Medic is a graphical debugging environment for UNIX.

The on-line help explains all the features of the program.

Since we use Code Medic for all our development, we guarantee that it will
continue to improve.  We have a very long list of features that we intend
to implement, so please tell us what features you need, so we will know
which ones on our list to implement first.

Menu items that seem to be permanently disabled are features waiting
to be implemented.


FAQ
===

For general questions, refer to the [Programs FAQ](../README.md).

Configuration
-------------

### When I run Code Medic, the "Choose binary" item on the Debug menu is disabled, and I am unable to get anything to work on the command line.  How do I fix this?

Use the "Change gbd binary..." on the File menu to select the gdb binary that you wish to use. (typically `/usr/bin/gdb`)


Generic usage
-------------

### Why isn't anything displayed in the Current Source window when I load my program into Code Medic?

If you did not compile your program with debugging information, then the debugger will not be able to find your source files.

You will be able to run your program, but Code Medic will stop the program immediately after it starts running, and you will have to use "Continue" on the Debug menu to make it continue.

Neither of these issues are bugs because debugging a program that does not contain debugging information is pointless.


### When I click to set breakpoints in Code Medic, it does nothing.  Why doesn't it work?

In some situations, gcc will not store the path to your source files in the executable, and Code Medic will be unable to find your source files unless you run Code Medic in the directory that contains the source.


### How do I specify arguments to the underlying gdb process?

Create a script that simply runs gdb with the desired arguments.  Then tell Code Medic to use this script as its "gdb binary".


### How do I specify a startup script for the underlying gdb process?

Put the script in `~/.gdbinit`.  (This is the normal way to provide a startup script for gdb.  Code Medic does not interfere with it.)


### Why does Code Medic require me to open a separate copy of each source file?  Can't it just use the Code Crusader windows?

The main reason for not doing it this way is that we don't think the text display in Code Medic should be editable.  If the displayed source code gets out of sync with the debugging info compiled in the binary, the result is endless confusion.  Code Medic therefore displays the version of the source code on saved on disk which corresponds to the binary, and Code Crusader displays the (unsaved) edited version that you intend to compile next.


### Why do I have to open the source file in Code Medic just to set a breakpoint?  Can't I just use the Code Crusader window?

You can set breakpoints with the `-b` command line option.  In Code Crusader, you can use the "Set breakpoint in debugger" item on the Commands menu.


### Why are the breakpoints that I set in my shared libraries disabled when I re-run my program?

This appears to be a bug in gdb itself.  It also only seems to happen in some shared libraries, not in others.


### Can I debug more than one program at a time with Code Medic?

Yes.  After starting Code Medic, delete `/tmp/.medic_MDI_*`.  Now run Code Medic again, and you will get a new set of windows.  Note that you will only be able to send commands from Code Crusader to the *last* copy of Code Medic that you started, because the above file (actually a socket) is used to send the commands.


Command Line window
-------------------

### Why doesn't the text printed by my program appear in the Command Line window?

This problem only occurs if you are using `printf()`.  The solution is to call `fflush(stdout)` after the call to `printf()`.  (The problem appears to be the result of Code Medic not running gdb with a tty.  We do this because tty's have too many problems of their own.)

The problem does not occur when using `cout`, even if you do not use `endl` to force a flush of the stream.


### How do I send text to my program?

Simply type it in the input area at the right of the (gdb) prompt.  (Do not type it in the output area where your program's output appears.)


Variables window
----------------

### Why does the Variables window display all the data in my struct on a single line?

Congratulations.  You have found a bug in our parser.  Please [send me](http://johnlindal.wix.com/aboutme) the output from running `print *your_expression*` on the gdb command line and we will fix the problem.
