#include "mplayer.hpp"
#include <chrono>
#include <iostream>
#include <thread>

MPlayer::MPlayer()
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

void MPlayer::reset()
{
  if (dev)
  {
    ao_close(dev);
    dev = nullptr;
  }
  if (mh != nullptr)
    mpg123_close(mh);

  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);
  buffer = std::shared_ptr<char>(new char[buffer_size],
                                 std::default_delete<char[]>());
}

void MPlayer::music(const char *mp3)
{
  track = mp3;
  this->reset();
  mpg123_open(mh, mp3);
  mpg123_getformat(mh, &rate, &channels, &encoding);

  format.bits = mpg123_encsize(encoding) * 8;
  format.rate = rate;
  format.channels = channels;
  format.byte_format = AO_FMT_NATIVE;
  format.matrix = 0;
  dev = ao_open_live(driver, &format, NULL);
}

void MPlayer::play(const std::atomic<bool> &pause,
                   const std::atomic<bool> &stop)
{
  // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // this->debug();
  while (mpg123_read(mh, buffer.get(), buffer_size, &done) == MPG123_OK)
  {
    if (stop.load())
      break;

    while (pause.load())
    {
      if (stop.load())
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    ao_play(dev, buffer.get(), done);
  }
}

MPlayer::~MPlayer()
{
  ao_close(dev);
  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();
  ao_shutdown();
}
