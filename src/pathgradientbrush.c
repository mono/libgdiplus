/*
 * pathgradientbrush.c
 *
 * Author:
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) 2004 Vladimir Vukicevic
 * Copyright (C) 2004 Novell, Inc.
 *
 */

#include "pathgradientbrush.h"

static GpStatus gdip_pgrad_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_pgrad_clone_brush (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_pgrad_destroy (GpBrush *brush);

static BrushClass pathgradient_vtable = { BrushTypePathGradient,
                                          gdip_pgrad_setup,
                                          gdip_pgrad_clone_brush,
                                          gdip_pgrad_destroy };

void
gdip_pathgradient_init (GpPathGradient *pg)
{
    gdip_brush_init (&pg->base, &pathgradient_vtable);
    pg->boundary = NULL;
    pg->boundaryColors = NULL;
    pg->center.X = 0.0f;
    pg->center.Y = 0.0f;
    pg->centerColor = 0;
    pg->wrapMode = 0;
    GdipCreateMatrix (&pg->transform);
}

GpPathGradient*
gdip_pathgradient_new (void)
{
    GpPathGradient *result = (GpPathGradient *) GdipAlloc (sizeof (GpPathGradient));

    if (result)
        gdip_pathgradient_init (result);

    return result;
}

GpStatus
gdip_pgrad_clone_brush (GpBrush *brush, GpBrush **clonedBrush)
{
    GpPathGradient *pgbrush;
    GpPathGradient *newbrush;

    g_return_val_if_fail (brush != NULL, InvalidParameter);

    newbrush = (GpPathGradient *) GdipAlloc (sizeof (GpPathGradient));

    g_return_val_if_fail (newbrush != NULL, OutOfMemory);

    pgbrush = (GpPathGradient *) brush;

    newbrush->base = pgbrush->base;
    if (pgbrush->boundary) {
        GdipClonePath (pgbrush->boundary, &newbrush->boundary);
    } else {
        newbrush->boundary = NULL;
    }

    if (pgbrush->boundaryColors) {
        newbrush->boundaryColors = GdipAlloc (sizeof(ARGB) * pgbrush->boundary->count);
        memcpy (newbrush->boundaryColors, pgbrush->boundaryColors, sizeof(ARGB) * pgbrush->boundary->count);
    } else {
        newbrush->boundaryColors = NULL;
    }
    newbrush->center = pgbrush->center;
    newbrush->centerColor = pgbrush->centerColor;
    newbrush->wrapMode = pgbrush->wrapMode;
    GdipCloneMatrix (pgbrush->transform, &newbrush->transform);

    GpPathGradient **pgbrushClone = (GpPathGradient **) clonedBrush;

    return Ok;
}

GpStatus
gdip_pgrad_destroy (GpBrush *brush)
{
    GpPathGradient *pgbrush;

    g_return_val_if_fail (brush != NULL, InvalidParameter);

    pgbrush = (GpPathGradient *) brush;

    if (pgbrush->boundary)
        GdipDeletePath (pgbrush->boundary);

    if (pgbrush->boundaryColors)
        GdipFree (pgbrush->boundaryColors);

    if (pgbrush->transform)
        GdipDeleteMatrix (pgbrush->transform);

    return Ok;
}

GpStatus
gdip_pgrad_setup (GpGraphics *graphics, GpBrush *brush)
{
    GpPathGradient *pgbrush;

    g_return_val_if_fail (graphics != NULL, InvalidParameter);
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    pgbrush = (GpPathGradient *) brush;

    if (pgbrush->boundary == NULL)
        return Ok;              /* do nothing */

    /* As a stub, we simply set the color to the center color */
    cairo_set_rgb_color (graphics->ct,
                         ARGB_RED_N(pgbrush->centerColor),
                         ARGB_GREEN_N(pgbrush->centerColor),
                         ARGB_BLUE_N(pgbrush->centerColor));

    cairo_set_alpha (graphics->ct,
                     ARGB_ALPHA_N(pgbrush->centerColor));

    return Ok;
}

GpStatus
GdipCreatePathGradient (GDIPCONST GpPointF *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient)
{
    g_return_val_if_fail (polyGradient != NULL, InvalidParameter);

    GpPathGradient *gp = gdip_pathgradient_new ();
    GpPath *gppath = NULL;

    GdipCreatePath (FillModeAlternate, /* ??? */
                    &gppath);
    GdipAddPathLine2 (gppath, points, count);

    gp->boundary = gppath;
    gp->wrapMode = wrapMode;

    *polyGradient = gp;

    return Ok;
}

GpStatus
GdipCreatePathGradientI (GDIPCONST GpPoint *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient)
{
    int i;
    GpStatus result;
    GpPointF *newPoints = GdipAlloc (sizeof (GpPointF) * count);
    for (i = 0; i < count; i++) {
        newPoints[i].X = points[i].X;
        newPoints[i].Y = points[i].Y;
    }
    result = GdipCreatePathGradient (newPoints, count, wrapMode, polyGradient);
    GdipFree (newPoints);
    return result;
}

GpStatus
GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient)
{
    GpPathGradient *gp;

    g_return_val_if_fail (path != NULL, InvalidParameter);
    g_return_val_if_fail (polyGradient != NULL, InvalidParameter);

    gp = gdip_pathgradient_new ();
    GdipClonePath ((GpPath *) path, &(gp->boundary));

    *polyGradient = gp;

    return Ok;
}

