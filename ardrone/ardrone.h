#ifndef __HEADER_ARDRONE_LIB__
#define __HEADER_ARDRONE_LIB__

// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
// -------------------------------------------------------------------------

// Coordinate system
//   Front of the AR.Drone is X-axis, left is Y-axis, upper is Z-axis.
//   Also front is 0.0 [rad], each axis CCW is positive.
//            X
//           +^-
//            |
//            |
//    Y <-----+ (0,0)
//            Z
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// PC : for logging
#include "../Utils.h"
#include "../AppConfig.h"

#include <wx/thread.h>

// Win32API
#include <windows.h>

// Thread
#include <process.h>

// WinSock
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")

// WinINet
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

// FFmpeg
extern "C"
{
	#include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

// OpenCV
#include <opencv2/opencv.hpp>
#pragma comment(lib, "opencv_calib3d249.lib")
#pragma comment(lib, "opencv_contrib249.lib")
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_features2d249.lib")
#pragma comment(lib, "opencv_flann249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_legacy249.lib")
#pragma comment(lib, "opencv_ml249.lib")
#pragma comment(lib, "opencv_objdetect249.lib")
#pragma comment(lib, "opencv_photo249.lib")
#pragma comment(lib, "opencv_stitching249.lib")
#pragma comment(lib, "opencv_video249.lib")
#pragma comment(lib, "opencv_videostab249.lib")

// Macro definitions
#define ARDRONE_VERSION_1           (1)             // AR.Drone 1.0
#define ARDRONE_VERSION_2           (2)             // AR.Drone 2.0
#define ARDRONE_SESSION_ID          "d2e081a3"      // SessionID
#define ARDRONE_PROFILE_ID          "be27e2e4"      // Profile ID
#define ARDRONE_APPLOCATION_ID      "d87f7e0c"      // Application ID
#define ARDRONE_VERSION_PORT        (5551)          // Port number for FTP
#define ARDRONE_NAVDATA_PORT        (5554)          // Port number for Navdata
#define ARDRONE_VIDEO_PORT          (5555)          // Port number for Video
#define ARDRONE_COMMAND_PORT        (5556)          // Port number for AT command
#define ARDRONE_CONFIG_PORT         (5559)          // Port number for configuration
#define ARDRONE_DEFAULT_ADDR        "192.168.1.1"   // Default IP address of AR.Drone
#define ARDRONE_NAVDATA_HEADER      (0x55667788)    // Header of Navdata

// Math constants
#ifndef M_PI
#define M_PI  (3.14159265358979323846264338327)
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180/M_PI)
#endif
#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180)
#endif

// State masks
typedef enum ARDRONE_STATE_MASK {
    ARDRONE_FLY_MASK            = 1 <<  0, // FLY MASK                  : (0) Ardrone is landed, (1) Ardrone is flying
    ARDRONE_VIDEO_MASK          = 1 <<  1, // VIDEO MASK                : (0) Video disable, (1) Video enable
    ARDRONE_VISION_MASK         = 1 <<  2, // VISION MASK               : (0) Vision disable, (1) Vision enable
    ARDRONE_CONTROL_MASK        = 1 <<  3, // CONTROL ALGO              : (0) Euler angles control, (1) Angular speed control
    ARDRONE_ALTITUDE_MASK       = 1 <<  4, // ALTITUDE CONTROL ALGO     : (0) Altitude control inactive (1) Altitude control active
    ARDRONE_USER_FEEDBACK_START = 1 <<  5, // USER feedback             :     Start button state 
    ARDRONE_COMMAND_MASK        = 1 <<  6, // Control command ACK       : (0) None, (1) One received
	ARDRONE_CAMERA_MASK         = 1 <<  7, // CAMERA MASK               : (0) Camera not ready, (1) Camera ready
    ARDRONE_TRAVELLING_MASK     = 1 <<  8, // Travelling mask           : (0) Disable, (1) Enable
    ARDRONE_USB_MASK            = 1 <<  9, // USB key                   : (0) Usb key not ready, (1) Usb key ready
    ARDRONE_NAVDATA_DEMO_MASK   = 1 << 10, // Navdata demo              : (0) All navdata, (1) Only navdata demo
    ARDRONE_NAVDATA_BOOTSTRAP   = 1 << 11, // Navdata bootstrap         : (0) Options sent in all or demo mode, (1) No navdata options sent
    ARDRONE_MOTORS_MASK         = 1 << 12, // Motors status             : (0) Ok, (1) Motors problem
    ARDRONE_COM_LOST_MASK       = 1 << 13, // Communication Lost        : (1) Com problem, (0) Com is ok
    ARDRONE_VBAT_LOW            = 1 << 15, // VBat low                  : (1) Too low, (0) Ok
    ARDRONE_USER_EL             = 1 << 16, // User Emergency Landing    : (1) User EL is ON, (0) User EL is OFF
    ARDRONE_TIMER_ELAPSED       = 1 << 17, // Timer elapsed             : (1) Elapsed, (0) Not elapsed
    ARDRONE_ANGLES_OUT_OF_RANGE = 1 << 19, // Angles                    : (0) Ok, (1) Out of range
    ARDRONE_ULTRASOUND_MASK     = 1 << 21, // Ultrasonic sensor         : (0) Ok, (1) Deaf
    ARDRONE_CUTOUT_MASK         = 1 << 22, // Cutout system detection   : (0) Not detected, (1) Detected
    ARDRONE_PIC_VERSION_MASK    = 1 << 23, // PIC Version number OK     : (0) A bad version number, (1) Version number is OK */
    ARDRONE_ATCODEC_THREAD_ON   = 1 << 24, // ATCodec thread ON         : (0) Thread OFF (1) thread ON
    ARDRONE_NAVDATA_THREAD_ON   = 1 << 25, // Navdata thread ON         : (0) Thread OFF (1) thread ON
    ARDRONE_VIDEO_THREAD_ON     = 1 << 26, // Video thread ON           : (0) Thread OFF (1) thread ON
    ARDRONE_ACQ_THREAD_ON       = 1 << 27, // Acquisition thread ON     : (0) Thread OFF (1) thread ON
    ARDRONE_CTRL_WATCHDOG_MASK  = 1 << 28, // CTRL watchdog             : (1) Delay in control execution (> 5ms), (0) Control is well scheduled
    ARDRONE_ADC_WATCHDOG_MASK   = 1 << 29, // ADC Watchdog              : (1) Delay in uart2 dsr (> 5ms), (0) Uart2 is good
    ARDRONE_COM_WATCHDOG_MASK   = 1 << 30, // Communication Watchdog    : (1) Com problem, (0) Com is ok
    ARDRONE_EMERGENCY_MASK      = 1 << 31  // Emergency landing         : (0) No emergency, (1) Emergency
};

