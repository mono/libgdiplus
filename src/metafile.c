/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "metafile.h"

static GpMetafile*
gdip_metafile_create ()
{
	GpMetafile* mf = (GpMetafile*) GdipAlloc (sizeof (GpMetafile));
	if (mf) {
		mf->base.type = imageMetafile;
		mf->delete = FALSE;
		mf->data = NULL;
		mf->length = 0;
	}
	return mf;
}

GpStatus
gdip_metafile_clone (GpMetafile *metafile, GpMetafile **clonedmetafile)
{
	GpMetafile *mf = gdip_metafile_create ();
	if (!mf)
		return OutOfMemory;

	memcpy (&mf->metafile_header, &metafile->metafile_header, sizeof (MetafileHeader));
	if (metafile->length > 0) {
		mf->data = GdipAlloc (metafile->length);
		memcpy (mf->data, metafile->data, metafile->length);
		mf->length = metafile->length;
	}

	*clonedmetafile = mf;
	return Ok;
}

GpStatus
gdip_metafile_dispose (GpMetafile *metafile)
{
	if (!metafile)
		return InvalidParameter;

	/* TODO deal with "delete" flag */
	metafile->length = 0;
	if (metafile->data) {
		GdipFree (metafile->data);
		metafile->data = NULL;
	}

	GdipFree (metafile);
	return Ok;
}

MetafilePlayContext*
gdip_metafile_play_setup (GpMetafile *metafile, GpGraphics *graphics, int x, int y, int width, int height)
{
	int i, n;
	MetaObject *obj;
	MetafilePlayContext *context;
	/* metafiles always render as 32bppRgb */
	int stride = width * 4;

	if (!metafile || !graphics)
		return NULL;

	n = metafile->metafile_header.WmfHeader.mtNoObjects;
	context = GdipAlloc (sizeof (MetafilePlayContext));
	context->metafile = metafile;
	context->graphics = graphics;
	context->x = x;
	context->y = y;
	context->width = width;
	context->height = height;
	context->scan0 = GdipAlloc (height * stride);
	metafile->base.surface = cairo_image_surface_create_for_data (context->scan0, CAIRO_FORMAT_ARGB32, width, height, stride);
	/* Create* functions store the object here */
	context->created.type = METAOBJECT_TYPE_EMPTY;
	context->created.ptr = NULL;
	/* SelectObject | DeleteObject works on this array */
	context->objects = (MetaObject*) GdipAlloc (n * sizeof (MetaObject));
	if (!context->objects) {
		GdipFree (context);
		return NULL;
	}

	obj = context->objects;
	for (i = 0; i < n; i++) {
		obj->type = METAOBJECT_TYPE_EMPTY;
		obj->ptr = NULL;
		obj++;
	}
	return context;
}

GpStatus
gdip_metafile_play (MetafilePlayContext *context)
{
	GpStatus status;

	if (!context)
		return InvalidParameter;

	switch (context->metafile->metafile_header.Type) {
	case METAFILETYPE_WMFPLACEABLE:
	case METAFILETYPE_WMF:
		return gdip_metafile_play_wmf (context);
	case METAFILETYPE_EMF:
	case METAFILETYPE_EMFPLUSONLY:
	case METAFILETYPE_EMFPLUSDUAL:
		return gdip_metafile_play_emf (context);
	default:
		g_warning ("Invalid metafile format %d", context->metafile->metafile_header.Type);
		break;
	}
	return NotImplemented;
}

GpStatus
gdip_metafile_play_cleanup (MetafilePlayContext *context)
{
	if (!context)
		return InvalidParameter;

	GdipFree (context->scan0);
	context->scan0 = NULL;
	context->graphics = NULL;
/* TODO cleanup surface */
	context->created.type = METAOBJECT_TYPE_EMPTY;
	context->created.ptr = NULL;
	if (context->objects) {
		int i;
		GpMetafile *metafile = context->metafile;
		MetaObject *obj = context->objects;
		/* free each object */
		for (i = 0; i < metafile->metafile_header.WmfHeader.mtNoObjects; i++) {
			/* gdip_metafile_DeleteObject (context, i); */
			obj++;
		}
		GdipFree (context->objects);
		context->objects = NULL;
	}
	context->selected_pen = NULL;
	context->selected_brush = NULL;
	return Ok;
}

