#include <unistd.h>

#include <iostream>

#include "Mouse.h"

using std::cout;
using std::endl;
using std::cerr;

struct runData {
  int device;
  int backlight_level;
  int8_t sens_slot;
  uint16_t sens_x;
  uint16_t sens_y;
};

void exitA(std::string const &);
void usage();

extern char *optarg;

void list_devices(Mouse* m)
{
  auto list = m->listDevices();

  for (auto& device: list) {
    cout << "Device " << device.second << " (address " << device.first << ")" << endl;
  }
}

int find_default_device(Mouse* m)
{
  auto list = m->listDevices();

  if (list.size() == 0) {
    cout << "No supported devices found!" << endl;
    return -1;
  }

  cout << "No device provided, using " << list[0].second << " (address " << list[0].first << ") as default" << endl;

  return list[0].first;
}

void print_backlight(Mouse* m)
{
  cout << "Current backlight level: " << static_cast<int>(m->getBackLightLevel()) << endl;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    usage();
    return -1;
  }

  int opt = 0, tmp;
  runData run{-1, -1, 0, 0, 0};

  bool help = false, get_backlight = false;

  Mouse m;

  m.init();

  while ((opt = getopt(argc, argv, "hd:lbB:S:x:y:wWr")) != -1) {
    switch (opt) {
      case 'd':
        run.device = strtoul(optarg, 0l, 10);
        break;

      case 'l':
        list_devices(&m);
        return 0;
        break;

      case 'r':
        m.rebootMouse();
        return 0;
        break;

      case 'b':
        get_backlight = true;
        break;

      case 'w':
        m.enableWheel(true);
        return 0;
        break;

      case 'W':
        m.enableWheel(false);
        return 0;
        break;

      case 'B':
        run.backlight_level = strtoul(optarg, 0l, 10);
        if (run.backlight_level < 0 || run.backlight_level > 3) help = true;
        break;

      case 'S':
        run.sens_slot = strtoul(optarg, 0l, 10);
        if (run.sens_slot < 0 || run.sens_slot > 5) help = true;
        break;

      case 'x':
        run.sens_x = strtoul(optarg, 0l, 10);
        if (run.sens_x < 0) help = true;
        break;

      case 'y':
        run.sens_y = strtoul(optarg, 0l, 10);
        if (run.sens_y < 0) help = true;
        break;

      case 'h':
        help = true;
        break;

      default:
        help = true;
        break;
    }
  }

  if (help) {
    usage();
    return 0;
  }

  if (run.device < 0) {
    run.device = find_default_device(&m);
  } 

  if (run.device < 0) {
    exitA("No device specified and auto-detection failed. Exiting.");
  }

  if (!m.selectDevice(run.device)) 
    exitA("No such device");

  if (get_backlight) {
    print_backlight(&m);
  }

  if (run.backlight_level >= 0) {
    m.setBackLightLevel(run.backlight_level);
    print_backlight(&m);
  }

  if (run.sens_slot >= 0 && run.sens_x && run.sens_y)
    m.setSensitivity(run.sens_slot, run.sens_x, run.sens_y);

  return 0;
}

void exitA(std::string const &str) {
  cerr << str << endl;
  exit(-1);
}

void usage() {
  cout << "Possible parameters:\n"
               "  -h        this help message\n"
               "  -r        reset mouse\n"
               "  -l        list devices\n"
               "  -d num    specify device address\n"
               "  -b        get backlight level\n"
               "  -B lvl    set baclklight level [0-3]\n"
               "  -W        disable mouse wheel\n"
               "  -w        enable mouse wheel\n"
               "  -S slot   set sensitivity for a slot, followed by -x and -y "
               "params\n"
               "    -x num    x sensitivity\n"
               "    -y num    y sensitivity" << endl;
}
