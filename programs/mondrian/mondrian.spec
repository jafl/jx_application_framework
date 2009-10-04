%define mondrian_version 1.0.0

Summary: Mondrian provides ...
Name: Mondrian
Version: %mondrian_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Development/Tools/GUI Builders
Source: mondrian_%mondrian_version.tar

%description
Mondrian provides ... \
(http://www.newplanetsoftware.com/)

%prep
%setup

%install

%define mondrian_doc_dir /usr/share/doc/mondrian-%mondrian_version
%define mondrian_lib_dir /usr/lib/mondrian

install -m755 mondrian /usr/X11R6/bin

mkdir -p %mondrian_lib_dir
cp lib/* %mondrian_lib_dir

mkdir -p %mondrian_doc_dir
cp README FAQ COPYING menu_strings %mondrian_doc_dir

mkdir -p %mondrian_doc_dir/HTML
cp docs_HTML/* %mondrian_doc_dir/HTML

%define redhat_app_path  /usr/share/applications
cp desktop/mondrian.desktop  %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory  %kde_app_path/.directory
cp desktop/mondrian.desktop  %kde_app_path/
cp desktop/mondrian.xpm  %kde_icon_path/
cp desktop/mondrian_small.xpm  %kde_icon_path/mini/mondrian.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory  %gnome_app_path/.directory
cp desktop/mondrian.desktop  %gnome_app_path/
cp desktop/mondrian.xpm  %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/mondrian

%post
gunzip %mondrian_lib_dir/*.gz

%postun
rm -rf %mondrian_lib_dir

%files

%docdir %mondrian_doc_dir

/usr/X11R6/bin/mondrian
%mondrian_lib_dir
%mondrian_doc_dir

%redhat_app_path/mondrian.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/mondrian.xpm
%kde_icon_path/mini/mondrian.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/mondrian.xpm

/usr/lib/menu/mondrian
