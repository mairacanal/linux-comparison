PART_NUMBER = bbb-gpio-benchmark
VERSION = 00A0

# To get the include files, clone the repository from
# # https://github.com/beagleboard/bb.org-overlays.git
# # parallel to this directory.
#
INCLUDE_PATH = ../bb.org-overlays/include

compile: $(PART_NUMBER)-$(VERSION).dtbo

install: compile
	cp $(PART_NUMBER)-$(VERSION).dtbo /lib/firmware

clean:
	rm -f *.dtbo *.dts.tmp *~

%.dtbo: %.dts
	cpp -nostdinc -undef -D__DTS__ -I$(INCLUDE_PATH) -x assembler-with-cpp -o $<.tmp $<
	dtc -Wno-unit_address_vs_reg -O dtb -o $@ -b 0 -@ $<.tmp
