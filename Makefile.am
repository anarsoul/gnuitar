SUBDIRS = src
a1dir = $(datadir)/doc/gnuitar/docs
a1_DATA = docs
a2dir = $(pkgdatadir)/distort2
a2_DATA = distort2
docdir = $(datadir)/doc/gnuitar
doc_DATA = docs/*.html NEWS ChangeLog FAQ AUTHORS COPYING INSTALL README TODO
distort2dir = $(pkgdatadir)
distort2_DATA = distort2/*
win32dir = $(pkgdatadir)/win32
win32_DATA = gnuitar.ico gnuitar.dsp gnuitar.mak gnuitar_small.ico

EXTRA_DIST = gnuitar.spec

BZIP2_ENV = -9

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(distdir).tar.gz $(distdir)
	BZIP2=$(BZIP2_ENV) $(TAR) choIf $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)
