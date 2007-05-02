
util: malloc-stats.so

%.so: %.la
	$(RM) $@
	$(LN_S) .libs/$*.so $@

CLEANFILES = *.so *.la

SOLDFLAGS = -module -avoid-version -export-dynamic -rpath /dev/null

EXTRA_LTLIBRARIES = malloc-stats.la backtrace-symbols.la


backtrace_symbols_la_LDFLAGS = $(SOLDFLAGS)
backtrace_symbols_la_LIBADD  = -lbfd -liberty
backtrace_symbols_la_SOURCES = backtrace-symbols.c

malloc_stats_la_LDFLAGS = $(SOLDFLAGS)
malloc_stats_la_LIBADD  = $(backtrace_symbols_la_LIBADD)
malloc_stats_la_SOURCES = $(backtrace_symbols_la_SOURCES) malloc-stats.c

EXTRA_DIST = \
	xr2cairo \
	cairo-api-update

