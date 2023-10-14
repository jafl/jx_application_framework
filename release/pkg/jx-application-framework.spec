Summary: JX Application Framework is a UI library for the X Window System.
Name: jx-application-framework
Version: %pkg_version
Release: 1
License: GPL
Group: System/Monitoring
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, boost-devel, curl

%description
JX Application Framework is a UI library for the X Window System.

%prep
%setup

%install

./install $RPM_BUILD_ROOT/usr/local

%files

/usr/local/lib
/usr/local/include
/usr/local/bin
/usr/local/share
