/*
Part of the java implementation of the s3eMapView extension.
This s3eMapActivity class is instantiated by the s3eMapView class.

s3eMapActivity is an Android activity class that creates and manages an
OverlayedMapFragment (subclass of SupportMapFragment), which contains the
actual GoogleMap.

Code is copyright (c) 2013 Get to Know Society.
Licensed under the zlib license - see LICENSE file.

*/

package com.bradenmacdonald;

import android.app.AlertDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

// Support packages:
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentTransaction;
import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.SupportMapFragment;

class OverlayedMapFragment extends SupportMapFragment {
	/* * * * * * * * * * * * *
	 * In order for the Google Map's OpenGL surface to correctly appear
	 * above the Marmalade app surface, we need to call
	 * setZOrderMediaOverlay(true) on the SupportMapFragment's SurfaceView
	 *
	 * Ideally, we could just set this using GoogleMapOptions, but sadly it 
	 * only allows us to set zOrderOnTop and not setZOrderMediaOverlay
	 * So, we have to subclass SupportMapFragment and use an ugly hack
	 * to set the 'zOrderMediaOverlay' true.
	 * * * * * * * * * * * * * */
	@Override
	public View onCreateView(android.view.LayoutInflater arg0,	ViewGroup arg1, Bundle arg2) {
		ViewGroup baseViewGroup = (ViewGroup)super.onCreateView(arg0, arg1, arg2);
		// Search through the descendants of baseViewGroup to find and fix the SurfaceView:
		java.util.ArrayList<View> touchables = baseViewGroup.getTouchables();
		for (int i=0; i < touchables.size(); i++) {
			if (touchables.get(i) instanceof android.view.SurfaceView) {
				mSurfaceView = (android.view.SurfaceView)touchables.get(i);
				mSurfaceView.setZOrderMediaOverlay(true);
				mSurfaceView.setVisibility(View.INVISIBLE); // Should be hidden initially
				break;
			}
		}
		return baseViewGroup;
	};
	public void onHiddenChanged(boolean hidden) {
		// Just making this Fragment's view invisible won't hide it...
		// we also need to set the SurfaceView invisible explicity
		if (mSurfaceView != null)
			mSurfaceView.setVisibility(hidden ? View.INVISIBLE : View.VISIBLE);
	}
	public void GoTo(final CameraUpdate dest, final boolean animated) {
		if (isHidden() || mSurfaceView.getWidth() == 0) {
			// Animating the map's camera before the map has been made
			// visible will cause the app to crash. Therefore, we must
			// wait until mSurfaceView is visible and has a layout
			// before changing the map's camera
			// Hackish solution: poll every 100ms until mSurfaceView
			// is visible.
			new android.os.Handler().postDelayed(new Runnable() {
				public void run() {
					GoTo(dest, animated);
				}
			}, 100);
		} else {
			// It's ok to show this now:
			if (animated)
				getMap().animateCamera(dest);
			else
				getMap().moveCamera(dest);
		}
	}
	private android.view.SurfaceView mSurfaceView;
}

public class s3eMapActivity extends FragmentActivity {
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		FrameLayout layout = new FrameLayout(this);
		layout.setId(LAYOUT_ID);
		setContentView(layout);

		//mMapFragment = SupportMapFragment.newInstance(op);
		mMapFragment = new OverlayedMapFragment();
		FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
		fragmentTransaction.add(LAYOUT_ID, mMapFragment);
		fragmentTransaction.commit();
	}

	@Override
	public void onStart(){
		super.onStart();
		mMap = mMapFragment.getMap();
		if (mMap == null) {
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setMessage("Unable to create a Google Map. Please make sure the Google Maps app is installed on your device.");
			builder.setCancelable(true);
			builder.setPositiveButton("OK", null);
			builder.create().show();
			// Throwing an exception here ensures that s3eMapViewCreate() will
			// return null, so that the Marmalade app knows that the map failed
			// to load.
			throw new RuntimeException("s3eMapActivity: onStart: Could not get a GoogleMap from mMapFragment.getMap()");
		}
		// Map is supposed to be hidden at first:
		getSupportFragmentManager().beginTransaction().hide(mMapFragment).commit();
	}
	public void setVisibility(boolean visible) {
		FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
		if (visible)
			fragmentTransaction.show(mMapFragment);
		else
			fragmentTransaction.hide(mMapFragment);
		fragmentTransaction.commit();
	}

	public void GoTo(CameraUpdate dest, boolean animate) { mMapFragment.GoTo(dest, animate); }

	public GoogleMap getMap() { return mMap; }
	private GoogleMap mMap;
	private OverlayedMapFragment mMapFragment;
	private static final int LAYOUT_ID = 505050; // arbitrary ID #
	private static final String TAG = "s3eMapActivity";
}
