#import <MapKit/MapKit.h>
#import "annotation.h"

@interface S3eMapViewController : NSObject < MKMapViewDelegate > {
	MKMapView* myMapView; // Our MKMapView
}
-(S3eMapViewController*)init;
-(void) dealloc;

// API Methods
- (void)mapView:(MKMapView *)mapView annotationView:(MKAnnotationView *)view calloutAccessoryControlTapped:(UIControl *)control;

// Getters
-(MKMapView*)mapView;

@end
