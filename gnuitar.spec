%define ver 0.1.0
%define rel 1b
%define prefix /usr/local

Summary: a GTK+ based guitar processor. Includes such effects as reverberator, sustain, distortion, phasor, wah-wah.
Name: gnuitar
Version: %{ver}
Release: %{rel}
Source: http://ziet.zhitomir.ua/~fonin/code/gnuitar-%{ver}.tar.gz
Copyright: GPL
URL: http://ziet.zhitomir.ua/~fonin/code.html
Requires: gtk+ >= 1.2.6
Group: Applications/Multimedia
Packager: Max V. Rudensky <fonin@ziet.zhitomir.ua>
BuildRoot: /var/tmp/%{name}-root

%changelog
* Thu Jan 01 2001 Max Rudensky <fonin@ziet.zhitomir.ua>
- First release


%description
This is a guitar effects software that allows you to 
use your Linux box as guitar processor.
Includes effects:
	o wah-wah
	o sustain
	o distortion
	o reverberator, echo, delay
	o tremolo
	o vibrato
	o chorus/flanger
See 'README' for more information.

%prep
rm -rf $RPM_BUILD_ROOT

%setup
%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix}
make

%install
make prefix=${RPM_BUILD_ROOT}%{prefix} install

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%doc COPYING ChangeLog TODO README AUTHORS
%attr(4755,root,root) %{prefix}/bin/gnuitar
# %{prefix}/share/%{name}-%{ver}/effects/*
