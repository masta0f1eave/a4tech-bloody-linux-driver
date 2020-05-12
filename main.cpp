#include <unistd.h>

#include <iostream>

#include "Mouse.h"

void exitA(std::string const &);
void usage();

extern char *optarg;

int main(int argc, char *argv[]) {
  int opt = 0, tmp;
  runData run{-1, -1, 0, 0, 0};

  bool help = false, get_backlight = false;
  Mouse m;
  m.init();
  if (argc <= 1) return -1;
  while ((opt = getopt(argc, argv, "d:lbB:S:x:y:")) != -1) {
    switch (opt) {
      case 'd':
        run.device = strtoul(optarg, 0l, 10);
        break;

      case 'l':
        m.listDevices();
        return 0;
        break;

      case 'b':
        get_backlight = true;
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

      default:
        help = true;
        break;
    }
  }

  if (!run.device || help) {
    usage();
  }

  if (!m.selectDevice(run.device)) exitA("No such device");

  if (get_backlight) {
    std::cout << static_cast<int>(m.getBackLightLevel());
    return 0;
  }

  if (run.backlight_level >= 0) m.setBackLightLevel(run.backlight_level);

  if (run.sens_slot >= 0 && run.sens_x && run.sens_y)
    m.setSensitivity(run.sens_slot, run.sens_x, run.sens_y);

  return 0;
}

void exitA(std::string const &str) {
  std::cerr << str << "\n";
  exit(-1);
}

void usage() {
  std::cout << "Possible parameters:\n"
               "  -l        list devices\n"
               "  -d num    specify device address\n"
               "  -b        get backlight level\n"
               "  -B lvl    set baclklight level [0-3]\n"
               "  -S slot   set sensitivity for a slot, followed by -x and -y "
               "params\n"
               "    -x num    x sensitivity\n"
               "    -y num    y sensitivity";
  exit(1);
}