SUBDIRS += graceful-server-client
SUBDIRS += ns-client
SUBDIRS += shutdown-rd
SUBDIRS += shutdown-wr

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done

clean:
	@set -e; for dir in $(SUBDIRS); do $(MAKE) -C $${dir} $@; done
