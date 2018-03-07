# If we built against the ups uhal, we want to build a ups product and
# not an rpm, so refuse to go ahead here. This isn't foolproof: the
# check is against the existence of the environment variable that
# indicates whether uhal is set up via ups in the current environment,
# not strictly how the build was done
ifdef SETUP_UHAL
.PHONY: rpm

rpm:
	$(warning '**********************************************************')
	$(warning '* uhal has been set up with ups: refusing to make an RPM *')
	$(warning '* You probably want to \'make ups\' instead              *')
	$(warning '**********************************************************')
	$(error exiting)
else
RPMBUILD_DIR=${PackagePath}/rpm/RPMBUILD


BUILD_REQUIRES_TAG = $(if ${PackageBuildRequires} ,BuildRequires: ${PackageBuildRequires} ,\# No BuildRequires tag )
REQUIRES_TAG = $(if ${PackageRequires} ,Requires: ${PackageRequires} ,\# No Requires tag )


.PHONY: rpm _rpmall
rpm: _rpmall
_rpmall: _all _spec_update _rpmbuild

.PHONY: _rpmbuild
_rpmbuild: _spec_update
	${MakeDir} -p ${RPMBUILD_DIR}/{RPMS/{i386,i586,i686,x86_64},SPECS,BUILD,SOURCES,SRPMS}
	${RpmBuild} --quiet -bb -bl --buildroot=${RPMBUILD_DIR}/BUILD --define  "_topdir ${RPMBUILD_DIR}" rpm/${PackageName}.spec
	find  ${RPMBUILD_DIR} -name "*.rpm" -exec mv {} $(PackagePath)/rpm \;

.PHONY: _spec_update	
_spec_update:
	${MakeDir} -p ${PackagePath}/rpm
	${Cp} ${BUILD_HOME}/config/specTemplate.spec ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__package__#${Package}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__packagename__#${PackageName}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__version__#$(PACKAGE_VER_MAJOR).$(PACKAGE_VER_MINOR).$(PACKAGE_VER_PATCH)#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__release__#${PACKAGE_RELEASE}.${PDT_OS}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__prefix__#${PDT_SYSROOT}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__sources_dir__#${RPMBUILD_DIR}/SOURCES#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__packagedir__#${PackagePath}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__os__#${PDT_OS}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__platform__#None#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__project__#${Project}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__author__#${Packager}#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__summary__#None#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__description__#None#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__url__#None#' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's#__includedirs__#$(Includes)#' $(PackagePath)/rpm/$(PackageName).spec
	${Sed} -i 's|^.*__build_requires__.*|${BUILD_REQUIRES_TAG}|' ${PackagePath}/rpm/${PackageName}.spec
	${Sed} -i 's|^.*__requires__.*|${REQUIRES_TAG}|' ${PackagePath}/rpm/${PackageName}.spec

.PHONY: cleanrpm _cleanrpm
cleanrpm: _cleanrpm
_cleanrpm:
	-rm -r rpm

endif
