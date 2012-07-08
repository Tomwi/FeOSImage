export THIS_MAKEFILE := Makefile.FeOS


CODECS 		:= $(notdir $(patsubst %/,%,$(dir $(wildcard codecs/*/Makefile))))


all: $(CODECS)
	@make --no-print-directory -f Makefile.FeOS

$(CODECS):
	@make --no-print-directory -C codecs/$@
	

clean:
	@make --no-print-directory -f Makefile.FeOS clean
	@for i in $(CODECS); do make --no-print-directory -C codecs/$$i clean; done
	
install: all
	@make --no-print-directory -f Makefile.FeOS install
	@for i in $(CODECS); do make --no-print-directory -C codecs/$$i install; done
	
	


