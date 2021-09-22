FAQ
===

General installation
--------------------

### How do I install the package that I downloaded?

If you downloaded the **`.tgz`** version, unpack it on the command line with `tar -xzf file_name.tgz`, switch to `root` (easiest via `sudo su -`), and then run the `install` script included in the package.  (The `uninstall` script will remove the program.)

If you downloaded the **`.rpm`** version, unpack it on the command line with `tar -xzf file_name.tgz`, switch to `root` (easiest via `sudo su -`), and then install the package(s) with `rpm -i --force file_name.rpm`.  If the installation fails because it cannot find the appropriate version of libstdc++, run `rpm -i --force --nodeps *.rpm`.

If you downloaded the **`.deb`** version, switch to `root` (easiest via `sudo su -`), and then install the package(s) with `dpkg -i file_name.deb`.  If the `.deb` does not work with your version of Debian, use the tar package instead.


### I just downloaded and installed the rpm.  Where is the binary?

Run `rpm -qlp` on the `.rpm` file to list the files that it installed.


### The binary that I downloaded doesn't work on my system.  Does this mean that I have to obtain the source?

If you have MacOS, upgrade to the latest version of [XQuartz](http://www.xquartz.org) (X11 for Mac).

If you are missing a system library (e.g. `libucb.so` on Solaris), then search for it on your system (easiest via `find`) and add its directory to your `LD_LIBRARY_PATH` environment variable.

Otherwise, the answer is yes.  Please [contact me](http://johnlindal.wix.com/aboutme).


### When I start the program, it crashes with the message "The X server does not have any PostScript fonts at all, not even Helvetica."  How do I fix this?

Install the `xorg-x11-fonts-75dpi` package.


### When I start the program, it crashes before I even get a window.  How do I fix this?

Try deleting the file `~/._program_name_.pref` and the directory `~/.jx/`.  Also, make sure that you have write permission on your home directory.  If this doesn't fix the problem, please [contact me](http://johnlindal.wix.com/aboutme).


### When I start the program, it complains that the preferences cannot be used.  How do I fix this?

This is normally caused by not having write permissions to your home directory or to the `._program_name_.pref` file inside your home directory.


### When I run the program, it is very slow.  How do I fix this?

Try hiding the toolbar below the menu in Editor windows.  Select "Toolbar buttons..." from the Preference menu and uncheck "Show toolbar".  This will produce a dramatic speed increase if you work on an NFS-mounted volume.


### When I start the program using MI/X on Windows&reg;, it crashes in JXFontManager.cc.  How do I fix this?

Install the fonts Helvetica and Courier.  (None of the defaults fonts provided by
MI/X are PostScript fonts.)


### Why can't you make the installation process as easy as it is on Mac and Windows?

We would like to, but web browsers on UNIX don't allow you to automatically run a program after you have downloaded it.  This is primarily for security reasons.  You have to set the executable flag on the program after you have downloaded it.

You also have to switch to `root` before you can install any RPM packages.  (If you are running your web browser as `root`, you are really asking for trouble.  For security reasons, you should never do anything as `root` unless you absolutely must.)

In addition, browsers on Windows think that `.rpm` files are Real Player files, so we have to use the `.tgz` suffix to allow people to download our software to a Windows computer (from which they then transfer the file to their UNIX computer).

That said, we are always open to suggestions for improving and simplifying the installation process!


General configuration
---------------------

### Where is the Meta key?

On Linux-Intel, it is usually labeled Alt.  The default on Debian Linux running on a UK keyboard apparently maps Meta to the Windows Logo key.  On other UNIX machines, Meta is sometimes labelled with a diamond.  If you can't find it by trying the various strangely labelled keys on your keyboard, please ask your system administrator.


### Why don't the Meta menu shortcuts work?

Add the following command to your `~/.xinitrc` file:

	xmodmap -e "keysym Alt_L = Meta_L Alt_L"

If you are using TightVNC, run `vncserver` with the `-compatiblekbd` option.


### Why doesn't Meta-click work?

Your window manager is probably grabbing the mouse click so the program never gets it.  Most window managers provide a configuration file that you can change so the mouse click isn't grabbed.  If your window manager doesn't allow this, ask the developers to add this feature!

If you are using Enlightenment, holding down both Meta and Control will get around this problem.

With KDE, go to their control panel LookNFeel/Window Behavior/Actions and set "Modifier Key + Left button" to "nothing".


### Why does the backspace key delete the character to the right instead of the character to the left?

This is a problem with your X server configuration.  Somehow, your backspace key is generating Delete instead of BackSpace.  On UNIX, the problem can be solved by adding

	xmodmap -e "keycode 22 = BackSpace"

to your `~/.xinitrc` file.  If you are using an X server for Microsoft Windows&reg;, please consult the appropriate documentation.


### Why do I see messages like `sh: warning: setlocale: LC_ALL: cannot change locale (en_US)`?

Some distributions, notably Ubuntu, have this problem.  Put the following in `~/.xsessionrc`:

	export LC_ALL=C

Note:  Unfortunately, Ubuntu 9.10 ignores `~/.xsessionrc`.  To work around this, add the above line to your `~/.profile` (assuming you are using the Bourne shell) and then log out and log back in.  If you are using a variant of `csh`, then put `setenv LC_ALL C` in `~/.login`.


### When I use XVision, the Meta key doesn't work.  How do I fix this?

Make a copy of the appropriate XVision key mapping file in <XVision installation directory>\System\keymap and then modify it to change the description on the first line (DESC) and to map the desired Windows virtual key code (usually VK_MENU) to the X keycode "0xffe7 (Meta_L)".  Save this file and then run <XVision installation directory>\Xkg.exe.  From this application's File menu, open the key mapping file you just created.  This compiles your .txt file into an .xkg file.  Choose "Save" from the File menu and quit the application.  Now open the XVision properties dialog and choose the Devices tab.  Your new keymap should appear in the Keyboard Map drop down list, based on the change you made to the first line of the .txt file.  Select it and then let XVision restart.

Thanks to Ian Wakeling for figuring this out!


### On MacOS, I don't want to display the location of files using System G.  How do I get it to use Finder?

[This script](http://yost.com/computers/MacStuff/reveal/index.html) will display the file in the Finder.  Update the \"Shell cmd to view file in file manager\" command in the Web Browser Preferences dialog to execute the script.


### When I type in an input area in a dialog window, buttons and checkboxes get activated.  How do I avoid this?

We are not sure what causes this.  We think that it may be the result of strange bugs in particular window managers.  What is happening is that the program is somehow being told that the Meta key is pressed.  Apparently, if you hold down the Meta key, this toggles the setting, i.e., turning the Meta key off, so that you can type normal characters.  The best solution is to switch to a different window manager.


Window manager issues
---------------------

### Which window manager do you recommend using?

All programs that are written with the **JX** Application Framework will work with any window manager, except for the bugs mentioned below.  I prefer [fvwm2](http://www.fvwm.org/) because it supports icons for iconfied windows.  This allows Drag-and-Drop onto iconfied windows as well as visible ones.

Many people consider fvwm2 to be too raw, however.  A good alternative is Openbox, when configured to run gnome-panel at startup.  To do this, create `~/.config/openbox/autostart.sh` with the following content:

	. $GLOBALAUTOSTART
	(sleep 2 && gnome-panel) &


### Why do my windows keep appearing in random places, even after I use the "Save window size as default" option?

Your window manager is ignoring the program's attempts to place the windows.  We know how to fix this for the following window managers:

* fvwm : Comment out "NoPPosition" from `~/.fvwmrc`
* fvwm2 : Use "UsePPosition" instead of "NoPPosition" in `~/.fvwm2rc`
* sawfish : Select the Placement item for Sawfish in the Gnome control panel and change the Method to "None".
* twm : Set "UsePPosition on" in `~/.twmrc`


### Why do I get the error:

	Error detected in JXWindow::HandleMapNotify():
	Your window manager is not setting the WM_STATE property correctly!

As the error says, the problem is that your window manager is not fully compliant because it doesn't set the WM_STATE property on every window.  Our only advice is to notify the developers and then switch to a window manager that works.


### Why do the windows move when I use the Windows or Files menu or the tabs in a dock?

Every window manager seems to have a different way of placing windows.  The program attempts to deduce the behavior at run time, so you can use it with any window manager.

On Ubuntu, go to Systems &rarr; Preference &rarr; Appearance &rarr; Visual Effects and then select "None".

If you have trouble with other window managers, please [contact me](http://johnlindal.wix.com/aboutme).


Miscellaneous
-------------

### Can I change the default font size?

You can change the default menu font size by creating the file `~/.jx/menu_font` with the following format:

	font_name
	font_size


### Does the program support themes?

No.  We have a design, but there are still unresolved issues.  If you want to see it happen, please consider volunteering!


### Have you considered porting the program to Gnome/GTK+ or KDE/Qt?

No.  The program already runs correctly in both environments, and porting it would require a complete rewrite from scratch.
