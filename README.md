ProtoDUNE-SP timing board software
==================================

Building
--------

The package can be built against the system version of cactus/uhal or
against a version of cactus from UPS. Running plain `make` will build
against the system version.

To build against a version in UPS, first set up the versions of uhal and python that you want. Eg:

```
> . /nfs/sw/artdaq/products/setups
> setup uhal v2_4_2 -q e14:prof:s50
> setup python v2_7_13d
```

(Note that the lines above will also setup a different version of the compiler)

Then run `make USE_UPS_CACTUS=1`

Creating a UPS product
----------------------

The core library can be made into a UPS product by the name of
`protodune_pdt_core` in order to be able to share functions with the
artdaq board reader (or whoever else wants to use them). To do this,
build against the UPS versions of the dependencies as described above,
and then go to the `core` directory and run `make ups UPS_PRODUCT_DIR=/path/to/product/dir`.

If you're building against a new version of uhal, you should first
edit the `RequiredUhalVersion` variable in `core/Makefile`.

If you want to build with a new set of UPS qualifiers (at time of
writing, the qualifiers are `e14:prof:s50`), you should add a new
stanza to `config/protodune_pdt_core.table` (copy the existing ones)
and edit `UPS_QUALIFIERS` in `config/mfUPSRules.mk` (or pass it on the
`make` command line).