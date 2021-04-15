Summary: Leibnitz provides electronic paper for calculations and graphs.
Name: Leibnitz
Version: %pkg_version
Release: 1
License: GPL
Group: Applications/Scientific
Source: Leibnitz_%pkg_version.tar
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
Leibnitz provides electronic paper for calculations and graphs.
(http://www.newplanetsoftware.com/leibnitz/)

%prep
%setup

%install

%define leibnitz_doc_dir  /usr/share/doc/leibnitz
%define leibnitz_lib_dir  /usr/lib/leibnitz
%define gnome_app_path    /usr/share/applications
%define gnome_icon_path   /usr/share/pixmaps

./install "$RPM_BUILD_ROOT" nozip

%post
gunzip %leibnitz_lib_dir/*.gz

%postun
rm -rf %leibnitz_lib_dir

%files

%docdir %leibnitz_doc_dir

/usr/bin/leibnitz
%leibnitz_lib_dir
%leibnitz_doc_dir

%gnome_app_path/leibnitz.desktop
%gnome_icon_path/leibnitz.xpm
