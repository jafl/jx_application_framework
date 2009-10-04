%define leibnitz_version 2.0.0

Summary: Leibnitz provides electronic paper for calculations and graphs.
Name: Leibnitz
Version: %leibnitz_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Applications/Scientific
Source: Leibnitz_%leibnitz_version.tar

%description
Leibnitz provides electronic paper for calculations and graphs. \
(http://www.newplanetsoftware.com/leibnitz/)

%prep
%setup 

%install

%define leibnitz_doc_dir /usr/share/doc/Leibnitz-%leibnitz_version
%define leibnitz_lib_dir /usr/lib/leibnitz

install -m755 leibnitz /usr/X11R6/bin

mkdir -p %leibnitz_lib_dir
cp lib/* %leibnitz_lib_dir

mkdir -p %leibnitz_doc_dir
cp README FAQ COPYING menu_strings %leibnitz_doc_dir

mkdir -p %leibnitz_doc_dir/HTML
cp docs_HTML/* %leibnitz_doc_dir/HTML

%define redhat_app_path     /usr/share/applications
cp desktop/leibnitz.desktop %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory                     %kde_app_path/.directory
cp desktop/new_planet_software.xpm       %kde_icon_path/
cp desktop/new_planet_software_small.xpm %kde_icon_path/mini/new_planet_software.xpm
cp desktop/leibnitz.desktop              %kde_app_path/
cp desktop/leibnitz.xpm                  %kde_icon_path/
cp desktop/leibnitz_small.xpm            %kde_icon_path/mini/leibnitz.xpm

mkdir -p %kde_icon_path/hicolor/16x16/apps %kde_icon_path/locolor/16x16/apps
cp desktop/new_planet_software_small.xpm %kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
cp desktop/new_planet_software_small.xpm %kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory               %gnome_app_path/.directory
cp desktop/new_planet_software.xpm %gnome_icon_path/
cp desktop/leibnitz.desktop        %gnome_app_path/
cp desktop/leibnitz.xpm            %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/leibnitz

%post
gunzip %leibnitz_lib_dir/*.gz

%postun
rm -rf %leibnitz_lib_dir

%files

%docdir %leibnitz_doc_dir

/usr/X11R6/bin/leibnitz
%leibnitz_lib_dir
%leibnitz_doc_dir

%redhat_app_path/leibnitz.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/leibnitz.xpm
%kde_icon_path/mini/leibnitz.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/leibnitz.xpm

/usr/lib/menu/leibnitz
