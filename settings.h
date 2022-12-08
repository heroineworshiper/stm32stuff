#ifndef SETTINGS_H
#define SETTINGS_H

// Compile time configuration for flying camera which follows a target
// bluetooth
// brushless motors

#define MARCY2



// flying camera following a target
#define USE_FOLLOWER
// sonar altitude
#define USE_SONAR
#define USE_BRUSHLESS
// embedded navigation
#define USE_NAV
#define PIC_USE_MPU9150
// use compass for heading
//#define USE_MAG

// ARM oscillator Hz
#define AIR_CLOCKSPEED 168000000
// ARM oscillator Hz
#define ARM_CLOCKSPEED 168000000


// Corona / coax
#define ROTORS 4
#define SERVOS 0

// Battery sensor
#define USE_BATTERY
// Network ground station instead of USB
//#define USE_WIFI
// Bluetooth on a phone only
#define USE_BLUETOOTH
// pass bluetooth to the debug port to configure the device
//#define BLUETOOTH_PASSTHROUGH

// USB device ground station
//#define USE_USB
//#define PIC_USE_PWM

// AHRS on copter
#define PIC_USE_AHRS

// start picoc programs in flight
#define INFLIGHT_PROGRAM
// waypoint recording
#define POSITION_TRAINING
// Use buttons for recording
//#define USE_BUTTONS


// ARM board type to compile
// flight board
#define COPTER_MODE
// right camera or ground
//#define GROUND_MODE
// left camera
//#define LEFT_MODE




#endif



