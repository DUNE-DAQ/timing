ProtoDUNE-SP timing board software
==================================

Building
--------

The package can be built against the system version of cactus/uhal or
against a version of cactus from UPS. If a version of uhal has been
set up with ups (ie, if the $SETUP_UHAL environment variable is set),
the ups version of uhal will be used.

To build against a version in UPS, first set up the versions of uhal and python that you want. Eg:

```
> . /nfs/sw/artdaq/products/setups
> setup uhal v2_4_2 -q e14:prof:s50
> setup python v2_7_13d
```

(Note that the lines above will also setup a different version of the compiler)

Then run `make`

Creating a UPS product
----------------------

The core library can be made into a UPS product (UPS's jargon for a
package) by the name of `protodune_pdt_core` in order to be able to
share functions with the artdaq board reader (or whoever else wants to
use them). To do this, build against the UPS versions of the
dependencies as described above, and run `make ups` in the core/
directory (or `make -C core ups` from the top directory).

If you're building against a new version of uhal, you should first
edit the `RequiredUhalVersion` variable in `core/Makefile`.

If you want to build with a new set of UPS qualifiers (at time of
writing, the qualifiers are `e14:prof:s50`), you should add it to
config/product_deps, and edit `UPS_QUALIFIERS` in
`config/mfUPSRules.mk` (or pass it on the `make` command line).

The UPS product version is taken from the version number in the
`PACKAGE_VER_*` variables in `core/Makefile`.

The resulting product is placed in ups/products. It can be used by
setting $PRODUCTS to point to that directory, or by copying
ups/products/protodune_pdt_core/vX_Y_Z into the appropriate place in
an existing $PRODUCTS directory, and declaring the product there with
`ups declare`.

Some more background on the UPS product
---------------------------------------

The logic for creating the UPS product is in
`config/create_ups_product.sh`. It basically just copies the files
we want into a directory in one of the directories in $PRODUCTS along
with a "table" file that describes the product and how to set it
up. The table file is created from `config/product_deps`, which itself is created from `config/product_deps.template`. Once the table file is created, we run `ups declare` with appropriate options, which causes
ups to create some more of the files it needs. Then we can run `setup
protodune_pdt_core vX_Y_Z -q QUALIFIERS` to enable the version vX_Y_Z
of `protodune_pdt_core` built with `QUALIFIERS`.

The qualifiers specify details about the build, eg debug or optimized,
and details of the compiler and art framework version that the
software was built against. See:

https://cdcvs.fnal.gov/redmine/projects/cet-is-public/wiki/AboutQualifiers

Once a particular version of the product is set up, environment
variables are set that allow it to be found by other packages. `set |
grep PROTODUNE_PDT_CORE` is the easiest way to see them.

Right now, only the core/ directory is made into a ups product, but it
should be straightforward to make ups products for the other
directories: `include config/mfUPSRules.mk` in the directory Makefile,
and make sure that the lines at the end of
`config/create_ups_product.sh` copy the necessary files.