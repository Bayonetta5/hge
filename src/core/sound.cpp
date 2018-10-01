/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: audio routines
*/


#include "hge_impl.h"


#define BASSDEF(f) (WINAPI *f)	// define the functions as pointers
#include "BASS/bass.h"

#define LOADBASSFUNCTION(f) *((void**)&f)=(void*)GetProcAddress(hBass,#f)


HEFFECT HGE_CALL HGE_Impl::Effect_Load(const char* filename, const hgeU32 size) {
    hgeU32 _size;
    BASS_CHANNELINFO info;
    void* data;

    if (hBass) {
        if (is_silent_) {
            return 1;
        }

        if (size) {
            data = (void *)filename;
            _size = size;
        }
        else {
            data = Resource_Load(filename, &_size);
            if (!data) {
                return NULL;
            }
        }

        HSAMPLE hs = BASS_SampleLoad(TRUE, data, 0, _size, 4, BASS_SAMPLE_OVER_VOL);
        if (!hs) {
            HSTREAM hstrm = BASS_StreamCreateFile(TRUE, data, 0, _size, BASS_STREAM_DECODE);
            if (hstrm) {
                hgeU32 length = (hgeU32)BASS_ChannelGetLength(hstrm);
                BASS_ChannelGetInfo(hstrm, &info);
                hgeU32 samples = length;
                if (info.chans < 2) {
                    samples >>= 1;
                }
                if ((info.flags & BASS_SAMPLE_8BITS) == 0) {
                    samples >>= 1;
                }
                void* buffer = BASS_SampleCreate(samples, info.freq, 2, 4,
                                                 info.flags | BASS_SAMPLE_OVER_VOL);
                if (!buffer) {
                    BASS_StreamFree(hstrm);
                    _PostError("Can't create sound effect: Not enough memory");
                }
                else {
                    BASS_ChannelGetData(hstrm, buffer, length);
                    hs = BASS_SampleCreateDone();
                    BASS_StreamFree(hstrm);
                    if (!hs) {
                        _PostError("Can't create sound effect");
                    }
                }
            }
        }

        if (!size) {
            Resource_Free(data);
        }
        return hs;
    }
    return 0;
}

HCHANNEL HGE_CALL HGE_Impl::Effect_Play(HEFFECT eff) {
    if (hBass) {
        HCHANNEL chn = BASS_SampleGetChannel(eff, FALSE);
        BASS_ChannelPlay(chn, TRUE);
        return chn;
    }
    return 0;
}

HCHANNEL HGE_CALL HGE_Impl::
Effect_PlayEx(HEFFECT eff, int volume, int pan, float pitch, bool loop) {
    if (hBass) {
        BASS_SAMPLE info;
        BASS_SampleGetInfo(eff, &info);

        HCHANNEL chn = BASS_SampleGetChannel(eff, FALSE);
        BASS_ChannelSetAttributes(chn, (int)(pitch * info.freq), volume, pan);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if (loop) {
            info.flags |= BASS_SAMPLE_LOOP;
        }
        BASS_ChannelSetFlags(chn, info.flags);
        BASS_ChannelPlay(chn, TRUE);
        return chn;
    }
    return 0;
}


void HGE_CALL HGE_Impl::Effect_Free(HEFFECT eff) {
    if (hBass) {
        BASS_SampleFree(eff);
    }
}


HMUSIC HGE_CALL HGE_Impl::Music_Load(const char* filename, hgeU32 size) {
    void* data;
    hgeU32 _size;

    if (hBass) {
        if (size) {
            data = (void *)filename;
            _size = size;
        }
        else {
            data = Resource_Load(filename, &_size);
            if (!data) {
                return 0;
            }
        }

        HMUSIC hm = BASS_MusicLoad(TRUE, data, 0, 0,
                                   BASS_MUSIC_PRESCAN | BASS_MUSIC_POSRESETEX | BASS_MUSIC_RAMP,
                                   0);
        if (!hm) {
            _PostError("Can't load music");
        }
        if (!size) {
            Resource_Free(data);
        }
        return hm;
    }
    return 0;
}

