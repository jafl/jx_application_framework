Overview
========

When a JX program is started, it looks in /usr/lib/<signature>/string_data
and ~/.<signature>/string_data to find a file name matching the locale or
language.  (The app signature is passed to the JXApplication constructor.)
The files are loaded in the following order:

1. `/usr/lib/<signature>/string_data/no_NO`
1. `/usr/lib/<signature>/string_data/no`
1. `/usr/lib/<signature>/string_data/default`
1. `~/.<signature>/string_data/no_NO`
1. `~/.<signature>/string_data/no`
1. `~/.<signature>/string_data/default`

This ensures that default is available for any text that has not been
translated and that the user's data overrides the system data.

Layouts
=======

All layouts should be compiled using the `--require-obj-names` option to
ensure that all labels have a fixed key for translations.

Fonts
=====

Different languages may require different font sizes.  JX supports the
following configuration values embedded in the string data:

* `NAME::FONT`
* `SIZE::FONT`
* `SIZE::ROWCOLHDR::FONT`
* `NAME::MONO::FONT`
* `SIZE::MONO::FONT`

These control the default fonts and sizes displayed by the program.  Users
can also override these values if they need bigger fonts for easier
reading.

Refer to DYNAMIC_LAYOUT.md for information on how layouts are adjusted to
fit content and font size.

You can test your layouts by running your application with
the `--pseudotranslate` option.
