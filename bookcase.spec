%define name    bookcase
%define version 0.3
%define release 1rls

Summary: Personal book collection database
Name: %{name}
Version: %{version}
Release: %{release}
License: GPL
Group: Graphical Desktop/KDE
Source: %{name}-%{version}.tar.bz2
URL: http://periapsis.org/bookcase/
Requires: kdebase
BuildRequires: gcc-c++ libqt3-devel
BuildRoot: %{_tmppath}/%{name}-buildroot

%description
Bookcase is a personal catalog application for your book collection.

%prep
rm -rf $RPM_BUILD_ROOT

%setup  -q

#find . -name "*.h" -exec perl -pi -e '$_.="\n" if eof' {} \;

%build
%configure
make all

%install
mkdir -p $RPM_BUILD_ROOT%{_bindir}
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1
#mkdir -p $RPM_BUILD_ROOT%{_datadir}/applnk/Office
mkdir -p $RPM_BUILD_ROOT%{_datadir}/pixmaps
#mkdir -p $RPM_BUILD_ROOT%{_datadir}/gnome/apps/Networking
make DESTDIR=$RPM_BUILD_ROOT install

(cd $RPM_BUILD_ROOT
mkdir -p ./usr/lib/menu
cat > ./usr/lib/menu/%{name} <<EOF
?package(%{name}):\
command="%_bindir/bookcase"\
icon="%{name}.png"\
kde_filename="bookcase"\
kde_command="bookcase -caption \"%c\" %i %m"\
title="Bookcase"\
longtitle="Book Collection Organizer"\
needs="kde"\
section="Office/Accessories"
EOF
)  


mkdir -p $RPM_BUILD_ROOT%{_miconsdir}
mkdir -p $RPM_BUILD_ROOT%{_liconsdir}


%post
%{update_menus}

%postun
%{clean_menus}

%files
%defattr (-,root,root)
%doc doc/ AUTHORS COPYING ChangeLog INSTALL NEWS README TODO
%{_bindir}/*
%{_mandir}/man1/*
#%{_datadir}/applnk/Internet/Everybuddy.kdelnk
#%{_datadir}/pixmaps/ebicon.xpm
#%{_datadir}/gnome/apps/Internet/Everybuddy.desktop
%{_libdir}/menu/*
%{_datadir}/sounds/everybuddy/*
%{_iconsdir}/*.png
%{_miconsdir}/*.png
%{_liconsdir}/*.png

%clean 
rm -rf $RPM_BUILD_ROOT

%changelog
* Wed Aug 21 2002  <robby@periapsis.org> 0.3-1rls
- Change to Qt3
- Use png
- Some wording changes

* Wed Oct 17 2001  <robby@radiojodi.com> 0.3-1mdk
- BuildRequires: gcc-c++ libqt

* Thu Oct 11 2001  <robby@radiojodi.com> 0.3-1mdk
- initial spec.in file
