s3eMapView
==========

This is a Marmalade Extension that provides geographical map support for 
various platforms.

Standard/Hybrid/Satellite maps can be displayed in a rectangular overlay that
will appear above your application's UI.

**POI support**: points of interest can be indicated on the map using a pin.
POIs provide an event callback so that your app can be notified when the user
has "selected" a POI.

Screenshot
![Screenshot](http://i.imgur.com/DWZL3.png)
Left: Android, center: iOS (Apple Maps), right: Mac OS X simulator


Platform Support and Notes
--------------------------

**READ THE FOLLOWING CAREFULLY FOR EACH PLATFORM YOU INTEND TO USE**

### iOS ###

iOS is fully supported. s3eMapView will use Apple Maps on iOS 6+, or 
Google Maps on iOS 5. 

### Android ###

Android is fully supported. 

__Attribution__: Google requires you to display their map licensing information
somewhere in your app. You can get the required string using
`s3eMapViewGetPlatformLicensingString()`. Note that that method will return
`NULL` on other platforms, so check the result before use.

__Instructions__: 
You must install the Android SDK, including the "Android Support Library" and
"Google Play Services" packages (in the "Extras" folder of the SDK Manager).
You must copy (or symlink) the following two `.jar` files to the `lib/android`
folder of the s3eMapView project. Due to licensing issues, I cannot distribute
these .jar files with this project.

`$ANDROID_SDK_ROOT/extras/google/google_play_services/libproject/google-play-services_lib/libs/google-play-services.jar`

`$ANDROID_SDK_ROOT/extras/android/support/v4/android-support-v4.jar`

You must create a custom Android XML Manifest for your app, and you must add
the following to the `<application>` section:

```
<activity android:name="com.bradenmacdonald.s3eMapActivity"></activity>
```

Finally, you must add an API key and various permissions you your app's 
Manifest as described [here](https://developers.google.com/maps/documentation/android/start#the_google_maps_api_key).

If you are experiencing errors, check the Android LogCat console for useful
error messages, and ensure that your API keys are valid.

### Windows Simulator ###

The Windows simulator is not supported. A minimal implementation is included
that will display a basic placeholder, showing where the map view would appear
when run on other platforms. The placeholder also lists all POIs added to the
map, allowing one to test apps that require use of POIs. You can trigger a 
"POI Selected" event by double-clicking on any clickable POI in the list.

### Mac OS X Simulator ###

The OS X simulator is mostly supported, using 
[MacMapKit](https://github.com/Oomph/MacMapKit). The resulting map will look
similar to the way an iOS map will look.

__Limitations__:
* POI subtitles, if present, are not shown on the callout
* User location is always shown
* Clickable POI callouts do not support the blue button that allows the user
  to "select" a POI. Instead, for testing purposes, you must *right-click* on
  a POI and choose "Select" from the menu.

__Instructions__:
To build the OS X extension, you will need to install the MacMapKit framework
and add it to the Xcode project for the OS X s3eMapView extension library.

1. Clone MacMapKit from https://github.com/Oomph/MacMapKit
2. Open MacMapKit in XCode, and build the Framework target for 32-bit
3. Find the resulting `MapKit.framework` folder (in ~/Library/Developer/XCode/DerivedData/...)
4. Copy `MapKit.framework` to /Library/Frameworks
5. Drag-and-drop `MapKit.framework` onto the XCode "Frameworks" group within
   this s3eMapView project, then press "Finish"
6. Under s3eMapView "Build Settings" > "Search Paths", add a new "Framework 
   Search Path": `/Library/Frameworks/`
7. Finally, under "Build Phases" > "Link Binary with Libraries", add the
   CoreLocation framework as well.

Usage Example
-------------
A simple example is shown below, and a full example is included in the
`example` folder.

```c++
#include <s3eMapView.h>

class MyApp {
	s3eMapView* m_pMap;

	void MyApp() {
		// Set up a map showing all the coffee shops we know of:
		m_pMap = s3eMapViewCreate();
		if (m_pMap) {
			s3eMapViewSetScreenRect(m_pMap, 20, 100, IwGxGetScreenWidth()-20*2, 400);
			s3eMapViewSetShowUserLocation(m_pMap, true);
			// Center the map on Calgary:
			s3eMapViewGoTo(m_pMap, s3eMapViewCoord(50.993014, -114.035339), s3eMapViewSpan(0.33, 0.15), true);
			// Add the coffee shops to the map:
			for (size_t i=0; i < g_coffeeShops.size(); i++) {
				CoffeeShop* p_location = g_coffeeShops[i];
				s3eMapViewCoord location(p_location->GetLocation().lat, p_location->GetLocation().lng);
				s3eMapViewAddPOI(m_pMap, location, p_location->GetName(), "A Coffee Shop");
			}
			// Important: map is created hidden by default, so we need to show
			// it explicitly:
			s3eMapViewSetVisible(m_pMap, true);
		} else { throw std::runtime_error("Unable to create a map"); }
	}
	~MyApp() {
		if (m_pMap)
			s3eMapViewDestroy(m_pMap);
	}
};
```


API Reference
-------------

### Data Structures

There are only two data structures that user applications can modify directly, 
`s3eMapViewCoord` and `s3eMapViewSpan`.


```c++
struct s3eMapViewCoord {
	double lat;
	double lng;
	s3eMapViewCoord(double _lat, double _lng); // Constructor
};
```
Simple struct to hold a latitude and longitude pair.  

<br/>

```c++
struct s3eMapViewSpan {
	double latDelta;
	double lngDelta;
	s3eMapViewSpan(double _latDelta, double _lngDelta); // Constructor
};
```
Simple struct to hold a geographic span, e.g. to specify the width and height
of an area in terms of latitude and longitude.


### Methods


```c++
s3eMapView* s3eMapViewCreate()
```
Use this method to create a new map. This will return `NULL` on unsupported
platforms, or if any error occurs while instantiating a map. If a map was
successfully created, this will return a pointer to the opaque `s3eMapView`
class, which you must pass as the first argument to all of the other methods
below.

Note: The map will be invisible when first created. You must call 
`s3eMapViewSetVisible(pMapView, true);` before you will see anything on screen.

<br/>

```c++
s3eResult s3eMapViewDestroy(s3eMapView* pMapView)
```
When you are done with the map, you must call this method to destroy the map
and free any resources used. 

<br/>

```c++
void s3eMapViewSetScreenRect(s3eMapView* pMapView, int x, int y, int w, int h)
```
Use this to set the position (x,y) and size (w,h) in pixels of the map view.

<br/>

```c++
void s3eMapViewSetVisible(s3eMapView* pMapView, bool visible)
```
Use this to show or hide the map.

<br/>

```c++
void s3eMapViewSetType(s3eMapView* pMapView, s3eMapViewType type)
```
Sets the map type. The `type` argument must be one of 
`S3E_MAPVIEW_TYPE_STANDARD`, `S3E_MAPVIEW_TYPE_SATELLITE`, or
`S3E_MAPVIEW_TYPE_HYBRID`

<br/>

```c++
void s3eMapViewSetShowUserLocation(s3eMapView* pMapView, bool show)
```
Sets whether or not the user's current location will be shown on the map. 
Setting this to `true` will use more battery. 

On OS X, this method currently does nothing, due to a platform-specific bug.

<br/>

```c++
void s3eMapViewGoTo(s3eMapView pMapView, const s3eMapViewCoord& center, const s3eMapViewSpan& span, bool animate)
```
Center the map on the location `center`, and ensure that the zoom level is
such that the view spans at least the latitude/longitude difference `span`.

The `animate` argument controls whether or not an animated camera transition
will be used, if available on the platform.

<br/>

```c++
s3eMapViewPOI* s3eMapViewAddPOI(s3eMapView* pMapView, const s3eMapViewCoord& location, const char* title, const char* subtitle, bool clickable, void* clickData)
```
Add a Point of Interest (POI, a.k.a a marker) to the map at the location
`location`. Optional strings can be given to specify a `title` and `subtitle`
that will appear on a callout when the user taps on the marker pin.

If "clickable" is true, the user will be able to "select" this marker.  
On iOS, this adds a blue "right arrow" button to the callout.  
On Android, this adds an arrow to the title.  
On OS X, this adds a "Select" option to the context menu that will appear when
a user right-clicks on the marker.  
Your app can register a callback that will get called when the user selects a
clickable POI. Whatever you pass as the `clickData` when creating the POI will
be passed to your callback as the `systemData` parameter.

Subtitles are not shown in the callout on OS X, but are visible if you right
click on the marker.

Returns: a pointer to the opaque `s3eMapViewPOI` class. You will only need
the return value if you want to remove this POI from the map before the
map is destroyed.

<br/>

```c++
void s3eMapViewRemovePOI(s3eMapView* pMapView, s3eMapViewPOI* poi)
```
Call this to remove a POI from the map. `poi` must be a pointer that was 
previously returned by `s3eMapViewAddPOI`. 

<br/>

```c++
const char* s3eMapViewGetPlatformLicensingString()
```
Android-only. This method returns a pointer to a string that you must display
somewhere in your app, in order to comply with licensing requirements.

<br/>

```c++
s3eMapViewRegister(s3eMapViewCallback cb, s3eCallback fn, void* userData)
```
Register a static method as a callback for POI selection events. The first
parameter must be `S3E_MAPVIEW_CALLBACK_POI_SELECTED`. The second is a pointer
to your callback method, which must have the signature  
`static int MyCallback(void* clickData, void* userData);`

Your callback should return `0`. Its `clickData` parameter will have whatever
value you passed to `s3eMapViewAddPOI`, and its `userData` parameter will have
whatever value you passed to `s3eMapViewRegister`.

Call `s3eMapViewUnRegister(S3E_MAPVIEW_CALLBACK_POI_SELECTED, fn);` when you
are done using the callback.

<br/>


Contribute
----------
Pull requests welcome!

Credits
-------
s3eMapView was created at Morningstar Enterprises by the Get to Know Society
for use in the [OISEAU: Agents of Nature](http://www.agentsofnature.com) app.


License
-------

Copyright (c) 2013 Get to Know Society

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

