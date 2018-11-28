ifeq ($(prefix),)
    prefix := /usr/local
endif

export prefix

TOPTARGETS := all clean install

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

datarootdir = $(prefix)/share
datadir = $(datarootdir)


$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

install:
	install -d $(DESTDIR)$(datadir)/iturhfprop/data
	install -m 744 -D ../P372/Data/*.txt $(DESTDIR)$(datadir)/iturhfprop/data
	install -m 744 -D ../P372/Data/*.bin $(DESTDIR)$(datadir)/iturhfprop/data

