//
// Created by maxmati on 4/18/15.
//

#include "Mouse.h"

#include <iostream>

using std::cout;
using std::endl;

void Mouse::init() {
  int ret = libusb_init(&context);
  if (ret < 0) {
    cout << "Init Error " << ret << endl;
    return;
  }

  //    libusb_set_debug(context, LIBUSB_LOG_LEVEL_DEBUG);
  libusb_set_debug(context, LIBUSB_LOG_LEVEL_INFO);

  discoverDevices();
}

void Mouse::discoverDevices() {
  libusb_device **devs;
  ssize_t cnt = libusb_get_device_list(context, &devs);
  if (cnt < 0) {
    cout << "Get Device Error" << endl;
    return;
  }

  for (int i = 0; i < cnt; ++i) {
    libusb_device_descriptor desc;
    libusb_get_device_descriptor(devs[i], &desc);
    if (isCompatibleDevice(desc)) {
      switch (int status = libusb_open(devs[i], &currentDevice)) {
        case 0:
          break;
        case LIBUSB_ERROR_NO_MEM:
          cout << "LIBUSB_ERROR_NO_MEM" << endl;
          continue;
        case LIBUSB_ERROR_ACCESS:
          cout << "LIBUSB_ERROR_ACCESS" << endl;
          continue;
        case LIBUSB_ERROR_NO_DEVICE:
          cout << "LIBUSB_ERROR_NO_DEVICE" << endl;
          continue;
        default:
          cout << "Status: " << status << endl;
          continue;
      }

      if (libusb_kernel_driver_active(currentDevice, 2) == 1)
        if (libusb_detach_kernel_driver(currentDevice, 2) != 0) {
          libusb_close(currentDevice);
          continue;
        }

      devices.insert(std::pair<int, libusb_device_handle *>(
          libusb_get_device_address(devs[i]), currentDevice));
    }
  }
  libusb_free_device_list(devs, 1);

  if (devices.size() == 0) {
    cout << "No suitable device found." << endl;
    return;
  }

  currentDevice = devices.begin()->second;
}

bool Mouse::isCompatibleDevice(libusb_device_descriptor &desc) {
  if (desc.idVendor != A4TECH_VID) return false;

  for (size_t i = 0; i < COMPATIBLE_PIDS_SIZE; ++i)
    if (desc.idProduct == COMPATIBLE_PIDS[i]) return true;

  return false;
}

Mouse::~Mouse() {
  for (auto &dev : devices) libusb_close(dev.second);
  if (context != nullptr) libusb_exit(context);
}

int Mouse::setBackLightLevel(uint8_t level) {
  uint8_t data[72] = {
      A4TECH_MAGIC,
      BACKLIGHT_OPCODE,
      0x00,
      0x00,
      BACKLIGHT_WRITE,
      0x00,
      0x00,
      0x00,
      level,
      0x00,
  };

  if (level < 0 || level > 3) {
    return -1;
  }

  if (writeToMouse(data, sizeof(data)) < 0) return -2;

  return 0;
}

int Mouse::writeToMouse(uint8_t data[], size_t size) {
  int res = libusb_control_transfer(currentDevice, 0x21, 9, 0x0307, 2, data,
                                    size, 10000);
  switch (res) {
    case LIBUSB_ERROR_TIMEOUT:
      cout << "LIBUSB_ERROR_TIMEOUT" << endl;
      return -1;
    case LIBUSB_ERROR_PIPE:
      cout << "LIBUSB_ERROR_PIPE" << endl;
      return -1;
    case LIBUSB_ERROR_NO_DEVICE:
      cout << "LIBUSB_ERROR_NO_DEVICE" << endl;
      return -1;
    case LIBUSB_ERROR_BUSY:
      cout << "LIBUSB_ERROR_BUSY" << endl;
      return -1;
    case LIBUSB_ERROR_INVALID_PARAM:
      cout << "LIBUSB_ERROR_INVALID_PARAM" << endl;
    default:
      return 0;
  }
}

int Mouse::readFromMouse(uint8_t *request, size_t requestSize,
                         uint8_t *response, size_t responseSize) {
  if (writeToMouse(request, requestSize) < 0) {
    return -1;
  }

  int res = libusb_control_transfer(currentDevice, 0xa1, 1, 0x0307, 2, response,
                                    responseSize, 10000);
  if (res < 0) {
    cout << "Unnable to receive data" << endl;
    return -2;
  }
}

uint8_t Mouse::getBackLightLevel() {
  uint8_t request[72] = {
      A4TECH_MAGIC, BACKLIGHT_OPCODE, 0x00, 0x00, BACKLIGHT_READ, 0x00,
  };
  uint8_t response[72];

  readFromMouse(request, sizeof(request), response, sizeof(response));

  return response[8];
}