static GpStatus
combine_headers (GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header)
{
	if (wmfPlaceableFileHeader) {
		header->Type = METAFILETYPE_WMFPLACEABLE;
		header->X = wmfPlaceableFileHeader->BoundingBox.Left;
		header->Y = wmfPlaceableFileHeader->BoundingBox.Top;
		header->Width = wmfPlaceableFileHeader->BoundingBox.Right - wmfPlaceableFileHeader->BoundingBox.Left;
		header->Height= wmfPlaceableFileHeader->BoundingBox.Bottom - wmfPlaceableFileHeader->BoundingBox.Top;
		header->DpiX = wmfPlaceableFileHeader->Inch;
		header->DpiY = wmfPlaceableFileHeader->Inch;
	} else {
		header->Type = METAFILETYPE_WMF;
	}
	header->Size = header->WmfHeader.mtSize * 2;
	header->Version = header->WmfHeader.mtVersion;
	header->EmfPlusFlags = 0;
	header->EmfPlusHeaderSize = 0;
	header->LogicalDpiX = 0;
	header->LogicalDpiY = 0;
	return Ok;
}

GpStatus 
gdip_get_metafileheader_from (void *pointer, MetafileHeader *header, bool useFile)
{
	int size;
	DWORD key;
	GpStatus status = NotImplemented;
	WmfPlaceableFileHeader aldus_header;

	/* peek at first DWORD to select the right format */
	size = sizeof (DWORD);
	if (gdip_read_wmf_data (pointer, (void*)&key, size, useFile) != size)
		return GenericError;

	switch (key) {
	case ALDUS_PLACEABLE_METAFILE_KEY:
		aldus_header.Key = key;
		size = sizeof (WmfPlaceableFileHeader) - size;
		if (gdip_read_wmf_data (pointer, (void*)(&aldus_header) + sizeof (DWORD), size, useFile) != size)
			return NotImplemented;
#if FALSE
g_warning ("ALDUS_PLACEABLE_METAFILE key %d, hmf %d, L %d, T %d, R %d, B %d, inch %d, reserved %d, checksum %d", 
	aldus_header.Key, aldus_header.Hmf, aldus_header.BoundingBox.Left, aldus_header.BoundingBox.Top,
	aldus_header.BoundingBox.Right, aldus_header.BoundingBox.Bottom, aldus_header.Inch, aldus_header.Reserved,
	aldus_header.Checksum);
#endif
		size = sizeof (METAHEADER);
		if (gdip_read_wmf_data (pointer, (void*)&header->WmfHeader, size, useFile) != size)
			return NotImplemented;

		status = combine_headers (&aldus_header, header);
		break;
	case WMF_TYPE_AND_HEADERSIZE_KEY:
		memcpy (&header->WmfHeader, &key, size);

		size = sizeof (METAHEADER) - size;
		if (gdip_read_wmf_data (pointer, (void*)(&header->WmfHeader) + sizeof (DWORD), size, useFile) != size)
			return NotImplemented;

		status = combine_headers (NULL, header);
		break;
	case EMF_EMR_HEADER_KEY:
		break;
	default:
		g_warning ("Unknown metafile format: key %d", key);
		status = GenericError;
	}

#if FALSE
g_warning ("METAHEADER type %d, header %d, version %d, size %d, #obj %d, max rec %d, #params %d",
	header->WmfHeader.mtType, header->WmfHeader.mtHeaderSize, header->WmfHeader.mtVersion,
	header->WmfHeader.mtSize, header->WmfHeader.mtNoObjects, header->WmfHeader.mtMaxRecord,
	header->WmfHeader.mtNoParameters);
#endif

	return status;
}

