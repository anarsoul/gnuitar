%define ver 0.1.0
%define rel 2b
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
rm -rf $RPM_BUILD_ROOT
make install prefix=${RPM_BUILD_ROOT}%{prefix}

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%doc COPYING ChangeLog TODO README AUTHORS
%attr(4755,root,root) %{prefix}/bin/gnuitar
%{prefix}/share/%{name}/*

%changelog
* Sun Mar 25 2001 Max Rudensky	<fonin@ziet.zhitomir.ua>
- Removed "Clip" function from distortion. I found it ambiguous and 
  not elegant.
- Make effect control windows ignore "delete_event", to prevent 
  accidentally close.
- Bank clist widget width auto-resizing
- Few entries in TODO
- Added section "BUGS" to README
* Sun Mar 11 2001 Max Rudensky	<fonin@ziet.zhitomir.ua>
- Fixed closing main window. There was a bug - after close application 
still was in memory.
* Thu Jan 01 2001 Max Rudensky <fonin@ziet.zhitomir.ua>
- First release

# end of file