HCHANNEL HGE_CALL HGE_Impl::Music_Play(HMUSIC mus, bool loop, int volume, int order, int row) {
    if (hBass) {
        hgeU32 pos = BASS_MusicGetOrderPosition(mus);
        if (order == -1) {
            order = LOWORD(pos);
        }
        if (row == -1) {
            row = HIWORD(pos);
        }
        BASS_ChannelSetPosition(mus, MAKEMUSICPOS(order, row));

        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(mus, &info);
        BASS_ChannelSetAttributes(mus, info.freq, volume, 0);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if (loop) {
            info.flags |= BASS_SAMPLE_LOOP;
        }
        BASS_ChannelSetFlags(mus, info.flags);

        BASS_ChannelPlay(mus, FALSE);

        return mus;
    }
    return 0;
}

void HGE_CALL HGE_Impl::Music_Free(HMUSIC mus) {
    if (hBass) {
        BASS_MusicFree(mus);
    }
}

void HGE_CALL HGE_Impl::Music_SetAmplification(HMUSIC music, int ampl) {
    if (hBass) {
        BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY, ampl);
    }
}

int HGE_CALL HGE_Impl::Music_GetAmplification(HMUSIC music) {
    if (hBass) {
        return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_AMPLIFY);
    }
    return -1;
}

int HGE_CALL HGE_Impl::Music_GetLength(HMUSIC music) {
    if (hBass) {
        return BASS_MusicGetOrders(music);
    }
    return -1;
}

void HGE_CALL HGE_Impl::Music_SetPos(HMUSIC music, int order, int row) {
    if (hBass) {
        BASS_ChannelSetPosition(music, MAKEMUSICPOS(order, row));
    }
}

bool HGE_CALL HGE_Impl::Music_GetPos(HMUSIC music, int* order, int* row) {
    if (hBass) {
        hgeU32 pos;
        pos = BASS_MusicGetOrderPosition(music);
        if (pos == -1) {
            return false;
        }
        *order = LOWORD(pos);
        *row = HIWORD(pos);
        return true;
    }
    return false;
}

void HGE_CALL HGE_Impl::Music_SetInstrVolume(HMUSIC music, int instr, int volume) {
    if (hBass) {
        BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr, volume);
    }
}

int HGE_CALL HGE_Impl::Music_GetInstrVolume(HMUSIC music, int instr) {
    if (hBass) {
        return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_INST + instr);
    }
    return -1;
}

void HGE_CALL HGE_Impl::Music_SetChannelVolume(HMUSIC music, int channel, int volume) {
    if (hBass) {
        BASS_MusicSetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel, volume);
    }
}

int HGE_CALL HGE_Impl::Music_GetChannelVolume(HMUSIC music, int channel) {
    if (hBass) {
        return BASS_MusicGetAttribute(music, BASS_MUSIC_ATTRIB_VOL_CHAN + channel);
    }
    return -1;
}

HSTREAM HGE_CALL HGE_Impl::Stream_Load(const char* filename, hgeU32 size) {
    void* data;
    hgeU32 _size;

    if (hBass) {
        if (is_silent_) {
            return 1;
        }

        if (size) {
            data = (void *)filename;
            _size = size;
        }
        else {
            data = Resource_Load(filename, &_size);
            if (!data) {
                return 0;
            }
        }
        HSTREAM hs = BASS_StreamCreateFile(TRUE, data, 0, _size, 0);
        if (!hs) {
            _PostError("Can't load stream");
            if (!size) {
                Resource_Free(data);
            }
            return 0;
        }
        if (!size) {
            CStreamList* stmItem = new CStreamList;
            stmItem->hstream = hs;
            stmItem->data = data;
            stmItem->next = sound_streams_;
            sound_streams_ = stmItem;
        }
        return hs;
    }
    return 0;
}

void HGE_CALL HGE_Impl::Stream_Free(HSTREAM stream) {
    CStreamList *stmItem = sound_streams_, *stmPrev = nullptr;

    if (hBass) {
        while (stmItem) {
            if (stmItem->hstream == stream) {
                if (stmPrev) {
                    stmPrev->next = stmItem->next;
                }
                else {
                    sound_streams_ = stmItem->next;
                }
                Resource_Free(stmItem->data);
                delete stmItem;
                break;
            }
            stmPrev = stmItem;
            stmItem = stmItem->next;
        }
        BASS_StreamFree(stream);
    }
}

