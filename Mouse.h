//
// Created by maxmati on 4/18/15.
//

#pragma once

#include <libusb.h>

#include <map>

static const int A4TECH_VID = 0x09da;

static const int BLOODY_V5_PID = 0x172A;
static const int BLOODY_V7_PID = 0xF613;
static const int BLOODY_V8_PID = 0x11F5;
static const int BLOODY_R7_PID = 0x1485;
static const int BLOODY_R8_1_PID = 0x14ee;
static const int BLOODY_R3_PID = 0x1a5a;
static const int BLOODY_RT5_PID = 0x7f1b;
static const int BLOODY_RT5_NEW_PID = 0x10d0;
static const int BLOODY_AL9_PID = 0xf633;
static const int BLOODY_R70_PID = 0xf643;
static const int BLOODY_A7_PID = 0x7e36;
static const int BLOODY_A9_PID = 0x1003;

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
    BLOODY_V5_PID,   BLOODY_V7_PID, BLOODY_V8_PID,  BLOODY_R7_PID,
    BLOODY_R8_1_PID, BLOODY_R3_PID, BLOODY_AL9_PID, BLOODY_R70_PID,
    BLOODY_A7_PID,   BLOODY_A9_PID, BLOODY_RT5_PID, BLOODY_RT5_NEW_PID,
    BLOODY_A9_PID};
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

static const uint16_t SUPP_MATRIX[] = {
    0x184, 0x313,  0x4A8,  0x62E, 0x7B6, 0x937,  0x0ABD, 0x0C51, 0x0FA0,
    0x0FA, 0x1F4,  0x2EE,  0x3E8, 0x4E2, 0x5DC,  0x6D6,  0x7D0,  0x320,
    0x640, 0x0C80, 0x0FA0, 0x3E8, 0x0FA, 0x1F4,  0x2EE,  0x3E8,  0x4E2,
    0x5DC, 0x6D6,  0x7D0,  0x8CA, 0x9C4, 0x0ABE, 0x0BB8, 0x0CB2, 0x0DAC,
    0x05,  0x00,   0x00,   0x04,  0x06,  0x09,   0x0B,   0x0D,   0x0F,
    0x12,  0x14,   0x16,   0x19,  0x1B,  0x1D,   0x20,   0x22,   0x24,
    0x27,  0x29,   0x2B,   0x2E,  0x30,  0x32,   0x34,   0x37,   0x39,
    0x3B,  0x3E,   0x40,   0x42,  0x45,  0x47,   0x49,   0x4C,   0x4E,
    0x50,  0x53,   0x55,   0x57,  0x5A,  0x5C,   0x5E,   0x61,   0x63,
    0x65,  0x68,   0x6A,   0x6C,  0x6F,  0x71,   0x73,   0x00,   0x00,
    0x04,  0x06,   0x09,   0x0B,  0x0E,  0x10,   0x12,   0x15,   0x17,
    0x19,  0x1B,   0x1D,   0x20,  0x22,  0x24,   0x27,   0x29,   0x2B,
    0x2E,  0x30,   0x32,   0x34,  0x37,  0x39,   0x3B,   0x3E,   0x40,
    0x42,  0x45,   0x48,   0x4A,  0x4D,  0x4E,   0x51,   0x54,   0x55,
    0x58,  0x5B,   0x5D,   0x5F,  0x62,  0x64,   0x66,   0x69,   0x6B,
    0x6D,  0x70,   0x72,   0x74,  0x77,  0x79,   0x7B,   0x7E,   0x80,
    0x82,  0x85,   0x87,   0x89,  0x8C,  0x8E,   0x91};

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
};

struct runData {
  int device;
  int backlight_level;
  int8_t sens_slot;
  uint16_t sens_x;
  uint16_t sens_y;
};
