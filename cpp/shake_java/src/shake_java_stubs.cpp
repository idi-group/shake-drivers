/*	Copyright (c) 2006-2009, University of Glasgow
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*
*		* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
*			in the documentation and/or other materials provided with the distribution.
*		* Neither the name of the University of Glasgow nor the names of its contributors may be used to endorse or promote products derived 
*			from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
*	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
*	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
*	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SHAKE_shake_device.h"
#include "shake_driver.h"

/*	Java native code to call the C shake_driver.dll.
*	Most functions are extremely simple, either calling the corresponding C
*	function directly, or converting array arguments to C-style arrays. The only
*	complicated bits of the code deals with callbacks used to relay events from
*	pressing the nav switch on the side of the SHAKE. 
*/

/*	collects required information about a callback method, used in the main_callback() function below */
typedef struct {
	jobject obj;
	jmethodID mid;
	__int64 dev;
} callback_info;

/*	global pointer to Java Virtual Machine instance */
static JavaVM *jvm = NULL;

/*	array of callback info structures. Limited to 8 here, but as only 1 callback per SHAKE is allowed
*	8 SHAKEs on a single PC is a little unrealistic anyway */
static callback_info callbacks[8];

/* number of populated structures in the array */
static int num_callbacks = 0;

/* the C code in shake_driver.dll must callback to a C function. Each time a callback is
*	registered, it is given a pointer to this function. The code below then finds the 
*	appropriate callback_info structure for the specified shake_device instance, and
*	calls the Java method specified in it with the original event parameter */
static void SHAKE_CALLBACK main_callback(shake_device* dev, int ev) {
	for(int i=0;i<num_callbacks;i++) {
		if((__int64)dev == callbacks[i].dev) {
			// must call AttachCurrentThread to attach this thread to the JVM, otherwise
			// it tends to cause a crash when making the callback
			JNIEnv *env;
			jvm->AttachCurrentThread((void**)&env, NULL);

			// Call the specified Java method
			env->CallVoidMethod(callbacks[i].obj, callbacks[i].mid, ev);

			// Detach the thread from the JVM; have to remember this, otherwise app will hang on exit
#ifdef CREME
			jvm->DetachCurrentThread(env);
#else
			jvm->DetachCurrentThread();
#endif
			break;
		}
	}
}

/*	adds a new callback to the callbacks array */
static int add_callback(JNIEnv* env, __int64 dev, jobject obj, jmethodID mid) {
	if(num_callbacks >= 8)
		return 0;

	callbacks[num_callbacks].dev = dev;
	callbacks[num_callbacks].obj = env->NewGlobalRef(obj);
	callbacks[num_callbacks].mid = mid;
	num_callbacks++;

	if(jvm == NULL) {
		env->GetJavaVM(&jvm);
	}
	shake_register_event_callback((shake_device*)dev, main_callback);

	return 1;
}

/* removes a given callback from the callbacks array */
static int remove_callback(JNIEnv* env, __int64 dev) {
	if(num_callbacks < 1)
		return 0;

	int pos = -1;
	for(int i=0;i<num_callbacks;i++) {
		if(callbacks[i].dev == dev) {
			pos = i;
			break;
		}
	}

	if(pos == -1)
		return 0;

	env->DeleteGlobalRef(callbacks[pos].obj);

	for(int i=pos;i<num_callbacks-1;i++) {
		callbacks[i].dev = callbacks[i+1].dev;
		callbacks[i].mid = callbacks[i+1].mid;
		callbacks[i].obj = callbacks[i+1].obj;
	}
	shake_register_event_callback((shake_device*)dev, NULL);
	num_callbacks--;
	if(num_callbacks == 0) {
		
	}

	return 1;
}

JNIEXPORT jlong JNICALL Java_SHAKE_shake_1device_shake_1init_1device(JNIEnv* env, jclass cla, jint port, jint devtype) {
	shake_device* dev = NULL;
	dev = shake_init_device(port, devtype);
	return (jlong)dev;
}