HCHANNEL HGE_CALL HGE_Impl::Stream_Play(HSTREAM stream, bool loop, int volume) {
    if (hBass) {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(stream, &info);
        BASS_ChannelSetAttributes(stream, info.freq, volume, 0);

        info.flags &= ~BASS_SAMPLE_LOOP;
        if (loop) {
            info.flags |= BASS_SAMPLE_LOOP;
        }
        BASS_ChannelSetFlags(stream, info.flags);
        BASS_ChannelPlay(stream, TRUE);
        return stream;
    }
    return 0;
}

void HGE_CALL HGE_Impl::Channel_SetPanning(HCHANNEL chn, int pan) {
    if (hBass) {
        BASS_ChannelSetAttributes(chn, -1, -1, pan);
    }
}

void HGE_CALL HGE_Impl::Channel_SetVolume(HCHANNEL chn, int volume) {
    if (hBass) {
        BASS_ChannelSetAttributes(chn, -1, volume, -101);
    }
}

void HGE_CALL HGE_Impl::Channel_SetPitch(HCHANNEL chn, float pitch) {
    if (hBass) {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(chn, &info);
        BASS_ChannelSetAttributes(chn, (int)(pitch * info.freq), -1, -101);
    }
}

void HGE_CALL HGE_Impl::Channel_Pause(HCHANNEL chn) {
    if (hBass) {
        BASS_ChannelPause(chn);
    }
}

void HGE_CALL HGE_Impl::Channel_Resume(HCHANNEL chn) {
    if (hBass) {
        BASS_ChannelPlay(chn, FALSE);
    }
}

void HGE_CALL HGE_Impl::Channel_Stop(HCHANNEL chn) {
    if (hBass) {
        BASS_ChannelStop(chn);
    }
}

void HGE_CALL HGE_Impl::Channel_PauseAll() {
    if (hBass) {
        BASS_Pause();
    }
}

void HGE_CALL HGE_Impl::Channel_ResumeAll() {
    if (hBass) {
        BASS_Start();
    }
}

void HGE_CALL HGE_Impl::Channel_StopAll() {
    if (hBass) {
        BASS_Stop();
        BASS_Start();
    }
}

bool HGE_CALL HGE_Impl::Channel_IsPlaying(HCHANNEL chn) {
    if (hBass) {
        if (BASS_ChannelIsActive(chn) == BASS_ACTIVE_PLAYING) {
            return true;
        }
        return false;
    }
    return false;
}

float HGE_CALL HGE_Impl::Channel_GetLength(HCHANNEL chn) {
    if (hBass) {
        return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetLength(chn));
    }
    return -1;
}

float HGE_CALL HGE_Impl::Channel_GetPos(HCHANNEL chn) {
    if (hBass) {
        return BASS_ChannelBytes2Seconds(chn, BASS_ChannelGetPosition(chn));
    }
    return -1;
}

void HGE_CALL HGE_Impl::Channel_SetPos(HCHANNEL chn, float fSeconds) {
    if (hBass) {
        BASS_ChannelSetPosition(chn, BASS_ChannelSeconds2Bytes(chn, fSeconds));
    }
}

void HGE_CALL HGE_Impl::Channel_SlideTo(HCHANNEL channel, float time, int volume, int pan,
                                        float pitch) {
    if (hBass) {
        BASS_CHANNELINFO info;
        BASS_ChannelGetInfo(channel, &info);

        int freq;
        if (pitch == -1) {
            freq = -1;
        }
        else {
            freq = (int)(pitch * info.freq);
        }

        BASS_ChannelSlideAttributes(channel, freq, volume, pan, hgeU32(time * 1000));
    }
}

bool HGE_CALL HGE_Impl::Channel_IsSliding(HCHANNEL channel) {
    if (hBass) {
        if (BASS_ChannelIsSliding(channel)) {
            return true;
        }
        return false;
    }
    return false;
}


//////// Implementation ////////


