/*
 * Windows-specific implementation of the s3eMapView extension.
 * For now, this just displays a green coloured rectangle to 
 * indicate where the map will appear on device, and also shows
 * a list of POIs added to the map, if any.
 *
 * Libraries needed:
 * edk.lib;kernel32.lib;user32.lib;gdi32.lib;gdiplus.lib
 *
 * Future improvements: 
 * Use a WPF thread to host a WPF-based OpenStreetMap control
 * or Google Maps control
 * "WPF and Win32 Interoperation" 
 * http://msdn.microsoft.com/en-us/library/ms742522.aspx
 * http://msdn.microsoft.com/en-us/library/ms744829.aspx
 *
 * Code is copyright (c) 2013 Get to Know Society.
 * Licensed under the zlib license - see LICENSE file.
 */
#include "s3eMapView_internal.h"
#include "s3eEdk.h"
#include "s3eEdk_windows.h"

#include <ctime>
#include <string>
#include <vector>
#include <tchar.h>
#include <CommCtrl.h>
#include <GdiPlus.h>

using namespace Gdiplus;
using std::string;

struct PoiInfo {
	// Point of Interest information struct.
	int id; // arbitrary ID. We cannot use this item's location in memory or in the ListBox UI element, because those change as further POI entries get added.
	string title;
	bool clickable;
	void* customData;
	s3eMapViewPOI* GetId() { return (s3eMapViewPOI*)id; } // Hack: return our ID as a pointer to the undefined 's3eMapViewPOI' type. See 'id' comment above.
};
struct s3eMapView {
	HWND hWnd; // window for our map
	char winClassStr[10]; // unique class name for our mapView's window class (HWND)
	ULONG_PTR gdiplusToken; // Required to use GDI+ for drawing
	HWND hPOIListBox; // handle for our POI list box
	std::vector<PoiInfo> poiList; // Vector of PoiInfo objects; kept in same order as the GUI ListBox control
	bool showUserLoc;
	s3eMapView() : hWnd(NULL), gdiplusToken(NULL), hPOIListBox(NULL), poiList(), showUserLoc(false) {} // defaults
};
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // forward declare



s3eResult s3eMapViewInit_platform() {
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void s3eMapViewTerminate_platform() {
    // Add any platform-specific termination code here
}

s3eMapView* s3eMapViewCreate_platform() {
	const HINSTANCE hInstance = s3eEdkGetHinstance();

	// Create a structure to hold our data:
	s3eMapView* info = new s3eMapView;
	// Generate a unique string ID for this window:
	static char idChar='A';
	strcpy_s(info->winClassStr, sizeof(info->winClassStr), "s3eMap_x");
	info->winClassStr[7]=idChar++;

	// Initialize GDI+ for drawing
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&info->gdiplusToken, &gdiplusStartupInput, NULL);


	// Create a window to hold the map:
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = info->winClassStr;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("s3eMapView_win32"), NULL);
		delete info;
		return NULL;
    }
	info->hWnd = CreateWindow(
		info->winClassStr,
		"s3eMapView", // title
		WS_CHILD|WS_CLIPCHILDREN, // style
		20, 200, // initial position
		300, 400, // initial size
		s3eEdkGetHwnd(), // parent window
		NULL, // menu
		s3eEdkGetHinstance(),
		info
	);
	if (!info->hWnd) {
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("s3eMapView_win32"), NULL);
		delete info;
		return NULL;
    }

	UpdateWindow(info->hWnd);

    return info;
}

s3eResult s3eMapViewDestroy_platform(s3eMapView* pMapView) {
	DestroyWindow(pMapView->hWnd);
	UnregisterClass(pMapView->winClassStr, s3eEdkGetHinstance());
	 GdiplusShutdown(pMapView->gdiplusToken);
	delete pMapView;
    return S3E_RESULT_SUCCESS;
}

void s3eMapViewSetScreenRect_platform(s3eMapView* pMapView, int x, int y, int w, int h) {
	MoveWindow(pMapView->hWnd, x,y, w,h, true);
}

void s3eMapViewSetVisible_platform(s3eMapView* pMapView, bool visible) {
	ShowWindow(pMapView->hWnd, visible? SW_SHOW : SW_HIDE);
}

void s3eMapViewSetType_platform(s3eMapView* pMapView, s3eMapViewType type) {
	// map type (normal, terrain, satellite). Not implemented on Windows.
}

void s3eMapViewSetShowUserLocation_platform(s3eMapView* info, bool show) {
	info->showUserLoc=show;
	InvalidateRect(info->hWnd, NULL, false); // draw next animation frame ASAP
}

void s3eMapViewGoTo_platform(s3eMapView* pMapView, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate) {
	// Not implemented on Windows.
}

s3eMapViewPOI* s3eMapViewAddPOI_platform(s3eMapView* info, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData) {
	static int id_generator = 555; // Arbitrary ID. See comment in PoiInfo struct definition
	PoiInfo poi;
	poi.title = title ? title : "Untitled";
	if (subtitle)
		poi.title += string("  (") + subtitle + ")";
	poi.id = id_generator++;
	poi.clickable = clickable;
	poi.customData = clickData;
	int index = SendMessage(info->hPOIListBox, LB_ADDSTRING, NULL, (LPARAM)poi.title.c_str());
	info->poiList.insert(info->poiList.begin()+index, poi);
	return poi.GetId();
}

void s3eMapViewRemovePOI_platform(s3eMapView* info, s3eMapViewPOI* poiID) {
	// Find the POI with matching ID, then remove it.
	for (size_t i=0;i<info->poiList.size();i++) {
		if (info->poiList[i].GetId()==poiID) {
			SendMessage(info->hPOIListBox, LB_DELETESTRING, i, NULL); // Remove it from the ListBox UI element
			info->poiList.erase(info->poiList.begin()+i); // Remove it from poiList vector
			return;
		}
	}
}

