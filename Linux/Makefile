ifeq ($(prefix),)
    prefix := /usr/local
endif

export prefix

mandir = $(prefix)/share/man
man1dir = $(mandir)/man1

TOPTARGETS := all clean

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

data_dir = $(prefix)/share/p533/data

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

install: $(SUBDIRS)
	install ITURHFProp.1 $(man1dir)
	
install-data:
	install -d $(data_dir)
	install -m 744 -D ../P372/Data/*.txt $(data_dir)
	install -m 744 -D ../P372/Data/*.bin $(data_dir)
