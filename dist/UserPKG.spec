Summary: Unprivileged Package Manager
Name: UserPKG

Version: 20140523
Release: 1%{dist}
License: GPLv2

Group: System Environment/Base
URL: http://www.dracolinux.org

Packager: Ole Andre Rodlie, <olear@dracolinux.org>
Vendor: DracoLinux, http://dracolinux.org

Source: DracoPKG-%{version}.tar.gz
BuildRoot: %{_tmppath}/DracoPKG-%{version}-%{release}-root

BuildRequires: qt-devel
Requires: qt

%description
Unprivileged Package Manager

%prep
%setup -n DracoPKG-%{version}

%build
qmake-qt4 CONFIG-=debug CONFIG+=release CONFIG+=libpkgsrc CONFIG+=staticlib CONFIG+=userpkg PREFIX=/usr LIBDIR=%{_libdir}
make

%install
make INSTALL_ROOT=%{buildroot} install
rm -rf %{buildroot}%{_includedir}
rm -rf %{buildroot}%{_libdir}

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/userpkg
%{_docdir}/*

%changelog
* Thu Apr 29 2014 Ole Andre Rodlie <olear@dracolinux.org> - 0.1-1
- initial version