GpStatus
GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (colors != NULL, InvalidParameter);

    *colors = brush->centerColor;
    return Ok;
}

GpStatus
GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    brush->centerColor = colors;
    return Ok;
}

GpStatus
GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *color, int *count)
{
    int i;

    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (color != NULL, InvalidParameter);
    g_return_val_if_fail (count != NULL, InvalidParameter);

    if (brush->boundary == NULL || brush->boundaryColors == NULL) {
        /* unclear what should happen in this case */
        *count = 0;
        return Ok;
    }

    for (i = 0; i < *count && i < brush->boundary->count; i++) {
        color[i] = brush->boundaryColors[i];
    }

    *count = i;
    return Ok;
}

GpStatus
GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *color, int *count)
{
    ARGB *colors;
    int i;

    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (color != NULL, InvalidParameter);
    g_return_val_if_fail (count != NULL, InvalidParameter);

    if (brush->boundary == NULL)
        return InvalidParameter;

    /* Unclear on what we should do if count < number of path points.  We're going to duplicate
     * the last color.
     */
    if (*count != brush->boundary->count || brush->boundaryColors != NULL) {
        if (brush->boundaryColors)
            GdipFree (brush->boundaryColors);
        brush->boundaryColors = (ARGB *) GdipAlloc (sizeof(ARGB) * brush->boundary->count);
    }

    colors = brush->boundaryColors;

    for (i = 0; i < *count; i++) {
        colors[i] = color[i];
    }

    if (*count < brush->boundary->count) {
        int lastColor = *count - 1;
        for ( ; i < brush->boundary->count; i++) {
            colors[i] = color[lastColor];
        }
    }

    return Ok;
}

GpStatus
GdipGetPathGradientPath (GpPathGradient *brush, GpPath *path)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (path != NULL, InvalidParameter);

    return NotImplemented; /* GdipGetPathGradientPath - NotImplemented on windows */
}

GpStatus
GdipSetPathGradientPath (GpPathGradient *brush, GDIPCONST GpPath *path)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (path != NULL, InvalidParameter);

    return NotImplemented; /* GdipSetPathGradientPath - NotImplemented on windows */
}

GpStatus
GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *point)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (point != NULL, InvalidParameter);

    point->X = brush->center.X;
    point->Y = brush->center.Y;

    return Ok;
}

