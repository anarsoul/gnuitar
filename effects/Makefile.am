EXTRA_DIST = HeyDay Phase_destruction Vibrato
pkgdir = $(datadir)/$(PACKAGE)
pkg_DATA = HeyDay Phase_destruction Vibrato

dist-hook:
	mkdir -p $(pkgdir)
	cp -p $(srcdir)/$(pkg_DATA) $(DESTDIR)$(pkgdir)

install-data-local:
	$(mkinstalldirs) \
		 $(DESTDIR)$(pkgdir)
	cp -p $(srcdir)/$(pkg_DATA) $(DESTDIR)$(pkgdir)
