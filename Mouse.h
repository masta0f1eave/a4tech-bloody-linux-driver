//
// Created by maxmati on 4/18/15.
//

#pragma once

#include <libusb.h>

#include <map>

static const int A4TECH_VID = 0x09da;

enum Pids {
  BLOODY_V3_PID      = 0xF113,
  BLOODY_V5_PID      = 0x172A,
  BLOODY_V7_PID      = 0xF613,
  BLOODY_V8_PID      = 0x11F5,
  BLOODY_V8M_PID     = 0x1094,
  BLOODY_R7_PID      = 0x1485,
  BLOODY_R8_1_PID    = 0x14ee,
  BLOODY_R8_PID      = 0x7c10,
  BLOODY_ZL5A_PID    = 0x1311,
  BLOODY_R3_PID      = 0x1a5a,
  BLOODY_RT5_PID     = 0x7f1b,
  BLOODY_RT5_NEW_PID = 0x10d0,
  BLOODY_AL9_PID     = 0xf633,
  BLOODY_R70_PID     = 0xf643,
  BLOODY_A7_PID      = 0x7e36,
  BLOODY_A9_PID      = 0x1003,
  BLOODY_A90_PID     = 0xf6e3,
};

enum Sensors {
  PIXART_3050 = 3050,
  PIXART_3212 = 3212,
  PIXART_3305 = 3305,
  PIXART_3325 = 3325,
  PIXART_3326 = 3326,
  PIXART_3327 = 3327,
  PIXART_3360 = 3360,
  PIXART_3389 = 3389,
  PIXART_9500 = 9500,
  PIXART_9800 = 9800,
};

enum Cpi
{
  CPI_3200 = 3200,
  CPI_3900 = 3900,
  CPI_4000 = 4000,
  CPI_5000 = 5000,
  CPI_6200 = 6200,
  CPI_8200 = 8200,
  CPI_10000 = 10000,
  CPI_12000 = 12000,
  CPI_16000 = 16000,
};

static const int COMPATIBLE_PIDS[] = {
    BLOODY_V3_PID,
    BLOODY_V5_PID,   
    BLOODY_V7_PID, 
    BLOODY_V8_PID,  
    BLOODY_V8M_PID,  
    BLOODY_R7_PID,
    BLOODY_R8_PID,
    BLOODY_ZL5A_PID,
    BLOODY_R8_1_PID, 
    BLOODY_R3_PID, 
    BLOODY_AL9_PID, 
    BLOODY_R70_PID,
    BLOODY_A7_PID,   
    BLOODY_A9_PID, 
    BLOODY_RT5_PID, 
    BLOODY_RT5_NEW_PID,
    BLOODY_A9_PID,
    BLOODY_A90_PID,
};

static const size_t COMPATIBLE_PIDS_SIZE =
    sizeof(COMPATIBLE_PIDS) / sizeof(COMPATIBLE_PIDS[0]);

static const int A4TECH_MAGIC = 0x07;

static const int BACKLIGHT_OPCODE = 0x11;
static const int BACKLIGHT_WRITE = 0x80;
static const int BACKLIGHT_READ = 0x00;

static const int SENSITIVITY_OPCODE = 0x0D;
static const int SENSITIVITY_MAGIC_1 = 0x04;
static const int SENSITIVITY_MAGIC_2 = 0x02;

static const int DEVICE_REPORT_OPCODE = 0x05;
static const int DEVICE_REPORT_OPCODE_2 = 0x1F;

static const int REQUEST_SIZE = 72;

// Used in 3050
static const uint16_t CPI_VALUES_1[] = {
  388, 787, 1192, 1582, 1974, 2359, 2749, 3153,
};

// CPI values from 3050 datasheet
static const uint16_t CPI_VALUES_2[] = {
  4000, 
  250, 500, 750, 1000, 1250, 1500, 1750, 2000,  
};

// Unused
static const uint16_t CPI_VALUES_3[] = {
  800, 1600, 3200, 4000,
};

// Unused
static const uint16_t CPI_VALUES_4[] = {
  1000, 
  250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500,
};

// Used in 3326
static const uint16_t SENSOR_POWER_1[] = {
  0, 0, 4, 6, 9, 11, 13, 15, 18, 20, 22, 25, 27, 29, 32, 34, 36, 39, 41, 43, 46, 48, 50, 52, 55, 57, 59, 
  62, 64, 66, 69, 71, 73, 76, 78, 80, 83, 85, 87, 90, 92, 94, 97, 99, 101, 104, 106, 108, 111, 113, 115,   
};

