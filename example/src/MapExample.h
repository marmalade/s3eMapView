#pragma once

#include <IwUI.h>
struct s3eMapView;

class MapExample : public IIwUIEventHandler {
public:
	MapExample();
	~MapExample();

	void Update();
	
	virtual bool FilterEvent(CIwEvent* pEvent) { return false; }
	virtual bool HandleEvent(CIwEvent* pEvent);
private:
	void HandlePOISelected(void* customData);
	static int HandlePOISelected(void* customData, void* thisPtr) { ((MapExample*)thisPtr)->HandlePOISelected(customData); return 0; }
	
	CIwUIButton* MakeButton(const char* caption) {
		CIwUIButton* p_btn = new CIwUIButton;
		p_btn->SetStyle("<button>");
		p_btn->SetCaption(caption);
		return p_btn;
	}
	
	CIwUIButton* m_pStandardMapBtn;
	CIwUIButton* m_pSatelliteMapBtn;
	CIwUIButton* m_pHybridMapBtn;
	bool m_isShowingUserLocation;
	CIwUIButton* m_pToggleUserLocBtn;
	bool m_isMapVisible;
	CIwUIButton* m_pToggleMapVisibilityBtn;
	s3eMapView* m_pMapView;
};
