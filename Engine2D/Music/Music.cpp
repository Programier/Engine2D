#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <audio/wave.h>
#include <vector>

extern "C"
{
    typedef struct _Music
    {
        struct _MusicData *privateField = nullptr;
    } Music;

    int init();
    void terminate();
    void play(Music &music);
    bool isInited();
    void stop(Music &music);
    bool isPlaying(Music &music);
    void stopLastInStack();
    void stopStackAt(int index);
    void clearStack();

    void loadWAV(std::string filename, Music &music);
    void deleteMusic(Music &music);

    static std::vector<Music> stack;

    struct _MusicData
    {
        ALuint source, buffer;
        ALsizei size, freq;
        ALenum format;
        ALvoid *data;
        ALboolean loop = AL_FALSE;
        bool isPlaying = false;
        std::string path;
        ALint source_tmp, playing_value = 0;
    };

    static ALCdevice *device = nullptr;
    static ALCcontext *context = nullptr;
    static bool inited = false;

    static void updateStack()
    {
        for (int i = 0; i < static_cast<int>(stack.size()); i++)
        {
            if (!isPlaying(stack[i]))
            {
                std::cout << "AUDIO: Deleting stack data by index: " << i << std::endl;
                deleteMusic(stack[i]);
                stack.erase(stack.begin() + i);
                i--;
            }
        }
    }

    void stopLastInStack()
    {
    	int index = stack.size() - 1;
    	if(index >= 0)
    	{
    		if (isPlaying(stack[index]))
                stop(stack[index]);
            deleteMusic(stack[index]);
            stack.erase(stack.begin() + index);
    	}
    }

    void clearStack()
    {
        std::cout << "AUDIO: Start cleaning audio stack data" << std::endl;
        while (!stack.empty())
        {
            if (isPlaying(stack[0]))
                stop(stack[0]);
            deleteMusic(stack[0]);
            stack.erase(stack.begin());
        }
        std::cout << "AUDIO: Stack cleaned succesfully" << std::endl;
    }

    int init()
    {
        if (!inited)
        {
            std::cout << "SPEAKER: Starting init Speaker device" << std::endl;
            // Initialize device
            device = alcOpenDevice(NULL);
            if (device == nullptr)
            {
                std::cerr << "SPEAKER: Failed to load speaker device" << std::endl;
                return -1;
            }

            // Initialize context
            context = alcCreateContext(device, NULL);
            if (!alcMakeContextCurrent(context))
            {
                std::cerr << "Failed to init context " << std::endl;
                return -1;
            }
            std::cout << "SPEAKER: Speaker succesfully initialized" << std::endl;
            inited = true;
        }
        else
            std::cerr << "SPEAKER: Speaker device already initialized" << std::endl;

        return 0;
    }

    void play(Music &music)
    {
        if (!isPlaying(music))
        {
            alSourcePlay(music.privateField->source);
            do
            {
                alGetSourcei(music.privateField->source, AL_SOURCE_STATE, &music.privateField->source_tmp);
            } while (music.privateField->source == AL_PLAYING);
            music.privateField->playing_value = music.privateField->source_tmp;
            music.privateField->isPlaying = true;
        }
        else
        {
            std::cout << "SPEAKER: The file is already playing. Create a new audio object" << std::endl;
            stack.push_back(Music());
            loadWAV(music.privateField->path, stack[stack.size() - 1]);
            play(stack[stack.size() - 1]);
        }
        updateStack();
    }

    void stop(Music &music)
    {
        alSourceStop(music.privateField->source);
        alGetSourcei(music.privateField->source, AL_SOURCE_STATE, &music.privateField->source_tmp);
    }

    void stopStackAt(int index)
    {
        if (index >= 0 && index < static_cast<int>(stack.size()))
        {
            if (isPlaying(stack[index]))
            {
                stop(stack[index]);
            }
            deleteMusic(stack[index]);
            stack.erase(stack.begin() + index);
        }
        else
        {
            std::cerr << "Failed to get data from index: " << index << std::endl;
        }
    }

    bool isPlaying(Music &music)
    {
        if (music.privateField->isPlaying)
        {
            alGetSourcei(music.privateField->source, AL_SOURCE_STATE, &music.privateField->source_tmp);
            return (music.privateField->source_tmp == music.privateField->playing_value);
        }
        return false;
    }

    void terminate()
    {
        updateStack();
        if (inited)
        {
            std::cout << "SPEAKER: Start closing speaker device" << std::endl;
            device = alcGetContextsDevice(context);
            alcMakeContextCurrent(NULL);
            alcDestroyContext(context);
            alcCloseDevice(device);
            inited = false;
            std::cout << "SPEAKER: Speaker device succesfully closed" << std::endl;
        }
        else
            std::cerr << "AUDIO: Speaker has been closed or not inited" << std::endl;
    }

    bool isInited()
    {
        return inited;
    }

    static inline ALenum getAlFormat(short channels, short samples)
    {
        bool stereo = (channels > 1);
        switch (samples)
        {
        case 16:
            return stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        case 8:
            return stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
        default:
            return -1;
        }
    }

    void loadWAV(std::string filename, Music &music)
    {
        std::cout << "AUDIO: Loading data from a file \"" << filename << "\"" << std::endl;
        music.privateField = new _MusicData();
        music.privateField->path = filename;
        alGenSources((ALuint)1, &music.privateField->source);

        alSourcef(music.privateField->source, AL_PITCH, 1);
        alSourcef(music.privateField->source, AL_GAIN, 1);
        alSource3f(music.privateField->source, AL_POSITION, 0, 0, 0);
        alSource3f(music.privateField->source, AL_VELOCITY, 0, 0, 0);
        alSourcei(music.privateField->source, AL_LOOPING, AL_FALSE);
        alGenBuffers((ALuint)1, &music.privateField->buffer);

        // Load audio
        WaveInfo *wave;
        char *bufferData;
        int ret;

        wave = WaveOpenFileForReading(filename.c_str());
        if (!wave)
        {
            std::cerr << "Failed to open wav file" << std::endl;
            return;
        }

        ret = WaveSeekFile(0, wave);
        if (ret)
        {
            std::cerr << "Failed to seek wave file" << std::endl;
            return;
        }

        bufferData = new char[wave->dataSize / sizeof(char)];
        ret = WaveReadFile(bufferData, wave->dataSize, wave);
        if (ret != static_cast<int>(wave->dataSize))
        {
            std::cerr << "Short read: " << ret << ", want: " << wave->dataSize << std::endl;
            return;
        }

        alBufferData(music.privateField->buffer, getAlFormat(wave->channels, wave->bitsPerSample),
                     bufferData, wave->dataSize, wave->sampleRate);
        alSourcei(music.privateField->source, AL_BUFFER, music.privateField->buffer);
        music.privateField->buffer = music.privateField->buffer;
        music.privateField->source = music.privateField->source;
        delete wave;
        delete[] bufferData;
        std::cout << "AUDIO: Data loaded successfully from a file \"" << filename << "\"" << std::endl;
    }

    void deleteMusic(Music &music)
    {
        std::cout << "AUDIO: Start freeing resources for the file \"" << music.privateField->path << "\"" << std::endl;
        alDeleteSources(1, &music.privateField->source);
        alDeleteBuffers(1, &music.privateField->buffer);
        delete music.privateField;
        music.privateField = nullptr;
    }
}