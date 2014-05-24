Summary: Package Management tools for DracoLinux
Name: DracoPKG

Version: 20140524
Release: 1%{dist}
License: GPLv2

Group: System Environment/Base
URL: http://www.dracolinux.org

Packager: Ole Andre Rodlie, <olear@dracolinux.org>
Vendor: DracoLinux, http://dracolinux.org

Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: qt-devel
Requires: qt
Requires: gcc-c++

%description
Package Management tools for DracoLinux.

%package devel
Summary: Header files for DracoPKG.
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: qt-devel

%description devel
Header files for DracoPKG.

%prep
%setup

%build
qmake-qt4 CONFIG-=debug CONFIG+=release PREFIX=/usr LIBDIR=%{_libdir}
make

%install
make INSTALL_ROOT=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
/etc/dbus-1/system.d/org.dracolinux.%{name}.conf
%{_sbindir}/%{name}
%{_bindir}/userpkg
%{_bindir}/pkgtray
/usr/share/dbus-1/system-services/org.dracolinux.%{name}.service
%{_libdir}/*
%{_docdir}/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/*

%changelog
* Thu Apr 29 2014 Ole Andre Rodlie <olear@dracolinux.org> - 0.1-1
- initial version
