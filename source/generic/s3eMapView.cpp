/*
Generic implementation of the s3eMapView extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "s3eMapView_internal.h"
s3eResult s3eMapViewInit() {
    //Add any generic initialisation code here
    return s3eMapViewInit_platform();
}

void s3eMapViewTerminate() {
    //Add any generic termination code here
    s3eMapViewTerminate_platform();
}

s3eMapView* s3eMapViewCreate() {
	return s3eMapViewCreate_platform();
}

s3eResult s3eMapViewDestroy(s3eMapView* pMapView) {
	return s3eMapViewDestroy_platform(pMapView);
}

void s3eMapViewSetScreenRect(s3eMapView* pMapView, int x, int y, int w, int h) {
	s3eMapViewSetScreenRect_platform(pMapView, x, y, w, h);
}

void s3eMapViewSetVisible(s3eMapView* pMapView, bool visible) {
	s3eMapViewSetVisible_platform(pMapView, visible);
}

void s3eMapViewSetType(s3eMapView* pMapView, s3eMapViewType type) {
	s3eMapViewSetType_platform(pMapView, type);
}

void s3eMapViewSetShowUserLocation(s3eMapView* pMapView, bool show) {
	s3eMapViewSetShowUserLocation_platform(pMapView, show);
}

void _s3eMapViewGoTo(s3eMapView* pMapView, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate) {
	s3eMapViewGoTo_platform(pMapView, center, span, animate);
}

s3eMapViewPOI* _s3eMapViewAddPOI(s3eMapView* pMapView, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData) {
	return s3eMapViewAddPOI_platform(pMapView, location, title, subtitle, clickable, clickData);
}

void s3eMapViewRemovePOI(s3eMapView* pMapView, s3eMapViewPOI* poi) {
	s3eMapViewRemovePOI_platform(pMapView, poi);
}

const char* s3eMapViewGetPlatformLicensingString() {
	return s3eMapViewGetPlatformLicensingString_platform(); // Generic implementation: no special license string is required
}
