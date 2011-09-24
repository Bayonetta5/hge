/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager helper class header
*/
#pragma once
#ifndef HGERESOURCE_H
#define HGERESOURCE_H


#include <hge.h>
#include <hgesprite.h>
#include <hgeanim.h>
#include <hgefont.h>
#include <hgeparticle.h>
#include <hgedistort.h>
#include <hgestrings.h>


#define RESTYPES 13
#define HGE_MAX_RESOURCE_CHARS 128


class hgeResourceManager;

struct ResDesc
{
	hgeChar		name[HGE_MAX_RESOURCE_CHARS];
	int			resgroup;
	uint32_t	handle;
	ResDesc*	next;

	ResDesc()	{ hge=hgeCreate(HGE_VERSION); }
	~ResDesc()	{ hge->Release(); }

	virtual uint32_t Get(hgeResourceManager *rm) = 0;
	virtual void  Free() = 0;

protected:
	static HGE	*hge;
};

/*
** HGE Resource manager class
*/
class hgeResourceManager
{
public:
	hgeResourceManager(const hgeString scriptname=0);
	~hgeResourceManager();

	void				ChangeScript(const hgeString scriptname=0);
	bool				Precache(int groupid=0);
	void				Purge(int groupid=0);

	void*				GetResource(const hgeString name, int resgroup=0);
	HTEXTURE			GetTexture(const hgeString name, int resgroup=0);
	HEFFECT				GetEffect(const hgeString name, int resgroup=0);
	HMUSIC				GetMusic(const hgeString name, int resgroup=0);
	HSTREAM				GetStream(const hgeString name, int resgroup=0);
	HTARGET				GetTarget(const hgeString name);

	hgeSprite*			GetSprite(const hgeString name);
	hgeAnimation*		GetAnimation(const hgeString name);
	hgeFont*			GetFont(const hgeString name);
	hgeParticleSystem*	GetParticleSystem(const hgeString name);
	hgeDistortionMesh*	GetDistortionMesh(const hgeString name);
	hgeStringTable*		GetStringTable(const hgeString name, int resgroup=0);

	ResDesc*			res[RESTYPES];

private:
	hgeResourceManager(const hgeResourceManager &);
	hgeResourceManager&	operator= (const hgeResourceManager&);
	void				_remove_all();
	void				_parse_script(const hgeString scriptname=0);

	static HGE			*hge;
};


#endif
