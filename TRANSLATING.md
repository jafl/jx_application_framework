Overview
========

When a JX program is started, it looks in /usr/lib/jx/string_data and ~/.jx/string_data to find a file name matching the app signature passed to the JXApplication constructor.  It also looks for a file with the app signature followed by the value of the LANG environment variable.  It loads them in the following order:

1. `/usr/lib/jx/string_data`
1. `/usr/lib/jx/string_data_no_NO.UTF-8`
1. `~/.jx/string_data`
1. `~/.jx/string_data_no_NO.UTF-8`

This ensures that default text is available for any text that has not been translated and that the user's data overrides the system data.

Layouts
=======

All layouts should be compiled using the `--require-obj-names` option to ensure that all labels have a fixed key for translations.

Fonts
=====

Different languages may require different font sizes.  JX supports the following configuration values embedded in the string data:

* `NAME::FONT`
* `SIZE::FONT`
* `SIZE::ROWCOLHDR::FONT`
* `NAME::MONO::FONT`
* `SIZE::MONO::FONT`

These control the default fonts and sizes displayed by the program.  Users can also override these values if they need bigger fonts for easier reading.
