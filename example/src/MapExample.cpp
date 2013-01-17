#include "MapExample.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include <s3eMapView.h>


using std::runtime_error;
using std::string;
using std::endl;
using std::cout;

MapExample::MapExample() : m_isShowingUserLocation(true), m_isMapVisible(true) {
	// First, make sure we can create a map on this platform/device:
	m_pMapView = s3eMapViewCreate();
	if (!m_pMapView)
		throw std::runtime_error("Unable to create a map");

	// Now set up our UI:
	CIwUIElement* p_ui_box = new CIwUIElement;
	// Create a vertical layout:
	CIwUILayoutVertical* p_layout = new CIwUILayoutVertical;
	p_ui_box->SetLayout(p_layout);
	p_ui_box->SetSizeToContent(true);
	
	CIwUILabel* p_header = new CIwUILabel;
	p_header->SetStyle("<label_medium>");
	p_header->SetProperty("alignH", IW_UI_ALIGN_CENTRE);
	p_header->SetCaption("s3eMapView Example");
	p_layout->AddElement(p_header);

	m_pStandardMapBtn = MakeButton("Standard Map");
	p_layout->AddElement(m_pStandardMapBtn);
	
	m_pSatelliteMapBtn = MakeButton("Satellite Map");
	p_layout->AddElement(m_pSatelliteMapBtn);
	
	m_pHybridMapBtn = MakeButton("Hybrid Map");
	p_layout->AddElement(m_pHybridMapBtn);
	
	m_pToggleUserLocBtn = MakeButton("Toggle User Location");
	p_layout->AddElement(m_pToggleUserLocBtn);
	
	m_pToggleMapVisibilityBtn = MakeButton("Toggle Map Visibility");
	p_layout->AddElement(m_pToggleMapVisibilityBtn);
	
	IwGetUIView()->AddElementToLayout(p_ui_box, IW_UI_ALIGN_CENTRE, IW_UI_ALIGN_BOTTOM);
	
	// Now configure the map to fit in the remaining space above the UI:
	s3eMapViewSetScreenRect(m_pMapView, 0, 0, IwGxGetScreenWidth(), IwGxGetScreenHeight() - p_ui_box->GetSize().y);
	s3eMapViewSetShowUserLocation(m_pMapView, m_isShowingUserLocation);
	// Center the map on North America:
	s3eMapViewGoTo(m_pMapView, s3eMapViewCoord(47.7, -100.9), s3eMapViewSpan(20, 80), true);
	
	// Add a few cities to the map. We pass the constant char* string pointer
	// as our custom data, which will be received by our event handler when
	// the user selects one of these cities.
	const char* STR_VANCOUVER = "Vancouver, BC";
	s3eMapViewAddPOI(m_pMapView, s3eMapViewCoord(49.261738,-123.113456), STR_VANCOUVER, NULL, true, (void*)STR_VANCOUVER);
	const char* STR_TORONTO = "Toronto, ON";
	s3eMapViewAddPOI(m_pMapView, s3eMapViewCoord(43.65344,-79.38409), STR_TORONTO, NULL, true, (void*)STR_TORONTO);
	const char* STR_SAN_DIEGO = "San Diego, CA";
	s3eMapViewAddPOI(m_pMapView, s3eMapViewCoord(32.716982,-117.162767), STR_SAN_DIEGO, NULL, true, (void*)STR_SAN_DIEGO);
	const char* STR_BOSTON = "Boston, MA";
	s3eMapViewAddPOI(m_pMapView, s3eMapViewCoord(42.360406,-71.057993), STR_BOSTON, NULL, true, (void*)STR_BOSTON);
	
	// Register our handler for click events:
	s3eMapViewRegister(S3E_MAPVIEW_CALLBACK_POI_SELECTED, &MapExample::HandlePOISelected, this);
	// Important: map is hidden by default, so we need to make it visible:
	s3eMapViewSetVisible(m_pMapView, m_isMapVisible);
}

MapExample::~MapExample() {
	if (m_pMapView) {
		s3eMapViewUnRegister(S3E_MAPVIEW_CALLBACK_POI_SELECTED, &MapExample::HandlePOISelected);
		// The above is only required if we're re-using this class, but good practice to include anyways
		s3eMapViewDestroy(m_pMapView); // Required
	}
}

bool MapExample::HandleEvent(CIwEvent* pEvent) {
	if (pEvent->GetID() == IWUI_EVENT_BUTTON) {
		CIwUIButton* p_button = IwSafeCast<CIwUIButton*>(pEvent->GetSender());
		if (p_button == m_pStandardMapBtn)
			s3eMapViewSetType(m_pMapView, S3E_MAPVIEW_TYPE_STANDARD);
		else if (p_button == m_pSatelliteMapBtn)
			s3eMapViewSetType(m_pMapView, S3E_MAPVIEW_TYPE_SATELLITE);
		else if (p_button == m_pHybridMapBtn)
			s3eMapViewSetType(m_pMapView, S3E_MAPVIEW_TYPE_HYBRID);
		else if (p_button == m_pToggleUserLocBtn)
			s3eMapViewSetShowUserLocation(m_pMapView, m_isShowingUserLocation = !m_isShowingUserLocation);
		else if (p_button == m_pToggleMapVisibilityBtn)
			s3eMapViewSetVisible(m_pMapView, m_isMapVisible = !m_isMapVisible);
		else
			throw std::runtime_error("Unknown button pressed.");
	}
	return true;
}

void MapExample::HandlePOISelected(void* customData) {
	const char* caption = (const char*)customData; // We just set each POI's customData equal to a constant string
	s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, (std::string("You clicked on ") + caption).c_str());
}

void MapExample::Update() {}
