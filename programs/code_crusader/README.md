Installing
==========

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the programs (jcc, makemake, ctags, dirdiff) need to be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.

The project_templates and text_templates directories should be copied to `~/.jxcb/`

The macros files can be loaded into Code Crusader by using the "Macros..." item on the Preferences menu in any Editor window.

The CPM_rules files can be loaded into Code Crusader by using the "Clean paragraph margin rules..." item on the Preferences menu in any Editor window.


Requirements
============

On OS X, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


Notes
=====

Code Crusader is a graphical development environment for UNIX.

The on-line help explains all the features of the program.

Since we use Code Crusader for all our development, we guarantee that it will continue to improve.  We have a very long list of features that we intend to implement, so please tell us what features you need, so we will know which ones on our list to implement first.

Menu items that seem to be permanently disabled are features waiting to be implemented.

The C++ parser in Code Crusader is home-grown and therefore not up to parsing all valid C++ header files.  It parses JX correctly, but it can be confused by g++.  This does not cause serious problems, however, because it usually only affects the functions that Code Crusader thinks are implemented by particular classes.

FAQ
===

For general questions, refer to the [Programs FAQ](../README.md).

Configuration
-------------

### When I start Code Crusader, I get a Save dialog asking to save a project.  How do I avoid this?

See the answer to the next question.


### I set the preferences to use an external editor and now I can't get it to do anything.  How do I get out of this?

Run it with a project file as a command line argument and then change the settings in the Miscellaneous Preferences dialog.

If you don't have any project files, then you must have been using it only as an editor.  In this case, run it with the command line option `--turn-off-external-editor` and then change the settings in the Miscellaneous Preferences dialog.


Editor
------

### How do I create a file that I can use as a template for creating many files
with a similar structure?  (e.g. a skeleton source file for programming)

Create the file and then use the "Save as template..." item on the File menu.


### Can I display all the text files in a single window to reduce the clutter on my screen?

Yes.  Right-click on the docktab at the left of the menu bar and select "Dock all windows in default configuration."


### Why doesn't Code Crusader support emacs' indenting style?

Code Crusader does not aim to enforce any particular coding standard.  Its auto-indent feature only remembers the indent from the previous line.

You can use Character Actions (in the Macro Editor dialog) to enforce any indenting style you want.


### Why isn't context-sensitive highlighting working on my files?

Check that the file suffix that you use is listed in the File Types dialog window and is set to the appropriate type.


### Why aren't comments in preprocessor directives styled correctly?

It's too much trouble for too little gain.  Currently, they are styled as part of the preprocessor directive, and this is technically correct, because the preprocessor is the one which sees them and removes them.


### Why doesn't the Functions menu appear in my source files?

