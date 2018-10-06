ifeq ($(prefix),)
    prefix := /usr/local
endif

export prefix

TOPTARGETS := all clean install

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

data_dir = /usr/local/share/p533/data

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

install-data:
	install -d $(data_dir)
	install -m 744 -D ../P372/Data/*.txt $(data_dir)
	install -m 744 -D ../P372/Data/*.bin $(data_dir)

