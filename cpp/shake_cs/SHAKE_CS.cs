using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

/* C# wrapper around shake_driver.dll. Defines a class called shake_device, which exposes 
 * most of the data from the SHAKE as properties. Register access functions are mapped directly
 * to the C equivalents. The only real difference from using the C/C++ code is in how to register 
 * a callback for nav switch events. In your own class, create a shake_device instance, then
 * add a handler for the shake_device.NavEvent event like this:
 *  shake_device sd = new shake_device();
 *  sd.NavEvent += new shake_device.NavButtonEventHandler(sd_NavEvent);
 * Just hit tab twice after the second line (in VS2005) and it should create the handler function for you. 
 * To remove the handler:
 *  sd.NavEvent -= <name of your handler function>
 */

namespace SHAKE.CS
{
    public enum SHAKE_SENSORS : short
    {
        ACC,
        GYRO,
        MAG,
        HEADING,
        CAP0,
        CAP1,
        ANALOG0,
        ANALOG1
    }

    public enum SHAKE_VIB_CHANNELS : short
    {
        MAIN, 
        LEFT,
        RIGHT
    }

    public enum SHAKE_EVENT : int
    {
        NAV_UP = 1,
        NAV_DOWN = 2,
        NAV_CENTRE = 3,
        NAV_NORMAL = 4,
        CS0_UPPER = 5,
        CS0_LOWER = 6,
        CS1_UPPER = 7,
        CS1_LOWER = 8,
        PLAYBACK_COMPLETE = 9,
        SHAKING_EVENT = 10,
    }

    public enum shake_type : int
    {
        SHAKE_SK6 = 0,
        SHAKE_SK7 = 1,
    }

    public class shake_device
    {
        // stores C pointer to the device instance
        private IntPtr dev;

        // indicates if connection currently active
        private bool connected = false;

        private int[] iacc = new int[3] { 0, 0, 0 };
        private int[] igyro = new int[3] { 0, 0, 0 };
        private int[] imag = new int[3] { 0, 0, 0 };
        private int[] iprox = new int[2] { 0, 0 };
        private int[] iana = new int[2] { 0, 0 };

        public const int SHAKE_SUCCESS = 1;
        public const int SHAKE_ERROR = -1;

        public const int SHAKE_POWER_CAP = 0x01;
        public const int SHAKE_POWER_ACC = 0x02;
        public const int SHAKE_POWER_GYRO = 0x04;
        public const int SHAKE_POWER_VIB = 0x08;
        public const int SHAKE_POWER_MAG = 0x10;
        public const int SHAKE_POWER_TEMP = 0x20;
        public const int SHAKE_POWER_ANALOG = 0x40;
        public const int SHAKE_POWER_NAV = 0x80;

        public const int SHAKE_POWER_CS0_INC = 0x01;
        public const int SHAKE_POWER_CS0_DEC = 0x02;
        public const int SHAKE_POWER_CS1_INC = 0x04;
        public const int SHAKE_POWER_CS1_DEC = 0x08;
        public const int SHAKE_POWER_CS_EVENTS = 0x10;

        public const int SHAKE_VIB_SAMPLE_MAX_LENGTH = 32;
        public const int SHAKE_VIB_PROFILE_MAX = 0xFF;

        public const int SHAKE_EXT_POWER = 0x01;
        public const int SHAKE_BAT_CHARGING = 0x02;
        public const int SHAKE_BAT_FULL = 0x04;

        public const int SHAKE_BATTERY_MAX = 0xFF;

        public const int SHAKE_TEMP_MIN = 0x00;
        public const int SHAKE_TEMP_MAX = 0x40;

        public const int SHAKE_LOGGING_PLAY = 0x01;
        public const int SHAKE_LOGGING_PAUSE = 0x02;
        public const int SHAKE_LOGGING_STOP = 0x03;
        public const int SHAKE_LOGGING_RECORD = 0x04;
        public const int SHAKE_LOGGING_RESET = 0x05;

        public const int SHAKE_LOGGING_RECORDING = 0x01;
        public const int SHAKE_LOGGING_PLAYING = 0x02;
        public const int SHAKE_LOGGING_MEMORYFULL = 0x04;