GpStatus 
gdip_get_metafile_from (void *pointer, GpMetafile **metafile, bool useFile)
{
	GpStatus status = OutOfMemory;
	GpMetafile *mf = gdip_metafile_create ();
	if (!mf)
		goto error;

	/* decode metafile header */
	status = gdip_get_metafileheader_from (pointer, &mf->metafile_header, useFile);
	if (status != Ok)
		goto error;

	switch (mf->metafile_header.Type) {
	case METAFILETYPE_WMFPLACEABLE:
	case METAFILETYPE_WMF:
		mf->base.image_format = WMF;
		/* note: mtSize is in WORD, mtSize contains the METAHEADER, mf->length is in bytes */
		mf->length = mf->metafile_header.WmfHeader.mtSize * 2 - sizeof (METAHEADER);
		mf->data = (BYTE*) GdipAlloc (mf->length);
		if (!mf->data)
			goto error;
		/* copy data in memory (to play it later) */
		if (gdip_read_wmf_data (pointer, (void*)mf->data, mf->length, useFile) != mf->length) {
			status = InvalidParameter;
			goto error;
		}
		break;
	case METAFILETYPE_EMF:
	case METAFILETYPE_EMFPLUSONLY:
	case METAFILETYPE_EMFPLUSDUAL:
		mf->base.image_format = EMF;
/* TODO - handle EMF */
		status = NotImplemented;
		goto error;
		break;
	}

	*metafile = mf;
	return Ok;
error:
	if (mf)
		gdip_metafile_dispose (mf);
	*metafile = NULL;
	return status;
}

/* public (GDI+) functions */

GpStatus
GdipCreateMetafileFromFile (GDIPCONST WCHAR *file, GpMetafile **metafile)
{
	FILE *fp;
	char *file_name;
	GpStatus status = GenericError;

	if (!file || !metafile)
		return InvalidParameter;

	file_name = (char *) ucs2_to_utf8 ((const gunichar2 *)file, -1);
	if (!file_name)
		return InvalidParameter;
	
	fp = fopen (file_name, "rb");
	if (fp) {
		status = gdip_get_metafile_from (fp, metafile, TRUE);
		fclose (fp);
	}
	GdipFree (file_name);
	return status;
}

/*
 * GdipCreateMetafileFromStream will never be implemented, as 'stream' is a COM IStream ...
 */
GpStatus
GdipCreateMetafileFromStream (void *stream, GpMetafile **metafile)
{
	return NotImplemented;
}
/*
 * instead we'll use delegates to load the metafile header with this function
 */
GpStatus GdipCreateMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	GpMetafile **metafile)
{
	GpStatus status = InvalidParameter;
	dstream_t *loader;

	if (!metafile)
		return InvalidParameter;

	loader = dstream_input_new (getBytesFunc, seekFunc);
	if (loader) {
		status = gdip_get_metafile_from (loader, metafile, FALSE);
		dstream_free (loader);
	}
	return status;
}

GpStatus
GdipCreateMetafileFromEmf (HENHMETAFILE hEmf, BOOL deleteEmf, GpMetafile **metafile)
{
	GpStatus status;

	if (!hEmf || !metafile)
		return InvalidParameter;

	status = gdip_metafile_clone ((GpMetafile*)hEmf, metafile);
	if (status != Ok)
		return status;

	(*metafile)->delete = deleteEmf;
	return Ok;
}

GpStatus
GdipCreateMetafileFromWmf (HMETAFILE hWmf, BOOL deleteWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile)
{
	GpStatus status;

	if (!hWmf || !metafile)
		return InvalidParameter;

	status = gdip_metafile_clone ((GpMetafile*)hWmf, metafile);
	if (status != Ok)
		return status;

	status = GdipGetMetafileHeaderFromWmf (hWmf, wmfPlaceableFileHeader, &(*metafile)->metafile_header);
	if (status != Ok) {
		GdipFree (*metafile);
		return status;
	}

	(*metafile)->delete = deleteWmf;
	return Ok;
}

GpStatus
GdipGetMetafileHeaderFromWmf (HMETAFILE hWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header)
{
	GpMetafile *mf;

	if (!hWmf || !wmfPlaceableFileHeader || !header)
		return InvalidParameter;

	mf = (GpMetafile*)hWmf;
	memcpy (header, &mf->metafile_header, sizeof (MetafileHeader));
	return combine_headers (wmfPlaceableFileHeader, header);
}

