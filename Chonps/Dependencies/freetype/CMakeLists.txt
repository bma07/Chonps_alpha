set(FREETYPE_HEADER 
    include/ft2build.h

	# freetype
	include/freetype/freetype.h
	include/freetype/ftadvanc.h
	include/freetype/ftbbox.h
	include/freetype/ftbdf.h
	include/freetype/ftbitmap.h
	include/freetype/ftbzip2.h
	include/freetype/ftcache.h
	include/freetype/ftchapters.h
	include/freetype/ftcid.h
	include/freetype/ftcolor.h
	include/freetype/ftdriver.h
	include/freetype/fterrdef.h
	include/freetype/fterrors.h
	include/freetype/ftfntfmt.h
	include/freetype/ftgasp.h
	include/freetype/ftglyph.h
	include/freetype/ftgxval.h
	include/freetype/ftgzip.h
	include/freetype/ftimage.h
	include/freetype/ftincrem.h
	include/freetype/ftlcdfil.h
	include/freetype/ftlist.h
	include/freetype/ftlogging.h
	include/freetype/ftlzw.h
	include/freetype/ftmac.h
	include/freetype/ftmm.h
	include/freetype/ftmodapi.h
	include/freetype/ftmoderr.h
	include/freetype/ftotval.h
	include/freetype/ftoutln.h
	include/freetype/ftparams.h
	include/freetype/ftpfr.h
	include/freetype/ftrender.h
	include/freetype/ftsizes.h
	include/freetype/ftsnames.h
	include/freetype/ftstroke.h
	include/freetype/ftsynth.h
	include/freetype/ftsystem.h
	include/freetype/fttrigon.h
	include/freetype/fttypes.h
	include/freetype/ftwinfnt.h
	include/freetype/otsvg.h
	include/freetype/t1tables.h
	include/freetype/ttnameid.h
	include/freetype/tttables.h
	include/freetype/tttags.h

	# config
	include/freetype/config/ftconfig.h
	include/freetype/config/ftheader.h
	include/freetype/config/ftmodule.h
	include/freetype/config/ftoption.h
	include/freetype/config/ftstdlib.h
	include/freetype/config/integer-types.h
	include/freetype/config/mac-support.h
	include/freetype/config/public-macros.h

	#internal
	include/freetype/internal/autohint.h
	include/freetype/internal/cffotypes.h
	include/freetype/internal/cfftypes.h
	include/freetype/internal/compiler-macros.h
	include/freetype/internal/ftcalc.h
	include/freetype/internal/ftdebug.h
	include/freetype/internal/ftdrv.h
	include/freetype/internal/ftgloadr.h
	include/freetype/internal/fthash.h
	include/freetype/internal/ftmemory.h
	include/freetype/internal/ftmmtypes.h
	include/freetype/internal/ftobjs.h
	include/freetype/internal/ftpsprop.h
	include/freetype/internal/ftrfork.h
	include/freetype/internal/ftserv.h
	include/freetype/internal/ftstream.h
	include/freetype/internal/fttrace.h
	include/freetype/internal/ftvalid.h
	include/freetype/internal/psaux.h
	include/freetype/internal/pshints.h
	include/freetype/internal/sfnt.h
	include/freetype/internal/svginterface.h
	include/freetype/internal/t1types.h
	include/freetype/internal/tttypes.h
	include/freetype/internal/wofftypes.h

	# services
	include/freetype/internal/services/svbdf.h
	include/freetype/internal/services/svcfftl.h
	include/freetype/internal/services/svcid.h
	include/freetype/internal/services/svfntfmt.h
	include/freetype/internal/services/svgldict.h
	include/freetype/internal/services/svgxval.h
	include/freetype/internal/services/svkern.h
	include/freetype/internal/services/svmetric.h
	include/freetype/internal/services/svmm.h
	include/freetype/internal/services/svotval.h
	include/freetype/internal/services/svpfr.h
	include/freetype/internal/services/svpostnm.h
	include/freetype/internal/services/svprop.h
	include/freetype/internal/services/svpscmap.h
	include/freetype/internal/services/svpsinfo.h
	include/freetype/internal/services/svsfnt.h
	include/freetype/internal/services/svttcmap.h
	include/freetype/internal/services/svtteng.h
	include/freetype/internal/services/svttglyf.h
	include/freetype/internal/services/svwinfnt.h
)

