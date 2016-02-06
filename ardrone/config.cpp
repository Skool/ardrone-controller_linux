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

#include "ardrone.h"

// --------------------------------------------------------------------------
// ARDrone::getConfig()
// Description  : Parse a configuration string.
// Return value : NONE
// --------------------------------------------------------------------------
void parse(const char *str, ARDRONE_CONFIG *config)
{
     // Split key and value
    char category[256] = {'\0'}, key[256] = {'\0'}, val[256] = {'\0'};
    
	sscanf(str, "%[^:]:%s = %[^\n]", category, key, val);
    
    // Parse the values
    if (!(strcmp(category, "general"))) 
	{
        if      (!strcmp(key, "num_version_config")) sscanf(val, "%d", &(config->general.num_version_config));
        else if (!strcmp(key, "num_version_mb"))     sscanf(val, "%d", &(config->general.num_version_mb));
        else if (!strcmp(key, "num_version_soft"))   strncpy(config->general.num_version_soft, val, 32);
        else if (!strcmp(key, "drone_serial"))       strncpy(config->general.drone_serial, val, 32);
        else if (!strcmp(key, "soft_build_date"))    strncpy(config->general.soft_build_date, val, 32);
        else if (!strcmp(key, "motor1_soft"))        sscanf(val, "%f", &(config->general.motor1_soft));
        else if (!strcmp(key, "motor1_hard"))        sscanf(val, "%f", &(config->general.motor1_hard));
        else if (!strcmp(key, "motor1_supplier"))    sscanf(val, "%f", &(config->general.motor1_supplier));
        else if (!strcmp(key, "motor2_soft"))        sscanf(val, "%f", &(config->general.motor2_soft));
        else if (!strcmp(key, "motor2_hard"))        sscanf(val, "%f", &(config->general.motor2_hard));
        else if (!strcmp(key, "motor2_supplier"))    sscanf(val, "%f", &(config->general.motor2_supplier));
        else if (!strcmp(key, "motor3_soft"))        sscanf(val, "%f", &(config->general.motor3_soft));
        else if (!strcmp(key, "motor3_hard"))        sscanf(val, "%f", &(config->general.motor3_hard));
        else if (!strcmp(key, "motor3_supplier"))    sscanf(val, "%f", &(config->general.motor3_supplier));
        else if (!strcmp(key, "motor4_soft"))        sscanf(val, "%f", &(config->general.motor4_soft));
        else if (!strcmp(key, "motor4_hard"))        sscanf(val, "%f", &(config->general.motor4_hard));
        else if (!strcmp(key, "motor4_supplier"))    sscanf(val, "%f", &(config->general.motor4_supplier));
        else if (!strcmp(key, "ardrone_name"))       strncpy(config->general.ardrone_name, val, 32);
        else if (!strcmp(key, "flying_time"))        sscanf(val, "%d", &(config->general.flying_time));
        else if (!strcmp(key, "navdata_demo"))       config->general.navdata_demo = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "com_watchdog"))       sscanf(val, "%d", &(config->general.com_watchdog));
        else if (!strcmp(key, "video_enable"))       config->general.video_enable = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "vision_enable"))      config->general.vision_enable = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "vbat_min"))           sscanf(val, "%d", &(config->general.vbat_min));
        else if (!strcmp(key, "localtime"))          sscanf(val, "%d", &(config->general.localtime));
        else if (!strcmp(key, "navdata_options"))    sscanf(val, "%d", &(config->general.navdata_options));
        else if (!strcmp(key, "gps_soft"))           sscanf(val, "%f", &(config->general.motor1_soft));
        else if (!strcmp(key, "gps_hard"))           sscanf(val, "%f", &(config->general.motor1_hard));
        else if (!strcmp(key, "localtime_zone"))     strncpy(config->general.localtime_zone, val, 32);
        else if (!strcmp(key, "timezone"))           strncpy(config->general.timezone, val, 32);
        else if (!strcmp(key, "battery_type"))       sscanf(val, "%d", &(config->general.battery_type));
    }
    else if (!(strcmp(category, "control"))) 
	{
        if      (!strcmp(key, "accs_offset"))             sscanf(val, "{ %f %f %f }", &(config->control.accs_offset[0]), &(config->control.accs_offset[1]), &(config->control.accs_offset[2]));
        else if (!strcmp(key, "accs_gains"))              sscanf(val, "{ %f %f %f %f %f %f %f %f %f }", &(config->control.accs_gains[0]), &(config->control.accs_gains[1]), &(config->control.accs_gains[2]), &(config->control.accs_gains[3]), &(config->control.accs_gains[4]), &(config->control.accs_gains[5]), &(config->control.accs_gains[6]), &(config->control.accs_gains[7]), &(config->control.accs_gains[8]));
        else if (!strcmp(key, "gyros_offset"))            sscanf(val, "{ %f %f %f }", &(config->control.gyros_offset[0]), &(config->control.gyros_offset[1]), &(config->control.gyros_offset[2]));
        else if (!strcmp(key, "gyros_gains"))             sscanf(val, "{ %f %f %f }", &(config->control.gyros_gains[0]), &(config->control.gyros_gains[1]), &(config->control.gyros_gains[2]));
        else if (!strcmp(key, "gyros110_offset"))         sscanf(val, "{ %f %f }", &(config->control.gyros110_offset[0]), &(config->control.gyros110_offset[1]));
        else if (!strcmp(key, "gyros110_gains"))          sscanf(val, "{ %f %f }", &(config->control.gyros110_gains[0]), &(config->control.gyros110_gains[1]));
        else if (!strcmp(key, "magneto_offset"))          sscanf(val, "{ %f %f %f }", &(config->control.magneto_offset[0]), &(config->control.magneto_offset[1]), &(config->control.magneto_offset[2]));
        else if (!strcmp(key, "magneto_radius"))          sscanf(val, "%f", &(config->control.magneto_radius));
        else if (!strcmp(key, "gyro_offset_thr_x"))       sscanf(val, "%f", &(config->control.gyro_offset_thr_x));
        else if (!strcmp(key, "gyro_offset_thr_y"))       sscanf(val, "%f", &(config->control.gyro_offset_thr_y));
        else if (!strcmp(key, "gyro_offset_thr_z"))       sscanf(val, "%f", &(config->control.gyro_offset_thr_z));
        else if (!strcmp(key, "pwm_ref_gyros"))           sscanf(val, "%d", &(config->control.pwm_ref_gyros));
        else if (!strcmp(key, "osctun_value"))            sscanf(val, "%d", &(config->control.osctun_value));
        else if (!strcmp(key, "osctun_test"))             config->control.osctun_test = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "altitude_max"))            sscanf(val, "%d", &(config->control.altitude_max));
        else if (!strcmp(key, "altitude_min"))            sscanf(val, "%d", &(config->control.altitude_min));
        else if (!strcmp(key, "outdoor"))                 config->control.outdoor = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "flight_without_shell"))    config->control.flight_without_shell = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "autonomous_flight"))       config->control.autonomous_flight = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "flight_anim"))             sscanf(val, "%d,%d", &(config->control.flight_anim[0]), &(config->control.flight_anim[1]));
        else if (!strcmp(key, "control_level"))           sscanf(val, "%d", &(config->control.control_level));
        else if (!strcmp(key, "euler_angle_max"))         sscanf(val, "%f", &(config->control.euler_angle_max));
        else if (!strcmp(key, "control_iphone_tilt"))     sscanf(val, "%f", &(config->control.control_iphone_tilt));
        else if (!strcmp(key, "control_vz_max"))          sscanf(val, "%f", &(config->control.control_vz_max));
        else if (!strcmp(key, "control_yaw"))             sscanf(val, "%f", &(config->control.control_yaw));
        else if (!strcmp(key, "manual_trim"))             config->control.manual_trim = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "indoor_euler_angle_max"))  sscanf(val, "%f", &(config->control.indoor_euler_angle_max));
        else if (!strcmp(key, "indoor_control_vz_max"))   sscanf(val, "%f", &(config->control.indoor_control_vz_max));
        else if (!strcmp(key, "indoor_control_yaw"))      sscanf(val, "%f", &(config->control.indoor_control_yaw));
        else if (!strcmp(key, "outdoor_euler_angle_max")) sscanf(val, "%f", &(config->control.outdoor_euler_angle_max));
        else if (!strcmp(key, "outdoor_control_vz_max"))  sscanf(val, "%f", &(config->control.outdoor_control_vz_max));
        else if (!strcmp(key, "outdoor_control_yaw"))     sscanf(val, "%f", &(config->control.outdoor_control_yaw));
        else if (!strcmp(key, "flying_mode"))             sscanf(val, "%d", &(config->control.flying_mode));
        else if (!strcmp(key, "hovering_range"))          sscanf(val, "%d", &(config->control.hovering_range));
        else if (!strcmp(key, "flying_camera_mode"))      sscanf(val, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &(config->control.flying_camera_mode[0]), &(config->control.flying_camera_mode[1]), &(config->control.flying_camera_mode[2]), &(config->control.flying_camera_mode[3]), &(config->control.flying_camera_mode[4]), &(config->control.flying_camera_mode[5]), &(config->control.flying_camera_mode[6]), &(config->control.flying_camera_mode[7]), &(config->control.flying_camera_mode[8]), &(config->control.flying_camera_mode[9]));
        else if (!strcmp(key, "flying_camera_enable"))    config->control.flying_camera_enable = (!strcmp(val, "TRUE")) ? true : false;
    }
    else if (!(strcmp(category, "network"))) 
	{
        if      (!strcmp(key, "ssid_single_player")) strncpy(config->network.ssid_single_player, val, 32);
        else if (!strcmp(key, "ssid_multi_player"))  strncpy(config->network.ssid_multi_player, val, 32);
        else if (!strcmp(key, "wifi_mode"))          sscanf(val, "%d", &(config->network.wifi_mode));
        else if (!strcmp(key, "wifi_rate"))          sscanf(val, "%d", &(config->network.wifi_rate));
        else if (!strcmp(key, "owner_mac"))          strncpy(config->network.owner_mac, val, 18);
    }
    else if (!(strcmp(category, "pic"))) 
	{
        if      (!strcmp(key, "ultrasound_freq"))     sscanf(val, "%d", &(config->pic.ultrasound_freq));
        else if (!strcmp(key, "ultrasound_watchdog")) sscanf(val, "%d", &(config->pic.ultrasound_watchdog));
        else if (!strcmp(key, "pic_version"))         sscanf(val, "%d", &(config->pic.pic_version));
    }
    else if (!(strcmp(category, "video"))) 
	{
        if      (!strcmp(key, "camif_fps"))           sscanf(val, "%d", &(config->video.camif_fps));
        else if (!strcmp(key, "camif_buffers"))       sscanf(val, "%d", &(config->video.camif_buffers));
        else if (!strcmp(key, "num_trackers"))        sscanf(val, "%d", &(config->video.num_trackers));
        else if (!strcmp(key, "video_storage_space")) sscanf(val, "%d", &(config->video.video_storage_space));
        else if (!strcmp(key, "video_on_usb"))        config->video.video_on_usb = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "video_file_index"))    sscanf(val, "%d", &(config->video.video_file_index));
        else if (!strcmp(key, "bitrate"))             sscanf(val, "%d", &(config->video.bitrate));
        else if (!strcmp(key, "bitrate_ctrl_mode"))   sscanf(val, "%d", &(config->video.bitrate_ctrl_mode));
        else if (!strcmp(key, "bitrate_storage"))     sscanf(val, "%d", &(config->video.bitrate_storage));
        else if (!strcmp(key, "codec_fps"))           sscanf(val, "%d", &(config->video.codec_fps));
        else if (!strcmp(key, "video_codec"))         sscanf(val, "%d", &(config->video.video_codec));
        else if (!strcmp(key, "video_slices"))        sscanf(val, "%d", &(config->video.video_slices));
        else if (!strcmp(key, "video_live_socket"))   sscanf(val, "%d", &(config->video.video_live_socket));
        else if (!strcmp(key, "max_bitrate"))         sscanf(val, "%d", &(config->video.max_bitrate));
        else if (!strcmp(key, "video_channel"))       sscanf(val, "%d", &(config->video.video_channel));
        else if (!strcmp(key, "exposure_mode"))       sscanf(val, "%d,%d,%d,%d", &(config->video.exposure_mode[0]), &(config->video.exposure_mode[1]), &(config->video.exposure_mode[2]), &(config->video.exposure_mode[3]));
        else if (!strcmp(key, "saturation_mode"))     sscanf(val, "%d", &(config->video.saturation_mode));
        else if (!strcmp(key, "whitebalance_mode"))   sscanf(val, "%d,%d", &(config->video.whitebalance_mode[0]), &(config->video.whitebalance_mode[1]));
    }
    else if (!(strcmp(category, "leds"))) 
	{
        if (!strcmp(key, "leds_anim")) sscanf(val, "%d,%d,%d", &(config->leds.leds_anim[0]), &(config->leds.leds_anim[1]), &(config->leds.leds_anim[2]));
    }
    else if (!(strcmp(category, "detect"))) 
	{
        if      (!strcmp(key, "enemy_colors"))              sscanf(val, "%d", &(config->detect.enemy_colors));
        else if (!strcmp(key, "enemy_without_shell"))       sscanf(val, "%d", &(config->detect.enemy_without_shell));
        else if (!strcmp(key, "groundstripe_colors"))       sscanf(val, "%d", &(config->detect.groundstripe_colors));
        else if (!strcmp(key, "detect_type"))               sscanf(val, "%d", &(config->detect.detect_type));
        else if (!strcmp(key, "detections_select_h"))       sscanf(val, "%d", &(config->detect.detections_select_h));
        else if (!strcmp(key, "detections_select_v_hsync")) sscanf(val, "%d", &(config->detect.detections_select_v_hsync));
        else if (!strcmp(key, "detections_select_v"))       sscanf(val, "%d", &(config->detect.detections_select_v));
    }
    else if (!(strcmp(category, "syslog"))) 
	{
        if      (!strcmp(key, "output"))   sscanf(val, "%d", &(config->syslog.output));
        else if (!strcmp(key, "max_size")) sscanf(val, "%d", &(config->syslog.max_size));
        else if (!strcmp(key, "nb_files")) sscanf(val, "%d", &(config->syslog.nb_files));
    }
    else if (!(strcmp(category, "custom"))) 
	{
        if      (!strcmp(key, "application_desc")) strncpy(config->custom.application_desc, val, 64);
        else if (!strcmp(key, "profile_desc"))     strncpy(config->custom.profile_desc, val, 64);
        else if (!strcmp(key, "session_desc"))     strncpy(config->custom.session_desc, val, 64);
        else if (!strcmp(key, "application_id"))   strncpy(config->custom.application_id, val, 8);
        else if (!strcmp(key, "profile_id"))       strncpy(config->custom.profile_id, val, 8);
        else if (!strcmp(key, "session_id"))       strncpy(config->custom.session_id, val, 8);
    }
    else if (!(strcmp(category, "userbox"))) 
	{
        if (!strcmp(key, "userbox_cmd")) sscanf(val, "%d", &(config->userbox.userbox_cmd));
    }
    else if (!(strcmp(category, "gps"))) 
	{
        if      (!strcmp(key, "latitude"))  sscanf(val, "%f", &(config->gps.latitude));
        else if (!strcmp(key, "longitude")) sscanf(val, "%f", &(config->gps.longitude));
        else if (!strcmp(key, "altitude"))  sscanf(val, "%f", &(config->gps.altitude));
        else if (!strcmp(key, "accuracy"))  sscanf(val, "%f", &(config->gps.accuracy));
    }
    else if (!(strcmp(category, "flightplan"))) 
	{
        if      (!strcmp(key, "default_validation_radius")) sscanf(val, "%f", &(config->flightplan.default_validation_radius));
        else if (!strcmp(key, "default_validation_time"))   sscanf(val, "%f", &(config->flightplan.default_validation_time));
        else if (!strcmp(key, "max_distance_from_takeoff")) sscanf(val, "%d", &(config->flightplan.max_distance_from_takeoff));
        else if (!strcmp(key, "gcs_ip"))                    sscanf(val, "%d", &(config->flightplan.gcs_ip));
        else if (!strcmp(key, "video_stop_delay"))          sscanf(val, "%d", &(config->flightplan.video_stop_delay));
        else if (!strcmp(key, "low_battery_go_home"))       config->flightplan.low_battery_go_home = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "automatic_heading"))         config->flightplan.automatic_heading = (!strcmp(val, "TRUE")) ? true : false;
        else if (!strcmp(key, "com_lost_action_delay"))     sscanf(val, "%d", &(config->flightplan.com_lost_action_delay));
        else if (!strcmp(key, "altitude_go_home"))          sscanf(val, "%d", &(config->flightplan.altitude_go_home));
        else if (!strcmp(key, "mavlink_js_roll_left"))      strncpy(config->flightplan.mavlink_js_roll_left, val, 3);
        else if (!strcmp(key, "mavlink_js_roll_right"))     strncpy(config->flightplan.mavlink_js_roll_right, val, 3);
        else if (!strcmp(key, "mavlink_js_pitch_front"))    strncpy(config->flightplan.mavlink_js_pitch_front, val, 3);
        else if (!strcmp(key, "mavlink_js_pitch_back"))     strncpy(config->flightplan.mavlink_js_pitch_back, val, 3);
        else if (!strcmp(key, "mavlink_js_yaw_left"))       strncpy(config->flightplan.mavlink_js_yaw_left, val, 3);
        else if (!strcmp(key, "mavlink_js_yaw_right"))      strncpy(config->flightplan.mavlink_js_yaw_right, val, 3);
        else if (!strcmp(key, "mavlink_js_go_up"))          strncpy(config->flightplan.mavlink_js_go_up, val, 3);
        else if (!strcmp(key, "mavlink_js_go_down"))        strncpy(config->flightplan.mavlink_js_go_down, val, 3);
        else if (!strcmp(key, "mavlink_js_inc_gains"))      strncpy(config->flightplan.mavlink_js_inc_gains, val, 3);
        else if (!strcmp(key, "mavlink_js_dec_gains"))      strncpy(config->flightplan.mavlink_js_dec_gains, val, 3);
        else if (!strcmp(key, "mavlink_js_select"))         strncpy(config->flightplan.mavlink_js_select, val, 3);
        else if (!strcmp(key, "mavlink_js_start"))          strncpy(config->flightplan.mavlink_js_start, val, 3);
    }
    else if (!(strcmp(category, "rescue"))) 
	{
        if (!strcmp(key, "rescue")) sscanf(val, "%d", &(config->rescue.rescue));
    }
}