GpStatus
GdipGetMetafileHeaderFromEmf (HENHMETAFILE hEmf, MetafileHeader *header)
{
	GpMetafile* metafile = (GpMetafile*)hEmf;

	if (!metafile || !header)
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case METAFILETYPE_EMF:
	case METAFILETYPE_EMFPLUSONLY:
	case METAFILETYPE_EMFPLUSDUAL:
		return GdipGetMetafileHeaderFromMetafile (metafile, header);
	/* you can get a HENHMETAFILE from a WMF metafile but it seems (unit tests) that you can't use it as such */
	case METAFILETYPE_WMFPLACEABLE:
	case METAFILETYPE_WMF:
	default:
		return InvalidParameter;
	}
}

GpStatus
GdipGetMetafileHeaderFromFile (GDIPCONST WCHAR *filename, MetafileHeader *header)
{
	FILE *fp;
	char *file_name;
	GpStatus status = InvalidParameter;

	if (!filename || !header)
		return InvalidParameter;

	file_name = (char *) ucs2_to_utf8 ((const gunichar2 *)filename, -1);
	if (!file_name)
		return InvalidParameter;
	
	fp = fopen (file_name, "rb");
	if (fp) {
		status = gdip_get_metafileheader_from (fp, header, TRUE);
		fclose (fp);
	}
	GdipFree (file_name);
	return status;
}

/*
 * GdipGetMetafileHeaderFromStream will never be implemented, as 'stream' is a COM IStream ...
 */
GpStatus
GdipGetMetafileHeaderFromStream (void *stream, MetafileHeader *header)
{
	return NotImplemented;
}
/*
 * instead we'll use delegates to load the metafile header with this function
 */
GpStatus 
GdipGetMetafileHeaderFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	MetafileHeader *header)
{
	GpStatus status = InvalidParameter;
	dstream_t *loader;

	if (!header)
		return status;

	loader = dstream_input_new (getBytesFunc, seekFunc);
	if (loader) {
		status = gdip_get_metafileheader_from (loader, header, FALSE);
		dstream_free (loader);
	}
	return status;
}

GpStatus
GdipGetMetafileHeaderFromMetafile (GpMetafile *metafile, MetafileHeader *header)
{
	if (!metafile || !header)
		return InvalidParameter;

	memcpy (header, &metafile->metafile_header, sizeof (MetafileHeader));
	return Ok;
}

GpStatus
GdipGetHemfFromMetafile (GpMetafile *metafile, HENHMETAFILE *hEmf)
{
	if (!metafile || !hEmf)
		return InvalidParameter;

	*hEmf = (HENHMETAFILE)metafile;
	return Ok;
}

GpStatus
GdipGetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT *metafileRasterizationLimitDpi)
{
	if (!metafile || !metafileRasterizationLimitDpi)
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case METAFILETYPE_EMF:
	case METAFILETYPE_EMFPLUSDUAL:
/* TODO */
		*metafileRasterizationLimitDpi = 0;
		return Ok;
	case METAFILETYPE_WMFPLACEABLE:
	case METAFILETYPE_WMF:
	case METAFILETYPE_EMFPLUSONLY:
	default:
		return WrongState;
	}
}

GpStatus
GdipSetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT metafileRasterizationLimitDpi)
{
	if (!metafile || (metafileRasterizationLimitDpi < 0))
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case METAFILETYPE_EMF:
	case METAFILETYPE_EMFPLUSDUAL:
		// values less than 10 left resolution unchanged (from GDI+ documentation)
		if (metafileRasterizationLimitDpi >= 10) {
			/* TODO */
		}
		return Ok;
	case METAFILETYPE_WMFPLACEABLE:
	case METAFILETYPE_WMF:
	case METAFILETYPE_EMFPLUSONLY:
	default:
		return WrongState;
	}
}

GpStatus
GdipPlayMetafileRecord (GDIPCONST GpMetafile *metafile, EmfPlusRecordType recordType, UINT flags, UINT dataSize, GDIPCONST BYTE* data)
{
	/* TODO */
	return NotImplemented;
}