set(FREETYPE_SOURCE
	# includes
	${FREETYPE_HEADER}

	# autofit
	src/autofit/afblue.c
	src/autofit/afblue.h
	src/autofit/afcjk.c
	src/autofit/afcjk.h
	src/autofit/afcover.h
	src/autofit/afdummy.c
	src/autofit/afdummy.h
	src/autofit/aferrors.h
	src/autofit/afglobal.c
	src/autofit/afglobal.h
	src/autofit/afhints.c
	src/autofit/afhints.h
	src/autofit/afindic.c
	src/autofit/afindic.h
	src/autofit/aflatin.c
	src/autofit/aflatin.h
	src/autofit/afloader.c
	src/autofit/afloader.h
	src/autofit/afmodule.c
	src/autofit/afmodule.h
	src/autofit/afranges.c
	src/autofit/afranges.h
	src/autofit/afscript.h
	src/autofit/afshaper.c
	src/autofit/afshaper.h
	src/autofit/afstyles.h
	src/autofit/aftypes.h
	src/autofit/afws-decl.h
	src/autofit/afws-iter.h
	src/autofit/ft-hb.c
	src/autofit/ft-hb.h

	# base
	src/base/ftadvanc.c
	src/base/ftbase.h
	src/base/ftbbox.c
	src/base/ftbdf.c
	src/base/ftbitmap.c
	src/base/ftcalc.c
	src/base/ftcid.c
	src/base/ftcolor.c
	src/base/ftdbgmem.c
	src/base/ftdebug.c
	src/base/fterrors.c
	src/base/ftfntfmt.c
	src/base/ftfstype.c
	src/base/ftgasp.c
	src/base/ftgloadr.c
	src/base/ftglyph.c
	src/base/ftgxval.c
	src/base/fthash.c
	src/base/ftinit.c
	src/base/ftlcdfil.c
	src/base/ftmac.c
	src/base/ftmm.c
	src/base/ftobjs.c
	src/base/ftotval.c
	src/base/ftoutln.c
	src/base/ftpatent.c
	src/base/ftpfr.c
	src/base/ftpsprop.c
	src/base/ftrfork.c
	src/base/ftsnames.c
	src/base/ftstream.c
	src/base/ftstroke.c
	src/base/ftsynth.c
	src/base/ftsystem.c
	src/base/fttrigon.c
	src/base/fttype1.c
	src/base/ftutil.c
	src/base/ftver.rc
	src/base/ftwinfnt.c
	src/base/md5.c
	src/base/md5.h

	# bdf
	src/bdf/bdf.h
	src/bdf/bdfdrivr.c
	src/bdf/bdfdrivr.h
	src/bdf/bdferror.h
	src/bdf/bdflib.c

	# bzip2
	src/bzip2/ftbzip2.c

	# cache
	src/cache/ftcbasic.c
	src/cache/ftccache.c
	src/cache/ftccache.h
	src/cache/ftccback.h
	src/cache/ftccmap.c
	src/cache/ftcerror.h
	src/cache/ftcglyph.c
	src/cache/ftcglyph.h
	src/cache/ftcimage.c
	src/cache/ftcimage.h
	src/cache/ftcmanag.c
	src/cache/ftcmanag.h
	src/cache/ftcmru.c
	src/cache/ftcmru.h
	src/cache/ftcsbits.c
	src/cache/ftcsbits.h

	# cff
	src/cff/cffcmap.c
	src/cff/cffcmap.h
	src/cff/cffdrivr.c
	src/cff/cffdrivr.h
	src/cff/cfferrs.h
	src/cff/cffgload.c
	src/cff/cffgload.h
	src/cff/cffload.c
	src/cff/cffload.h
	src/cff/cffobjs.c
	src/cff/cffobjs.h
	src/cff/cffparse.c
	src/cff/cffparse.h
	src/cff/cfftoken.h

	# cid
	src/cid/ciderrs.h
	src/cid/cidgload.c
	src/cid/cidgload.h
	src/cid/cidload.c
	src/cid/cidload.h
	src/cid/cidobjs.c
	src/cid/cidobjs.h
	src/cid/cidparse.c
	src/cid/cidparse.h
	src/cid/cidriver.c
	src/cid/cidriver.h
	src/cid/cidtoken.h

	# gxvalid
	src/gxvalid/gxvalid.h
	src/gxvalid/gxvbsln.c
	src/gxvalid/gxvcommn.c
	src/gxvalid/gxvcommn.h
	src/gxvalid/gxverror.h
	src/gxvalid/gxvfeat.c
	src/gxvalid/gxvfeat.h
	src/gxvalid/gxvfgen.c
	src/gxvalid/gxvjust.c
	src/gxvalid/gxvkern.c
	src/gxvalid/gxvlcar.c
	src/gxvalid/gxvmod.c
	src/gxvalid/gxvmod.h
	src/gxvalid/gxvmort.c
	src/gxvalid/gxvmort.h
	src/gxvalid/gxvmort0.c
	src/gxvalid/gxvmort1.c
	src/gxvalid/gxvmort2.c
	src/gxvalid/gxvmort4.c
	src/gxvalid/gxvmort5.c
	src/gxvalid/gxvmorx.c
	src/gxvalid/gxvmorx.h
	src/gxvalid/gxvmorx0.c
	src/gxvalid/gxvmorx1.c
	src/gxvalid/gxvmorx2.c
	src/gxvalid/gxvmorx4.c
	src/gxvalid/gxvmorx5.c
	src/gxvalid/gxvopbd.c
	src/gxvalid/gxvprop.c
	src/gxvalid/gxvtrak.c

	# gzip
	src/gzip/ftgzip.c

	# lzw
	src/lzw/ftlzw.c
	src/lzw/ftzopen.h

	# otvalid
	src/otvalid/otvalid.h
	src/otvalid/otvbase.c
	src/otvalid/otvcommn.c
	src/otvalid/otvcommn.h
	src/otvalid/otverror.h
	src/otvalid/otvgdef.c
	src/otvalid/otvgpos.c
	src/otvalid/otvgpos.h
	src/otvalid/otvgsub.c
	src/otvalid/otvjstf.c
	src/otvalid/otvmath.c
	src/otvalid/otvmod.c
	src/otvalid/otvmod.h

	# pcf
	src/pcf/pcf.h
	src/pcf/pcfdrivr.c
	src/pcf/pcfdrivr.h
	src/pcf/pcferror.h
	src/pcf/pcfread.c
	src/pcf/pcfread.h
	src/pcf/pcfutil.c
	src/pcf/pcfutil.h
	
	# pfr
	src/pfr/module.mk
	src/pfr/pfrcmap.c
	src/pfr/pfrcmap.h
	src/pfr/pfrdrivr.c
	src/pfr/pfrdrivr.h
	src/pfr/pfrerror.h
	src/pfr/pfrgload.c
	src/pfr/pfrgload.h
	src/pfr/pfrload.c
	src/pfr/pfrload.h
	src/pfr/pfrobjs.c
	src/pfr/pfrobjs.h
	src/pfr/pfrsbit.c
	src/pfr/pfrsbit.h
	src/pfr/pfrtypes.h
	src/pfr/rules.mk

    # psaux
	src/psaux/afmparse.c
	src/psaux/afmparse.h
	src/psaux/cffdecode.c
	src/psaux/cffdecode.h
	src/psaux/psarrst.c
	src/psaux/psarrst.h
	src/psaux/psauxerr.h
	src/psaux/psauxmod.c
	src/psaux/psauxmod.h
	src/psaux/psblues.c
	src/psaux/psblues.h
	src/psaux/psconv.c
	src/psaux/psconv.h
	src/psaux/pserror.c
	src/psaux/pserror.h
	src/psaux/psfixed.h
	src/psaux/psfont.c
	src/psaux/psfont.h
	src/psaux/psft.c
	src/psaux/psft.h
	src/psaux/psglue.h
	src/psaux/pshints.c
	src/psaux/pshints.h
	src/psaux/psintrp.c
	src/psaux/psintrp.h
	src/psaux/psobjs.c
	src/psaux/psobjs.h
	src/psaux/psread.c
	src/psaux/psread.h
	src/psaux/psstack.c
	src/psaux/psstack.h
	src/psaux/pstypes.h
	src/psaux/t1cmap.c
	src/psaux/t1cmap.h
	src/psaux/t1decode.c
	src/psaux/t1decode.h

	# pshinter
	src/pshinter/pshalgo.c
	src/pshinter/pshalgo.h
	src/pshinter/pshglob.c
	src/pshinter/pshglob.h
	src/pshinter/pshmod.c
	src/pshinter/pshmod.h
	src/pshinter/pshnterr.h
	src/pshinter/pshrec.c
	src/pshinter/pshrec.h

	# psnames
	src/psnames/psmodule.c
	src/psnames/psmodule.h
	src/psnames/psnamerr.h
	src/psnames/pstables.h

	# raster
	src/raster/ftmisc.h
	src/raster/ftraster.c
	src/raster/ftraster.h
	src/raster/ftrend1.c
	src/raster/ftrend1.h
	src/raster/rasterrs.h

	# sdf
	src/sdf/ftbsdf.c
	src/sdf/ftsdf.c
	src/sdf/ftsdf.h
	src/sdf/ftsdfcommon.c
	src/sdf/ftsdfcommon.h
	src/sdf/ftsdferrs.h
	src/sdf/ftsdfrend.c
	src/sdf/ftsdfrend.h

	# sfnt
	src/sfnt/pngshim.c
	src/sfnt/pngshim.h
	src/sfnt/sfdriver.c
	src/sfnt/sfdriver.h
	src/sfnt/sferrors.h
	src/sfnt/sfobjs.c
	src/sfnt/sfobjs.h
	src/sfnt/sfwoff.c
	src/sfnt/sfwoff.h
	src/sfnt/sfwoff2.c
	src/sfnt/sfwoff2.h
	src/sfnt/ttbdf.c
	src/sfnt/ttbdf.h
	src/sfnt/ttcmap.c
	src/sfnt/ttcmap.h
	src/sfnt/ttcmapc.h
	src/sfnt/ttcolr.c
	src/sfnt/ttcolr.h
	src/sfnt/ttcpal.c
	src/sfnt/ttcpal.h
	src/sfnt/ttkern.c
	src/sfnt/ttkern.h
	src/sfnt/ttload.c
	src/sfnt/ttload.h
	src/sfnt/ttmtx.c
	src/sfnt/ttmtx.h
	src/sfnt/ttpost.c
	src/sfnt/ttpost.h
	src/sfnt/ttsbit.c
	src/sfnt/ttsbit.h
	src/sfnt/ttsvg.c
	src/sfnt/ttsvg.h
	src/sfnt/woff2tags.c
	src/sfnt/woff2tags.h

	# smooth
	src/smooth/ftgrays.c
	src/smooth/ftgrays.h
	src/smooth/ftsmerrs.h
	src/smooth/ftsmooth.c
	src/smooth/ftsmooth.h

	# svg
	src/svg/ftsvg.c
	src/svg/ftsvg.h
	src/svg/svgtypes.h

	# truetype
	src/truetype/ttdriver.c
	src/truetype/ttdriver.h
	src/truetype/tterrors.h
	src/truetype/ttgload.c
	src/truetype/ttgload.h
	src/truetype/ttgxvar.c
	src/truetype/ttgxvar.h
	src/truetype/ttinterp.c
	src/truetype/ttinterp.h
	src/truetype/ttobjs.c
	src/truetype/ttobjs.h
	src/truetype/ttpload.c
	src/truetype/ttpload.h
	src/truetype/ttsubpix.c
	src/truetype/ttsubpix.h

	# type1
	src/type1/t1afm.c
	src/type1/t1afm.h
	src/type1/t1driver.c
	src/type1/t1driver.h
	src/type1/t1errors.h
	src/type1/t1gload.c
	src/type1/t1gload.h
	src/type1/t1load.c
	src/type1/t1load.h
	src/type1/t1objs.c
	src/type1/t1objs.h
	src/type1/t1parse.c
	src/type1/t1parse.h
	src/type1/t1tokens.h

	# type42
	src/type42/t42drivr.c
	src/type42/t42drivr.h
	src/type42/t42error.h
	src/type42/t42objs.c
	src/type42/t42objs.h
	src/type42/t42parse.c
	src/type42/t42parse.h
	src/type42/t42types.h

	# winfonts
	src/winfonts/fnterrs.h
	src/winfonts/winfnt.c
	src/winfonts/winfnt.h
)

add_compile_definitions(
	_CRT_SECURE_NO_WARNINGS
	FT2_BUILD_LIBRARY
	FT_CONFIG_OPTION_SYSTEM_ZLIB
)

add_library(freetype STATIC ${FREETYPE_SOURCE})

target_include_directories(freetype 
    PRIVATE
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/freetype/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/zlib/include
		${PROJECT_SOURCE_DIR}/Chonps/Dependencies/freetype/include/freetype
)

target_link_libraries(freetype
	PRIVATE
		zlib
)