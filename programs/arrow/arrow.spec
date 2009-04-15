%define arrow_version 2.0.1

Summary: A mail reading program for X-Windows.
Name: Arrow
Version: %arrow_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Applications/Communications
Source: Arrow_%arrow_version.tar

%description
Arrow is am e-mail program that was written to address the e-mail needs
of users migrating from Windows and the Macintosh to Linux. It currently
supports viewing, composing, and organizing of e-mail, including secure
e-mail via PGP, POP access, and a spell checker.

%prep
%setup

%install

%define arrow_doc_dir /usr/share/doc/Arrow-%arrow_version
%define arrow_lib_dir /usr/lib/arrow

install -m755 arrow /usr/X11R6/bin

mkdir -p %arrow_lib_dir
cp lib/* %arrow_lib_dir

mkdir -p %arrow_doc_dir
cp README FAQ COPYING menu_strings %arrow_doc_dir

mkdir -p %arrow_doc_dir/HTML
cp docs_HTML/* %arrow_doc_dir/HTML

%define redhat_app_path   /usr/share/applications
cp desktop/arrow.desktop  %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory                     %kde_app_path/.directory
cp desktop/new_planet_software.xpm       %kde_icon_path/
cp desktop/new_planet_software_small.xpm %kde_icon_path/mini/new_planet_software.xpm
cp desktop/arrow.desktop                 %kde_app_path/
cp desktop/arrow.xpm                     %kde_icon_path/
cp desktop/arrow_small.xpm               %kde_icon_path/mini/arrow.xpm

mkdir -p %kde_icon_path/hicolor/16x16/apps %kde_icon_path/locolor/16x16/apps
cp desktop/new_planet_software_small.xpm %kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
cp desktop/new_planet_software_small.xpm %kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory               %gnome_app_path/.directory
cp desktop/new_planet_software.xpm %gnome_icon_path/
cp desktop/arrow.desktop           %gnome_app_path/
cp desktop/arrow.xpm               %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/arrow

%post
gunzip %arrow_lib_dir/*.gz

%postun
rm -rf %arrow_lib_dir

%files

%docdir %arrow_doc_dir

/usr/X11R6/bin/arrow
%arrow_lib_dir
%arrow_doc_dir

%redhat_app_path/arrow.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/arrow.xpm
%kde_icon_path/mini/arrow.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/arrow.xpm

/usr/lib/menu/arrow
