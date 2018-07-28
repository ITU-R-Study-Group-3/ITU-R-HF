ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

export PREFIX

TOPTARGETS := all clean install

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
