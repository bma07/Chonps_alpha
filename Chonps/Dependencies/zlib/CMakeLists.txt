set(ZLIB_SOURCE
	include/adler32.c
	include/compress.c
	include/crc32.c
	include/crc32.h
	include/deflate.c
	include/deflate.h
	include/gzclose.c
	include/gzguts.h
	include/gzlib.c
	include/gzread.c
	include/gzwrite.c
	include/infback.c
	include/inffast.c
	include/inffast.h
	include/inffixed.h
	include/inflate.c
	include/inflate.h
	include/inftrees.c
	include/inftrees.h
	include/trees.c
	include/trees.h
	include/uncompr.c
	include/zconf.h
	include/zlib.h
	include/zutil.c
	include/zutil.h
)

add_compile_definitions(
	_CRT_SECURE_NO_WARNINGS
	NO_FSEEKO
	_CRT_SECURE_NO_DEPRECATE
	_CRT_NONSTDC_NO_DEPRECATE
)

add_library(zlib STATIC ${ZLIB_SOURCE})