using UnityEngine;
using System.Collections;
using System;
using System.Runtime.InteropServices;


public class PSVRExample : MonoBehaviour {

	[DllImport ("UnityPSVR")]
	private static extern IntPtr enumerateDevices();

	[DllImport ("UnityPSVR")]
	private static extern void freeDeviceInfos (IntPtr devices);

	[DllImport ("UnityPSVR")]
	private static extern ushort IsDeviceFound();
	[DllImport ("UnityPSVR")]
	private static extern IntPtr InitPSVR();
	[DllImport ("UnityPSVR")]
	private static extern void StartTracking(IntPtr ctx);
	[DllImport ("UnityPSVR")]
	private static extern void StopTracking(IntPtr ctx);
	[DllImport ("UnityPSVR")]
	private static extern IntPtr GetState(IntPtr ctx);
	/*
	struct  PSVRContext*        InitPSVR();
	void                        StartTracking(struct PSVRContext* ctx);
	struct  PSVRState*          GetState(struct PSVRContext* ctx);
	void                        StopTracking(struct PSVRContext* ctx);
	void                        ResetYaw(struct PSVRContext* ctx);
*/
	// Use this for initialization

	IntPtr ctxPtr;
	PSVRContext ctx;

	void Start () {
		ctxPtr = IntPtr.Zero;
		if (IsDeviceFound () == 1) {
			ctxPtr = InitPSVR ();
			ctx = (PSVRContext)Marshal.PtrToStructure (ctxPtr, typeof(PSVRContext));
			StartTracking (ctxPtr);

		}
	}

	// Update is called once per frame
	void Update () {
		if (ctxPtr != IntPtr.Zero) {
			IntPtr statePtr = GetState (ctxPtr);
			// Debug.Log (statePtr);
			if (statePtr != IntPtr.Zero) {
				PSVRState state = (PSVRState)Marshal.PtrToStructure (statePtr, typeof(PSVRState));
				/*
				Debug.Log ( state.status.ToString() + 
					" , pitch: " + state.pitch.ToString()+ 
					" , yaw: " + state.yaw.ToString()+ 
					" , roll: " + state.roll.ToString());
					*/
				int pitch = state.pitch > 125 ? state.pitch - 255 : state.pitch;
				int roll = state.roll > 125 ? state.roll - 255 : state.roll;

				Quaternion rotation = Quaternion.Euler(new Vector3(pitch - 40, 0, roll));
				gameObject.transform.rotation = rotation;
			}



		}
	}
	void OnDestroy() {
		if (ctxPtr != IntPtr.Zero) {
			StopTracking (ctxPtr);
		}

	}
}

[StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
public struct HidDeviceInfo {
	public IntPtr path;
	public ushort vendor_id;
	public ushort product_id;
	public IntPtr serial_number;
	public ushort release_number;
	public IntPtr manufacturer_string;
	public IntPtr product_string;
	public ushort usage_page;
	public ushort usage;
	public int interface_number;
	public IntPtr next;
};

[StructLayout(LayoutKind.Sequential)]
struct PSVRState {
	public short yawAccel;
	public short pitchAccel;
	public short rollAccel;

	public short yaw;
	public short pitch;
	public short roll;

	public short xAccel;
	public short yAccel;

	public ushort status;


};

[StructLayout(LayoutKind.Sequential)]
struct PSVRContext {
	public IntPtr currentState;
	public IntPtr device;
	public IntPtr loopThread;
	public ushort stopLoop;
	public IntPtr threadStatus;
	public IntPtr mutex;
	public ushort isTracking;
};