        public const int SHAKE_AUDIO_DATA_LEN = 0x20;
        public const int SHAKE_AUDIO_SAMPLE_RATE = 7200;
        public const int SHAKE_AUDIO_SAMPLE_SIZE = 16;
        public const int SHAKE_MIDI_AMPLITUDE_MUTE = 0x00;
        public const int SHAKE_MIDI_AMPLITUDE_MIN = 0x01;
        public const int SHAKE_MIDI_AMPLITUDE_MAX = 0xFF;
        public const int SHAKE_MIDI_NOTE_MIN = 0x21;
        public const int SHAKE_MIDI_NOTE_MAX = 0x69;

        public const int SHAKE_MIDI_WAVEFORM_SINE = 0;
        public const int SHAKE_MIDI_WAVEFORM_SQUARE = 1;
        public const int SHAKE_MIDI_WAVEFORM_TRIANGLE = 2;
        public const int SHAKE_MIDI_WAVEFORM_SAWTOOTH = 3;

        public const int SHAKE_AUDIO_PAGE_SIZE = 1056;
        public const int SHAKE_AUDIO_AMPLITUDE_MIN = 0;
        public const int SHAKE_AUDIO_AMPLITUDE_MAX = 255;
        public const int SHAKE_AUDIO_UPLOAD_MIN = 0;
        public const int SHAKE_AUDIO_UPLOAD_MAX = 7999;

        private SHAKEEventCB callback = null;
        private SHAKEAudioCB audio_callback = null;

        public shake_device()
        {
            dev = IntPtr.Zero;
        }

        public bool connect(int com_port, shake_type device_type)
        {
            if(connected)
                close();

            dev = shake_init_device(com_port, (int) device_type);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            register_nav_callback();
            register_audio_callback();
            return true;
        }

        public bool connect_rfcomm_i64(Int64 btaddr, shake_type device_type)
        {
            if (connected)
                close();

            dev = shake_init_device_rfcomm_i64(btaddr, (int) device_type);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            register_nav_callback();
            register_audio_callback();
            return true;
        }

        public bool connect_rfcomm_str(string btaddr, shake_type device_type)
        {
            if (connected)
                close();

            dev = shake_init_device_rfcomm_str(btaddr, (int) device_type);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            register_nav_callback();
            register_audio_callback();
            return true;
        }
        
        public bool close()
        {
            if (!connected)
                return false;

            connected = false;
            register_nav_callback();
            callback = null;

            shake_free_device(dev);
            return true;
        }

        public bool IsConnected
        {
            get
            {
                return connected;
            }
        }

        public int accx 
        {
            get
            {
                return shake_accx(dev);
            }
        }

        public int accy
        {
            get
            {
                return shake_accy(dev);
            }
        }

        public int accz
        {
            get
            {
                return shake_accz(dev);
            }
        }

        public int[] acc
        {
            get 
            {
                shake_acc(dev, iacc);
                return iacc;
            }
        }

        public int gyrx
        {
            get
            {
                return shake_gyrx(dev);
            }
        }

        public int gyry
        {
            get
            {
                return shake_gyry(dev);
            }
        }

        public int gyrz
        {
            get
            {
                return shake_gyrz(dev);
            }
        }

        public int[] gyr
        {
            get
            {
                shake_gyr(dev, igyro);
                return igyro;
            }
        }

        public int magx
        {
            get
            {
                return shake_magx(dev);
            }
        }

        public int magy
        {
            get
            {
                return shake_magy(dev);
            }
        }

        public int magz
        {
            get
            {
                return shake_magz(dev);
            }
        }

        public int[] mag
        {
            get
            {
                shake_mag(dev, imag);
                return imag;
            }
        }

        public int heading
        {
            get
            {
                return shake_heading(dev);
            }
        }

        public int cap0
        {
            get 
            {
                return shake_cap0(dev);
            }
        }

        public int cap1
        {
            get 
            {
                return shake_cap1(dev);
            }
        }

        public int[] cap
        {
            get 
            {
                shake_cap(dev, iprox);
                return iprox;
            }
        }

