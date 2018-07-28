ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

export PREFIX

TOPTARGETS := all clean install

SUBDIRS := ../P533/Linux/ \
	../P372/Linux/ \
	../ITURHFProp/Linux

data_dir = /usr/local/share/p533/data

DATAFILES = ../P372/Data/COEFF01W.txt \
	../P372/Data/COEFF02W.txt\
	../P372/Data/COEFF03W.txt\
	../P372/Data/COEFF04W.txt\
	../P372/Data/COEFF05W.txt\
	../P372/Data/COEFF06W.txt\
	../P372/Data/COEFF07W.txt\
	../P372/Data/COEFF08W.txt\
	../P372/Data/COEFF09W.txt\
	../P372/Data/COEFF10W.txt\
	../P372/Data/COEFF11W.txt\
	../P372/Data/COEFF12W.txt\
	../P372/Data/ionos01.bin\
	../P372/Data/ionos02.bin\
	../P372/Data/ionos03.bin\
	../P372/Data/ionos04.bin\
	../P372/Data/ionos05.bin\
	../P372/Data/ionos06.bin\
	../P372/Data/ionos07.bin\
	../P372/Data/ionos08.bin\
	../P372/Data/ionos09.bin\
	../P372/Data/ionos10.bin\
	../P372/Data/ionos11.bin\
	../P372/Data/ionos12.bin\
	"../P372/Data/P1239-3 Decile Factors.txt"


$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)

install-data:
	install -d $(data_dir)
	install -m 744 -D ../P372/Data/*.txt $(data_dir)
	install -m 744 -D ../P372/Data/*.bin $(data_dir)