int Mouse::getDeviceId() {
  uint8_t request[REQUEST_SIZE] = {
      A4TECH_MAGIC,
      DEVICE_REPORT_OPCODE,
      0x00,
  };
  uint8_t response[REQUEST_SIZE];

  readFromMouse(request, REQUEST_SIZE, response, REQUEST_SIZE);
  int devId = response[24] + (response[9] << 8);

  std::fill_n(request, REQUEST_SIZE, 0);
  request[0] = A4TECH_MAGIC;
  request[1] = DEVICE_REPORT_OPCODE_2;
  readFromMouse(request, REQUEST_SIZE, response, REQUEST_SIZE);
  devId += (static_cast<uint16_t>(response[10]) & 0xFFFC) << 16;

  return devId;
}

int Mouse::setSensitivity(uint8_t slot, uint16_t x, uint16_t y) {
  Coords res = convertCoords(x, y);
  uint8_t data[72] = {
      A4TECH_MAGIC,
      SENSITIVITY_OPCODE,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      SENSITIVITY_MAGIC_1,
      slot,
      res.x,
      res.y,
      res.power,
      SENSITIVITY_MAGIC_2,
      0x00,
  };

  if (writeToMouse(data, sizeof(data)) < 0) return -2;

  return 0;
}

void Mouse::listDevices() {
  std::cout << "Available devices:" << endl;
  for (auto &devHand : devices) {
    libusb_device *device = libusb_get_device(devHand.second);

    libusb_device_descriptor desc;
    libusb_get_device_descriptor(device, &desc);

    std::string name;

    switch (desc.idProduct) {
      case BLOODY_V5_PID:
        name = "Bloody V5";
        break;
      case BLOODY_V7_PID:
        name = "Bloody V7";
        break;
      case BLOODY_V8_PID:
        name = "Bloody V8";
        break;
      case BLOODY_R7_PID:
        name = "Bloody R7";
        break;
      case BLOODY_R8_1_PID:
        name = "Bloody R8-1";
        break;
      case BLOODY_R3_PID:
        name = "Bloody R3";
        break;
      case BLOODY_AL9_PID:
        name = "Bloody AL9";
        break;
      case BLOODY_R70_PID:
        name = "Bloody R70";
        break;
      case BLOODY_A7_PID:
        name = "Bloody A7";
        break;
      case BLOODY_A9_PID:
        name = "Bloody A9";
        break;
      case BLOODY_RT5_PID:
        name = "Bloody RT5";
        break;
      case BLOODY_RT5_NEW_PID:
        name = "Bloody RT5 (new dongle)";
        break;
      default:
        name = "Unknown";
    }

    std::cout << devHand.first << ":" << name << endl;
  }
}

bool Mouse::selectDevice(int address) {
  if (devices.count(address) == 0) return false;

  currentDevice = devices.at(address);
  devInfo.compositeId = getDeviceId();
  fillDeviceInfo();
  return true;
}

uint8_t Mouse::transformCoord(int16_t b, uint16_t c) {
  int div = c / 0x10 >> 1;
  if (div < 0) div = (c / 0x10 & 1) + div;
  return (div + b) / (c / 0x10);
}

uint8_t Mouse::transformCoord_2(uint16_t a, uint16_t b) {
  return (a / (b / 64.0) + 0.5);
}

Coords Mouse::convertCoords(uint16_t x, uint16_t y) {
  if (devInfo.maxCPI < x) x = devInfo.maxCPI;
  if (devInfo.maxCPI < y) y = devInfo.maxCPI;

  switch (devInfo.sensorType) {
    case 0xBEA:
      return convertCoords_BEA(x, y);
    case 0xC8C:
      return convertCoords_C8C(x, y);
    case 0xCE9:
      return convertCoords_CE9(x, y);
    case 0xCFD:
      return convertCoords_CFD(x, y);
    case 0xCFE:
      return convertCoords_CFE(x, y);
    case 0xCFF:
      return convertCoords_CFF(x, y);
    case 0xD20:
      return convertCoords_D20(x, y);
    case 0xD3D:
      return convertCoords_D3D(x, y);
    case 0x25C1:
      return convertCoords_251C(x, y);
    case 0x2648:
      return convertCoords_2648(x, y);
    default:
      break;
  }
  return Coords();
}

