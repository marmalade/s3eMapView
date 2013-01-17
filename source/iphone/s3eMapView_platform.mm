/*
iphone-specific implementation of the s3eMapView extension.

This file is responsible for instantiating an s3eMapViewController, and passing
messages from Marmalade to the controller or its MKMapView.

Code is copyright (c) 2013 Get to Know Society.
Licensed under the zlib license - see LICENSE file.
*/
#include "s3eMapView_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_iphone.h"

#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>

#import "annotation.h"
#import "controller.h"

struct s3eMapView {
	// Wrap our platform-specific class in this generic struct
	S3eMapViewController* controller;
	MKMapView* mapView() const { return controller.mapView; }
};
struct s3eMapViewPOI { /* Cast to/from S3eMapViewAnnotation* */; }; // Alias for S3eMapViewAnnotation

s3eResult s3eMapViewInit_platform() {
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void s3eMapViewTerminate_platform() {
    // Add any platform-specific termination code here
}

s3eMapView* s3eMapViewCreate_platform() {
	// Critically important: reset the OpenGL context so Marmalade's OpenGL use doesn't interfere with iOS 6 MapKit GL use
	[EAGLContext setCurrentContext:nil]; // Random crashes will occur if this line is omitted.
	
	// Allocate an object that will hold state information:
	s3eMapView* info = new s3eMapView;
	
	info->controller = [[S3eMapViewController alloc] init];
	UIView* view = s3eEdkGetUIView();
	[view addSubview:info->controller.mapView];
	return info;
}

s3eResult s3eMapViewDestroy_platform(s3eMapView* pWrapper) {
    [pWrapper->mapView() removeFromSuperview];
	[pWrapper->controller release];
	delete pWrapper;
    return S3E_RESULT_SUCCESS;
}

void s3eMapViewSetScreenRect_platform(s3eMapView* pWrapper, int x, int y, int w, int h) {
	// Convert from pixels (expected by Marmalade apps) to points:
	const float scaleFactor = s3eEdkGetIPhoneScaleFactor();
	x = int(float(x) / scaleFactor);
	y = int(float(y) / scaleFactor);
	w = int(float(w) / scaleFactor);
	h = int(float(h) / scaleFactor);
	pWrapper->mapView().frame = CGRectMake(x, y, w, h);
}

void s3eMapViewSetVisible_platform(s3eMapView* pMapView, bool visible) {
	pMapView->mapView().hidden = !visible;
}

void s3eMapViewSetType_platform(s3eMapView* pWrapper, s3eMapViewType type) {
	pWrapper->mapView().mapType =
		type == S3E_MAPVIEW_TYPE_SATELLITE ? MKMapTypeSatellite :
		type == S3E_MAPVIEW_TYPE_HYBRID ? MKMapTypeHybrid :
		/* else: */ MKMapTypeStandard;
}

void s3eMapViewSetShowUserLocation_platform(s3eMapView* pWrapper, bool show) {
	pWrapper->mapView().showsUserLocation = show;
}

void s3eMapViewGoTo_platform(s3eMapView* pWrapper, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate) {
	CLLocationCoordinate2D _center = CLLocationCoordinate2DMake(center->lat, center->lng);
	MKCoordinateSpan _span = MKCoordinateSpanMake(span->latDelta, span->lngDelta);
	[pWrapper->mapView() setRegion:MKCoordinateRegionMake(_center, _span) animated:animate];
}

s3eMapViewPOI* s3eMapViewAddPOI_platform(s3eMapView* pWrapper, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData) {
	CLLocationCoordinate2D _location = CLLocationCoordinate2DMake(location->lat, location->lng);
	S3eMapViewAnnotation* annotation = [[S3eMapViewAnnotation alloc] initWithCoordinate:_location];
	if (title)
		annotation.title = [NSString stringWithUTF8String:title];
	if (subtitle)
		annotation.subtitle = [NSString stringWithUTF8String:subtitle];
	annotation.clickable = clickable;
	annotation.customData=clickData;
	[pWrapper->mapView() addAnnotation:[annotation autorelease]];
    return (s3eMapViewPOI*)annotation;
}

void s3eMapViewRemovePOI_platform(s3eMapView* pWrapper, s3eMapViewPOI* poi) {
	[pWrapper->mapView() removeAnnotation:(S3eMapViewAnnotation*)poi];
}

const char* s3eMapViewGetPlatformLicensingString_platform() { return NULL; }
