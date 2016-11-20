fdesign

`fdesign` is the widget layout editor written for use with the XForms
library.

There are two important points to keep in mind when editing `fdesign`
files.  First, the widgets that `fdesign` provides are those that XForms
provides.  These are not identical to the JX widgets.  The file `class_map`
shows which XForms widgets map to JX widgets.  The first column is the type
selected in the `fdesign` control window.  The second column is the type
selected in the Options dialog window obtained by right-double-clicking on
the widget.

jfdesign

This script file runs `fdesign` with the options that are appropriate for
generating an `.fd` file for use with `jxlayout`.  The script assumes that
`fdesign` is on your search path.

jxlayout

`jxlayout` was written to generate code for JX from an `fdesign` file.  It
is a simple, text-based program that generates C++ code to construct the
Widgets and arrange them in the Window.  The best way to understand how
`jxlayout` works is to study `testjx.fd` and the associated code.

Since each Window should have its own custom WindowDirector, Document, or
DialogDirector, the creation and layout code is only a small part of the
total code required.  The rest of the code must be written by hand.  As an
example, a class `MyDialog` derived from `DialogDirector` requires at least
a constructor, a destructor, and methods for retrieving the information
when the Window is closed.  In order to retrieve this information, the
relevant objects in the window must be declared as instance variables.
This, in turn, requires that the header file for `MyDialog` be directly
modified by `jxlayout`.  In addition, it is most convenient if the source
code can also be placed directly in the source file for MyDialog.  This is
accomplished by delimiting a region of the source and header files for
explicit use by `jxlayout`.  The region begins with `// begin JXLayout` and
ends with `// end JXLayout`.  `jxlayout` replaces everything inside this
region with code generated from the `.fd` file.  This code can, of course,
be part of a larger function.  To create additional Widgets (e.g. menus) or
adjust the initial values of Widgets constructed by `jxlayout`, simply
append this code to the function after the `// end JXLayout` delimiter.

Unnamed objects in the `.fd` file are assigned unique, arbitrary names in
the resulting C++ code.  Named objects are considered to be instance
variables and are declared in the header file.  If one needs to refer to an
object inside the function, but nowhere else, one can enclose the name in
parentheses (e.g.  `(okButton)`).  Thus, unlike unnamed objects, the name
will not be arbitrary, but it will be declared locally in the function
instead of at class scope in the header file.  If one needs to use a local
variable that has already been declared, one can enclose the name in
brackets.  This is primarily useful for arrays (e.g. `<radioButton[2]>`).

`class_map` defines how objects that `fdesign` understands map to JX
classes.  Classes that are not listed in this file and custom classes
derived directly from `JXWidget` can be included in the `.fd` file by
creating an `fdesign` box object of type `NO_BOX` and setting the label to
be the name of the JX class.  The most common examples are `JXMenuBar` and
`JXScrollbarSet`.  Custom classes usually require extra arguments in the
constructor.  This can be handled by arranging these arguments to be first
for the constructor, and then using `MyWidget(arg1,arg2,` as the label.
`jxlayout` will append the standard arguments in order to complete the
constructor.  (Such code will obviously not compile unless arg1 and arg2
are defined before the `// begin JXLayout` delimiter.)

A Widget's container is calculated by finding the smallest Widget that has
already been encountered in the `.fd` file and that contains the new
Widget.  `jxlayout` translates the XForms gravity specifications into JX
resizing options.  `NoGravity` translates to elastic resizing.  To fix an
edge, add the corresponding compass direction to `NWGravity`.

The code generated in the region delimited by `// begin JXLayout` and `//
end JXLayout` always starts with the creation of a `JXWindow` object.  If
you want to create a layout for the inside of a `JXWidget` (e.g. each page
of a `JXCardFile`), you can create a new form in `fdesign`, give it the
same name, and then specify a different tag to look for in the source and
header files by appending "__tag__encl" to the name of the form.  As an
example, the form called `MyDirector` will generate code in
`MyDirector.cpp` and `MyDirector.h` between `// begin JXLayout` and `// end
JXLayout`, and all Widgets will be constructed with window as their
enclosure.  The form called `MyDirector__sub1__myWidget` will generate code
in `MyDirector.cpp` and `MyDirector.h` between `// begin sub1` and `// end
sub1`, and all Widgets will be constructed with `myWidget` as their
enclosure.

The callback argument field in `fdesign` is primarily used to set the ID of
RadioButtons, but it can also be used to pass initial arguments to any
other constructor.

Changes from previous versions

4.0.0

Updated to work with latest `fdesign` file format: 15000

Switched from `--tag--encl` to `__tag_encl` because the latest version of
`fdesign` is picker.

3.2.0

Updated defaults so code path is `./code` and code suffix is `.cpp`

3.1.0

Automatically detects whether each file is indented with spaces or tabs,
and then applies the same indenting to the generated code.

3.0.0

Added `need_string_list` to allow configuring which classes require a
string as the first ctor arg.

If `JXStaticText` height <= 20px and label alignment is not top, then it is
set to not wrap text and the text is vertically centered.

Removed option to specify additional `JXWindow` constructor arguments.
`JXWindow` no longer accepts `JXColormap`, and the window title should be
loaded via `JGetString()`.

Updated code generator to no longer insert call to `SetWindow()`.
`JXWindow` automatically calls `JXWindowDirector::SetWindow()`.

Updated to parse sliders correctly.

2.1.1

If you specify constructor arguments for `JXStaticText`, `jxlayout` no
longer appends a call to `JGetString()`.

2.1.0

If a layout's enclosure is not a `JXWindow`, then the enclosure is adjusted
to match the size of the layout before the widgets are created and is put
back to its original size afterwards.  This removes the requirement that
sub-layouts must be sized to fit their destination.

Fixed bug so it complains if directory for string database files does not
exist.

2.0.0

All strings are placed in `JStringManager` database files, and all code is
generated to use `JStringManager`.  To allow regeneration of individual
layouts, the strings for each layout are placed in separate files.  The
directory in which to place these files is specified with the `-sp` option.

Changed the gravity to sizing mapping to use only `NWGravity` since it is
much more convenient to use a single menu.  Use the `convert_gravity`
script to update your existing `.fd` files.

The "callback argument" specified in the options dialog in `fdesign` can
now be used to specify the initial constructor arguments to any class.

Since the `egcs` thunks bug appears to be here to stay, `need_create_list`
is now used to determine which widgets need to be constructed via
`Create()`.  You can add your own custom widgets to this list whenever the
thunks bug bites you.

Fixed bug so unnamed objects from different layouts in the same class no
longer use the same names.

Renamed `-c` option to `--choose` for consistency with `makemake`.

Added option `--post-cmd` to specify command to execute after all files
have been modified.  This is primarily for instructing your editor to
reload the contents of the files.  The command is not run if no files were
modified.

1.0.6

If the new header file is identical to the old one, it is left untouched to
avoid unnecessary recompilation.