        public int analog0
        {
            get 
            {
                return shake_analog0(dev);
            }
        }

        public int analog1
        {
            get 
            {
                return shake_analog1(dev);
            }
        }

        public int[] analog
        {
            get 
            {
                shake_analog(dev, iana);
                return iana;
            }
        }

        public int peakaccel
        {
            get
            {
                return shake_shaking_peakaccel(dev);
            }
        }

        public int direction
        {
            get
            {
                return shake_shaking_direction(dev);
            }
        }

        public int timestamp
        {
            get
            {
                return shake_shaking_timestamp(dev);
            }
        }

        public int heart_rate
        {
            get
            {
                return shake_heart_rate(dev);
            }
        }

        public string rfid_tid
        {
            get
            {
                return shake_rfid_tid(dev);
            }
        }

        public bool rfid_scan()
        {
            if (shake_rfid_scan(dev) == SHAKE_ERROR)
                return false;
            return true;
        }

        public int data_timestamp(SHAKE_SENSORS sensor)
        {
            return shake_data_timestamp(dev, (int)sensor);
        }

        /* public event handler definition for nav button events */
        public delegate void SHAKEEventHandler(SHAKE_EVENT ev);

        /* the event object used to signal nav events */
        public event SHAKEEventHandler SHAKE_Event;

        /* triggers nav event */
        protected virtual void OnSHAKEEvent(SHAKE_EVENT ev)
        {
            if (SHAKE_Event != null)
                SHAKE_Event(ev);
        }

        /* signature for callback passed to C code.
         * Has to be declared as Cdecl to avoid crashes when the callback is used */
        private delegate void SHAKEEventCB(IntPtr dev, int ev);

        /* this is the actual function that gets called by the C code, it just relays the event to the OnNavEvent func */
        private void NavButtonCB_internal(IntPtr dev, int ev)
        {
            OnSHAKEEvent((SHAKE_EVENT)ev);
        }

        /* registers the callback func with the C code */
        private bool register_nav_callback()
        {
            if (callback == null)
            {
                callback = new SHAKEEventCB(NavButtonCB_internal);
            }
            else
            {
                callback = null;
            }
            
            // NOTE that this is the STDCALL variation of the normal cdecl callback,
            // because the .NET Compact Framework doesn't support C delegates with non-stdcall
            // calling conventions. On XP, .NET 2.0 and above can handle it though.
            shake_register_event_callback_STDCALL(dev,  callback);
            return true;
        }

        /* public event handler definition for nav button events */
        public delegate void SHAKEAudioHandler(short[] mic_samples, short[] playback_samples);

        /* the event object used to signal nav events */
        public event SHAKEAudioHandler SHAKE_Audio;

        /* triggers nav event */
        protected virtual void OnSHAKEAudio(short[] mic_samples, short[] playback_samples)
        {
            if (SHAKE_Audio != null)
                SHAKE_Audio(mic_samples, playback_samples);
        }

        /* signature for callback passed to C code.
         * Has to be declared as Cdecl to avoid crashes when the callback is used */
        private delegate void SHAKEAudioCB(IntPtr dev, short[] mic_samples, int mic_len, short[] playback_samples, int playback_len);

        /* this is the actual function that gets called by the C code, it just relays the event to the OnNavEvent func */
        private void AudioCB_internal(IntPtr dev, short[] mic_samples, int mic_len, short[] playback_samples, int playback_len)
        {
            OnSHAKEAudio(mic_samples, playback_samples);
        }

        /* registers the callback func with the C code */
        private bool register_audio_callback()
        {
            if (audio_callback == null)
            {
                audio_callback = new SHAKEAudioCB(AudioCB_internal);
            }
            else
            {
                audio_callback = null;
            }

            // NOTE that this is the STDCALL variation of the normal cdecl callback,
            // because the .NET Compact Framework doesn't support C delegates with non-stdcall
            // calling conventions. On XP, .NET 2.0 and above can handle it though.
            shake_register_audio_callback_STDCALL(dev, audio_callback);
            return true;
        }

