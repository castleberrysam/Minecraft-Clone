#include <stdlib.h>
#include "sound.h"

static ALCdevice *device = NULL;
static ALCcontext *context = NULL;
static ALuint sources[256];

static bool prepare_lib(void)
{
  device = alcOpenDevice(NULL);
  if(device == NULL) {
#ifdef DEBUG
    fprintf(stderr, "[SOUND] failed to open the default audio device\n");
#endif
    return false;
  }

  context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);

  alGenSources(8, sources);
  alListenerf(AL_GAIN, 1.0);

  return true;
}

void sound_cleanup_lib(void)
{
  alDeleteSources(8, sources);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
}

bool sound_init(Sound *sound, char *filepath, double pitch, double gain)
{
  if(device == NULL) {prepare_lib();}
  
  sound->file = malloc(sizeof(OggVorbis_File));
  int error = ov_fopen(filepath, sound->file);
  if(error != 0) {
#ifdef DEBUG
    if(error == OV_ENOTVORBIS) {
      fprintf(stderr, "[SOUND] path %s is not an ogg vorbis file\n", filepath);
    } else {
      fprintf(stderr, "[SOUND] path %s is corrupted or does not exist\n", filepath);
    }
#endif
    return false;
  }

  sound->info = ov_info(sound->file, -1);
  if(sound->info->channels > 1) {
#ifdef DEBUG
    fprintf(stderr, "[SOUND] can't load ogg files with more than one channel\n");
#endif
    return false;
  }

  long num_read = 0, num_total = ov_pcm_total(sound->file, -1) * 2;
  char *data = malloc(num_total);
  do {
    int bitstream;
    num_read += ov_read(sound->file, &data[num_read], num_total-num_read, 0, 2, 1, &bitstream);
    if(bitstream != 0) {
#ifdef DEBUG
      fprintf(stderr, "[SOUND] can't load ogg files with more than one bitstream\n");
#endif
      free(data);
      return false;
    }
  } while(num_read < num_total);  
  
  alGenBuffers(1, &sound->buffer);
  alBufferData(sound->buffer, AL_FORMAT_MONO16, data, num_total, sound->info->rate);
  free(data);

  sound->pitch = pitch;
  sound->gain = gain;
  
  return true;
}

void sound_delete(Sound *sound)
{
  ov_clear(sound->file);
  free(sound->file);

  alDeleteBuffers(1, &sound->buffer);
}

void sound_play_static(Sound *sound, Vector3d *pos)
{
  int source = -1;
  for(int i=0;i<8;++i) {
    ALint state;
    alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
    if(state == AL_STOPPED || state == AL_INITIAL) {
      source = sources[i];
      break;
    }
  }
  if(source == -1) {
#ifdef DEBUG
    fprintf(stderr, "[SOUND] max sound source limit exceeded\n");
#endif
    return;
  }
  
  alSource3f(source, AL_POSITION, pos->x, pos->y, pos->z);
  alSourcei(source, AL_BUFFER, sound->buffer);
  alSourcef(source, AL_PITCH, sound->pitch);
  alSourcef(source, AL_GAIN, sound->gain);
  alSourcePlay(source);
}
