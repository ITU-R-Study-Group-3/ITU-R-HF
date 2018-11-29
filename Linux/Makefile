
prefix ?= /usr/local
export prefix

mandir = $(prefix)/share/man
man1dir = $(mandir)/man1

TOPTARGETS := all clean

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

datarootdir = $(prefix)/share
datadir = $(datarootdir)

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

install: $(SUBDIRS)
	install -d $(DESTDIR)$(man1dir)
	install ITURHFProp.1 $(DESTDIR)$(man1dir)
	install -d $(DESTDIR)$(datadir)/iturhfprop/data
	install -m 744 -D ../P372/Data/*.txt $(DESTDIR)$(datadir)/iturhfprop/data
	install -m 744 -D ../P372/Data/*.bin $(DESTDIR)$(datadir)/iturhfprop/data

	