GpStatus
GdipGetPathGradientCenterPointI (GpPathGradient *brush, GpPoint *point)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (point != NULL, InvalidParameter);

    point->X = brush->center.X;
    point->Y = brush->center.Y;

    return Ok;
}

GpStatus
GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *point)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (point != NULL, InvalidParameter);

    brush->center.X = point->X;
    brush->center.Y = point->Y;

    return Ok;
}

GpStatus
GdipSetPathGradientCenterPointI (GpPathGradient *brush, GDIPCONST GpPoint *point)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (point != NULL, InvalidParameter);

    brush->center.X = point->X;
    brush->center.Y = point->Y;

    return Ok;
}

GpStatus
GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect)
{
    GpPathData pdata;
    GpRectF nrect = {0.0f, 0.0f, 0.0f, 0.0f};
    int i;

    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (rect != NULL, InvalidParameter);

    if (brush->boundary == NULL) {
        *rect = nrect;
        return Ok;
    }

    GdipGetPathData (brush->boundary, &pdata);

    for (i = 0; i < pdata.Count; i++) {
        gdip_rect_expand_by (&nrect, &(pdata.Points[i]));
    }

    *rect = nrect;
    return Ok;
}

GpStatus
GdipGetPathGradientRectI (GpPathGradient *brush, GpRect *rect)
{
    GpRectF nrect;
    GpStatus status;
    status = GdipGetPathGradientRect (brush, &nrect);
    rect->X = nrect.X;
    rect->Y = nrect.Y;
    rect->Width = nrect.Width;
    rect->Height = nrect.Height;
    return status;
}

GpStatus
GdipGetPathGradientPointCount (GpPathGradient *brush, int *count)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (count != NULL, InvalidParameter);

    return GdipGetPointCount (brush->boundary, count);
}

GpStatus
GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, int *count)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (count != NULL, InvalidParameter);

    /* Always equal to the number of points for us */
    return GdipGetPointCount (brush->boundary, count);
}

GpStatus
GdipSetPathGradientGammaCorrection (GpPathGradient *brush, BOOL useGammaCorrection)
{
    return NotImplemented;      /* GdipSetPathGradientGammaCorrection */
}

GpStatus
GdipGetPathGradientGammaCorrection (GpPathGradient *brush, BOOL *useGammaCorrection)
{
    return NotImplemented;      /*  GdipGetPathGradientGammaCorrection */
}

GpStatus
GdipGetPathGradientBlendCount (GpPathGradient *brush, int *count)
{
    return NotImplemented;      /* GdipGetPathGradientBlendCount */
}

GpStatus
GdipGetPathGradientBlend (GpPathGradient *brush, float *blend, float *positions, int count)
{
    return NotImplemented;      /* GdipGetPathGradientBlend */
}

GpStatus
GdipSetPathGradientBlend (GpPathGradient *brush, GDIPCONST float *blend, GDIPCONST float *positions, int count)
{
    return NotImplemented;      /* GdipSetPathGradientBlend */
}

GpStatus
GdipGetPathGradientPresetBlendCount (GpPathGradient *brush, int *count)
{
    return NotImplemented;      /* GdipGetPathGradientPresetBlendCount */
}

GpStatus
GdipGetPathGradientPresetBlend (GpPathGradient *brush, ARGB *blend, float *positions, int count)
{
    return NotImplemented;      /* GdipGetPathGradientPresetBlend */
}

GpStatus
GdipSetPathGradientPresetBlend (GpPathGradient *brush, GDIPCONST ARGB *blend, GDIPCONST float *positions, int count)
{
    return NotImplemented;      /* GdipSetPathGradientPresetBlend */
}

GpStatus
GdipSetPathGradientSigmaBlend (GpPathGradient *brush, float focus, float scale)
{
    return NotImplemented;      /* GdipSetPathGradientSigmaBlend */
}

