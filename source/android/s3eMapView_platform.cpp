/*
Android-specific implementation of the s3eMapView extension.
This code instantiates an instance of the Java "s3eMapView" class.

Code is copyright (c) 2013 Get to Know Society.
Licensed under the zlib license - see LICENSE file.
*/
#include "s3eMapView_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jclass g_s3eMapViewCls;
static jmethodID g_s3eMapViewCreate;
static jmethodID g_s3eMapViewDestroy;
static jmethodID g_s3eMapViewSetScreenRect;
static jmethodID g_s3eMapViewSetVisible;
static jmethodID g_s3eMapViewSetType;
static jmethodID g_s3eMapViewSetShowUserLocation;
static jmethodID g__s3eMapViewGoTo;
static jmethodID g__s3eMapViewAddPOI;
static jmethodID g_s3eMapViewRemovePOI;
static jmethodID g_s3eMapViewGetLicensingString;

struct s3eMapView : public _jobject {}; // Declaring this makes 's3eMapView*' equivalent to 'jobject'
void Java_s3eMapView_notifyAppOfPOIClick(JNIEnv *env, jobject obj, jint clickData); // Forward declaration. Implemented at end of this file.

s3eResult s3eMapViewInit_platform() {
	// Get the environment from the pointer
	JNIEnv* env = s3eEdkJNIGetEnv();

	// Get the extension class
	g_s3eMapViewCls = s3eEdkAndroidFindClass("s3eMapView");
	if (!g_s3eMapViewCls)
		goto fail;

	g_s3eMapViewDestroy = env->GetMethodID(g_s3eMapViewCls, "Destroy", "()V");
	if (!g_s3eMapViewDestroy)
		goto fail;

	g_s3eMapViewSetScreenRect = env->GetMethodID(g_s3eMapViewCls, "SetScreenRect", "(IIII)V");
	if (!g_s3eMapViewSetScreenRect)
		goto fail;

	g_s3eMapViewSetVisible = env->GetMethodID(g_s3eMapViewCls, "SetVisible", "(Z)V");
	if (!g_s3eMapViewSetVisible)
		goto fail;

	g_s3eMapViewSetType = env->GetMethodID(g_s3eMapViewCls, "SetType", "(I)V");
	if (!g_s3eMapViewSetType)
		goto fail;

	g_s3eMapViewSetShowUserLocation = env->GetMethodID(g_s3eMapViewCls, "SetShowUserLocation", "(Z)V");
	if (!g_s3eMapViewSetShowUserLocation)
		goto fail;

	g__s3eMapViewGoTo = env->GetMethodID(g_s3eMapViewCls, "GoTo", "(DDDDZ)V");
	if (!g__s3eMapViewGoTo)
		goto fail;

	g__s3eMapViewAddPOI = env->GetMethodID(g_s3eMapViewCls, "AddPOI", "(DDLjava/lang/String;Ljava/lang/String;ZI)I");
	if (!g__s3eMapViewAddPOI)
		goto fail;

	g_s3eMapViewRemovePOI = env->GetMethodID(g_s3eMapViewCls, "RemovePOI", "(I)V");
	if (!g_s3eMapViewRemovePOI)
		goto fail;

	g_s3eMapViewGetLicensingString = env->GetStaticMethodID(g_s3eMapViewCls, "GetLicensingString", "()Ljava/lang/String;");
	if (!g_s3eMapViewGetLicensingString)
		goto fail;

	// Register our handler for POI click events:
	static const JNINativeMethod jnm = { "notifyAppOfPOIClick", "(I)V", (void*)&Java_s3eMapView_notifyAppOfPOIClick };
	if (env->RegisterNatives(g_s3eMapViewCls, &jnm, 1) != 0)
		goto fail;

	IwTrace(MAPVIEW, ("MAPVIEW init success"));

	// Add any platform-specific initialisation code here
	return S3E_RESULT_SUCCESS;

fail:
	jthrowable exc = env->ExceptionOccurred();
	if (exc) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		IwTrace(s3eMapView, ("One or more java methods could not be found"));
	}
	return S3E_RESULT_ERROR;

}

void s3eMapViewTerminate_platform() {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->DeleteGlobalRef(g_s3eMapViewCls);
}