Coords Mouse::convertCoords_BEA(uint16_t x, uint16_t y) {
  uint16_t cpiThreshold;
  if (devInfo.maxCPI >= 0xFA0)
    cpiThreshold = 0xF3C;
  else
    cpiThreshold = devInfo.maxCPI;

  if (cpiThreshold < x) x = cpiThreshold;
  if (cpiThreshold < y) y = cpiThreshold;

  const uint16_t *mPtr = SUPP_MATRIX + 9;
  for (uint8_t i = 1; i <= 8; ++i, ++mPtr)
    if (abs(x - *mPtr) <= 0x32 && abs(y - *mPtr) <= 0x32)
      return Coords{0x10, 0x10, static_cast<uint8_t>(i + 0x80)};

  mPtr = SUPP_MATRIX + 9;
  for (uint8_t i = 1; i <= 8; ++i, ++mPtr)
    if (*mPtr >= x && *mPtr >= y)
      return Coords{transformCoord(x, *mPtr), transformCoord(y, *mPtr),
                    static_cast<uint8_t>(i + 0x80)};

  return Coords{transformCoord(x, SUPP_MATRIX[0x10]),
                transformCoord(y, SUPP_MATRIX[0x10]), 0x88};
}

Coords Mouse::convertCoords_C8C(uint16_t x, uint16_t y) {
  if (static_cast<int16_t>(x) < 0x32) x = 0x32;
  if (static_cast<int16_t>(y) < 0x32) y = 0x32;

  return Coords{static_cast<uint8_t>((x + 0x13) / 0x26),
                static_cast<uint8_t>((x + 0x13) / 0x26), 0};
}

Coords Mouse::convertCoords_CE9(uint16_t x, uint16_t y) {
  const uint16_t *mPtr;

  mPtr = SUPP_MATRIX;
  for (int i = 0; i < 8; ++i, ++mPtr)
    if (*mPtr == x && *mPtr == y)
      return Coords{0x10, 0x10, static_cast<uint8_t>(i + 0x80)};

  mPtr = SUPP_MATRIX;
  for (int i = 0; i < 8; ++i, ++mPtr)
    if (*mPtr >= x && *mPtr >= y)
      return Coords{transformCoord(x, SUPP_MATRIX[0]),
                    transformCoord(y, SUPP_MATRIX[0]),
                    static_cast<uint8_t>(i + 0x80)};

  return Coords{transformCoord(x, SUPP_MATRIX[7]),
                transformCoord(y, SUPP_MATRIX[7]), 0x87};
}

Coords Mouse::convertCoords_CFD(uint16_t x, uint16_t y) {
  if (static_cast<int>(x) < 0x32) x = 0x32;
  if (static_cast<int>(y) < 0x32) y = 0x32;

  return Coords{static_cast<uint8_t>((x + 0x16) / 0x2B - 1),
                static_cast<uint8_t>((y + 0x16) / 0x2B - 1), 0};
}

Coords Mouse::convertCoords_CFE(uint16_t x, uint16_t y) {
  if (x > 0x1388)
    return Coords{transformCoord_2(x, 0x1388), transformCoord_2(y, 0x1388),
                  static_cast<uint8_t>(SUPP_MATRIX[0x57])};
  else if (x == y)
    return Coords{0x40, 0x40, static_cast<uint8_t>((x + 0x16) / 0x2B - 1)};
  else if (y > x)
    return Coords{static_cast<uint8_t>((x << 6) / y + 0.5), 0x40,
                  static_cast<uint8_t>(SUPP_MATRIX[y / 0x64 + 0x25])};
  else
    return Coords{0x40, static_cast<uint8_t>((y << 6) / x + 0.5),
                  static_cast<uint8_t>(SUPP_MATRIX[x / 0x64 + 0x25])};
}

Coords Mouse::convertCoords_CFF(uint16_t x, uint16_t y) {
  if (x > 0x1838)
    return Coords{transformCoord_2(x, 0x1838), transformCoord_2(y, 0x1838),
                  static_cast<uint8_t>(SUPP_MATRIX[0x96])};
  else if (x == y)
    return Coords{0x40, 0x40, static_cast<uint8_t>((x + 0x16) / 0x2C - 1)};
  else if (y > x)
    return Coords{static_cast<uint8_t>((x << 6) / y + 0.5), 0x40,
                  static_cast<uint8_t>(SUPP_MATRIX[y / 0x64 + 0x58])};
  else
    return Coords{0x40, static_cast<uint8_t>((y << 6) / x + 0.5),
                  static_cast<uint8_t>(SUPP_MATRIX[x / 0x64 + 0x58])};
}

Coords Mouse::convertCoords_D20(uint16_t x, uint16_t y) {
  if (static_cast<int>(x) < 0x64) x = 0x64;
  if (static_cast<int>(y) < 0x64) y = 0x64;

  return Coords{static_cast<uint8_t>((x + 0x32) / 0x64),
                static_cast<uint8_t>((y + 0x32) / 0x64), 0};
}

Coords Mouse::convertCoords_D3D(uint16_t x, uint16_t y) {
  // TODO: Fix 8 bit limit
  if (static_cast<int>(x) < 0x32) x = 0x32;
  if (static_cast<int>(y) < 0x32) y = 0x32;

  return Coords{static_cast<uint8_t>((x + 0x19) / 0x32),
                static_cast<uint8_t>((y + 0x19) / 0x32), 0};
}

