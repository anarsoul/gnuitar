%define ver 0.2.1
%define rel 1
%define prefix /usr

Summary: a GTK+ based guitar processor. Includes such effects as reverberator, sustain, distortion, phasor, wah-wah.
Name: gnuitar
Version: %{ver}
Release: %{rel}
Source: http://ns2.ziet.zhitomir.ua/~fonin/projects/gnuitar/gnuitar-%{ver}.tar.gz
Copyright: GPL
URL: http://ns2.ziet.zhitomir.ua/~fonin/downloads.php#gnuitar
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
	o phasor
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
%doc COPYING ChangeLog TODO README AUTHORS NEWS docs
%attr(4755,root,root) %{prefix}/bin/gnuitar
%prefix/*

%changelog
* Thu Mar 13 2003 Max Rudensky <fonin@ziet.zhitomir.ua>
- Release 0.2.1
- Change sampling params from the interface;
- Meaningful measure units (%, ms) for every slider control;
- New phasor option - turn on bandpass filter;
- A lot of code cleanups;
- Delay was fixed to reset memory on "Time" slider update;
- Minor documentation updates.
* Thu Feb 6 2003 Max Rudensky <fonin@ziet.zhitomir.ua>
- Release 0.2.0
- Windows port;
- Write track to .wav file in Win32 version;
- Meaningful measure units for vibrato and reverb effects (ms and percents);
- HTML documentation; launch browser from a program to see the doc;
- Tremolo bugfix: certain values of the effect params caused array bound
  error and segfault;
- Fix: when a the write track checkbox is clicked, and then action is cancelled,
  checkbox remains toggled.
- Added icon ;-)
- Major code cleanups for easier maintenance.
* Sun Mar 25 2001 Max Rudensky	<fonin@ziet.zhitomir.ua>
- Release 0.1.0-b3 aka 0.1.3
- Removed "Clip" function from distortion. I found it ambiguous and 
  not elegant.
- Make effect control windows ignore "delete_event", to prevent 
  accidentally close.
- Bank clist widget width auto-resizing
- Few entries in TODO
- Added section "BUGS" to README
- Program does setuid(real user id) immediately after setting 
  real time priority to pumper thread. All open(...,O_CREAT | O_EXCL) 
  (file open error if the file already exists) through the code 
  replaced with simply O_CREAT (file will be overwritten).
* Sun Mar 11 2001 Max Rudensky	<fonin@ziet.zhitomir.ua>
- Release 0.1.0-b2 aka 0.1.2
- Fixed closing main window. There was a bug - after close application 
still was in memory.
* Thu Jan 01 2001 Max Rudensky <fonin@ziet.zhitomir.ua>
- First release (0.1.0)

# end of file
