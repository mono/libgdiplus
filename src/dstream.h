#ifndef __DSTREAM_H
#define __DSTREAM_H

#include "gdip.h"

typedef struct _dstream_pvt dstream_private;
typedef struct _dstream dstream_t;
struct _dstream {
	dstream_private *pvt;
};

dstream_t *dstream_input_new (GetBytesDelegate read, SeekDelegate seek);
int dstream_read (dstream_t *loader, void *buffer, int size, char peek);
void dstream_skip (dstream_t *loader, int nbytes);
void dstream_free (dstream_t *loader);
void dstream_keep_exif_buffer (dstream_t *loader);
void dstream_get_exif_buffer (dstream_t *loader, unsigned char **ptr, unsigned int *length);

#endif