// Flight animation IDs
typedef enum ARDRONE_ANIMATION_ID {
    ARDRONE_ANIM_PHI_M30_DEG = 0,
    ARDRONE_ANIM_PHI_30_DEG,
    ARDRONE_ANIM_THETA_M30_DEG,
    ARDRONE_ANIM_THETA_30_DEG,
    ARDRONE_ANIM_THETA_20DEG_YAW_200DEG,
    ARDRONE_ANIM_THETA_20DEG_YAW_M200DEG,
    ARDRONE_ANIM_TURNAROUND,
    ARDRONE_ANIM_TURNAROUND_GODOWN,
    ARDRONE_ANIM_YAW_SHAKE,
    ARDRONE_ANIM_YAW_DANCE,
    ARDRONE_ANIM_PHI_DANCE,
    ARDRONE_ANIM_THETA_DANCE,
    ARDRONE_ANIM_VZ_DANCE,
    ARDRONE_ANIM_WAVE,
    ARDRONE_ANIM_PHI_THETA_MIXED,
    ARDRONE_ANIM_DOUBLE_PHI_THETA_MIXED,
    ARDRONE_NB_ANIM_MAYDAY
};

// LED animation IDs
enum ARDRONE_LED_ANIMATION_ID {
    BLINK_GREEN_RED = 0,
    BLINK_GREEN,
    BLINK_RED,
    BLINK_ORANGE,
    SNAKE_GREEN_RED,
    FIRE,
    STANDARD,
    RED,
    GREEN,
    RED_SNAKE,
    BLANK,
    RIGHT_MISSILE,
    LEFT_MISSILE,
    DOUBLE_MISSILE,
    FRONT_LEFT_GREEN_OTHERS_RED,
    FRONT_RIGHT_GREEN_OTHERS_RED,
    REAR_RIGHT_GREEN_OTHERS_RED,
    REAR_LEFT_GREEN_OTHERS_RED,
    LEFT_GREEN_RIGHT_RED,
    LEFT_RED_RIGHT_GREEN,
    BLINK_STANDARD
};

