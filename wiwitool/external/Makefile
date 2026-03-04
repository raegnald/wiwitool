SUBDIRS := json stb

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -k -C $@

clean:
	rm -f */*.o
