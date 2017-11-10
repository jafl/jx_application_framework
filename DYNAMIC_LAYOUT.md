Overview
========

In order to accommodate both differences in content length from translation
and differences in font size from different operating systems, JX
automatically adjusts every window's layout.

Technical Implementation
========================

When a JX window is created, it registers a `JXUrgentTask` to adjust the
layout after all the widgets have been created.  This is done in two
passes:  first horizontally and then vertically.  The algorithm tries to
build a grid that fits all the widgets in the layout.  If a layout cannot
be fit into a grid, it will print an error message and not modify the
layout.  In this case, you may need to enclose a group of widgets inside a
`JXWidgetSet` and call `SetNeedsInternalFTC()` on container.  You can
create a `JXWidgetSet` in `jxlayout` by placing a shadow box.

You can debug layouts by running your application with either
`--debug-ftc-horiz` or `--debug-ftc-vert`.  This renders the grid layout,
so you can see what the algorithm is doing.  Unfortunately, this grid
blocks all mouse clicks to the window.  If you need to debug a layout that
cannot be reached via keystrokes, run your application normally and then
hold down Control-Meta-Hyper to turn on horizontal debugging when the
window is created.  Control-Meta-Hyper-Shift turns on vertical debugging.