/////////////////////////////////////
// Data options
enum NAVDATA_TAG
{
    ARDRONE_NAVDATA_DEMO_TAG            =  0,
    ARDRONE_NAVDATA_TIME_TAG            =  1,
    ARDRONE_NAVDATA_RAW_MEASURES_TAG    =  2,
    ARDRONE_NAVDATA_PHYS_MEASURES_TAG   =  3,
    ARDRONE_NAVDATA_GYROS_OFFSETS_TAG   =  4,
    ARDRONE_NAVDATA_EULER_ANGLES_TAG    =  5,
    ARDRONE_NAVDATA_REFERENCES_TAG      =  6,
    ARDRONE_NAVDATA_TRIMS_TAG           =  7,
    ARDRONE_NAVDATA_RC_REFERENCES_TAG   =  8,
    ARDRONE_NAVDATA_PWM_TAG             =  9,
    ARDRONE_NAVDATA_ALTITUDE_TAG        = 10,
    ARDRONE_NAVDATA_VISION_RAW_TAG      = 11,
    ARDRONE_NAVDATA_VISION_OF_TAG       = 12,
    ARDRONE_NAVDATA_VISION_TAG          = 13,
    ARDRONE_NAVDATA_VISION_PERF_TAG     = 14,
    ARDRONE_NAVDATA_TRACKERS_SEND_TAG   = 15,
    ARDRONE_NAVDATA_VISION_DETECT_TAG   = 16,
    ARDRONE_NAVDATA_WATCHDOG_TAG        = 17,
    ARDRONE_NAVDATA_IPHONE_ANGLES_TAG   = 18,
    ARDRONE_NAVDATA_ADC_DATA_FRAME_TAG  = 18,
    ARDRONE_NAVDATA_VIDEO_STREAM_TAG    = 19,
    ARDRONE_NAVDATA_GAME_TAG            = 20,       // AR.Drone 1.7.4
    ARDRONE_NAVDATA_PRESSURE_RAW_TAG    = 21,       // AR.Drone 2.0
    ARDRONE_NAVDATA_MAGNETO_TAG         = 22,       // AR.Drone 2.0
    ARDRONE_NAVDATA_WIND_TAG            = 23,       // AR.Drone 2.0
    ARDRONE_NAVDATA_KALMAN_PRESSURE_TAG = 24,       // AR.Drone 2.0
    ARDRONE_NAVDATA_HDVIDEO_STREAM_TAG  = 25,       // AR.Drone 2.0
    ARDRONE_NAVDATA_WIFI_TAG            = 26,       // AR.Drone 2.0
    ARDRONE_NAVDATA_ZIMMU3000_TAG       = 27,       // AR.Drone 2.0
    ARDRONE_NAVDATA_GPS_TAG             = 27,       // AR.Drone 2.4.1
    ARDRONE_NAVDATA_CKS_TAG             = 0xFFFF
};


/////////////////////////////////////
// USB recording states
enum HDVIDEO
{
	NAVDATA_HDVIDEO_STORAGE_FIFO_IS_FULL	= (1<<0),
	NAVDATA_HDVIDEO_USBKEY_IS_PRESENT		= (1<<8),
	NAVDATA_HDVIDEO_USBKEY_IS_RECORDING		= (1<<9),
	NAVDATA_HDVIDEO_USBKEY_IS_FULL			= (1<<10)
};

// TCP Class
class TCPSocket {
public:
    TCPSocket();                            // Constructor
    virtual ~TCPSocket();                   // Destructor
    int  open(const char *addr, int port);  // Initialize
    int  send2(void *data, int size);       // Send data
    int  sendf(char *str, ...);             // Send with format
    int  receive(void *data, int size);     // Receive data
    void close(void);                       // Finalize
private:
    SOCKET sock;                            // Sockets
    sockaddr_in server_addr, client_addr;   // Server/Client IP adrress
};

// UDP Class
class UDPSocket {
public:
    UDPSocket();                            // Constructor
    ~UDPSocket();                           // Destructor
    int  open(const char *addr, int port);  // Initialize
    int  send2(void *data, int size);       // Send data
    int  sendf(char *str, ...);             // Send with format
    int  receive(void *data, int size);     // Receive data
    void close(void);                       // Finalize
private:
    SOCKET sock;                            // Sockets
    sockaddr_in server_addr, client_addr;   // Server/Client IP adrress
};


// Navdata
#pragma pack(push, 1)
struct ARDRONE_NAVDATA {
    // 3x3 matrix
    struct matrix33_t { 
        float m11, m12, m13;
        float m21, m22, m23;
        float m31, m32, m33;
    };

    // 3x1 vector
    union vector31_t {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
    };

    // 2x1 vector
    union vector21_t {
        float v[2];
        struct {
            float x;
            float y;
        };
    };

    // Velocities
    struct velocities_t {
        float x;
        float y;
        float z;
    };

    // Screen point
    struct screen_point_t {
        int x;
        int y;
    };

    // Header
    unsigned int header;
    unsigned int ardrone_state;
    unsigned int sequence;
    unsigned int vision_defined;

    // Demo
    struct NAVDATA_DEMO {
        unsigned short tag;
        unsigned short size;
        unsigned int   ctrl_state;
        unsigned int   vbat_flying_percentage;
        float          theta;
        float          phi;
        float          psi;
        int            altitude;
        float          vx;
        float          vy;
        float          vz;
        unsigned int   num_frames;                // Don't use
        matrix33_t     detection_camera_rot;      // Don't use
        vector31_t     detection_camera_trans;    // Don't use
        unsigned int   detection_tag_index;       // Don't use
        unsigned int   detection_camera_type;     // Don't use
        matrix33_t     drone_camera_rot;          // Don't use
        vector31_t     drone_camera_trans;        // Don't use
    } demo;

