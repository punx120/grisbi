# Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir) \
			  -I$(top_srcdir)/src \
			  -I$(top_srcdir)/src/widgets \
			  $(GLIB_CFLAGS) $(GTK_CFLAGS) $(LIBETATSPAGES_CFLAGS)

noinst_LTLIBRARIES = libetatspages.la

libetatspages_la_SOURCES = \
	etats_page_accounts.c \
	etats_page_amount.c \
	etats_page_budget.c \
	etats_page_category.c \
	etats_page_payee.c \
	etats_page_period.c \
	etats_page_text.c \
	etats_page_transfer.c \
	etats_page_accounts.h \
	etats_page_amount.h \
	etats_page_budget.h \
	etats_page_category.h \
	etats_page_payee.h \
	etats_page_period.h \
	etats_page_text.h \
	etats_page_transfer.h

libetatspages_la_LIBADD= $(LIBETATSPAGES_LIBS)

CLEANFILES = \
	*~

MAINTAINERCLEANFILES = \
	Makefile.in
	
EXTRA_DIST = \
	etats_page_accounts.ui \
	etats_page_amount.ui \
	etats_page_budget.ui\
	etats_page_category.ui \
	etats_page_payee.ui \
	etats_page_period.ui \
	etats_page_text.ui \
	etats_page_transfer.ui

