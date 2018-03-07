PACKAGES = \
	core \
	daemon \
	python \
	tests

VIRTUAL_PACKAGES = $(addsuffix /.virtual.Makefile,${PACKAGES})

FLAGS = $(ifeq $(MAKEFLAGS) "","",-$(MAKEFLAGS))

TARGETS=clean build all rpm cleanrpm objs

.PHONY: $(TARGETS) ups cleanups
default: build

$(TARGETS): ${VIRTUAL_PACKAGES}

${VIRTUAL_PACKAGES}:
	${MAKE} ${FLAGS} -C $(@D) $(MAKECMDGOALS)

ups:
	${MAKE} -f config/Makefile.ups.mk $(MAKECMDGOALS)

cleanups:
	${MAKE} -f config/Makefile.ups.mk $(MAKECMDGOALS)
