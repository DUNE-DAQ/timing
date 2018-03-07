PDT_ROOT= $(shell pwd)
BUILD_HOME = $(PDT_ROOT)
include ${PDT_ROOT}/config/mfCommonDefs.mk

INSTALL_SCRIPT := $(PDT_ROOT)/config/create_ups_product.sh
UPS_PRODUCT_DIR ?= $(PDT_ROOT)/ups/products
# UPS product names can't contain dashes so replace them with underscores
PRODUCTNAME := protodune_timing

.PHONY: ups _ups 

ups: _ups

_ups:
	mkdir -p $(UPS_PRODUCT_DIR)
	$(INSTALL_SCRIPT) -p $(PRODUCTNAME) \
	                  -v v$(UPS_PACKAGE_VER_MAJOR)_$(UPS_PACKAGE_VER_MINOR)_$(UPS_PACKAGE_VER_PATCH) \
	                  -u $(REQUIRED_UHAL_VERSION) \
	                  -b $(PDT_ROOT) \
	                  -i $(UPS_PRODUCT_DIR) \
	                  -q $(UPS_QUALIFIERS)


.PHONY: cleanups _cleanups 

cleanups: _cleanups

_cleanups:
	rm -rf $(PDT_ROOT)/ups/