GpStatus
GdipSetPathGradientLinearBlend (GpPathGradient *brush, float focus, float scale)
{
    return NotImplemented;      /* GdipSetPathGradientLinearBlend */
}

GpStatus
GdipGetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode *wrapMode)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (wrapMode != NULL, InvalidParameter);

    *wrapMode = brush->wrapMode;
    return Ok;
}

GpStatus
GdipSetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode wrapMode)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    brush->wrapMode = wrapMode;
    return Ok;
}

GpStatus
GdipGetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (matrix != NULL, InvalidParameter);

    *matrix = *(brush->transform);
    return Ok;
}

GpStatus
GdipSetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (matrix != NULL, InvalidParameter);

    *(brush->transform) = *matrix;
    return Ok;
}

GpStatus
GdipResetPathGradientTransform (GpPathGradient *brush)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    cairo_matrix_set_identity ((cairo_matrix_t *) brush->transform);
    return Ok;
}

GpStatus
GdipMultiplyPathGradientTransform (GpPathGradient *brush, GDIPCONST GpMatrix *matrix, GpMatrixOrder order)
{
    cairo_matrix_t mat;
    g_return_val_if_fail (brush != NULL, InvalidParameter);
    g_return_val_if_fail (matrix != NULL, InvalidParameter);

    if (order == MatrixOrderPrepend)
        cairo_matrix_multiply (&mat, matrix, brush->transform);
    else if (order == MatrixOrderAppend)
        cairo_matrix_multiply (&mat, brush->transform, matrix);
    else
        return InvalidParameter;

    *(brush->transform) = mat;

    return Ok;
}

GpStatus
GdipTranslatePathGradientTransform (GpPathGradient *brush, float dx, float dy, GpMatrixOrder order)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    if (order == MatrixOrderAppend) {
        cairo_matrix_translate (brush->transform, dx, dy);
    } else if (order == MatrixOrderPrepend) {
        cairo_matrix_t mat, matres;
        cairo_matrix_set_identity (&mat);
        cairo_matrix_translate (&mat, dx, dy);
        cairo_matrix_multiply (&matres, &mat, brush->transform);
        *(brush->transform) = matres;
    } else {
        return InvalidParameter;
    }

    return Ok;
}

GpStatus
GdipScalePathGradientTransform (GpPathGradient *brush, float sx, float sy, GpMatrixOrder order)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    if (order == MatrixOrderAppend) {
        cairo_matrix_scale (brush->transform, sx, sy);
    } else if (order == MatrixOrderPrepend) {
        cairo_matrix_t mat, matres;
        cairo_matrix_set_identity (&mat);
        cairo_matrix_scale (&mat, sx, sy);
        cairo_matrix_multiply (&matres, &mat, brush->transform);
        *(brush->transform) = matres;
    } else {
        return InvalidParameter;
    }

    return Ok;
}

GpStatus
GdipRotatePathGradientTransform (GpPathGradient *brush, float angle, GpMatrixOrder order)
{
    g_return_val_if_fail (brush != NULL, InvalidParameter);

    if (order == MatrixOrderAppend) {
        cairo_matrix_rotate (brush->transform, angle * DEGTORAD);
    } else if (order == MatrixOrderPrepend) {
        cairo_matrix_t mat, matres;
        cairo_matrix_set_identity (&mat);
        cairo_matrix_rotate (&mat, angle * DEGTORAD);
        cairo_matrix_multiply (&matres, &mat, brush->transform);
        *(brush->transform) = matres;
    } else {
        return InvalidParameter;
    }

    return Ok;
}

GpStatus
GdipGetPathGradientFocusScales (GpPathGradient *brush, float *xScale, float *yScale)
{
    return NotImplemented; /* GdipGetPathGradientFocusScales */
}

GpStatus
GdipSetPathGradientFocusScales (GpPathGradient *brush, float xScale, float yScale)
{
    return NotImplemented; /* GdipSetPathGradientFocusScales */
}


