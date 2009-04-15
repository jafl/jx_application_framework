%define <Binary>_version <Version>

Summary: <Program> provides ...
Name: <Program>
Version: %<Binary>_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: ...
Group: ...
Source: <Binary>_%<Binary>_version.tar

%description
<Program> provides ... \
(<URL>)

%prep
%setup

%install

%define <Binary>_doc_dir /usr/share/doc/<Binary>-%<Binary>_version
%define <Binary>_lib_dir /usr/lib/<Binary>

install -m755 <Binary> /usr/X11R6/bin

mkdir -p %<Binary>_lib_dir
cp lib/* %<Binary>_lib_dir

mkdir -p %<Binary>_doc_dir
cp README FAQ COPYING menu_strings %<Binary>_doc_dir

mkdir -p %<Binary>_doc_dir/HTML
cp docs_HTML/* %<Binary>_doc_dir/HTML

%define redhat_app_path  /usr/share/applications
cp desktop/<Binary>.desktop  %redhat_app_path/

%define kde_app_path  /usr/share/applnk/NPS_Utilities
%define kde_icon_path /usr/share/icons
mkdir -p %kde_app_path %kde_icon_path/mini
cp desktop/directory  %kde_app_path/.directory
cp desktop/<Binary>.desktop  %kde_app_path/
cp desktop/<Binary>.xpm  %kde_icon_path/
cp desktop/<Binary>_small.xpm  %kde_icon_path/mini/<Binary>.xpm

%define gnome_app_path  /usr/share/gnome/apps/NPS_Utilities
%define gnome_icon_path /usr/share/pixmaps
mkdir -p %gnome_app_path
cp desktop/directory  %gnome_app_path/.directory
cp desktop/<Binary>.desktop  %gnome_app_path/
cp desktop/<Binary>.xpm  %gnome_icon_path/

mkdir -p /usr/lib/menu
cp desktop/mandrake_menu /usr/lib/menu/<Binary>

%post
gunzip %<Binary>_lib_dir/*.gz

%postun
rm -rf %<Binary>_lib_dir

%files

%docdir %<Binary>_doc_dir

/usr/X11R6/bin/<Binary>
%<Binary>_lib_dir
%<Binary>_doc_dir

%redhat_app_path/<Binary>.desktop

%kde_app_path
%kde_icon_path/new_planet_software.xpm
%kde_icon_path/mini/new_planet_software.xpm
%kde_icon_path/<Binary>.xpm
%kde_icon_path/mini/<Binary>.xpm
%kde_icon_path/hicolor/16x16/apps/new_planet_software.xpm
%kde_icon_path/locolor/16x16/apps/new_planet_software.xpm

%gnome_app_path
%gnome_icon_path/new_planet_software.xpm
%gnome_icon_path/<Binary>.xpm

/usr/lib/menu/<Binary>