    // Timestamp
    struct NAVDATA_TIME {
        unsigned short tag;
        unsigned short size;
        unsigned int   time;
    } time;

    // Raw measurements
    struct NAVDATA_RAW_MEASURES {
        unsigned short tag;
        unsigned short size;
        unsigned short raw_accs[3];         // filtered accelerometers
        short          raw_gyros[3];        // filtered gyrometers
        short          raw_gyros_110[2];    // gyrometers  x/y 110 deg/s
        unsigned int   vbat_raw;            // battery voltage raw (mV)
        unsigned short us_debut_echo;
        unsigned short us_fin_echo;
        unsigned short us_association_echo;
        unsigned short us_distance_echo;
        unsigned short us_courbe_temps;
        unsigned short us_courbe_valeur;
        unsigned short us_courbe_ref;
        unsigned short flag_echo_ini;
        //unsigned short frame_number;
        unsigned short nb_echo;
        unsigned int   sum_echo;
        int            alt_temp_raw;
        short          gradient;
    } raw_measures;

    // Physical measurements
    struct NAVDATA_PHYS_MEASURES {
        unsigned short tag;
        unsigned short size;
        float          accs_temp;
        unsigned short gyro_temp;
        float          phys_accs[3];
        float          phys_gyros[3];
        unsigned int   alim3V3;         // 3.3 volt alim       [LSB]
        unsigned int   vrefEpson;       // ref volt Epson gyro [LSB]
        unsigned int   vrefIDG;         // ref volt IDG gyro   [LSB]
    } phys_measures;

    // Gyros offsets
    struct NAVDATA_GYROS_OFFSETS {
        unsigned short tag;
        unsigned short size;
        float          offset_g[3];
    } gyros_offsets;

    // Euler angles
    struct NAVDATA_EULER_ANGLES {
        unsigned short tag;
        unsigned short size;
        float          theta_a;
        float          phi_a;
    } euler_angles;

    // References
    struct NAVDATA_REFERENCES {
        unsigned short tag;
        unsigned short size;
        int            ref_theta;
        int            ref_phi;
        int            ref_theta_I;
        int            ref_phi_I;
        int            ref_pitch;
        int            ref_roll;
        int            ref_yaw;
        int            ref_psi;
        float          vx_ref;
        float          vy_ref;
        float          theta_mod;
        float          phi_mod;
        float          k_v_x;
        float          k_v_y;
        unsigned int   k_mode;
        float          ui_time;
        float          ui_theta;
        float          ui_phi;
        float          ui_psi;
        float          ui_psi_accuracy;
        int            ui_seq;
    } references;

    // Trims
    struct NAVDATA_TRIMS {
        unsigned short tag;
        unsigned short size;
        float          angular_rates_trim_r;
        float          euler_angles_trim_theta;
        float          euler_angles_trim_phi;
    } trims;

    // RC references
    struct NAVDATA_RC_REFERENCES {
        unsigned short tag;
        unsigned short size;
        int            rc_ref_pitch;
        int            rc_ref_roll;
        int            rc_ref_yaw;
        int            rc_ref_gaz;
        int            rc_ref_ag;
    } rc_references;

    // PWM
    struct NAVDATA_PWM {
        unsigned short tag;
        unsigned short size;
        unsigned char  motor1;
        unsigned char  motor2;
        unsigned char  motor3;
        unsigned char  motor4;
        unsigned char  sat_motor1;
        unsigned char  sat_motor2;
        unsigned char  sat_motor3;
        unsigned char  sat_motor4;
        float          gaz_feed_forward;
        float          gaz_altitude;
        float          altitude_integral;
        float          vz_ref;
        int            u_pitch;
        int            u_roll;
        int            u_yaw;
        float          yaw_u_I;
        int            u_pitch_planif;
        int            u_roll_planif;
        int            u_yaw_planif;
        float          u_gaz_planif;
        unsigned short current_motor1;
        unsigned short current_motor2;
        unsigned short current_motor3;
        unsigned short current_motor4;
        float          altitude_prop;
        float          altitude_der;
    } pwm;

    // Altitude
    struct NAVDATA_ALTITUDE {
        unsigned short tag;
        unsigned short size;
        int            altitude_vision;
        float          altitude_vz;
        int            altitude_ref;
        int            altitude_raw;
        float          obs_accZ;
        float          obs_alt;
        vector31_t     obs_x;
        unsigned int   obs_state;
        vector21_t     est_vb;
        unsigned int   est_state;
    } altitude;

    // Vision (raw)
    struct NAVDATA_VISION_RAW {
        unsigned short tag;
        unsigned short size;
        float          vision_tx_raw;
        float          vision_ty_raw;
        float          vision_tz_raw;
    } vision_raw;