// Used in 3327
static const uint16_t SENSOR_POWER_2[] = {
  0, 0, 4, 6, 9, 11, 14, 16, 18, 21, 23, 25, 27, 29, 32, 34, 36, 39, 41, 43, 46, 48, 50, 52, 55, 57, 59, 
  62, 64, 66, 69, 72, 74, 77, 78, 80, 81, 84, 85, 88, 91, 93, 95, 98, 100,  102, 105, 107, 109, 112, 114,
  116, 119, 121, 123, 126, 128, 130, 133, 135, 137, 140, 142, 145
};

/*
static const uint16_t SUPP_MATRIX[] = {
    // 0
    0x184, 0x313,  0x4A8,  0x62E, 0x7B6, 0x937,  0x0ABD, 0x0C51, 

    // 8
    4000,
    250, 500, 750, 1000, 1250, 1500, 1750, 2000,  

    // 17
    800, 1600, 3200, 4000,

    // 21
    1000, 
    250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500,

    // 36
    0x05, // ??? 

    // 37
    0, 0, 4, 6, 9, 11, 13, 15, 18, 20, 22, 25, 27, 29, 32, 34, 36, 39, 41, 43, 46, 48, 50, 52, 55, 57, 59, 
    62, 64, 66, 69, 71, 73, 76, 78, 80, 83, 85, 87, 90, 92, 94, 97, 99, 101,  104, 106, 108, 111, 113, 115,   

    // 88
    0, 0, 4, 6, 9, 11, 14, 16, 18, 21, 23, 25, 27, 29, 32, 34, 36, 39, 41, 43, 46, 48, 50, 52, 55, 57, 59, 
    62, 64, 66, 69, 72, 74, 77, 78, 80, 81, 84, 85, 88, 91, 93, 95, 98, 100, 102, 105, 107, 109, 112, 114,
    116, 119, 121, 123, 126, 128, 130, 133, 135, 137, 140, 142, 145
};*/

struct DevInfo {
  uint compositeId;
  uint maxCPI;
  uint sensorType;
};

struct Coords {
  uint8_t x;
  uint8_t y;
  uint8_t power;
};

class Mouse {
 public:
  ~Mouse();
  void init();
  void listDevices();
  bool selectDevice(int address);
  int setBackLightLevel(uint8_t level);
  uint8_t getBackLightLevel();
  int setSensitivity(uint8_t slot, uint16_t x, uint16_t y);
  int getDeviceId();

 private:
  DevInfo devInfo;
  std::map<int, libusb_device_handle *> devices;
  libusb_device_handle *currentDevice = nullptr;
  libusb_context *context = nullptr;

  int writeToMouse(uint8_t data[], size_t size);
  int readFromMouse(uint8_t *request, size_t requestSize, uint8_t *response,
                    size_t responseSize);

  void discoverDevices();
  void fillDeviceInfo();

  bool isCompatibleDevice(libusb_device_descriptor &desc);

  uint8_t transformCoord(int16_t, uint16_t);
  uint8_t transformCoord_2(uint16_t, uint16_t);

  // convertCoords is a proxy function for different Sensors
  Coords convertCoords(uint16_t x, uint16_t y);
  Coords convertCoords_3050(uint16_t x, uint16_t y);
  Coords convertCoords_3212(uint16_t x, uint16_t y);
  Coords convertCoords_3305(uint16_t x, uint16_t y);
  Coords convertCoords_3325(uint16_t x, uint16_t y);
  Coords convertCoords_3326(uint16_t x, uint16_t y);
  Coords convertCoords_3327(uint16_t x, uint16_t y);
  Coords convertCoords_3360(uint16_t x, uint16_t y);
  Coords convertCoords_3389(uint16_t x, uint16_t y);
  Coords convertCoords_9500(uint16_t x, uint16_t y);
  Coords convertCoords_9800(uint16_t x, uint16_t y);

  Coords convertCoords_arith(float x, float y, float clamp, float bias, float scale, float offset = 0.0);
};

struct runData {
  int device;
  int backlight_level;
  int8_t sens_slot;
  uint16_t sens_x;
  uint16_t sens_y;
};
