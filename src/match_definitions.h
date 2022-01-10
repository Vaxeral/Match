#ifndef DEFINITIONS_HEADER_202112270551
#define DEFINITIONS_HEADER_202112270551

#include <stdio.h> // fprintf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS

#define LOGERR(message) fprintf(stderr, "%s:%i error: %s\n", __FILE__, __LINE__, message)
#define LOGMSG(message) fprintf(stdout, "%s:%i log: %s\n", __FILE__, __LINE__, message)

#ifdef DEBUG
	#define ASSERT(assertion, message) (!(assertion)) ? LOGERR(message), abort() : (void)0
	#define ASSERT_NO_ERROR(error_code, message) (error_code) ? LOGERR(message), abort() : (void)0
#else
	#define ASSERT(assertion, message)
	#define ASSERT_NO_ERROR(error_code, message)
#endif

#define hex_to_rgba(hex) \
	(hex & 0xff000000) >> 24, \
	(hex & 0xff0000) >> 16, \
	(hex & 0xff00) >> 8, \
	hex & 0xff

#define COLOR_CREAM hex_to_rgba(0xfff1a011)

#define MATCH_GEM_KIND_CLIP_RECT_WIDTH 64
#define MATCH_GEM_KIND_CLIP_RECT_HEIGHT 64

#endif