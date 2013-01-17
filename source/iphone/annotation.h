#import <MapKit/MapKit.h>

@interface S3eMapViewAnnotation : NSObject <MKAnnotation> {
}

@property (nonatomic) CLLocationCoordinate2D coordinate;
@property (nonatomic, copy) NSString *title;
@property (nonatomic, copy) NSString *subtitle;
@property (nonatomic, assign) Boolean clickable;
@property (nonatomic, assign) void* customData;

- (id)initWithCoordinate:(CLLocationCoordinate2D)coordinate;

@end
