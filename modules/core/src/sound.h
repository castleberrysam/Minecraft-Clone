#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>
#include "vector.h"

typedef struct {
  ALuint buffer;
  OggVorbis_File *file;
  vorbis_info *info;
} Sound;

void sound_cleanup_lib(void);
bool sound_init(Sound *sound, char *filepath);
void sound_delete(Sound *sound);
void sound_play_static(Sound *sound, Vector3d *pos);

#endif /* SOUND_H */