    // Vision (offset?)
    struct NAVDATA_VISION_OF {
        unsigned short tag;
        unsigned short size;
        float          of_dx[5];
        float          of_dy[5];
    } vision_of;

    // Vision
    struct NAVDATA_VISION {
        unsigned short tag;
        unsigned short size;
        unsigned int   vision_state;
        int            vision_misc;
        float          vision_phi_trim;
        float          vision_phi_ref_prop;
        float          vision_theta_trim;
        float          vision_theta_ref_prop;
        int            new_raw_picture;
        float          theta_capture;
        float          phi_capture;
        float          psi_capture;
        int            altitude_capture;
        unsigned int   time_capture;    // time in TSECDEC format (see config.h)
        velocities_t   body_v;
        float          delta_phi;
        float          delta_theta;
        float          delta_psi;
        unsigned int   gold_defined;
        unsigned int   gold_reset;
        float          gold_x;
        float          gold_y;
    } vision;

    // Vision performances
    struct NAVDATA_VISION_PERF {
        unsigned short tag;
        unsigned short size;
        float          time_szo;
        float          time_corners;
        float          time_compute;
        float          time_tracking;
        float          time_trans;
        float          time_update;
        float          time_custom[20];
    } vision_perf;

    // Trackers
    struct NAVDATA_TRACKERS_SEND {
        unsigned short tag;
        unsigned short size;
        int            locked[30];
        screen_point_t point[30];
    } trackers_send;

    // Vision detection
    struct NAVDATA_VISION_DETECT {
        unsigned short tag;
        unsigned short size;
        unsigned int   nb_detected;
        unsigned int   type[4];
        unsigned int   xc[4];
        unsigned int   yc[4];
        unsigned int   width[4];
        unsigned int   height[4];
        unsigned int   dist[4];
        float          orientation_angle[4];
        matrix33_t     rotation[4];
        vector31_t     translation[4];
        unsigned int   camera_source[4];
    } vision_detect;

    // Watchdog
    struct NAVDATA_WATCHDOG {
        unsigned short tag;
        unsigned short size;
        int            watchdog;
    } watchdog;

    // ADC data
    struct NAVDATA_ADC_DATA_FRAME {
        unsigned short tag;
        unsigned short size;
        unsigned int   version;
        unsigned char  data_frame[32];
    } adc_data_frame;

    // Video stream
    struct NAVDATA_VIDEO_STREAM {
        unsigned short tag;
        unsigned short size;
        unsigned char  quant;               // quantizer reference used to encode frame [1:31]
        unsigned int   frame_size;          // frame size (bytes)
        unsigned int   frame_number;        // frame index
        unsigned int   atcmd_ref_seq;       // atmcd ref sequence number
        unsigned int   atcmd_mean_ref_gap;  // mean time between two consecutive atcmd_ref (ms)
        float          atcmd_var_ref_gap;
        unsigned int   atcmd_ref_quality;   // estimator of atcmd link quality

        // drone2
        unsigned int   out_bitrate;         // measured out throughput from the video tcp socket
        unsigned int   desired_bitrate;     // last frame size generated by the video encoder
        int            data1;
        int            data2;
        int            data3;
        int            data4;
        int            data5;
        unsigned int   tcp_queue_level;
        unsigned int   fifo_queue_level;
    } video_stream;

    // Games
    struct NAVDATA_GAMES {
        unsigned short tag;
        unsigned short size;
        unsigned int   double_tap_counter;
        unsigned int   finish_line_counter;
    } games;

    // Preassure (raw)
    struct NAVDATA_PRESSURE_RAW {
        unsigned short tag;
        unsigned short size;
        unsigned int   up;
        unsigned short ut;
        unsigned int   temperature_meas;
        unsigned int   pression_meas;
    } pressure_raw;

    // Magneto
    struct NAVDATA_MAGNETO {
        unsigned short tag;
        unsigned short size;
        short          mx;
        short          my;
        short          mz;
        vector31_t     magneto_raw;             // magneto in the body frame, in mG
        vector31_t     magneto_rectified;
        vector31_t     magneto_offset;
        float          heading_unwrapped;
        float          heading_gyro_unwrapped;
        float          heading_fusion_unwrapped;
        char           magneto_calibration_ok;
        unsigned int   magneto_state;
        float          magneto_radius;
        float          error_mean;
        float          error_var;
        float          tmp1, tmp2;              // dummy ?
    } magneto;

    // Wind
    struct NAVDATA_WIND {
        unsigned short tag;
        unsigned short size;
        float          wind_speed;              // estimated wind speed [m/s]
        float          wind_angle;              // estimated wind direction in North-East frame [rad] e.g. if wind_angle is pi/4, wind is from South-West to North-East
        float          wind_compensation_theta;
        float          wind_compensation_phi;
        float          state_x1;
        float          state_x2;
        float          state_x3;
        float          state_x4;
        float          state_x5;
        float          state_x6;
        float          magneto_debug1;
        float          magneto_debug2;
        float          magneto_debug3;
    } wind;