s3eMapView* s3eMapViewCreate_platform() {
	// Instantiate a new instance of s3eMapView and return a pointer to it
	jobject result = NULL;
	JNIEnv* env = s3eEdkJNIGetEnv();
	jmethodID cons = env->GetMethodID(g_s3eMapViewCls, "<init>", "()V"); // Constructor for s3eMapView
	if (cons) {
		jobject instance = env->NewObject(g_s3eMapViewCls, cons);
		if (instance && !env->ExceptionOccurred()) {
			result = env->NewGlobalRef(instance);
			env->DeleteLocalRef(instance);
		}
	}
	if (result) {
		return (s3eMapView*)result;
	} else {
		jthrowable exc = env->ExceptionOccurred();
		if (exc) {
			env->ExceptionDescribe();
			env->ExceptionClear();
			IwTrace(s3eMapView, ("s3eMapViewCreate_android: Unable to instantiate s3eMapView instance!"));
		}
		return NULL;
	}
}

s3eResult s3eMapViewDestroy_platform(s3eMapView* pMapView) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewDestroy);
	env->DeleteGlobalRef(pMapView);
	return S3E_RESULT_SUCCESS;
}

void s3eMapViewSetScreenRect_platform(s3eMapView* pMapView, int x, int y, int w, int h) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewSetScreenRect, x, y, w, h);
}

void s3eMapViewSetVisible_platform(s3eMapView* pMapView, bool visible) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewSetVisible, visible);
}

void s3eMapViewSetType_platform(s3eMapView* pMapView, s3eMapViewType type) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewSetType, (int)type);
}

void s3eMapViewSetShowUserLocation_platform(s3eMapView* pMapView, bool show) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewSetShowUserLocation, show);
}

void s3eMapViewGoTo_platform(s3eMapView* pMapView, const s3eMapViewCoord* center, const s3eMapViewSpan* span, bool animate) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g__s3eMapViewGoTo, center->lat, center->lng, span->latDelta, span->lngDelta, animate);
}

s3eMapViewPOI* s3eMapViewAddPOI_platform(s3eMapView* pMapView, const s3eMapViewCoord* location, const char* title, const char* subtitle, bool clickable, void* clickData) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	jstring title_jstr = env->NewStringUTF(title);
	jstring subtitle_jstr = env->NewStringUTF(subtitle);
	int id = env->CallIntMethod(pMapView, g__s3eMapViewAddPOI, location->lat, location->lng, title_jstr, subtitle_jstr, clickable, (int)clickData);
	env->DeleteLocalRef(title_jstr);
	env->DeleteLocalRef(subtitle_jstr);
	return (s3eMapViewPOI*)id;
}

void s3eMapViewRemovePOI_platform(s3eMapView* pMapView, s3eMapViewPOI* poi) {
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallVoidMethod(pMapView, g_s3eMapViewRemovePOI, (int)poi);
}

// Helper function to get the licensing string that Google requires 
// be displayed in one's app
static char gs_s3eMapView_licenseInfoStrBuffer[600];
const char* s3eMapViewGetPlatformLicensingString_platform() {
	JNIEnv* env = s3eEdkJNIGetEnv();
	jstring licenseStr = (jstring)env->CallStaticObjectMethod(g_s3eMapViewCls, g_s3eMapViewGetLicensingString);
	const char* licenseStr_asCStr = env->GetStringUTFChars(licenseStr, 0);
	// We now copy the chars from the Java buffer to our long-lived static buffer:
	strncpy(gs_s3eMapView_licenseInfoStrBuffer, licenseStr_asCStr, sizeof(gs_s3eMapView_licenseInfoStrBuffer));
	// Free memory:
	env->ReleaseStringUTFChars(licenseStr, licenseStr_asCStr);
	env->DeleteLocalRef(licenseStr);
	return gs_s3eMapView_licenseInfoStrBuffer;
}

void Java_s3eMapView_notifyAppOfPOIClick(JNIEnv *env, jobject obj, jint clickData) {
	// This method gets called when the user selects a POI by clicking on its callout.
	// We need to call any callbacks that the Marmalade app may have registered:
	s3eEdkCallbacksEnqueue(S3E_EXT_MAPVIEW_HASH, S3E_MAPVIEW_CALLBACK_POI_SELECTED, (void*)clickData);
}
