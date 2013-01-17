#import "annotation.h"


@implementation S3eMapViewAnnotation

@synthesize coordinate;
@synthesize title;
@synthesize subtitle;
@synthesize clickable;
@synthesize customData;

- (id)initWithCoordinate:(CLLocationCoordinate2D)coord{
	self = [super init];
	if (self != nil) {
		self->coordinate = coord;
	}
	return self;
}

@end