const char* s3eMapViewGetPlatformLicensingString_platform() { return NULL; }

void OnPaint(s3eMapView* info, HDC hdc) {
	Graphics _graphics(hdc);
	RECT _winRect;
	GetWindowRect(info->hWnd, &_winRect);
	RectF winRect(0, 0, (float)_winRect.right-(float)_winRect.left, (float)_winRect.bottom-(float)_winRect.top);
	// Create a double-buffered Graphics object
	Bitmap dbufferBitmap(_winRect.right-_winRect.left, _winRect.bottom-_winRect.top);
	Graphics graphics(&dbufferBitmap);

	// Fill our map view with a linear gradient background:
	LinearGradientBrush linGrBrush(
		Point(0, 0),
		Point((int)winRect.Width, (int)winRect.Height),
		Color(255, 255, 255, 153), // faint yellow
		Color(255, 204, 255, 153)  // faint green
	);
	graphics.FillRectangle(&linGrBrush, winRect);

	// Draw a 4px dark grey border:
	Pen pen(Color(255, 128, 128, 128), 2);
	graphics.DrawRectangle(&pen, winRect);
	
	// Draw a centered header string at the top:
	SolidBrush  brush(Color(255, 0, 0, 0)); // Black
	FontFamily  fontFamily(L"Segoe UI");
	Font        font(&fontFamily, 24, FontStyleBold, UnitPixel);
	StringFormat fmt;
	fmt.SetAlignment(StringAlignmentCenter);
	graphics.DrawString(L"Map View", -1, &font, winRect, &fmt, &brush);

	// If "show user location" is true, display an animated blue circle
	if (info->showUserLoc) {
		FILETIME ft_now; GetSystemTimeAsFileTime(&ft_now);
		int currentMS = ft_now.dwLowDateTime/10000; // currentMS now holds a number related to the current time in milliseconds
		const BYTE alpha=BYTE(155+100*(sin(float(currentMS%3000)/3000*2*3.141592))); // Make the alpha value oscillate w/ 3000ms period

		const float diameter=30;
		RectF cRect(winRect.Width/2-diameter/2, winRect.Height*.3f-diameter/2, diameter, diameter);
		RECT blueCircRect2 = {(int)cRect.X, (int)cRect.Y, int(cRect.X+cRect.Width), int(cRect.Y+cRect.Height)};
		Color ctrColor(alpha, 40, 40, 120);
		SolidBrush brush(ctrColor);
		graphics.FillEllipse(&brush, cRect);
	}
	_graphics.DrawImage(&dbufferBitmap,0,0);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	s3eMapView* info = (s3eMapView*)GetWindowLongPtr(hWnd, GWL_USERDATA); 

	if (WM_CREATE==message) {
		LPCREATESTRUCT winInfo = (LPCREATESTRUCT)lParam;
		// During the WM_CREATE message, we need to get 'info' pointer from lParam
		// since the GWL_USERDATA has not been set yet.
		info = (s3eMapView*)winInfo->lpCreateParams;
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)info); // store a pointer to 'info' in the window's custom user data
		// Create a list box:
		info->hPOIListBox = CreateWindow(
			WC_LISTBOX,
			"",
			LBS_STANDARD|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			0,0,     // initial position - will get set below in the WM_SIZE handler
			100,100, // width/height - will get set below in the WM_SIZE handler
			hWnd,    // parent window - will get list box events
			NULL, winInfo->hInstance, NULL);
		if (!info->hPOIListBox)
			return -1; // Signify an error occurred.
		SetTimer(hWnd, NULL, 15, NULL); // Set our redraw timer.
	} else if (WM_SIZE==message) {
		// Update size/position of the listbox:
		const int winWidth = LOWORD(lParam);
		const int winHeight = HIWORD(lParam);
		// Move/Size the POI list box:
		const int poiPadding = 10;
		const int poiHeight = int(winHeight*0.4f);
		MoveWindow(info->hPOIListBox, poiPadding,winHeight-poiHeight-poiPadding, winWidth-2*poiPadding,poiHeight, true);
	} else if (WM_COMMAND==message) {
		if (HIWORD(wParam)==LBN_DBLCLK && lParam==(LPARAM)info->hPOIListBox) {
			// double-click notification from our POI list box
			int clickIndex = SendMessage(info->hPOIListBox, LB_GETCURSEL, NULL, NULL);
			if (clickIndex >= 0 && clickIndex < (int)info->poiList.size()) {
				PoiInfo poiClicked = info->poiList[clickIndex];
				if (poiClicked.clickable)
					s3eEdkCallbacksEnqueue(S3E_EXT_MAPVIEW_HASH, S3E_MAPVIEW_CALLBACK_POI_SELECTED, poiClicked.customData);
			}
		} else if (HIWORD(wParam)==LBN_SELCHANGE && lParam==(LPARAM)info->hPOIListBox) {
			// Only allow user to click on POI items that have clickable set true
			int clickIndex = SendMessage(info->hPOIListBox, LB_GETCURSEL, NULL, NULL);
			if (!info->poiList[clickIndex].clickable)
				SendMessage(info->hPOIListBox, LB_SETCURSEL, -1, NULL);
		} else return DefWindowProc(hWnd, message, wParam, lParam);
	} else if (WM_PAINT==message) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		OnPaint(info, hdc);
		EndPaint(hWnd, &ps);
	} else if (WM_TIMER==message) {
		RedrawWindow(info->hWnd, NULL, NULL, RDW_INVALIDATE|RDW_NOCHILDREN); // Draw next frame ASAP
		SetTimer(info->hWnd, NULL, 15, NULL); // Redraw every 15 ms
	} else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
