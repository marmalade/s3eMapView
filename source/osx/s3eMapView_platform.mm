/*
OS X implementation of the s3eMapView extension.

This file is responsible for instantiating an s3eMapViewController, and passing
messages from Marmalade to the controller or its MKMapView.

Code is copyright (c) 2013 Get to Know Society.
Licensed under the zlib license - see LICENSE file.
*/
#include "s3eMapView_internal.h"
#include "s3eDebug.h"

#import "AppKit/AppKit.h"
#import "MapKit/MapKit.h"
// If you get errors with the above line, you need to:
// 1. Clone https://github.com/Oomph/MacMapKit
// 2. Open MacMapKit in XCode, and build the Framework target for 32-bit
// 3. Find the resulting "MapKit.framework" folder (in ~/Library/Developer/XCode/DerivedData/...)
// 4. Copy "MapKit.framework" to /Library/Frameworks
// 5. Drag-and-drop "MapKit.framework" onto the XCode "Frameworks" group within this s3eMapView project, then press "Finish"
// 6. Under s3eMapView "Build Settings" > "Search Paths", add a new "Framework Search Path": "/Library/Frameworks/"
// 7. Finally, under "Build Phases" > "Link Binary with Libraries", add the CoreLocation framework as well.

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
	s3eMapView* wrapper = new s3eMapView;
	
	NSWindow* mainWin = [NSApp mainWindow];
	wrapper->controller = [[S3eMapViewController alloc] init];
	NSView* view = mainWin.contentView;
	[view addSubview:wrapper->controller.mapView positioned:NSWindowAbove relativeTo:nil];
	return wrapper;
}

s3eResult s3eMapViewDestroy_platform(s3eMapView* pWrapper) {
	[pWrapper->controller.mapView removeFromSuperview];
	[pWrapper->controller release];
    return S3E_RESULT_SUCCESS;
}

void s3eMapViewSetScreenRect_platform(s3eMapView* pWrapper, int x, int y, int w, int h) {
	NSWindow* mainWin = [NSApp mainWindow];
	NSView* view = mainWin.contentView;
	NSSize screenSize = view.bounds.size;
	NSLog(@"View size is %dx%d. Setting y to %d-%d-%d=%d", (int)screenSize.width, (int)screenSize.height, (int)screenSize.height,y,h, int(screenSize.height-y-h));
	pWrapper->controller.mapView.frame = NSMakeRect(x, screenSize.height-y-h, w, h); // Note inverted Y axis
}

void s3eMapViewSetVisible_platform(s3eMapView* pWrapper, bool visible) {
	[pWrapper->controller.mapView setHidden:!visible];
}

void s3eMapViewSetType_platform(s3eMapView* pWrapper, s3eMapViewType type) {
	pWrapper->mapView().mapType =
		type == S3E_MAPVIEW_TYPE_SATELLITE ? MKMapTypeSatellite :
		type == S3E_MAPVIEW_TYPE_HYBRID ? MKMapTypeHybrid :
		/* else: */ MKMapTypeStandard;
}

void s3eMapViewSetShowUserLocation_platform(s3eMapView* pWrapper, bool show) {
	//[pWrapper->mapView() setShowsUserLocation:show];
	if (!show)
		NSLog(@"Note: s3eMapViewSetShowUserLocation(false) is ignored on OS X."); // Causes map to be invisibile :-/
}

void s3eMapViewGoTo_platform(s3eMapView* pWrapper, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate) {
	CLLocationCoordinate2D _center = CLLocationCoordinate2DMake(center->lat, center->lng);
	MKCoordinateSpan _span = MKCoordinateSpanMake(span->latDelta, span->lngDelta);
	[pWrapper->controller setRegion:MKCoordinateRegionMake(_center, _span) animated:animate];
}

s3eMapViewPOI* s3eMapViewAddPOI_platform(s3eMapView* pWrapper, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData) {
	CLLocationCoordinate2D _location = CLLocationCoordinate2DMake(location->lat, location->lng);
	S3eMapViewAnnotation* annotation = [[S3eMapViewAnnotation alloc] initWithCoordinate:_location];
	if (title)
		annotation.title = [NSString stringWithUTF8String:title];
	if (subtitle)
		annotation.subtitle = [NSString stringWithUTF8String:subtitle];
	annotation.clickable = clickable;
	annotation.customData = clickData;
	[pWrapper->controller addAnnotation:[annotation autorelease]]; // Note: our custom controller class handles this, not the MapView
    return (s3eMapViewPOI*)annotation;
}

void s3eMapViewRemovePOI_platform(s3eMapView* pWrapper, s3eMapViewPOI* poi) {
	[pWrapper->controller removeAnnotation:(S3eMapViewAnnotation*)poi]; // Note: our custom controller class handles this, not the MapView
}

const char* s3eMapViewGetPlatformLicensingString_platform() { return NULL; }
