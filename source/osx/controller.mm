#import "controller.h"
#include <s3eEdk.h>
#include "s3eMapView_internal.h"

@implementation S3eMapViewController

-(S3eMapViewController*)init {
	self = [super init];
	myMapView = [[MKMapView alloc] initWithFrame:NSMakeRect(10, 10, 300, 400)]; // Default size/shape. Change with 3eMapViewSetScreenRect()
	[myMapView setHidden:YES];
	myMapView.delegate = self;
	annotationQueue = [[NSMutableArray alloc] init];
	deferredRegion = MKCoordinateRegionMake(CLLocationCoordinate2DMake(37.331705, -122.030598), MKCoordinateSpanMake(0.3,0.3));
	myMapView.showsUserLocation = YES; // For some some yet-undetermined reason, the map is buggy (mostly invisible) when this is NO
	return self;
}

-(void) dealloc {
	[myMapView.delegate release];
	myMapView.delegate = nil;
	[myMapView release];
	[super dealloc];
}

- (MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation {
	NSAssert(mapView==myMapView, @"Invalid mapView");
	if([annotation isKindOfClass:[MKUserLocation class]])
        return nil; // Don't change the user location dot
	
	if ([annotation isKindOfClass:[S3eMapViewAnnotation class]]) { // This is a POI that we've added
																   // First, try to re-use an existing annotation view for this pin, if we can:
		MKPinAnnotationView *pinView = (MKPinAnnotationView*)[mapView dequeueReusableAnnotationViewWithIdentifier:@"Pin"];
		if(pinView == nil) {
			pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"Pin"] autorelease];
			pinView.pinColor = MKPinAnnotationColorPurple;
			pinView.animatesDrop = YES;
			pinView.canShowCallout = YES;
		} else {
			pinView.annotation = annotation;
		}
		return pinView;
	}
	return nil;
}

- (void)addAnnotation:(S3eMapViewAnnotation*)annotation {
	// We can only add annotations to the map if the javascript has loaded; if not, queue it to be added later.
	if (mapJsReady)
		[myMapView addAnnotation:annotation];
	else
		[annotationQueue addObject:annotation];
}
- (void)removeAnnotation:(S3eMapViewAnnotation*)annotation {
	if (mapJsReady)
		[myMapView removeAnnotation:annotation];
	else {
		// The annotation hasn't been added to the map yet. Just remove it from the queue
		for (unsigned int i=0;i<annotationQueue.count;i++) {
			if ([annotationQueue objectAtIndex:i] == annotation) {
				[annotationQueue removeObjectAtIndex:i];
				break;
			}
		}
	}
}

- (NSArray *)mapView:(MKMapView *)mapView contextMenuItemsForAnnotationView:(MKAnnotationView *)view {
	NSAssert(mapView==myMapView, @"Invalid mapView");
	// MacMapKit doesn't support the blue arrow button (UIButtonTypeDetailDisclosure)
	// in an annotation callout that is used on iOS to "select" that annotation...
	// so we allow selection of annotations via a context menu.
	if ([view.annotation isKindOfClass:[S3eMapViewAnnotation class]] && ((S3eMapViewAnnotation*)view.annotation).clickable) {
		// MacMapKit doesn't support subtitles so we add the title and subtitle to the context menu for debugging purposes:
		NSMutableArray* menuItems = [[NSMutableArray alloc] init];
		if (view.annotation.title) {
			NSMenuItem *titleItem = [[[NSMenuItem alloc] initWithTitle:view.annotation.title action:NULL keyEquivalent:@""] autorelease];
			[menuItems addObject:titleItem];
		}
		if (view.annotation.subtitle) {
			NSMenuItem *subtitleItem = [[[NSMenuItem alloc] initWithTitle:view.annotation.subtitle action:NULL keyEquivalent:@""] autorelease];
			[menuItems addObject:subtitleItem];
		}
		NSMenuItem *selectItem = [[[NSMenuItem alloc] initWithTitle:@"Select" action:@selector(annotationSelectMenuClicked:) keyEquivalent:@""] autorelease];
		selectItem.target = self;
		selectItem.representedObject = view.annotation;
		[menuItems addObject:selectItem];
		return [menuItems autorelease];
	} else {
		return nil;
	}
}

- (void)annotationSelectMenuClicked:(id)sender {
	// The "select" menu option was selected, which we use to represent
	// pressing the UIButtonTypeDetailDisclosure button
	// since that callout button is not available in MacMapKit.
	S3eMapViewAnnotation* annotation = ((NSMenuItem*)sender).representedObject;
	s3eEdkCallbacksEnqueue(S3E_EXT_MAPVIEW_HASH, S3E_MAPVIEW_CALLBACK_POI_SELECTED, annotation.customData);
}

- (void)setRegion:(MKCoordinateRegion)region animated:(BOOL)animated {
	if (mapJsReady)
		[myMapView setRegion:region animated:animated];
	else
		deferredRegion = region; // "animated" boolean is not stored
}

- (void)mapViewDidFinishLoadingMap:(MKMapView *)mapView {
	NSAssert(mapView==myMapView, @"Invalid mapView");
	
	// We are not be able to send commands to the map, add annotations, etc. until
	// the map's javascript has loaded in the WebView. See MacMapKit issue #7
	// So we queue any commands/annotations/etc. until this method gets called.
	// At this point (mapViewDidFinishLoadingMap), everything has loaded, so
	// we can process all the queued commands.
	// n.b. This problem doesn't occur on iOS; only on OS X
	
	if (!mapJsReady) {
		mapJsReady = true;
		NSLog(@"Map has loaded. Any queued POI or map commands will get processed now.");
		// Add annotations:
		while ([annotationQueue count] > 0) {
			S3eMapViewAnnotation* annotation = [annotationQueue objectAtIndex:0];  // take out the first one
			[mapView addAnnotation:annotation];
			[annotationQueue removeObjectAtIndex:0];
		}
		// Send setRegion command:
		[mapView setRegion:deferredRegion animated:NO];
	}
}
- (bool)isMapJsReady { return mapJsReady; }
- (MKMapView*)mapView { return myMapView; }

@end