        public bool upload_audio_sample(int address, short[] sample_data)
        {
            if (shake_upload_audio_sample(dev, address, sample_data) == SHAKE_ERROR)
                return false;
            return true;
        }

        public bool play_audio_sample(int start_address, int end_address, int amplitude)
        {
            if (shake_play_audio_sample(dev, start_address, end_address, amplitude) == SHAKE_ERROR)
                return false;
            return true;
        }

        public bool exp_upload_vib_sample(int address, byte[] sample_data)
        {
            if (shake_exp_upload_vib_sample(dev, address, sample_data) == SHAKE_ERROR)
                return false;
            return true;
        }

        public bool exp_play_vib_sample(int start_address, int end_address, int amplitude)
        {
            if (shake_exp_play_vib_sample(dev, start_address, end_address, amplitude) == SHAKE_ERROR)
                return false;
            return true;
        }

        public bool exp_write_gp_register(int reg_number, int value)
        {
            if (shake_exp_write_gp_register(dev, reg_number, value) == SHAKE_ERROR)
                return false;
            return true;
        }

        /*  Data logging */
        public int logged_packets_count
        {
            get
            {
                return shake_logged_packets_count(dev);
            }
        }

        public bool logging_play(string filename)
        {
            if (shake_logging_play(dev, filename) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool logging_pause()
        {
            if (shake_logging_pause(dev) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool logging_stop()
        {
            if (shake_logging_stop(dev) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool logging_record()
        {
            if (shake_logging_record(dev) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool logging_reset()
        {
            if (shake_logging_reset(dev) == SHAKE_ERROR)
                return false;

            return true;
        }

        public int logging_status()
        {
            return shake_logging_status(dev);
        }

        public int logging_memory_status()
        {
            return shake_logging_memory_status(dev);
        }

        public int logging_packet_count()
        {
            return shake_logging_packet_count(dev);
        }

        public int logging_packets_read()
        {
            return shake_logging_packets_read(dev);
        }

        /*  Register access */
        public bool read_power_state(ref byte value)
        {
            if (shake_read_power_state(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_power_state(byte value)
        {
            if (shake_write_power_state(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_power_state_2(ref byte value)
        {
            if (shake_read_power_state_2(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_power_state_2(byte value)
        {
            if (shake_write_power_state_2(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_acc_config(ref byte value)
        {
            if (shake_read_acc_config(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_acc_config(byte value)
        {
            if (shake_write_acc_config(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_data_format(ref byte value)
        {
            if (shake_read_data_format(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_data_format(byte value)
        {
            if (shake_write_data_format(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_digital_filter(SHAKE_SENSORS sensor, ref byte value)
        {
            if (shake_read_digital_filter(dev, (int)sensor, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_digital_filter(SHAKE_SENSORS sensor, byte value)
        {
            if (shake_write_digital_filter(dev, (int)sensor, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_sample_rate(SHAKE_SENSORS sensor, ref byte value)
        {
            if (shake_read_sample_rate(dev, (int)sensor, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_sample_rate(SHAKE_SENSORS sensor, byte value)
        {
            if (shake_write_sample_rate(dev, (int)sensor, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_calib_bypass(ref byte value)
        {
            if (shake_read_calib_bypass(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_calib_bypass(byte value)
        {
            if (shake_write_calib_bypass(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cx_bypass(ref byte value)
        {
            if (shake_read_cx_bypass(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cx_bypass(byte value)
        {
            if (shake_write_cx_bypass(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_temp_compensation(ref byte value)
        {
            if (shake_read_temp_compensation(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_temp_compensation(byte value)
        {
            if (shake_write_temp_compenstation(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_packet_streaming(ref byte value)
        {
            if (shake_read_packet_streaming(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_packet_streaming(byte value)
        {
            if (shake_write_packet_streaming(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs0_inc(ref byte value)
        {
            if (shake_read_cs0_inc(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs0_inc(byte value)
        {
            if (shake_write_cs0_inc(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs0_dec(ref byte value)
        {
            if (shake_read_cs0_dec(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs0_dec(byte value)
        {
            if (shake_write_cs0_dec(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs0_inc_profile(ref byte value)
        {
            if (shake_read_cs0_inc_profile(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs0_inc_profile(byte value)
        {
            if (shake_write_cs0_inc_profile(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs0_dec_profile(ref byte value)
        {
            if (shake_read_cs0_dec_profile(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs0_dec_profile(byte value)
        {
            if (shake_write_cs0_dec_profile(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs1_inc(ref byte value)
        {
            if (shake_read_cs1_inc(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs1_inc(byte value)
        {
            if (shake_write_cs1_inc(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs1_dec(ref byte value)
        {
            if (shake_read_cs1_dec(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs1_dec(byte value)
        {
            if (shake_write_cs1_dec(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs1_inc_profile(ref byte value)
        {
            if (shake_read_cs1_inc_profile(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs1_inc_profile(byte value)
        {
            if (shake_write_cs1_inc_profile(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cs1_dec_profile(ref byte value)
        {
            if (shake_read_cs1_dec_profile(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cs1_dec_profile(byte value)
        {
            if (shake_write_cs1_dec_profile(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_cap_thresholds(ref byte[] values)
        {
            if (values == null || values.Length != 8)
                return false;

            if (shake_read_cs0_inc(dev, out values[0]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs0_dec(dev, out values[1]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs0_inc_profile(dev, out values[2]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs0_dec_profile(dev, out values[3]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs1_inc(dev, out values[4]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs1_dec(dev, out values[5]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs1_inc_profile(dev, out values[6]) == SHAKE_ERROR)
                return false;

            if (shake_read_cs1_dec_profile(dev, out values[7]) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_cap_thresholds(byte[] values)
        {
            if (values == null || values.Length != 8)
                return false;

            if (shake_write_cs0_inc(dev, values[0]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs0_dec(dev, values[1]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs0_inc_profile(dev, values[2]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs0_dec_profile(dev, values[3]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs1_inc(dev, values[4]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs1_dec(dev, values[5]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs1_inc_profile(dev, values[6]) == SHAKE_ERROR)
                return false;

            if (shake_write_cs1_dec_profile(dev, values[7]) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_audio_config(ref byte value)
        {
            if (shake_read_audio_config(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_audio_config(byte value)
        {
            if (shake_write_audio_config(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_midi_amplitude(byte value)
        {
            if (shake_write_midi_amplitude(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_midi_note(byte value)
        {
            if (shake_write_midi_note(dev, value) == SHAKE_ERROR)
                return false;

            return true;

        }

        public bool write_midi_waveform(byte value)
        {
            if (shake_write_midi_waveform(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_shaking_config(ref byte value)
        {
            if (shake_read_shaking_config(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_config(byte value)
        {
            if (shake_write_shaking_config(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_shaking_accel_threshold(ref byte value)
        {
            if (shake_read_shaking_accel_threshold(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_accel_threshold(byte value)
        {
            if (shake_write_shaking_accel_threshold(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_shaking_holdoff_time(ref byte value)
        {
            if (shake_read_shaking_holdoff_time(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_holdoff_time(byte value)
        {
            if (shake_write_shaking_holdoff_time(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }


        public bool read_shaking_vibration_profile(ref byte value)
        {
            if (shake_read_shaking_vibration_profile(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_vibration_profile(byte value)
        {
            if (shake_write_shaking_vibration_profile(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_shaking_hpf_constant(ref byte value)
        {
            if (shake_read_shaking_hpf_constant(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_hpf_constant(byte value)
        {
            if (shake_write_shaking_hpf_constant(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_shaking_lpf_constant(ref byte value)
        {
            if (shake_read_shaking_lpf_constant(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_shaking_lpf_constant(byte value)
        {
            if (shake_write_shaking_lpf_constant(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool reset_shaking_detection()
        {
            if (shake_reset_shaking_detection(dev) == SHAKE_ERROR)
                return false;
            return true;
        }
        

        public bool write_packet_request(byte value)
        {
            if (shake_write_packet_request(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_data_request(byte value)
        {
            if (shake_write_data_request(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool upload_vib_sample(byte profile, List<int> samples)
        {
            if (shake_upload_vib_sample(dev, profile, samples.ToArray(), samples.Count >> 1) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool playvib(SHAKE_VIB_CHANNELS channel, byte profile)
        {
            if (shake_playvib(dev, (int)channel, profile) == SHAKE_ERROR)
                return false;

            return true;
        }

        public double read_temperature()
        {
            return shake_read_temperature(dev);
        }

        public bool read_battery_level(ref byte value)
        {
            if (shake_read_battery_level(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_power_status(ref byte value)
        {
            if (shake_read_power_status(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_expansion_config(ref byte value)
        {
            if (shake_read_expansion_config(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_expansion_config(byte value)
        {
            if (shake_write_expansion_config(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool shake_read_rfid_config(ref byte value)
        {
            if (shake_read_rfid_config(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_rfid_config(byte value)
        {
            if (shake_write_rfid_config(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read_rfid_scan_freq(ref byte value)
        {
            if (shake_read_rfid_scan_freq(dev, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write_rfid_scan_freq(byte value)
        {
            if (shake_write_rfid_scan_freq(dev, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool read(int addr, ref byte value)
        {
            if (shake_read(dev, addr, out value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public bool write(int addr, byte value)
        {
            if (shake_write(dev, addr, value) == SHAKE_ERROR)
                return false;

            return true;
        }

        public double info_firmware_revision
        {
            get
            {
                return shake_info_firmware_revision(dev);
            }
        }

        public double info_hardware_revision
        {
            get
            {
                return shake_info_hardware_revision(dev);
            }
        }

        public string info_serial_number
        {
            get
            {
                return shake_info_serial_number(dev);
            }
        }

        public string info_module_slot1
        {

            get
            {
                return shake_info_module_slot1(dev);
            }
        }

        public string info_module_slot2
        {
            get
            {
                return shake_info_module_slot2(dev);
            }
        }

        public bool info_retrieve()
        {
            if (shake_info_retrieve(dev) == SHAKE_ERROR)
                return false;

            return true;
        }

        // Initialisation functions
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_init_device")]
        private static extern IntPtr shake_init_device(int com_port, int device_type);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_init_device_rfcomm_i64")]
        private static extern IntPtr shake_init_device_rfcomm_i64(Int64 btaddr, int device_type);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_init_device_rfcomm_str")]
        private static extern IntPtr shake_init_device_rfcomm_str(string btaddr, int device_type);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_free_device")]
        private static extern int shake_free_device(IntPtr dev);

        // Information functions
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_firmware_revision")]
        private static extern double shake_info_firmware_revision(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_hardware_revision")]
        private static extern double shake_info_hardware_revision(IntPtr dev);

        // TODO sort this on CE
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_serial_number")]
        private static extern string shake_info_serial_number(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_module_slot1")]
        private static extern string shake_info_module_slot1(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_module_slot2")]
        private static extern string shake_info_module_slot2(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_info_retrieve")]
        private static extern int shake_info_retrieve(IntPtr dev);

        // Data access functions
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_accx")]
        private static extern int shake_accx(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_accy")]
        private static extern int shake_accy(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_accz")]
        private static extern int shake_accz(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_acc")]
        private static extern int shake_acc(IntPtr dev, int[] xyz);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_gyrx")]
        private static extern int shake_gyrx(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_gyry")]
        private static extern int shake_gyry(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_gyrz")]
        private static extern int shake_gyrz(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_gyr")]
        private static extern int shake_gyr(IntPtr dev, int[] xyz);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_magx")]
        private static extern int shake_magx(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_magy")]
        private static extern int shake_magy(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_magz")]
        private static extern int shake_magz(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_mag")]
        private static extern int shake_mag(IntPtr dev, int[] xyz);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_heading")]
        private static extern int shake_heading(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_cap0")]
        private static extern int shake_cap0(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_cap1")]
        private static extern int shake_cap1(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_cap")]
        private static extern int shake_cap(IntPtr dev, int[] proxvel);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_analog0")]
        private static extern int shake_analog0(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_analog1")]
        private static extern int shake_analog1(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_analog")]
        private static extern int shake_analog(IntPtr dev, int[] a0a1);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_shaking_peakaccel")]
        private static extern int shake_shaking_peakaccel(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_shaking_direction")]
        private static extern int shake_shaking_direction(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_shaking_timestamp")]
        private static extern int shake_shaking_timestamp(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_data_timestamp")]
        private static extern int shake_data_timestamp(IntPtr dev, int sensor_id);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_heart_rate")]
        private static extern int shake_heart_rate(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_rfid_tid")]
        private static extern string shake_rfid_tid(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_rfid_scan")]
        private static extern int shake_rfid_scan(IntPtr dev);


        //[DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_register_event_callback", 
        //    CallingConvention = CallingConvention.Cdecl)]
        //private static extern int shake_register_event_callback(IntPtr dev, SHAKEEventCB callback);

		[DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_register_event_callback_STDCALL")]
        private static extern int shake_register_event_callback_STDCALL(IntPtr dev, SHAKEEventCB callback);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_register_audio_callback_STDCALL")]
        private static extern int shake_register_audio_callback_STDCALL(IntPtr dev, SHAKEAudioCB callback);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_upload_audio_sample")]
        private static extern int shake_upload_audio_sample(IntPtr dev, int address, short[] sample_data);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_play_audio_sample")]
        private static extern int shake_play_audio_sample(IntPtr dev, int start_address, int end_address, int amplitude);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_exp_upload_vib_sample")]
        private static extern int shake_exp_upload_vib_sample(IntPtr dev, int address, byte[] sample_data);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_exp_play_vib_sample")]
        private static extern int shake_exp_play_vib_sample(IntPtr dev, int start_address, int end_address, int amplitude);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_exp_write_gp_register")]
        private static extern int shake_exp_write_gp_register(IntPtr dev, int reg_number, int value);


        // Data logging functions
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logged_packets_count")]
        private static extern int shake_logged_packets_count(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_play", CharSet=CharSet.Ansi)]
        private static extern int shake_logging_play(IntPtr dev, string filename);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_pause")]
        private static extern int shake_logging_pause(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_stop")]
        private static extern int shake_logging_stop(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_record")]
        private static extern int shake_logging_record(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_reset")]
        private static extern int shake_logging_reset(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_status")]
        private static extern int shake_logging_status(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_memory_status")]
        private static extern int shake_logging_memory_status(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_packet_count")]
        private static extern int shake_logging_packet_count(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_logging_packets_read")]
        private static extern int shake_logging_packets_read(IntPtr dev);

        // Register functions
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_power_state")]
        private static extern int shake_read_power_state(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_power_state")]
        private static extern int shake_write_power_state(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_power_state_2")]
        private static extern int shake_read_power_state_2(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_power_state_2")]
        private static extern int shake_write_power_state_2(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_data_format")]
        private static extern int shake_read_data_format(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_data_format")]
        private static extern int shake_write_data_format(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_acc_config")]
        private static extern int shake_read_acc_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_acc_config")]
        private static extern int shake_write_acc_config(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_sample_rate")]
        private static extern int shake_read_sample_rate(IntPtr dev, int sensor_id, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_sample_rate")]
        private static extern int shake_write_sample_rate(IntPtr dev, int sensor_id, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_sample_rate")]
        private static extern int shake_read_digital_filter(IntPtr dev, int sensor_id, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_sample_rate")]
        private static extern int shake_write_digital_filter(IntPtr dev, int sensor_id, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_calib_bypass")]
        private static extern int shake_read_calib_bypass(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_calib_bypass")]
        private static extern int shake_write_calib_bypass(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cx_bypass")]
        private static extern int shake_read_cx_bypass(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cx_bypass")]
        private static extern int shake_write_cx_bypass(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_temp_compensation")]
        private static extern int shake_read_temp_compensation(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_temp_compensation")]
        private static extern int shake_write_temp_compenstation(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_packet_streaming")]
        private static extern int shake_read_packet_streaming(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_packet_streaming")]
        private static extern int shake_write_packet_streaming(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_audio_config")]
        private static extern int shake_read_audio_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_audio_config")]
        private static extern int shake_write_audio_config(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_midi_amplitude")]
        private static extern int shake_write_midi_amplitude(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_midi_note")]
        private static extern int shake_write_midi_note(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_midi_waveform")]
        private static extern int shake_write_midi_waveform(IntPtr dev, byte value);


        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs0_inc")]
        private static extern int shake_read_cs0_inc(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs0_inc")]
        private static extern int shake_write_cs0_inc(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs0_dec")]
        private static extern int shake_read_cs0_dec(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs0_dec")]
        private static extern int shake_write_cs0_dec(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs0_inc_profile")]
        private static extern int shake_read_cs0_inc_profile(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs0_inc_profile")]
        private static extern int shake_write_cs0_inc_profile(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs0_dec_profile")]
        private static extern int shake_read_cs0_dec_profile(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs0_dec_profile")]
        private static extern int shake_write_cs0_dec_profile(IntPtr dev, byte value);


        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs1_inc")]
        private static extern int shake_read_cs1_inc(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs1_inc")]
        private static extern int shake_write_cs1_inc(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs1_dec")]
        private static extern int shake_read_cs1_dec(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs1_dec")]
        private static extern int shake_write_cs1_dec(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs1_inc_profile")]
        private static extern int shake_read_cs1_inc_profile(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs1_inc_profile")]
        private static extern int shake_write_cs1_inc_profile(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_cs1_dec_profile")]
        private static extern int shake_read_cs1_dec_profile(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_cs1_dec_profile")]
        private static extern int shake_write_cs1_dec_profile(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_config")]
        private static extern int shake_read_shaking_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_config")]
        private static extern int shake_write_shaking_config(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_accel_threshold")]
        private static extern int shake_read_shaking_accel_threshold(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_accel_threshold")]
        private static extern int shake_write_shaking_accel_threshold(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_holdoff_time")]
        private static extern int shake_read_shaking_holdoff_time(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_holdoff_time")]
        private static extern int shake_write_shaking_holdoff_time(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_vibration_profile")]
        private static extern int shake_read_shaking_vibration_profile(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_vibration_profile")]
        private static extern int shake_write_shaking_vibration_profile(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_hpf_constant")]
        private static extern int shake_read_shaking_hpf_constant(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_hpf_constant")]
        private static extern int shake_write_shaking_hpf_constant(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_shaking_lpf_constant")]
        private static extern int shake_read_shaking_lpf_constant(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_shaking_lpf_constant")]
        private static extern int shake_write_shaking_lpf_constant(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_reset_shaking_detection")]
        private static extern int shake_reset_shaking_detection(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_heart_rate_config")]
        private static extern int shake_read_heart_rate_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_heart_rate_config")]
        private static extern int shake_write_heart_rate_config(IntPtr dev, byte value);

        
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_packet_request")]
        private static extern int shake_write_packet_request(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_data_request")]
        private static extern int shake_write_data_request(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_playvib")]
        private static extern int shake_playvib(IntPtr dev, int channel, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_upload_vib_sample")]
        private static extern int shake_upload_vib_sample(IntPtr dev, byte profile, int[] samples, int sample_len);
    
        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_battery_level")]
        private static extern int shake_read_battery_level(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_power_status")]
        private static extern int shake_read_power_status(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_temperature")]
        private static extern float shake_read_temperature(IntPtr dev);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_expansion_config")]
        private static extern int shake_read_expansion_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_expansion_config")]
        private static extern int shake_write_expansion_config(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_rfid_config")]
        private static extern int shake_read_rfid_config(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_rfid_config")]
        private static extern int shake_write_rfid_config(IntPtr dev, byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read_rfid_scan_freq")]
        private static extern int shake_read_rfid_scan_freq(IntPtr dev, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write_rfid_scan_freq")]
        private static extern int shake_write_rfid_scan_freq(IntPtr dev, byte value);


        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_read")]
        private static extern int shake_read(IntPtr dev, int addr, out byte value);

        [DllImport("shake_driver.dll", SetLastError = true, EntryPoint = "shake_write")]
        private static extern int shake_write(IntPtr dev, int addr, byte value);

    }
}
