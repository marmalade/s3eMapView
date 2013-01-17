#import <MapKit/MapKit.h> // See note in s3eMapView_platform.mm
#import "annotation.h"

@interface S3eMapViewController : NSObject < MKMapViewDelegate > {
	MKMapView* myMapView; // Our MKMapView
	bool mapJsReady; // Will not be able to send commands to the map, add annotations, etc. until this is true. See MacMapKit issue #7
	
	// queued map commands and objects that we'll process once the JS has loaded:
	NSMutableArray *annotationQueue;
	MKCoordinateRegion deferredRegion;
}
-(S3eMapViewController*)init;

-(MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation;
-(void)mapViewDidFinishLoadingMap:(MKMapView *)mapView;
-(bool)isMapJsReady;
-(void)addAnnotation:(S3eMapViewAnnotation*)annotation;
-(void)removeAnnotation:(S3eMapViewAnnotation*)annotation;
-(void)annotationSelectMenuClicked:(id)sender;
- (NSArray *)mapView:(MKMapView *)mapView contextMenuItemsForAnnotationView:(MKAnnotationView *)view;
-(void)setRegion:(MKCoordinateRegion)region animated:(BOOL)animated;
-(void) dealloc;
-(MKMapView*)mapView;

@end