// --------------------------------------------------------------------------
// ARDrone::getConfig()
// Description  : Get current configurations of AR.Drone.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::getConfig(void)
{
	int iRet = 1;
	
	// Open the IP address and port
    TCPSocket sockConfig;
    if (!sockConfig.open(ip, ARDRONE_CONFIG_PORT)) 
	{
        CVDRONE_ERROR("TCPSocket::open(port=%d) failed. (%s, %d)\n", ARDRONE_CONFIG_PORT, __FILE__, __LINE__);
        return 0;
    }

	  // Send requests
    UDPSocket tmpCommand;
    if(0 == tmpCommand.open(ip, ARDRONE_COMMAND_PORT))
	{
		DoLog("Failed to open temporary command port to send the config request !", MSG_ERROR);		
		return 0; // Note: Destructors will close the TCP connection
	}
    tmpCommand.sendf("AT*CTRL=%d,5,0\r", seq++);
    tmpCommand.sendf("AT*CTRL=%d,4,0\r", seq++);
    Sleep(500);
    tmpCommand.close();
   

    // Receive data
    char buf[10000] = {'\0'};
    int size = sockConfig.receive((void*)&buf, sizeof(buf));

    // Received something
    if (size > 0) 
	{        
        // Saving config.ini
        FILE *file = fopen("Data\\Log\\Config.log", "w");
        if (file) 
		{
            fprintf(file, buf);
            fclose(file);
        }
		else
		{
			iRet = 0;
		}

        // Clear config struct
        memset(&config, 0, sizeof(config));

        // Parsing configurations
        char *token = strtok(buf, "\n");
        if (token != NULL)
		{
			parse(token, &config);
		}
        
		while (token != NULL) 
		{
            token = strtok(NULL, "\n");
            if (token != NULL) 
			{
				parse(token, &config);
			}
        }
    }
   
    // Finalize
    sockConfig.close();

    return iRet;
}