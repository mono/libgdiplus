check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = ../googletest/googletest/src/gtest-all.cc ../googletest/googletest/src/gtest_main.cc
libgtest_la_CPPFLAGS = -I$(top_srcdir)/googletest/googletest/include -I$(top_srcdir)/googletest/googletest
libgtest_la_LDFLAGS = -pthread

check_PROGRAMS = gtest

gtest_SOURCES = test.cpp
gtest_DEPS = $(top_builddir)/src/libgdiplus.la
gtest_LDADD = libgtest.la $(top_builddir)/src/libgdiplus.la
gtest_LDFLAGS = -pthread
gtest_CPPFLAGS = -I$(top_srcdir)/googletest/googletest/include -I$(top_srcdir)/googletest/googletest -pthread -I$(top_srcdir) -I$(top_builddir)/src -I$(top_srcdir)/src $(GDIPLUS_CPPFLAGS)

TESTS = gtest
