set(LIBPNG_SOURCE
	include/png.c
	include/png.h
	include/pngconf.h
	include/pngdebug.h
	include/pngerror.c
	include/pngget.c
	include/pnginfo.h
	include/pnglibconf.h
	include/pngmem.c
	include/pngpread.c
	include/pngpriv.h
	include/pngread.c
	include/pngrio.c
	include/pngrtran.c
	include/pngrutil.c
	include/pngset.c
	include/pngstruct.h
	include/pngtest.c
	include/pngtrans.c
	include/pngwio.c
	include/pngwrite.c
	include/pngwtran.c
	include/pngwutil.c

	include/arm/arm_init.c
	include/arm/filter_neon.S
	include/arm/filter_neon_intrinsics.c
	include/arm/palette_neon_intrinsics.c

	include/intel/filter_sse2_intrinsics.c
	include/intel/intel_init.c
)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

add_library(libpng STATIC ${LIBPNG_SOURCE})

target_include_directories(libpng 
	PRIVATE ${PROJECT_SOURCE_DIR}/Chonps/Dependencies/zlib/include
)

target_link_libraries(libpng
	PRIVATE
		zlib
)