Coords Mouse::convertCoords_251C(uint16_t x, uint16_t y) {
  // Probably not working because of resulting 8bit size of x and y.
  // Also there's no such sensor so far.

  if (static_cast<int>(x) < 0x32) x = 0x32;
  if (static_cast<int>(y) < 0x32) y = 0x32;

  return Coords{static_cast<uint8_t>((x + 11.25) / 22.5),
                static_cast<uint8_t>((y + 11.25) / 22.5), 0};
}

Coords Mouse::convertCoords_2648(uint16_t x, uint16_t y) {
  // Probably not working because of resulting 8bit size of x and y.
  // Also there's no such sensor so far.

  if (static_cast<int>(x) < 0x32) x = 0x32;
  if (static_cast<int>(y) < 0x32) y = 0x32;

  return Coords{static_cast<uint8_t>((x + 0x19) / 0x32),
                static_cast<uint8_t>((y + 0x19) / 0x32), 0};
}

void Mouse::fillDeviceInfo() {
  uint16_t majorId;
  if (!(devInfo.compositeId & 0xFFFC0000))
    majorId = 0x10;
  else
    majorId = devInfo.compositeId >> 0x10 & 0xFFFC;

  uint8_t minorId = devInfo.compositeId & 0xFF;

  switch (majorId) {
    case 0x49C:
      devInfo.sensorType = 3389;
      devInfo.maxCPI = 16000;
      break;

    case 0x480:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x484:
      devInfo.sensorType = 0x2D0;
      devInfo.maxCPI = 0x2EE0;
      break;

    case 0x490:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x2F0:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x2B8:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0x3E80;
      break;

    case 0x2D4:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2EE0;
      break;

    case 0x2E0:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x2B4:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0x3E80;
      break;

    case 0x2B0:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0x3E80;
      break;

    case 0x238:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x23C:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x218:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x504:
    case 0x508:
      devInfo.sensorType = 0xCFF;
      devInfo.maxCPI = 0x2710;
      break;

    case 0x500:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0x3E80;
      break;

    case 0x4D8:
      devInfo.sensorType = 0xCFE;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4E0:
      devInfo.sensorType = 0xCFE;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4E4:
      devInfo.sensorType = 0xCFE;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4CC:
      devInfo.sensorType = 0xCFF;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4BC:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4C0:
      devInfo.sensorType = 0xCFE;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4C4:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4B8:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0x3E80;
      break;

    case 0x4A0:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4A4:
      devInfo.sensorType = 0xD3D;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x1E0:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0xF4:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0xF0:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0x6C:
    case 0xE8:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0xE4:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0xA8:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0xB4:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0xE0:
    case 0x298:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x4B4:
    case 0x164:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x184:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x1B8:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x1CC:
      devInfo.sensorType = 0xCFD;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x16C:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x160:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x14C:
      switch (minorId) {
        case 0xB8:
          break;

        case 0xC:
          devInfo.sensorType = 0xCFD;
          devInfo.maxCPI = 0xFA0;
          break;

        case 0xD:
          devInfo.sensorType = 0xCFD;
          devInfo.maxCPI = 0xFA0;
          break;

        case 0x14:
          devInfo.sensorType = 0xCFD;
          devInfo.maxCPI = 0xFA0;
          break;

        default:
          devInfo.sensorType = 0xBEA;
          devInfo.maxCPI = 0xFA0;
          break;
      }
      break;

    case 0x88:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0x54:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x5C:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x60:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0xEC:
    case 0x64:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0x68:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0x40:
      switch (minorId) {
        case 0xB2:
          devInfo.sensorType = 0xBEA;
          devInfo.maxCPI = 0xFA0;
          break;

        case 0xBC:
          devInfo.sensorType = 0xBEA;
          devInfo.maxCPI = 0xFA0;
          break;

        case 0x31:
          devInfo.sensorType = 0xBEA;
          devInfo.maxCPI = 0xFA0;
          break;

        default:
          devInfo.sensorType = 0xCE9;
          devInfo.maxCPI = 0xC80;
          break;
      }
      break;

    case 0x34:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x38:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x3C:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x30:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    case 0x10:
    case 0x78:
    case 0x290:
      devInfo.sensorType = 0xCE9;
      devInfo.maxCPI = 0xC80;
      break;

    case 0x14:
      devInfo.sensorType = 0xBEA;
      devInfo.maxCPI = 0xFA0;
      break;

    case 0x1C:
      devInfo.sensorType = 0x2648;
      devInfo.maxCPI = 0x2008;
      break;

    default:
      std::cout << "Your device id " << std::hex << devInfo.compositeId
                << " is't found, please open issue, providing your "
                   "devId.\nSome functionality will not be available, until "
                   "issue is resolved.\n";
      break;
  }
}