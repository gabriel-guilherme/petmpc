#include "lib_play_mp3.hpp"

LibPlayMP3::LibPlayMP3()
{
  track = "";
  ao_initialize();
  driver = ao_default_driver_id();
  mpg123_init();
  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);

  buffer = std::shared_ptr<char>(new char[buffer_size],
                                 std::default_delete<char[]>());
}

void LibPlayMP3::music(char *mp3)
{
  track = mp3;
  mpg123_open(mh, mp3);
  mpg123_getformat(mh, &rate, &channels, &encoding);

  format.bits = mpg123_encsize(encoding) * 8;
  format.rate = rate;
  format.channels = channels;
  format.byte_format = AO_FMT_NATIVE;
  format.matrix = 0;
  dev = ao_open_live(driver, &format, NULL);
}

void LibPlayMP3::play()
{
  std::cout << "\033[33;1m\u25B6 Playing the song: \033[35;1m ";
  std::cout << track << "\033[m\n";

  while (mpg123_read(mh, buffer.get(), buffer_size, &done) == MPG123_OK)
  {
    ao_play(dev, buffer.get(), done);
  }
}

LibPlayMP3::~LibPlayMP3()
{
  ao_close(dev);
  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();
  ao_shutdown();
}
