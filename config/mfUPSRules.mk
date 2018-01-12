INSTALL_SCRIPT := $(PDT_ROOT)/config/create_ups_product.sh
UPS_PRODUCT_DIR ?= $(PDT_ROOT)/ups/products
UPS_QUALIFIERS ?= e14:prof:s50
# UPS product names can't contain dashes so replace them with underscores
PRODUCTNAME := $(subst -,_,$(PackageName))

.PHONY: ups _ups

ups: _ups

_ups:
	mkdir -p $(UPS_PRODUCT_DIR)
	$(INSTALL_SCRIPT) -p $(PRODUCTNAME) \
	                  -v v$(PACKAGE_VER_MAJOR)_$(PACKAGE_VER_MINOR)_$(PACKAGE_VER_PATCH) \
	                  -u $(RequiredUhalVersion) \
	                  -b $(PDT_ROOT) \
	                  -i $(UPS_PRODUCT_DIR) \
	                  -q $(UPS_QUALIFIERS)
