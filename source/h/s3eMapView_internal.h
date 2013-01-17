/*
 * Internal header for the s3eMapView extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef S3EMAPVIEW_INTERNAL_H
#define S3EMAPVIEW_INTERNAL_H

#include "s3eTypes.h"
#include "s3eMapView.h"
#include "s3eMapView_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult s3eMapViewInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult s3eMapViewInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void s3eMapViewTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void s3eMapViewTerminate_platform();
s3eMapView* s3eMapViewCreate_platform();

s3eResult s3eMapViewDestroy_platform(s3eMapView* pMapView);

void s3eMapViewSetScreenRect_platform(s3eMapView* pMapView, int x, int y, int w, int h);

void s3eMapViewSetVisible_platform(s3eMapView* pMapView, bool visible);

void s3eMapViewSetType_platform(s3eMapView* pMapView, s3eMapViewType type);

void s3eMapViewSetShowUserLocation_platform(s3eMapView* pMapView, bool show);

void s3eMapViewGoTo_platform(s3eMapView* pMapView, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate);

s3eMapViewPOI* s3eMapViewAddPOI_platform(s3eMapView* pMapView, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData);

void s3eMapViewRemovePOI_platform(s3eMapView* pMapView, s3eMapViewPOI* poi);

const char* s3eMapViewGetPlatformLicensingString_platform();

#endif /* !S3EMAPVIEW_INTERNAL_H */