    // Kalman filter
    struct NAVDATA_KALMAN_PRESSURE {
        unsigned short tag;
        unsigned short size;
        float          offset_pressure;
        float          est_z;
        float          est_zdot;
        float          est_bias_PWM;
        float          est_biais_pression;
        float          offset_US;
        float          prediction_US;
        float          cov_alt;
        float          cov_PWM;
        float          cov_vitesse;
        bool           bool_effet_sol;
        float          somme_inno;
        bool           flag_rejet_US;
        float          u_multisinus;
        float          gaz_altitude;
        bool           flag_multisinus;
        bool           flag_multisinus_debut;
    } kalman_pressure;

    // HD video stream
    struct NAVDATA_HDVIDEO_STREAM {
        unsigned short tag;
        unsigned short size;
        unsigned int   hdvideo_state;
        unsigned int   storage_fifo_nb_packets;
        unsigned int   storage_fifo_size;
        unsigned int   usbkey_size;           // USB key in kbytes - 0 if no key present
        unsigned int   usbkey_freespace;      // USB key free space in kbytes - 0 if no key present
        unsigned int   frame_number;          // 'frame_number' PaVE field of the frame starting to be encoded for the HD stream
        unsigned int   usbkey_remaining_time; // time in seconds
    } hdvideo_stream;

    // WiFi
    struct NAVDATA_WIFI {
        unsigned short tag;
        unsigned short size;
        unsigned int   link_quality;
    } wifi;

    // Zimmu 3000
    struct NAVDATA_ZIMMU_3000 {
        unsigned short tag;
        unsigned short size;
        int            vzimmuLSB;
        float          vzfind;
    } zimmu_3000;

    // GPS (for AR.Drone 2.4.1, or later)
    // From https://github.com/paparazzi/paparazzi/blob/master/sw/airborne/boards/ardrone/at_com.h
    struct NAVDATA_GPS {
        unsigned short tag;                  /*!< Navdata block ('option') identifier */
        unsigned short size;                 /*!< set this to the size of this structure */
        double         lat;                  /*!< Latitude */
        double         lon;                  /*!< Longitude */
        double         elevation;            /*!< Elevation */
        double         hdop;                 /*!< hdop */
        int            data_available;       /*!< When there is data available */
        unsigned char  unk_0[8];
        double         lat0;                 /*!< Latitude ??? */
        double         lon0;                 /*!< Longitude ??? */
        double         lat_fuse;             /*!< Latitude fused */
        double         lon_fuse;             /*!< Longitude fused */
        unsigned int   gps_state;            /*!< State of the GPS, still need to figure out */
        unsigned char  unk_1[40];
        double         vdop;                 /*!< vdop */
        double         pdop;                 /*!< pdop */
        float          speed;                /*!< speed */
        unsigned int   last_frame_timestamp; /*!< Timestamp from the last frame */
        float          degree;               /*!< Degree */
        float          degree_mag;           /*!< Degree of the magnetic */
        unsigned char  unk_2[16];
        struct {
            unsigned char sat;
            unsigned char cn0;
        } channels[12];
        int             gps_plugged;         /*!< When the gps is plugged */
        unsigned char   unk_3[108];
        double          gps_time;            /*!< The gps time of week */
        unsigned short  week;                /*!< The gps week */
        unsigned char   gps_fix;             /*!< The gps fix */
        unsigned char   num_sattelites;      /*!< Number of sattelites */
        unsigned char   unk_4[24];
        double          ned_vel_c0;          /*!< NED velocity */
        double          ned_vel_c1;          /*!< NED velocity */
        double          ned_vel_c2;          /*!< NED velocity */
        double          pos_accur_c0;        /*!< Position accuracy */
        double          pos_accur_c1;        /*!< Position accuracy */
        double          pos_accur_c2;        /*!< Position accuracy */
        float           speed_acur;          /*!< Speed accuracy */
        float           time_acur;           /*!< Time accuracy */
        unsigned char   unk_5[72];
        float           temprature;
        float           pressure;
    } gps;

    // Check sum
    struct NAVDATA_CKS {
        unsigned short tag;
        unsigned short size;
        unsigned int   cks;
    } cks;
};
#pragma pack(pop)


// Configurations
struct ARDRONE_CONFIG 
{
    struct CONFIG_GENERAL 
	{
        int   num_version_config;
        int   num_version_mb;
        char  num_version_soft[32];
        char  drone_serial[32];
        char  soft_build_date[32];
        float motor1_soft;
        float motor1_hard;
        float motor1_supplier;
        float motor2_soft;
        float motor2_hard;
        float motor2_supplier;
        float motor3_soft;
        float motor3_hard;
        float motor3_supplier;
        float motor4_soft;
        float motor4_hard;
        float motor4_supplier;
        char  ardrone_name[32];
        int   flying_time;
        bool  navdata_demo;
        int   com_watchdog;
        bool  video_enable;
        bool  vision_enable;
        int   vbat_min;
        int   localtime;
        int   navdata_options;
        float gps_soft;
        float gps_hard;
        char  localtime_zone[32];
        char  timezone[32];
        int   battery_type;
    } general;

