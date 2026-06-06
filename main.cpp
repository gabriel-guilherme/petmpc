#include "mplayer.hpp"
#include "msession.hpp"
#include "music.hpp"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char **argv)
{
  if (argc != 1)
  {
    std::cerr << "Usage: ./petmpc\n";
    return EXIT_FAILURE;
  }

  MSession session;
  if (session.init(argv) != 0)
  {
    std::cerr << "Error during initialization..\n";
  }

  auto p = std::make_unique<MPlayer>();
  auto m = session.find_music_on_database(5)->get_path();
  p->music(m.c_str());
  p->play();

  return 0;
}
