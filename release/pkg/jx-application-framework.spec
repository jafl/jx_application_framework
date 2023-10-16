Summary: JX Application Framework is a UI library for the X Window System.
Name: jx-application-framework
Version: %pkg_version
Release: 1
License: GPL
Group: Development/Libraries/C and C++
Source: %pkg_name
Requires: gcc-c++, make, rpm-build, imake, libtool, libX11-devel, libXinerama-devel, libXft-devel, libXmu-devel, libXi-devel, xorg-x11-proto-devel, xorg-x11-server-devel, libXdamage-devel, libXpm-devel, libjpeg-devel, libpng-devel, gd-devel, libicu-devel, pcre-devel, libxml2-devel, boost-devel, expat-devel, libdrm-devel, xforms-devel, aspell, aspell-en, findutils, curl, perl, python-libxml2

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
