#include "mplayer.hpp"
#include "msession.hpp"
#include "ui/window.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main(int argc, char **argv)
{
  if (argc != 1)
  {
    std::cerr << "Uso: ./petmpc\n";
    return EXIT_FAILURE;
  }

  ui::Window w;
  if (w.init())
    w.run();

  return EXIT_SUCCESS;
}
