/*
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "application.h"
#include "stdarg.h"

#include "./driver/AP_OpticalFlow.h" // ArduCopter OpticalFlow Library

PRODUCT_ID(PLATFORM_ID);
PRODUCT_VERSION(2);

#if 0
	SYSTEM_MODE(AUTOMATIC);
#else
	SYSTEM_MODE(MANUAL);
#endif

AP_OpticalFlow_ADNS3080 flowSensor;

/* This function is called once at start up ----------------------------------*/
void setup()
{
	// Configure wifi
	Particle.disconnect();

	pinMode(D7,OUTPUT);
	digitalWrite(D7,LOW);

	Serial.begin(115200);
	Serial.println("OpticalFlow library test ver 1.5");
	delay(1000);

	// flowSensor initialization
	if( flowSensor.init() == false ){
		Serial.println("Failed to initialise ADNS3080");
		while(1){
			digitalWrite(D7,HIGH);
			delay(200);
			digitalWrite(D7,LOW);
			delay(200);
		}
	}
	digitalWrite(D7,HIGH);

	flowSensor.set_orientation(AP_OPTICALFLOW_ADNS3080_PINS_FORWARD);
	flowSensor.set_field_of_view(AP_OPTICALFLOW_ADNS3080_08_FOV);

	delay(2000);
}

//
// display menu
//
void display_menu()
{
	Serial.println();
	Serial.println("please choose from the following options:");
	Serial.println("     c - display all config");
	Serial.println("     f - set frame rate");
	Serial.println("     i - display image");
	Serial.println("     I - display image continuously");
	Serial.println("     m - display motion");
	Serial.println("     r - set resolution");
	Serial.println("     s - set shutter speed");
	Serial.println("     z - clear all motion");
	Serial.println("     a - frame rate auto/manual");
	Serial.println();
}

