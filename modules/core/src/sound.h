#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include "vector.h"

typedef struct Sound {
  ALuint buffer;
  OggVorbis_File *file;
  vorbis_info *info;
  double pitch;
  double gain;
} Sound;

void sound_cleanup_lib(void);
bool sound_init(Sound *sound, char *filepath, double pitch, double gain);
void sound_delete(Sound *sound);
void sound_play_static(Sound *sound, Vector3d *pos);
void sound_play_ambient(Sound *sound, bool loop);
void sound_stop(Sound *sound);

#endif /* SOUND_H */
