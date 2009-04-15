%define systemg_version 2.2.1

Summary: System G provides a graphical interface to UNIX file system.
Name: System_G
Version: %systemg_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: File tools
Source: System_G_%systemg_version.tar

%description
System G provides an elegant, graphical interface to UNIX file system. \
(http://www.newplanetsoftware.com/systemg/)

%prep
%setup

%install

%define systemg_doc_dir /usr/share/doc/System_G-%systemg_version
%define systemg_lib_dir /usr/lib/systemg

install -m755 systemg /usr/X11R6/bin

mkdir -p %systemg_lib_dir
cp lib/* %systemg_lib_dir

mkdir -p %systemg_doc_dir
cp README FAQ COPYING menu_strings %systemg_doc_dir

mkdir -p %systemg_doc_dir/HTML
cp docs_HTML/* %systemg_doc_dir/HTML

%define redhat_app_path    /usr/share/applications
cp desktop/systemg.desktop        %redhat_app_path/
cp desktop/systemg_choose.desktop %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory                     %kde_app_path/.directory
cp desktop/new_planet_software.xpm       %kde_icon_path/
cp desktop/new_planet_software_small.xpm %kde_icon_path/mini/new_planet_software.xpm
cp desktop/systemg.desktop               %kde_app_path/
cp desktop/systemg.xpm                   %kde_icon_path/
cp desktop/systemg_small.xpm             %kde_icon_path/mini/systemg.xpm

mkdir -p %kde_icon_path/hicolor/16x16/apps %kde_icon_path/locolor/16x16/apps
cp desktop/new_planet_software_small.xpm %kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
cp desktop/new_planet_software_small.xpm %kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory               %gnome_app_path/.directory
cp desktop/new_planet_software.xpm %gnome_icon_path/
cp desktop/systemg.desktop         %gnome_app_path/
cp desktop/systemg.xpm             %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/systemg

%post
gunzip %systemg_lib_dir/*.gz

%postun
rm -rf %systemg_lib_dir

%files

%docdir %systemg_doc_dir

/usr/X11R6/bin/systemg
%systemg_lib_dir
%systemg_doc_dir

%redhat_app_path/systemg.desktop
%redhat_app_path/systemg_choose.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/systemg.xpm
%kde_icon_path/mini/systemg.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/systemg.xpm

/usr/lib/menu/systemg
