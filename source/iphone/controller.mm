#import "controller.h"
#import "annotation.h"
#include "s3eMapView_internal.h"
#include <s3eEdk.h>

@implementation S3eMapViewController

-(S3eMapViewController*)init {
	self = [super init];
	myMapView = [[MKMapView alloc] initWithFrame:CGRectMake(10, 10, 300, 400)]; // Default size/shape. Change with s3eMapViewSetScreenRect()
	[myMapView setHidden:YES];
	myMapView.delegate = self;
	myMapView.showsUserLocation = YES;
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
		S3eMapViewAnnotation* annotationObj = annotation;
		NSString* reuseId = annotationObj.clickable ? @"ClickablePin" : @"Pin";
		MKPinAnnotationView *pinView = (MKPinAnnotationView*)[mapView dequeueReusableAnnotationViewWithIdentifier:reuseId];
		if(pinView == nil) {
			pinView = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:reuseId] autorelease];
			pinView.pinColor = MKPinAnnotationColorPurple;
			pinView.animatesDrop = YES;
			pinView.canShowCallout = YES;
			if (annotationObj.clickable) {
				UIButton* rightButton = [UIButton buttonWithType:UIButtonTypeDetailDisclosure];
				pinView.rightCalloutAccessoryView = rightButton;
			}
		} else {
			pinView.annotation = annotation;
		}
		return pinView;
	}
	return nil;
}

- (void)mapView:(MKMapView *)mapView annotationView:(MKAnnotationView *)view calloutAccessoryControlTapped:(UIControl *)control {
	NSAssert(mapView==myMapView, @"Invalid mapView");
	if ([view.annotation isKindOfClass:[S3eMapViewAnnotation class]]) {
		S3eMapViewAnnotation* annotation = view.annotation;
		s3eEdkCallbacksEnqueue(S3E_EXT_MAPVIEW_HASH, S3E_MAPVIEW_CALLBACK_POI_SELECTED, annotation.customData);
	}
}

- (MKMapView*)mapView { return myMapView; }

@end
