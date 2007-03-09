SUBDIRS = src
#a1dir = $(datadir)/doc/gnuitar/docs
#a1_DATA = docs
docdir = $(datadir)/doc/gnuitar
doc_DATA = docs/*.html NEWS ChangeLog FAQ AUTHORS COPYING INSTALL README TODO
win32 = gnuitar.ico gnuitar.dsp gnuitar.mak gnuitar_small.ico gnuitar.vcproj \
	gnuitar.sln


EXTRA_DIST = gnuitar.spec docs/*.html docs/Makefile docs/toc.sh \
	FAQ $(win32) 

BZIP2_ENV = -9

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(distdir).tar.gz $(distdir)
	BZIP2=$(BZIP2_ENV) $(TAR) choIf $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)
