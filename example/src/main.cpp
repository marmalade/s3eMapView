#include <stdexcept>
#include <string>
#include <time.h>
#include "s3eDevice.h"
#include "IwUI.h"

#include "MapExample.h"

int main() {
	IwUIInit();
	// Initialize IwUI singletons:
	new CIwUIView;
	new CIwUIController;
	
	// Load UI resources:
	IwGetResManager()->LoadGroup("iwui_style/iwui_style.group");
	// Set the graphics system to clear to a grey screen:
	IwGxSetColClear(0xaa,0xaa,0xaa,0xff);
	
	// Instantiate a MapExample
	try {
		MapExample* p_example = new MapExample;
		IwGetUIController()->AddEventHandler(p_example);
		
		// Main loop
		while (1) {
			const uint64 start = s3eTimerGetMs();
			
			s3eDeviceYield(0); // Process device events
			s3eKeyboardUpdate();
			s3ePointerUpdate();
			IwGetUIController()->Update(); // Process the event loop
			
			p_example->Update();
			
			// Draw the screen:
			IwGxClear();
			IwGetUIView()->Render();
			IwGxFlush();
			IwGxSwapBuffers();
			
			// Maintain frame rate
			const static uint MS_PER_FRAME = 1000 / 30; // 30 Frames per second
			while ((s3eTimerGetMs() - start) < MS_PER_FRAME) {
				int32 yield = (int32) (MS_PER_FRAME - (s3eTimerGetMs() - start));
				if (yield<0)
					break;
				s3eDeviceYield(yield);
			}
			if (s3eDeviceCheckQuitRequest())
				break;
		}
		delete p_example;
	} catch(std::runtime_error e) {
		s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, (std::string("Fatal error: ")+e.what()).c_str());
	}
	
	// Clean up:
	delete IwGetUIController();
	delete IwGetUIView();

	IwUITerminate();
}
