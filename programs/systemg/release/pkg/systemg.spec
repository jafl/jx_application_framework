Summary: System G provides a graphical interface to UNIX file system.
Name: System_G
Version: %pkg_version
Release: 1
License: GPL
Group: Filemanager
Source: System_G_%pkg_version.tar
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
System G provides a graphical interface to UNIX file system,
similar to Mac OS 7.
(http://www.newplanetsoftware.com/systemg/)

%prep
%setup

%install

%define systemg_doc_dir /usr/share/doc/systemg
%define systemg_lib_dir /usr/lib/systemg
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "$RPM_BUILD_ROOT" nozip

%post
gunzip %systemg_lib_dir/*.gz

%postun
rm -rf %systemg_lib_dir

%files

%docdir %systemg_doc_dir

/usr/bin/systemg
%systemg_lib_dir
%systemg_doc_dir

%gnome_app_path/systemg.desktop
%gnome_app_path/systemg_choose.desktop
%gnome_icon_path/systemg.xpm
