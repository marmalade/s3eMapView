#import <MapKit/MapKit.h> // See note in s3eMapView_platform.mm

@interface S3eMapViewAnnotation : NSObject <MKAnnotation> {
	CLLocationCoordinate2D coordinate;
	NSString *title;
	NSString *subtitle;
	Boolean clickable;
	void* customData; // User-specified data that gets past to the S3E_MAPVIEW_CALLBACK_POI_SELECTED callback
}

@property (nonatomic) CLLocationCoordinate2D coordinate;
@property (nonatomic, copy) NSString *title;
@property (nonatomic, copy) NSString *subtitle;
@property (nonatomic, assign) Boolean clickable;
@property (nonatomic, assign) void* customData;

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@end
