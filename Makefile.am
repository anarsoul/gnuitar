SUBDIRS = src effects

EXTRA_DIST = gnuitar.spec TODO README AUTHORS ChangeLog COPYING

BZIP2_ENV = -9

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(distdir).tar.gz $(distdir)
	BZIP2=$(BZIP2_ENV) $(TAR) choIf $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)