JNIEXPORT jlong JNICALL Java_SHAKE_shake_1device_shake_1init_1device_1rfcomm(JNIEnv *, jclass, jlong btaddr, jint devtype) {
	shake_device* dev = NULL;
	dev = shake_init_device_rfcomm_i64(btaddr, devtype);
	return (jlong)dev;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1free_1device (JNIEnv* env, jclass, jlong dev) {
	return shake_free_device((shake_device*)dev);
}

JNIEXPORT jfloat JNICALL Java_SHAKE_shake_1device_shake_1info_1firmware_1revision(JNIEnv *, jclass, jlong dev) { 
	return shake_info_firmware_revision((shake_device*)dev);
}

JNIEXPORT jfloat JNICALL Java_SHAKE_shake_1device_shake_1info_1hardware_1revision(JNIEnv *, jclass, jlong dev) { 
	return shake_info_hardware_revision((shake_device*)dev);
}

JNIEXPORT jstring JNICALL Java_SHAKE_shake_1device_shake_1info_1serial_1number(JNIEnv* env, jclass, jlong dev) { 
	return env->NewStringUTF(shake_info_serial_number((shake_device*)dev));
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1info_1module_1slot1(JNIEnv *, jclass, jlong dev) {
	return shake_info_module_slot1((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1info_1module_1slot2(JNIEnv *, jclass, jlong dev) {
	return shake_info_module_slot2((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1info_1retrieve(JNIEnv *, jclass, jlong dev) {
	return shake_info_retrieve((shake_device*)dev);
}

JNIEXPORT jstring JNICALL Java_SHAKE_shake_1device_shake_1info_1module_1name(JNIEnv *env, jclass, jint mtype) {
	return env->NewStringUTF(shake_info_module_name(mtype));
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1accx(JNIEnv *, jclass, jlong dev) { 
	return shake_accx((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1accy(JNIEnv *, jclass, jlong dev) { 
	return shake_accy((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1accz(JNIEnv *, jclass, jlong dev) { 
	return shake_accz((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1acc(JNIEnv* env, jclass, jlong dev, jintArray acc) { 
	int xyz[3];
	jsize arraylength = env->GetArrayLength(acc);
	jint* arr = env->GetIntArrayElements(acc, 0);
	int ret = shake_acc((shake_device*)dev, xyz);
	arr[0] = xyz[0];
	arr[1] = xyz[1];
	arr[2] = xyz[2];
	
	//printf("%ld %ld %ld || %d %d %d \n", arr[0], arr[1], arr[2], xyz[0], xyz[1], xyz[2] );
	env->ReleaseIntArrayElements(acc, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1gyrx(JNIEnv*, jclass, jlong dev) { 
	return shake_gyrx((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1gyry(JNIEnv *, jclass, jlong dev) { 
	return shake_gyry((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1gyrz(JNIEnv *, jclass, jlong dev) { 
	return shake_gyrz((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1gyr(JNIEnv* env, jclass, jlong dev, jintArray gyr) { 
	int xyz[3];
	jsize arraylength = env->GetArrayLength(gyr);
	jint* arr = env->GetIntArrayElements(gyr, 0);
	int ret = shake_gyr((shake_device*)dev, xyz);
	arr[0] = xyz[0];
	arr[1] = xyz[1];
	arr[2] = xyz[2];
	env->ReleaseIntArrayElements(gyr, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1magx(JNIEnv *, jclass, jlong dev) { 
	return shake_magx((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1magy(JNIEnv *, jclass, jlong dev) { 
	return shake_magy((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1magz(JNIEnv *, jclass, jlong dev) { 
	return shake_magz((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1mag(JNIEnv* env, jclass, jlong dev, jintArray mag) { 
	int xyz[3];
	jsize arraylength = env->GetArrayLength(mag);
	jint* arr = env->GetIntArrayElements(mag, 0);
	int ret = shake_mag((shake_device*)dev, xyz);
	arr[0] = xyz[0];
	arr[1] = xyz[1];
	arr[2] = xyz[2];
	env->ReleaseIntArrayElements(mag, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1heading(JNIEnv *, jclass, jlong dev) { 
	return shake_heading((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1cap0(JNIEnv *, jclass, jlong dev) { 
	return sk6_cap0((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1cap1(JNIEnv *, jclass, jlong dev) { 
	return sk6_cap1((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1cap(JNIEnv* env, jclass, jlong dev, jintArray cap) { 
	int proxboth[2];
	jsize arraylength = env->GetArrayLength(cap);
	jint* arr = env->GetIntArrayElements(cap, 0);
	int ret = sk6_cap((shake_device*)dev, proxboth);
	arr[0] = proxboth[0];
	arr[1] = proxboth[1];
	env->ReleaseIntArrayElements(cap, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1analog0(JNIEnv *, jclass, jlong dev) { 
	return shake_analog0((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1analog1(JNIEnv *, jclass, jlong dev) { 
	return shake_analog1((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1analog(JNIEnv* env, jclass, jlong dev, jintArray analog) { 
	int a0a1[2];
	jsize arraylength = env->GetArrayLength(analog);
	jint* arr = env->GetIntArrayElements(analog, 0);
	int ret = shake_analog((shake_device*)dev, a0a1);
	arr[0] = a0a1[0];
	arr[1] = a0a1[1];
	env->ReleaseIntArrayElements(analog, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1shaking_1peakaccel(JNIEnv *, jclass, jlong dev) {
	return shake_shaking_peakaccel((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1shaking_1direction(JNIEnv *, jclass, jlong dev) {
	return shake_shaking_direction((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1shaking_1timestamp(JNIEnv *, jclass, jlong dev) {
	return shake_shaking_timestamp((shake_device*)dev);
}


/*	registers a callback for nav switch events from the SHAKE */
JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1register_1event_1callback(JNIEnv* env, jclass cla, jobject obj, jlong dev) {	
	/* if no function specified, effect is to clear any existing callback */
	if(obj == NULL) {
		return remove_callback(env, dev);
	}
	/* obtain a pointer to the Java callback method. The "(I)V" string indicates a function
	*	with an integer parameter returning void. */
	jclass classobj = env->GetObjectClass(obj );
	if(classobj == NULL)
		return SHAKE_ERROR;
	jmethodID callbackID = env->GetMethodID(classobj, "callback", "(I)V" );
	if(callbackID == NULL)
		return SHAKE_ERROR;

	/* add the callback to the global list */
	return add_callback(env, dev, obj, callbackID);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1power_1state(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_power_state((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1power_1state(JNIEnv *, jclass, jlong dev, jint val) { 
	return shake_write_power_state((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1power_1state_12(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_power_state_2((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1power_1state_12(JNIEnv *, jclass, jlong dev, jint val) { 
	return shake_write_power_state_2((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1acc_1config(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_acc_config((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1acc_1config(JNIEnv *, jclass, jlong dev, jint val) { 
	return shake_write_acc_config((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1data_1format(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_data_format((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1data_1format(JNIEnv *, jclass, jlong dev, jint val) { 
	return shake_write_data_format((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1calib_1bypass(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_calib_bypass((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1calib_1bypass(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_calib_bypass((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1cx_1bypass(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_cx_bypass((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1cx_1bypass(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_cx_bypass((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1temp_1compensation(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_temp_compensation((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1temp_1compensation(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_temp_compensation((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1packet_1streaming(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_packet_streaming((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1packet_1streaming(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_packet_streaming((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1audio_1config(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_audio_config((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1audio_1config(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_audio_config((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1midi_1amplitude(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_midi_amplitude((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1midi_1note(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_midi_note((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1midi_1waveform(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_midi_waveform((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs0_1inc(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs0_inc((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}


JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs0_1inc(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs0_inc((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs0_1dec(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs0_dec((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs0_1dec(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs0_dec((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs0_1inc_1profile(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs0_inc_profile((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs0_1inc_1profile(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs0_inc_profile((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs0_1dec_1profile(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs0_dec_profile((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs0_1dec_1profile(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs0_dec_profile((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs1_1inc(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs1_inc((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs1_1inc(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs1_inc((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs1_1dec(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs1_dec((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs1_1dec(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs1_dec((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs1_1inc_1profile(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs1_inc_profile((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs1_1inc_1profile(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs1_inc_profile((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cs1_1dec_1profile(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = sk6_read_cs1_dec_profile((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cs1_1dec_1profile(JNIEnv *, jclass, jlong dev, jint val) {
	return sk6_write_cs1_dec_profile((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1read_1cap_1thresholds(JNIEnv* env, jclass, jlong dev, jbyteArray vals) {
	if(	env->GetArrayLength(vals) != 8)
		return -1;

	jbyte* vals_ptr = env->GetByteArrayElements(vals, 0);
	int ret = sk6_read_cap_thresholds((shake_device*)dev, (unsigned char*)vals_ptr);
	env->ReleaseByteArrayElements(vals, vals_ptr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1write_1cap_1thresholds(JNIEnv* env, jclass, jlong dev, jbyteArray vals) {
	if(env->GetArrayLength(vals) != 8)
		return -1;

	jbyte* vals_ptr = env->GetByteArrayElements(vals, 0);
	int ret = sk6_write_cap_thresholds((shake_device*)dev, (unsigned char*)vals_ptr);
	env->ReleaseByteArrayElements(vals, vals_ptr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1sample_1rate(JNIEnv *, jclass, jlong dev, jint sensor) { 
	unsigned char val;
	int ret = shake_read_sample_rate((shake_device*)dev, sensor, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1sample_1rate(JNIEnv *, jclass, jlong dev, jint sensor, jint val) { 
	return shake_write_sample_rate((shake_device*)dev, sensor, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1digital_1filter(JNIEnv *, jclass, jlong dev, jint sensor) { 
	unsigned char val;
	int ret = shake_read_digital_filter((shake_device*)dev, sensor, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1digital_1filter(JNIEnv *, jclass, jlong dev, jint sensor, jint val) { 
	return shake_write_digital_filter((shake_device*)dev, sensor, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1config(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_config((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1config(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_config((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1accel_1threshold(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_accel_threshold((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1accel_1threshold(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_config((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1holdoff_1time(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_holdoff_time((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1holdoff_1time(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_holdoff_time((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1vibration_1profile(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_vibration_profile((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1vibration_1profile(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_vibration_profile((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1hpf_1constant(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_hpf_constant((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1hpf_1constant(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_hpf_constant((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1shaking_1lpf_1constant(JNIEnv *, jclass, jlong dev) {
	unsigned char val;
	int ret = shake_read_shaking_lpf_constant((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1shaking_1lpf_1constant(JNIEnv *, jclass, jlong dev, jint value) {
	return shake_write_shaking_lpf_constant((shake_device*)dev, value);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1reset_1shaking_1detection(JNIEnv *, jclass, jlong dev) {
	return shake_reset_shaking_detection((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1packet_1request(JNIEnv *, jclass, jlong dev, jint val) { 
	return shake_write_packet_request((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write_1data_1request(JNIEnv *, jclass, jlong dev, jint val) {
	return shake_write_data_request((shake_device*)dev, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1playvib(JNIEnv *, jclass, jlong dev, jint channel, jbyte val) {
	return shake_playvib((shake_device*)dev, (shake_vib_channels)channel, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk6_1upload_1vib_1sample(JNIEnv* env, jclass, jlong dev, jbyte profile, jintArray samples) { 
	int sample_len = env->GetArrayLength(samples);
	if(sample_len > 64)
		return SHAKE_ERROR;

	int sample_arr[64];
	jint* arr = env->GetIntArrayElements(samples, 0);
	for(int i=0;i<sample_len;i++) {
		sample_arr[i] = arr[i];
	}

	int ret = sk6_upload_vib_sample((shake_device*)dev, profile, sample_arr, sample_len >> 1);
	env->ReleaseIntArrayElements(samples, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1battery_1level(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_battery_level((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read_1power_1status(JNIEnv *, jclass, jlong dev) { 
	unsigned char val;
	int ret = shake_read_power_status((shake_device*)dev, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jfloat JNICALL Java_SHAKE_shake_1device_shake_1read_1temperature(JNIEnv *, jclass, jlong dev) { 
	return shake_read_temperature((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1read(JNIEnv *, jclass, jlong dev, jint addr) { 
	unsigned char val;
	int ret = shake_read((shake_device*)dev, addr, &val);
	if(ret == SHAKE_ERROR)
		return -1;
	return val;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1write(JNIEnv *, jclass, jlong dev, jint addr, jint val) { 
	return shake_write((shake_device*)dev, addr, val);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1packet_1count(JNIEnv *, jclass, jlong dev) {
	return shake_logging_packet_count((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1play(JNIEnv* env, jclass, jlong dev, jcharArray filename) {
	jchar* filename_bytes = env->GetCharArrayElements(filename, 0);
	int ret = shake_logging_play((shake_device*)dev, (char*)filename_bytes);
	env->ReleaseCharArrayElements(filename, filename_bytes, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1pause(JNIEnv *, jclass, jlong dev) {
	return shake_logging_pause((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1stop(JNIEnv *, jclass, jlong dev) {
	return shake_logging_stop((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1record(JNIEnv *, jclass, jlong dev) {
	return shake_logging_record((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1reset(JNIEnv *, jclass, jlong dev) {
	return shake_logging_reset((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1status(JNIEnv *, jclass, jlong dev) {
	return shake_logging_status((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1memory_1status(JNIEnv *, jclass, jlong dev) {
	return shake_logging_memory_status((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1logging_1packets_1read(JNIEnv *, jclass, jlong dev) {
	return shake_logging_packets_read((shake_device*)dev);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_shake_1data_1timestamp(JNIEnv *, jclass, jlong dev, jint sensor) {
	return shake_data_timestamp((shake_device*)dev, sensor);
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk7_1roll_1pitch_1heading(JNIEnv * env, jclass, jlong dev, jintArray rph) {
	int foo[3];
	jsize arraylength = env->GetArrayLength(rph);
	jint* arr = env->GetIntArrayElements(rph, 0);
	int ret = sk7_roll_pitch_heading((shake_device*)dev, foo);
	arr[0] = foo[0];
	arr[1] = foo[1];
	arr[2] = foo[2];
	
	env->ReleaseIntArrayElements(rph, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk7_1roll_1pitch_1heading_1quaternions(JNIEnv * env, jclass, jlong dev, jintArray rphq) {
	int foo[4];
	jsize arraylength = env->GetArrayLength(rphq);
	jint* arr = env->GetIntArrayElements(rphq, 0);
	int ret = sk7_roll_pitch_heading_quaternions((shake_device*)dev, foo);
	arr[0] = foo[0];
	arr[1] = foo[1];
	arr[2] = foo[2];
	arr[3] = foo[3];
	env->ReleaseIntArrayElements(rphq, arr, 0);
	return ret;
}

JNIEXPORT jint JNICALL Java_SHAKE_shake_1device_sk7_1configure_1roll_1pitch_1heading(JNIEnv *, jclass, jlong dev, jint value) {
	return sk7_configure_roll_pitch_heading((shake_device*)dev, value);
}