//
// display config
//
void display_config()
{
	Serial.print("Config: ");
	Serial.print(flowSensor.read_register(ADNS3080_CONFIGURATION_BITS),BIN);
	delayMicroseconds(50);
	Serial.print(",");
	Serial.print(flowSensor.read_register(ADNS3080_EXTENDED_CONFIG),BIN);
	delayMicroseconds(50);
	Serial.println();

	// product id
	Serial.print("\tproduct id:     ");
	Serial.print(flowSensor.read_register(ADNS3080_PRODUCT_ID),HEX);
	delayMicroseconds(50);
	Serial.print(" (hex)");
	Serial.println();

	// frame rate
	Serial.print("\tframe rate:     ");
	Serial.print(flowSensor.get_frame_rate());
	if( flowSensor.get_frame_rate_auto() == true ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();

	// resolution
	Serial.print("\tresolution:     ");
	Serial.print(flowSensor.get_resolution());
	Serial.println();

	// shutter speed
	Serial.print("\tshutter speed:  ");
	Serial.print(flowSensor.get_shutter_speed());
	if( flowSensor.get_shutter_speed_auto() ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();
}

//
// set frame rate
//
void set_frame_rate()
{
	int value;

	// frame rate
	Serial.print("frame rate:     ");
	Serial.print(flowSensor.get_frame_rate());
	if( flowSensor.get_frame_rate_auto() == true ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();

	Serial.println("Choose new frame rate:");
	Serial.println("\ta) auto");
	Serial.println("\t2) 2000 f/s");
	Serial.println("\t3) 3000 f/s");
	Serial.println("\t4) 4000 f/s");
	Serial.println("\t5) 5000 f/s");
	Serial.println("\t6) 6400 f/s");
	Serial.println("\tx) exit (leave unchanged)");

	// get user input
	Serial.flush();
	while( !Serial.available() ) {
		delay(20);
	}
	value = Serial.read();

	if( value == 'a' || value == 'A')
		flowSensor.set_frame_rate_auto(true);
	if( value == '2' )
		flowSensor.set_frame_rate(2000);
	if( value == '3' )
		flowSensor.set_frame_rate(3000);
	if( value == '4' )
		flowSensor.set_frame_rate(4000);
	if( value == '5' )
		flowSensor.set_frame_rate(5000);
	if( value == '6' )
		flowSensor.set_frame_rate(6469);

	// display new frame rate
	Serial.print("frame rate:     ");
	Serial.print(flowSensor.get_frame_rate());
	if( flowSensor.get_frame_rate_auto() == true ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();
}

// display_image - captures and displays image from flowSensor flowSensor
void display_image()
{
	Serial.println("image data --------------");
	flowSensor.print_pixel_data(&Serial);
	Serial.println("-------------------------");
}

// display_image - captures and displays image from flowSensor flowSensor
void display_image_continuously()
{
	int i;
	Serial.println("press any key to return to menu");

	Serial.flush();

	while( !Serial.available() ) {
		display_image();
		i=0;
		while( i<20 && !Serial.available() ) {
			delay(100);  // give the viewer a bit of time to catchup
			i++;
		}
	}

	Serial.flush();
}

//
// set resolutiojn
//
void set_resolution()
{
	int value;
	int resolution = flowSensor.get_resolution();
	Serial.print("resolution: ");
	Serial.println(resolution);
	Serial.println("Choose new value:");
	Serial.println("    1) 1600");
	Serial.println("    4) 400");
	Serial.println("    x) exit");
	Serial.println();

	// get user input
	Serial.flush();
	while( !Serial.available() ) {
		delay(20);
	}
	value = Serial.read();

	// update resolution
	if( value == '1' ) {
		flowSensor.set_resolution(ADNS3080_RESOLUTION_1600);
	}
	if( value == '4' ) {
		flowSensor.set_resolution(ADNS3080_RESOLUTION_400);
	}

	Serial.print("new resolution: ");
	Serial.println(flowSensor.get_resolution());
}

//
// set shutter speed
//
void set_shutter_speed()
{
	int value;

	// shutter speed
	Serial.print("shutter speed:     ");
	Serial.print(flowSensor.get_shutter_speed());
	if( flowSensor.get_shutter_speed_auto() == true ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();

	Serial.println("Choose new shutter speed:");
	Serial.println("\ta) auto");
	Serial.println("\t1) 1000 clock cycles");
	Serial.println("\t2) 2000 clock cycles");
	Serial.println("\t3) 3000 clock cycles");
	Serial.println("\t4) 4000 clock cycles");
	Serial.println("\t5) 5000 clock cycles");
	Serial.println("\t6) 6000 clock cycles");
	Serial.println("\t7) 7000 clock cycles");
	Serial.println("\t8) 8000 clock cycles");
	Serial.println("\t9) 9000 clock cycles");
	Serial.println("\tx) exit (leave unchanged)");

	// get user input
	Serial.flush();
	while( !Serial.available() ) {
		delay(20);
	}
	value = Serial.read();

	if( value == 'a' || value == 'A')
		flowSensor.set_shutter_speed_auto(true);
	if( value == '1' )
		flowSensor.set_shutter_speed(1000);
	if( value == '2' )
		flowSensor.set_shutter_speed(2000);
	if( value == '3' )
		flowSensor.set_shutter_speed(3000);
	if( value == '4' )
		flowSensor.set_shutter_speed(4000);
	if( value == '5' )
		flowSensor.set_shutter_speed(5000);
	if( value == '6' )
		flowSensor.set_shutter_speed(6000);
	if( value == '7' )
		flowSensor.set_shutter_speed(7000);
	if( value == '8' )
		flowSensor.set_shutter_speed(8000);
	if( value == '9' )
		flowSensor.set_shutter_speed(9000);

	// display new shutter speed
	Serial.print("shutter speed:     ");
	Serial.print(flowSensor.get_shutter_speed());
	if( flowSensor.get_shutter_speed_auto() == true ) {
		Serial.print(" (auto)");
	}else{
		Serial.print(" (manual)");
	}
	Serial.println();
}

//
// display motion - show x,y and squal values constantly until user presses a key
//
void display_motion()
{
	boolean first_time = true;
	Serial.flush();

	// display instructions on how to exit
	Serial.println("press x to return to menu..");
	delay(1000);

	unsigned int publish_rate = 0;
	while( !Serial.available() ) {
		if( flowSensor.update() ){
			flowSensor.update_position(0,0,0,1,100);
			// check for errors
			if( flowSensor._overflow ){
				Serial.println("overflow!!");
			}
			if( (++publish_rate&0x0007)==0x0007 ){
				// x,y,squal
				Serial.print("x/dx: ");
				Serial.print(flowSensor.x,DEC);
				Serial.print("/");
				Serial.print(flowSensor.dx,DEC);
				Serial.print("\ty/dy: ");
				Serial.print(flowSensor.y,DEC);
				Serial.print("/");
				Serial.print(flowSensor.dy,DEC);
				Serial.print("\tsqual:");
				Serial.print(flowSensor.surface_quality,DEC);
				Serial.println();
			}
			first_time = false;
		}
		// short delay
		delay(5);
	}

	// flush the serial
	Serial.flush();
}

void loop()
{
	int value;

	// display menu to user
	display_menu();

	// wait for user to enter something
	while( !Serial.available() ) {
		delay(20);
	}

	// get character from user
	value = Serial.read();

	switch( value ) {

		case 'c' :
			// display all config
			display_config();
			break;

		case 'f' :
			// set frame rate
			set_frame_rate();
			break;

		case 'i' :
			// display image
			display_image();
			break;

		case 'I' :
			// display image continuously
			display_image_continuously();
			break;

		case 'm' :
			// display motion
			display_motion();
			break;

		case 'r' :
			// set resolution
			set_resolution();
			break;

		case 's' :
			// set shutter speed
			set_shutter_speed();
			break;

		case 'z' :
			// clear and reset everything
			flowSensor.clear_motion();
			break;

		default:
			Serial.println("unrecognised command");
			Serial.println();
			break;
	}
}
