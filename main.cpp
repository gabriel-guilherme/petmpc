#include "lib_play_mp3.hpp"

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    auto p = std::make_unique<LibPlayMP3>();
    p->music(argv[1]);
    p->play();
  }
  else
  {
    std::cerr << "Enter the song\n";
    return EXIT_FAILURE;
  }
  return 0;
}
