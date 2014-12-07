EXTRA_DIST=libG4Track.la libG4SrcCfg.la .xlibs

install-exec-local:
	if test ! -d $(srcdir)/.libs;then \
		@mkdir_p@ $(srcdir)/.libs; \
	fi; \
	if test ! -d $(DESTDIR)/$(libdir);then \
		@mkdir_p@ $(DESTDIR)/$(libdir); \
	fi; \
	@INSTALL@ $(srcdir)/.xlibs/* $(srcdir)/.libs; \
	@LIBTOOL@ install @INSTALL@ $(srcdir)/libG4Track.la $(DESTDIR)/$(libdir); \
	@LIBTOOL@ install @INSTALL@ $(srcdir)/libG4SrcCfg.la $(DESTDIR)/$(libdir); \
	@LIBTOOL@ finish $(DESTDIR)/$(libdir)


uninstall-local:
	@LIBTOOL@ uninstall rm $(DESTDIR)/$(libdir)/libG4Track.la
	@LIBTOOL@ uninstall rm $(DESTDIR)/$(libdir)/libG4SrcCfg.la
