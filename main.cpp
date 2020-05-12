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
    std::cout << m.getBackLightLevel();
    return 0;
  }

  if (run.backlight_level >= 0) m.setBackLightLevel(run.backlight_level);

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
               "  -B lvl    set baclklight level [0-3]";
  exit(1);
}