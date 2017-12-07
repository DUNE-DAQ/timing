INSTALL_SCRIPT := $(PDT_ROOT)/config/install_as_ups_package.sh
UPS_PRODUCT_DIR ?= /nfs/home/phrodrig/protodune/timing/upsify/myproducts
UPS_QUALIFIERS ?= e14:prof:s50

.PHONY: ups _ups

ups: _ups

_ups:
	$(INSTALL_SCRIPT) -v v$(PACKAGE_VER_MAJOR)_$(PACKAGE_VER_MINOR)_$(PACKAGE_VER_PATCH) \
	                  -u $(RequiredUhalVersion) \
	                  -b $(PDT_ROOT) \
	                  -i $(UPS_PRODUCT_DIR) \
	                  -q $(UPS_QUALIFIERS)