    struct CONFIG_CONTROL 
	{
        float accs_offset[3];
        float accs_gains[9];
        float gyros_offset[3];
        float gyros_gains[3];
        float gyros110_offset[2];
        float gyros110_gains[2];
        float magneto_offset[3];
        float magneto_radius;
        float gyro_offset_thr_x;
        float gyro_offset_thr_y;
        float gyro_offset_thr_z;
        int   pwm_ref_gyros;
        int   osctun_value;
        bool  osctun_test;
        int   altitude_max;
        int   altitude_min;
        bool  outdoor;
        bool  flight_without_shell;
        bool  autonomous_flight;
        int   flight_anim[2];
        int   control_level;
        float euler_angle_max;
        float control_iphone_tilt;
        float control_vz_max;
        float control_yaw;
        bool  manual_trim;
        float indoor_euler_angle_max;
        float indoor_control_vz_max;
        float indoor_control_yaw;
        float outdoor_euler_angle_max;
        float outdoor_control_vz_max;
        float outdoor_control_yaw;
        int   flying_mode;
        int   hovering_range;
        int   flying_camera_mode[10];
        bool  flying_camera_enable;
    } control;

    struct CONFIG_NETWORK 
	{
        char ssid_single_player[32];
        char ssid_multi_player[32];
        int  wifi_mode;
        int  wifi_rate;
        char owner_mac[18];
    } network;

    struct CONFIG_PIC 
	{
        int ultrasound_freq;
        int ultrasound_watchdog;
        int pic_version;
    } pic;

    struct CONFIG_VIDEO 
	{
        int  camif_fps;
        int  camif_buffers;
        int  num_trackers;
        int  video_storage_space;
        bool video_on_usb;
        int  video_file_index;
        int  bitrate;
        int  bitrate_ctrl_mode;
        int  bitrate_storage;
        int  codec_fps;
        int  video_codec;
        int  video_slices;
        int  video_live_socket;
        int  max_bitrate;
        int  video_channel;
        int  exposure_mode[4];
        int  saturation_mode;
        int  whitebalance_mode[2];
    } video;

    struct CONFIG_LEDS 
	{
        int leds_anim[3];
    } leds;

    struct CONFIG_DETECT 
	{
        int enemy_colors;
        int enemy_without_shell;
        int groundstripe_colors;
        int detect_type;
        int detections_select_h;
        int detections_select_v_hsync;
        int detections_select_v;
    } detect;

    struct CONFIG_SYSLOG 
	{
        int output;
        int max_size;
        int nb_files;
    } syslog;

    struct CONFIG_CUSTOM 
	{
        char application_desc[64];
        char profile_desc[64];
        char session_desc[64];
        char application_id[9];
        char profile_id[9];
        char session_id[9];
    } custom;

    struct CONFIG_USERBOX 
	{
        int userbox_cmd;
    } userbox;

    struct CONFIG_GPS 
	{
        float latitude;
        float longitude;
        float altitude;
        float accuracy;
        //int ephemeris_uploaded;
    } gps;

    struct FLIGHT_PLAN 
	{
        float default_validation_radius;
        float default_validation_time;
        int   max_distance_from_takeoff;
        int   gcs_ip;
        int   video_stop_delay;
        bool  low_battery_go_home;
        bool  automatic_heading;
        int   com_lost_action_delay;
        int   altitude_go_home;
        char  mavlink_js_roll_left[3];
        char  mavlink_js_roll_right[3];
        char  mavlink_js_pitch_front[3];
        char  mavlink_js_pitch_back[3];
        char  mavlink_js_yaw_left[3];
        char  mavlink_js_yaw_right[3];
        char  mavlink_js_go_up[3];
        char  mavlink_js_go_down[3];
        char  mavlink_js_inc_gains[3];
        char  mavlink_js_dec_gains[3];
        char  mavlink_js_select[3];
        char  mavlink_js_start[3];
    } flightplan;

    struct RESCUE 
	{
        int rescue;
    } rescue;
};

// Version information
struct ARDRONE_VERSION {
    int major;
    int minor;
    int revision;
};

// AR.Drone class
class ARDrone {
public:
    // Constructor / Destructor
    ARDrone();
    virtual ~ARDrone();

    // Initialize
    int open(wxStopWatch* pWatch, const char *ardrone_addr = ARDRONE_DEFAULT_ADDR);

    // Finalize (Automatically called)
    void close(void);

    // Get an image for OpenCV
    IplImage* getImage(void);

    // Get AR.Drone's firmware version
    int getVersion(int *major = NULL, int *minor = NULL, int *revision = NULL);
	// Dump drone config to file
	int getConfig(void);

