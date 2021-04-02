#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
typedef struct _Music{
     struct _MusicData *privateField = nullptr;
} Music;

namespace Speaker
{
    void init();
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
void deleteMusic(Music &music);

#endif