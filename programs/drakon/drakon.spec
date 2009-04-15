%define drakon_version 1.1.0

Summary: Drakon provides a simple way to manage UNIX processes.
Name: Drakon
Version: %drakon_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: File tools
Source: Drakon_%drakon_version.tar

%description
Drakon provides a simple way to manage UNIX processes. \
(http://www.newplanetsoftware.com/drakon/)

%prep
%setup

%install

%define drakon_doc_dir /usr/share/doc/Drakon-%drakon_version
%define drakon_lib_dir /usr/lib/drakon

install -m755 drakon /usr/X11R6/bin

mkdir -p %drakon_lib_dir
cp lib/* %drakon_lib_dir

mkdir -p %drakon_doc_dir
cp README FAQ COPYING menu_strings %drakon_doc_dir

mkdir -p %drakon_doc_dir/HTML
cp docs_HTML/* %drakon_doc_dir/HTML

%define redhat_app_path    /usr/share/applications
cp desktop/drakon.desktop  %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory                     %kde_app_path/.directory
cp desktop/new_planet_software.xpm       %kde_icon_path/
cp desktop/new_planet_software_small.xpm %kde_icon_path/mini/new_planet_software.xpm
cp desktop/drakon.desktop                %kde_app_path/
cp desktop/drakon.xpm                    %kde_icon_path/
cp desktop/drakon_small.xpm              %kde_icon_path/mini/drakon.xpm

mkdir -p %kde_icon_path/hicolor/16x16/apps %kde_icon_path/locolor/16x16/apps
cp desktop/new_planet_software_small.xpm %kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
cp desktop/new_planet_software_small.xpm %kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory               %gnome_app_path/.directory
cp desktop/new_planet_software.xpm %gnome_icon_path/
cp desktop/drakon.desktop          %gnome_app_path/
cp desktop/drakon.xpm              %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/drakon

%post
gunzip %drakon_lib_dir/*.gz

%postun
rm -rf %drakon_lib_dir

%files

%docdir %drakon_doc_dir

/usr/X11R6/bin/drakon
%drakon_lib_dir
%drakon_doc_dir

%redhat_app_path/drakon.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/drakon.xpm
%kde_icon_path/mini/drakon.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/drakon.xpm

/usr/lib/menu/drakon
