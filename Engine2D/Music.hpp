#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
typedef struct _Music
{
    struct _MusicData *privateField = nullptr;
} Music;

namespace Speaker
{
    int init();
    void terminate();
    void play(Music &music);
    bool isInited();
    void stop(Music &music);
    bool isPlaying(Music &music);
    void stopLastInStack();
    void stopStackAt(int index);
    void clearStack();
}

void loadWAV(std::string filename, Music &music);
Music loadWAV(std::string filename);
void deleteMusic(Music &music);

#endif