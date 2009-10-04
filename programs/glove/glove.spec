%define glove_version 2.0.0

Summary: Data acquisition, manipulation, and analysis program for X-Windows. 
Name: Glove
Version: %glove_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Applications/Scientific
Source: Glove_%glove_version.tar

%description 
Glove was designed as a flexible tool to address a number of issues. We
needed a generic plotting tool, an interface to data acquisition systems,
sophisticated (and correct) statistical analysis, and the ability to easily
and intuitively manipulate the data we had.

%prep
%setup

%install

%define glove_doc_dir /usr/share/doc/Glove-%glove_version
%define glove_lib_dir /usr/lib/glove

install -m755 glove /usr/X11R6/bin

mkdir -p %glove_lib_dir
cp lib/* %glove_lib_dir

mkdir -p %glove_doc_dir
cp README FAQ COPYING menu_strings %glove_doc_dir

mkdir -p %glove_doc_dir/HTML
cp docs_HTML/* %glove_doc_dir/HTML

%define redhat_app_path    /usr/share/applications
cp desktop/glove.desktop   %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory                     %kde_app_path/.directory
cp desktop/new_planet_software.xpm       %kde_icon_path/
cp desktop/new_planet_software_small.xpm %kde_icon_path/mini/new_planet_software.xpm
cp desktop/glove.desktop                 %kde_app_path/
cp desktop/glove.xpm                     %kde_icon_path/
cp desktop/glove_small.xpm               %kde_icon_path/mini/glove.xpm

mkdir -p %kde_icon_path/hicolor/16x16/apps %kde_icon_path/locolor/16x16/apps
cp desktop/new_planet_software_small.xpm %kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
cp desktop/new_planet_software_small.xpm %kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory               %gnome_app_path/.directory
cp desktop/new_planet_software.xpm %gnome_icon_path/
cp desktop/glove.desktop         %gnome_app_path/
cp desktop/glove.xpm             %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/glove

%post
gunzip %glove_lib_dir/*.gz

%postun
rm -rf %glove_lib_dir

%files

%docdir %glove_doc_dir

/usr/X11R6/bin/glove
%glove_lib_dir
%glove_doc_dir

%redhat_app_path/glove.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/glove.xpm
%kde_icon_path/mini/glove.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/glove.xpm

/usr/lib/menu/glove
