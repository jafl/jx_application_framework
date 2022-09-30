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

%define <Binary>_doc_dir /usr/local/share/doc/<Binary>
%define gnome_app_path   /usr/local/share/applications
%define gnome_icon_path  /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %<Binary>_doc_dir

/usr/local/bin/<Binary>
%<Binary>_doc_dir

%gnome_app_path/<Binary>.desktop
%gnome_icon_path/<Binary>.xpm
