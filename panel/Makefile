GENERATORS = p3-panel.py p2-panel.py

TARGETS = \
	$(GENERATORS:-panel.py=-engraving.data) \
	$(GENERATORS:-panel.py=-engraving.eps) \
	$(GENERATORS:-panel.py=-engraving.gcode) \
	$(GENERATORS:-panel.py=-cutouts.data) \
	$(GENERATORS:-panel.py=-cutouts.eps) \
	$(GENERATORS:-panel.py=-cutouts.gcode) \
	$(GENERATORS:-panel.py=-outline.data) \
	$(GENERATORS:-panel.py=-outline.eps) \
	$(GENERATORS:-panel.py=-outline.gcode) \
	$(GENERATORS:-panel.py=-helper.data) \
	$(GENERATORS:-panel.py=-helper.eps) \
	$(GENERATORS:-panel.py=-helper.gcode) \
	$(GENERATORS:-panel.py=-preview.eps) \
	$(GENERATORS:-panel.py=-preview.pdf) \
	$(GENERATORS:-panel.py=-preview.png)

CLEANFILES = \
	$(GENERATORS:-panel.py=-engraving0.svg) \
	$(GENERATORS:-panel.py=-engraving1.svg) \
	$(GENERATORS:-panel.py=-engraving.eps) \
	$(GENERATORS:-panel.py=-engraving.gcode) \
	$(GENERATORS:-panel.py=-cutouts.eps) \
	$(GENERATORS:-panel.py=-cutouts.gcode) \
	$(GENERATORS:-panel.py=-outline.eps) \
	$(GENERATORS:-panel.py=-outline.gcode) \
	$(GENERATORS:-panel.py=-helper.eps) \
	$(GENERATORS:-panel.py=-helper.gcode)

MAINTAINERCLEANFILES = \
	$(GENERATORS:-panel.py=-engraving.data) \
	$(GENERATORS:-panel.py=-cutouts.data) \
	$(GENERATORS:-panel.py=-outline.data) \
	$(GENERATORS:-panel.py=-helper.data) \
	$(GENERATORS:-panel.py=-preview.eps) \
	$(GENERATORS:-panel.py=-preview.pdf) \
	$(GENERATORS:-panel.py=-preview.png)


all: $(TARGETS)

%-engraving0.svg: %-panel.py gen-svg.py
	./gen-svg.py $< > $@

%-engraving1.svg: %-engraving0.svg
	cp $< $@
	inkscape \
	        --verb EditSelectAll \
	        --verb ObjectToPath \
	        --verb FileSave \
	        --verb FileQuit \
	    "$@"

%-engraving.data: %-engraving1.svg parse-svg.py
	./parse-svg.py < $< > $@

%-cutouts.data: %-panel.py gen-cutouts.py
	./gen-cutouts.py $< > $@

%-outline.data: %-panel.py gen-outline.py
	./gen-outline.py $< > $@

%-helper.data: %-cutouts.data %-outline.data gen-helper.py
	cat $(@:-helper.data=-cutouts.data) \
	    $(@:-helper.data=-outline.data) | ./gen-helper.py > $@

%-engraving.eps: %-engraving.data data2eps.py
	./data2eps.py engraving < $< > $@

%-cutouts.eps: %-cutouts.data data2eps.py
	./data2eps.py cutouts < $< > $@

%-outline.eps: %-outline.data data2eps.py
	./data2eps.py outline < $< > $@

%-helper.eps: %-helper.data data2eps.py
	./data2eps.py helper < $< > $@

%-preview.eps: %-engraving.data %-cutouts.data %-outline.data data2eps.py holes.eps
	(./data2eps.py pengraving < $(@:-preview.eps=-engraving.data); \
	 ./data2eps.py pcutouts < $(@:-preview.eps=-cutouts.data); \
	 cat holes.eps) > $@

%-engraving.gcode: %-engraving.data data2gcode.py
	./data2gcode.py engraving < $< > $@

%-cutouts.gcode: %-cutouts.data data2gcode.py
	./data2gcode.py cutouts < $< > $@

%-outline.gcode: %-outline.data data2gcode.py
	./data2gcode.py outline < $< > $@

%-helper.gcode: %-helper.data data2gcode.py
	./data2gcode.py helper < $< > $@

%-preview.pdf: %-preview.eps
	SOURCE_DATE_EPOCH=0 ps2pdf -dEPSCrop $<

%-preview.png: %-preview.eps
	convert -density 360x360 -alpha remove -resize 1000x \
	        +set date:create +set date:modify $< $@

clean:
	rm -f $(CLEANFILES)

maintainer-clean: clean
	rm -f $(MAINTAINERCLEANFILES)

.PHONY: all clean maintainer-clean