bool HGE_Impl::_SoundInit() {
    if (!use_sound_ || hBass) {
        return true;
    }

    hBass = LoadLibrary("bass.dll");
    if (!hBass) {
        _PostError("Can't load BASS.DLL");
        return false;
    }

    LOADBASSFUNCTION(BASS_GetVersion);

    if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
        _PostError("Incorrect BASS.DLL version");
        return false;
    }

    LOADBASSFUNCTION(BASS_GetDeviceDescription);
    LOADBASSFUNCTION(BASS_Init);
    LOADBASSFUNCTION(BASS_Free);
    LOADBASSFUNCTION(BASS_Start);
    LOADBASSFUNCTION(BASS_Pause);
    LOADBASSFUNCTION(BASS_Stop);
    LOADBASSFUNCTION(BASS_SetConfig);
    //LOADBASSFUNCTION(BASS_ErrorGetCode);

    LOADBASSFUNCTION(BASS_SampleLoad);
    LOADBASSFUNCTION(BASS_SampleCreate);
    LOADBASSFUNCTION(BASS_SampleCreateDone);
    LOADBASSFUNCTION(BASS_SampleGetInfo);
    LOADBASSFUNCTION(BASS_SampleGetChannel);
    LOADBASSFUNCTION(BASS_SampleFree);

    LOADBASSFUNCTION(BASS_MusicLoad);
    LOADBASSFUNCTION(BASS_MusicFree);
    LOADBASSFUNCTION(BASS_MusicGetOrders);
    LOADBASSFUNCTION(BASS_MusicGetOrderPosition);
    LOADBASSFUNCTION(BASS_MusicSetAttribute);
    LOADBASSFUNCTION(BASS_MusicGetAttribute);

    LOADBASSFUNCTION(BASS_StreamCreateFile);
    LOADBASSFUNCTION(BASS_StreamFree);

    LOADBASSFUNCTION(BASS_ChannelGetInfo);
    LOADBASSFUNCTION(BASS_ChannelGetAttributes);
    LOADBASSFUNCTION(BASS_ChannelSetAttributes);
    LOADBASSFUNCTION(BASS_ChannelSlideAttributes);
    LOADBASSFUNCTION(BASS_ChannelIsSliding);
    LOADBASSFUNCTION(BASS_ChannelSetFlags);
    LOADBASSFUNCTION(BASS_ChannelGetData);
    LOADBASSFUNCTION(BASS_ChannelPlay);
    LOADBASSFUNCTION(BASS_ChannelPause);
    LOADBASSFUNCTION(BASS_ChannelStop);
    LOADBASSFUNCTION(BASS_ChannelIsActive);
    LOADBASSFUNCTION(BASS_ChannelGetLength);
    LOADBASSFUNCTION(BASS_ChannelGetPosition);
    LOADBASSFUNCTION(BASS_ChannelSetPosition);
    LOADBASSFUNCTION(BASS_ChannelSeconds2Bytes);
    LOADBASSFUNCTION(BASS_ChannelBytes2Seconds);

    is_silent_ = false;
    if (!BASS_Init(-1, sample_rate_, 0, hwnd_, nullptr)) {
        System_Log("BASS Init failed, using no sound");
        BASS_Init(0, sample_rate_, 0, hwnd_, nullptr);
        is_silent_ = true;
    }
    else {
        System_Log("Sound Device: %s", BASS_GetDeviceDescription(1));
        System_Log("Sample rate: %ld\n", sample_rate_);
    }

    //BASS_SetConfig(BASS_CONFIG_BUFFER, 5000);
    //BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 50);

    _SetFXVolume(fx_volume_);
    _SetMusVolume(mus_volume_);
    _SetStreamVolume(stream_volume_);

    return true;
}

void HGE_Impl::_SoundDone() {
    CStreamList* stmItem = sound_streams_;

    if (hBass) {
        BASS_Stop();
        BASS_Free();

        //int err = BASS_ErrorGetCode();

        FreeLibrary(hBass);
        hBass = nullptr;

        while (stmItem) {
            CStreamList* stmNext = stmItem->next;
            Resource_Free(stmItem->data);
            delete stmItem;
            stmItem = stmNext;
        }
        sound_streams_ = nullptr;
    }
}

void HGE_Impl::_SetMusVolume(int vol) {
    if (hBass) {
        BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, vol);
    }
}

void HGE_Impl::_SetStreamVolume(int vol) {
    if (hBass) {
        BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
    }
}

void HGE_Impl::_SetFXVolume(int vol) {
    if (hBass) {
        BASS_SetConfig(BASS_CONFIG_GVOL_SAMPLE, vol);
    }
}
