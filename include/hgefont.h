/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class header
*/


#pragma once


#include "hge.h"
#include "hgesprite.h"


#define HGETEXT_LEFT		0
#define HGETEXT_RIGHT		1
#define HGETEXT_CENTER		2
#define HGETEXT_HORZMASK	0x03

#define HGETEXT_TOP			0
#define HGETEXT_BOTTOM		4
#define HGETEXT_MIDDLE		8
#define HGETEXT_VERTMASK	0x0C

/*
** HGE Font class
*/
class hgeFont {
public:
    hgeFont(const char* filename, bool bMipmap = false);
    ~hgeFont();

    void Render(float x, float y, int align, const char* string);
    void printf(float x, float y, int align, const char* format, ...);
    void printfb(float x, float y, float w, float h, int align, const char* format, ...);

    void SetColor(hgeU32 col);
    void SetZ(float z);
    void SetBlendMode(int blend);

    void SetScale(const float scale) {
        fScale = scale;
    }

    void SetProportion(const float prop) {
        fProportion = prop;
    }

    void SetRotation(const float rot) {
        fRot = rot;
    }

    void SetTracking(const float tracking) {
        fTracking = tracking;
    }

    void SetSpacing(const float spacing) {
        fSpacing = spacing;
    }

    hgeU32 GetColor() const {
        return dwCol;
    }

    float GetZ() const {
        return fZ;
    }

    int GetBlendMode() const {
        return nBlend;
    }

    float GetScale() const {
        return fScale;
    }

    float GetProportion() const {
        return fProportion;
    }

    float GetRotation() const {
        return fRot;
    }

    float GetTracking() const {
        return fTracking;
    }

    float GetSpacing() const {
        return fSpacing;
    }

    hgeSprite* GetSprite(const char chr) const {
        return letters[static_cast<unsigned char>(chr)];
    }

    float GetPreWidth(const char chr) const {
        return pre[static_cast<unsigned char>(chr)];
    }

    float GetPostWidth(const char chr) const {
        return post[static_cast<unsigned char>(chr)];
    }

    float GetHeight() const {
        return fHeight;
    }

    float GetStringWidth(const char* string, bool bMultiline = true) const;

private:
    hgeFont();
    hgeFont(const hgeFont& fnt);
    hgeFont& operator=(const hgeFont& fnt);

    char* _get_line(char* file, char* line);

    static HGE* hge;

    static char buffer[1024];

    HTEXTURE hTexture;
    hgeSprite* letters[256];
    float pre[256];
    float post[256];
    float fHeight;
    float fScale;
    float fProportion;
    float fRot;
    float fTracking;
    float fSpacing;

    hgeU32 dwCol;
    float fZ;
    int nBlend;
};
