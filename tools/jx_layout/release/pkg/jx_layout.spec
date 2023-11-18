Summary: JX Layout Editor provides ...
Name: %app_name
Version: %pkg_version
Release: 1
License: ...
Group: ...
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, boost-devel, curl

%description
JX Layout Editor provides ...
(https://github.com/jafl/jx_application_framework)

%prep
%setup

%install

%define jx_layout_doc_dir /usr/local/share/doc/jx_layout
%define gnome_app_path   /usr/local/share/applications
%define gnome_icon_path  /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %jx_layout_doc_dir

/usr/local/bin/jx_layout
%jx_layout_doc_dir

%gnome_app_path/jx_layout.desktop
%gnome_icon_path/jx_layout.xpm