Check that the file suffix that you use is listed in the File Types dialog window and is set to the appropriate type.  You also need to use the version of ctags included with Code Crusader, and it needs to be first on your execution path.  If neither of these is the problem, please [contact me](http://johnlindal.wix.com/aboutme).


### How do I print a file so the pages come out in reverse order?

When printing plain text, simply turn on the option in the Page Setup dialog.  You can also do it from the command line:

    a2ps --output=- *file_name* | psselect -r | lpr

To print styled (PostScript) pages in reverse order, use:

    psselect -r *file_name* | lpr


### How do I print a file with the syntax highlighting?

One option is to use the "Print with styles..." item on the File menu.

The Linux utility `c2ps` can also be used.  A script such as:

    #!/bin/sh
    c2ps -t 3 -x -b -f 8 -d 0.80 $1 | lpr

can be run from the command line with the name of the file to be printed.


### Why isn't my favorite font available in the Editor Preferences dialog?

Since it is primarily designed for editing code, Code Crusader only allows monospace fonts.  If you have a monospace font that isn't listed, please [contact me](http://johnlindal.wix.com/aboutme).


### Can I use Asian (multibyte) languages in Code Crusader?

Not yet.  Someday I hope to add support for Unicode to the underlying library (**JX**).


Project
-------

### Why doesn't it save my project file?

Make sure that the `.jcc` file is writable.  Code Crusader does not save the project file if it is write protected because this would annoy users who share the project file over CVS.


### When I try to compile, it complains that it can't find `makemake`.  How do I fix this?

`makemake` is part of the standard distribution.  Make sure that it is on your execution path.


### When I try to compile, it complains that it can't find `make`.  How do I fix this?

`make` may be called something else on your machine.  Try creating a symbolic link `~/bin/make` that points to your version of `make`, and make sure that `~/bin/` is at the front of your execution path.


### When I try to compile, it complains that it can't find my header files.  How do I fix this?

You need to add the directories to `SEARCHDIRS` in Make.header.

The directories that you specify in the Search Paths dialog are not passed to the compiler, both because different compilers require different command line options about which Code Crusader cannot know, and because you may want to include files in the C++ class tree that are not directly part of your program or visa versa.


### When I try to compile my C++ program, I get linker errors.  How do I fix this?

You have to edit `Make.header`.  Check the on-line tutorial for more information.


### How do I compile code for Qt/KDE?

Create a project that uses `qmake`.


### How do I configure the project to work with GTK+?

Change Make.header to do the following:

    CFLAGS    := ${shell gtk-config --cflags}
    LOADLIBES := ${shell gtk-config --libs}


### My program requires `bison`.  How do I tell `make` to compile the source files that depend on `lex.yy.h` *after* running `bison`?

Create a build target that runs `makecheck my_bison_file.y` before performing the actual build.

It is also useful to modify the default rule for .y to the following:

    %.c : %.y
        @if test -e $*.tab.h; then mv $*.tab.h $*.tab.h_old; fi
        @if ! ${YACC} ${YFLAGS} $&lt;; then \
           if test -e $*.tab.h_old; then mv $*.tab.h_old $*.tab.h; fi; \
           false; fi
        @if test -e $*.tab.h_old -a -z "`diff $*.tab.h $*.tab.h_old 2&gt; /dev/null`"; \
         then mv $*.tab.h_old $*.tab.h; fi

This avoids changing the modification time of the .tab.h file unless its content changes, thereby avoiding unnecessary recompilation of other source files.


### How do I put my source files in a directory other than the source directory?

Please refer to [this example](multi_version.md).


### Can I build multiple versions of a program (e.g. debug and release) or library (e.g. static and shared) from a single project?

Yes.  Refer to [this explanation](multi_version.md).


### Can I build several different programs or libraries from a single project?

Not yet.

If you have one program and several libraries, read the on-line help to see how to link the libraries' project files to the program's project file.

If you have several programs, you could put each in a separate subdirectory and have a top level project that contains the subproject `.jcc` files and a Makefile that simply builds each subdirectory (makemake + make).  This is unfortunately rather ugly, which is why we plan to implement the feature correctly so you can use a single project file.


### When I run my command line program, the output from my program's print statements does not appear in the Output window.  How do I fix this?

You need to flush the program's output buffer.  In C, use `fflush(stdout)`.  In C++, use `cout << flush`.


### When I try to use CVS or SVN, it hangs because ssh cannot ask for my password.  What can I do about this?

First, set the environment variable `SSH_ASKPASS` to point to an AskPass program, like `/usr/libexec/openssh/x11-ssh-askpass`, `/usr/libexec/openssh/gnome-ssh-askpass`, or [`nps-ssh-askpass`](http://sourceforge.net/p/nps-ssh-askpass/).  Then kill the terminal, if any, from which you ran Code Crusader.  This will force SSH to run the AskPass program to get your password.  [`nps-ssh-askpass`](http://sourceforge.net/p/nps-ssh-askpass/) has the advantage that it runs on Linux, Mac OS X, and Cygwin.


C++ class tree
--------------

### Why don't my C++ classes show up in the class tree?

Check that the header file suffix that you use is listed in the File Types dialog window and is set to the type "C/C++ header".  Then check that you have defined all preprocessor symbols used in your class definitions.  The preprocessor definitions are accessed from the Tree menu.  If neither of these is the problem, please [contact me](http://johnlindal.wix.com/aboutme).


### Why don't Qt classes show up in the class tree?

Define `Q_EXPORT`, `Q_ENUMS()`, and `Q_PROPERTY()` to be removed (replaced by the empty string).


### Why can't Code Crusader find my source files from the class tree?

Check that the source file suffix that you use is listed in the File Types dialog window and is set to the type "C/C++ source".  Then check that the directory containing your source files is listed in the Search Paths dialog.  If none of these issues is the problem, please [contact me](http://johnlindal.wix.com/aboutme).


Java class tree
---------------

### Why don't my Java classes show up in the class tree?

Check that the header file suffix that you use is listed in the File Types dialog window and is set to the type "Java".  Then check that you have defined all preprocessor symbols used in your class definitions.  The preprocessor definitions are accessed from the Tree menu.  If neither of these is the problem, please [contact me](http://johnlindal.wix.com/aboutme).


### Why can't Code Crusader find my source files from the class tree?

Check that the source file suffix that you use is listed in the File Types dialog window and is set to the type "Java".  Then check that the directory containing your source files is listed in the Search Paths dialog.  If none of these issues is the problem, please [contact me](http://johnlindal.wix.com/aboutme).


### Mouse and key bindings

### Why doesn't Meta-click work in the C/C++ Styles and HTML Styles dialogs?

### Why doesn't Meta-drag initiate Drag-and-Drop in the Editor windows?

Your window manager is probably grabbing the mouse click so Code Crusader never gets it.  Most window managers provide a configuration file that you can change so the mouse click isn't grabbed.  If your window manager doesn't allow this, ask the developers to add this feature!

If you are using Enlightenment, holding down both Meta and Control will get around this problem.

We agree that Code Crusader should allow you to change the bindings.  This is on the list of things to do.  It is worse when the window manager isn't configurable, however, because then **all** other programs suffer.


### Why don't the Home/End keys move the caret to the beginning/end of the line?

You can change this behavior in the Editor Preferences dialog.

For those of you who don't care about the theory, the on-line help explains all the key shortcuts, including that Meta-left/right-arrow moves the caret to the beginning/end of the line.

The reason that this is this default is simple consistency.  Home/End scrolls every other widget that has a scrollbar.  The arrow keys are already designated as the keys that move the caret, so modified arrow keys should provide useful shortcuts.

Also, since the Page Up/Down keys affect the scrollbar, the Home/End keys should also affect the scrollbar, since they are grouped together, at least on Macintosh&reg; and PC keyboards.

As usual, the Macintosh&reg; got it right and Windows&reg;/Motif got it wrong.


### Why doesn't the caret follow when I scroll the text?

You can change this behavior in the Editor Preferences dialog.

For those of you who don't care about the theory, the on-line help explains all the key shortcuts, including that Ctrl-up/down-arrow moves the caret to the top/bottom of the visible text.

The reason that this is the default is that it can be very useful to have the caret stay where it is.  If you scroll somewhere (e.g. via a scrolltab!) to peek at something, you can get back to where you were by simply pressing any arrow key.
