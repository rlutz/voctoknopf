include config.mk

SCHEMATICS = voctoknopf.sch bank-0.sch bank-1.sch bank-2.sch bank-3.sch
EXTRA_SCHEMATICS = redgreen.sch

TARGETS = voctoknopf.net voctoknopf.pdf

all: $(TARGETS)

voctoknopf.net: $(SCHEMATICS) $(EXTRA_SCHEMATICS)
	$(GNETLIST) -g pcbfwd -o $@ $(SCHEMATICS)

voctoknopf.pdf: $(SCHEMATICS) $(EXTRA_SCHEMATICS)
	$(GAF) export -p iso_a4 -m "1cm;1cm;1cm;1cm" -l landscape -c -o $@ $^

maintainer-clean:
	rm -f $(TARGETS)

.PHONY: all maintainer-clean
