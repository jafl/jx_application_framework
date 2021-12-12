Summary: <Program> provides ...
Name: %app_name
Version: %pkg_version
Release: 1
License: ...
Group: ...
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
<Program> provides ...
(<URL>)

%prep
%setup

%install

%define <Binary>_doc_dir /usr/share/doc/<Binary>
%define gnome_app_path   /usr/share/applications
%define gnome_icon_path  /usr/share/pixmaps

./install "$RPM_BUILD_ROOT"/usr

%files

%docdir %<Binary>_doc_dir

/usr/bin/<Binary>
%<Binary>_doc_dir

%gnome_app_path/<Binary>.desktop
%gnome_icon_path/<Binary>.xpm