    // Get sensor values
    double getRoll(void);       // Roll angle  [rad]
    double getPitch(void);      // Pitch angle [rad]
    double getYaw(void);        // Yaw angle   [rad]
    double getAltitude(void);   // Altitude    [m]
    double getVelocity(double *vx = NULL, double *vy = NULL, double *vz = NULL); // Velocity [m/s]

    // Battery charge [%]
    unsigned int getBatteryPercentage(void);

    // Take off / Landing / Emergency
    void takeoff(void);
    void landing(void);
    void emergency(void);

    // Move with velocity [m/s]
    void move(double vx, double vy, double vr);
    void move3D(double vx, double vy, double vz, double vr);

    // Change camera channel
    void setCamera(int channel);

    // Others
    bool onGround(void);                            // Check on ground
    void setAnimation(int id, int duration);        // Flight animation
    void setLED(int id, float freq, int duration);  // LED animation
    void startVideoRecord(void);                    // Video recording for AR.Drone 2.0
    void stopVideoRecord(void);                     // You should set a USB key with > 100MB to your drone	

protected:
	void Clear();

    // IP address
    char ip[16];

    // Sequence number
    unsigned long int seq;

    // Camera image
    IplImage *img;

    // Sockets
    UDPSocket sockCommand;
    UDPSocket sockNavdata;
    UDPSocket sockVideo;	// Only used for Ar Drone 1

    // Version information
    ARDRONE_VERSION version;

    // Navigation data
    ARDRONE_NAVDATA navdata;

	// Configurations
    ARDRONE_CONFIG config;

    // Video
    AVFormatContext *pFormatCtx;
    AVCodecContext  *pCodecCtx;
    AVFrame         *pFrame, *pFrameBGR;
    uint8_t         *bufferBGR;
    SwsContext      *pConvertCtx;

    // Thread for command
    bool   bflagCommand;
	bool   bNeedCommandRestart;
    HANDLE threadCommand;
	wxCriticalSection CSCommand, CSCommandWD;
    UINT   loopCommand(void);
	long   lCommandUpdateTime;	
    static UINT WINAPI runCommand(void *args) 
	{
        return reinterpret_cast<ARDrone*>(args)->loopCommand();
    }

    // Thread for navdata
    bool   bflagNavdata;
	bool   bNeedNavdataRestart;
    HANDLE threadNavdata;
	wxCriticalSection CSNavdata, CSNavdataWD;
    UINT   loopNavdata(void);
	long   lNavdataUpdateTime;
    static UINT WINAPI runNavdata(void *args)
	{
        return reinterpret_cast<ARDrone*>(args)->loopNavdata();
    }

    // Thread for video
    bool   bflagVideo;
	bool   bNeedVideoRestart;
    HANDLE threadVideo;
	wxCriticalSection CSVideo, CSVideoWD;
    UINT   loopVideo(void);
	long   lVideoUpdateTime;
    static UINT WINAPI runVideo(void *args)
	{
        return reinterpret_cast<ARDrone*>(args)->loopVideo();
    }

	// Thread for watchdog
    bool   bflagWatchdog;
    HANDLE threadWatchdog;
	// Watch used for update time
	wxStopWatch* m_pWatch;
    UINT   loopWatchdog(void);
    static UINT WINAPI runWatchdog(void *args) 
	{
        return reinterpret_cast<ARDrone*>(args)->loopWatchdog();
    }

	// Lock and unlock functions
	void LockNavdata();
	void UnlockNavdata();
	void LockCommand();
	void UnlockCommand();
	void LockVideo();
	void UnlockVideo();

    // Initialize (internal)
    int initNavdata(void);
    int initVideo(void);
    int initCommand(void);
	bool initWatchdog(void);

    // Get informations (internal)
    int getVersionInfo(void);
	//int getConfig(void);
    int getNavdata(void);
    int getVideo(void);

    // Send commands (internal)
    void resetWatchDog(void);
    void resetEmergency(void);

    // Finalize (internal)
    void finalizeNavdata(void);
    void finalizeVideo(void);
    void finalizeCommand(void);
	void finalizeWatchdog(void);
};

// --------------------------------------------------------------------------
// CVDRONE_ERROR(Message)
// Description  : Show an error message.
// Return value : NONE
// --------------------------------------------------------------------------
CV_INLINE void CVDRONE_ERROR(const char *message, ...)
{
    char *arg;
    char str[256];

    // Apply format
    va_start(arg, message);
    vsprintf(str, message, arg);
    va_end(arg);

    // Show message box
    //MessageBox(NULL, str, "CVDRONE ERROR MESSAGE", MB_OK|MB_ICONERROR|MB_TOPMOST|MB_SETFOREGROUND);
	DoLog(wxString::Format("[CvDrone] %s", str), MSG_ERROR);
}

#endif