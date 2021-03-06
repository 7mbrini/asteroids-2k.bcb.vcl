/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _ASTEROIDS_H_
#define _ASTEROIDS_H_

#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

#include "audio.h"
#include "video.h"
#include "vectors.h"

#define ASTEROID_EXPLOSIONTICKS		64
#define ASTEROID_NDEBRIS			16


enum enAsteroidClass { acBig, acMedium, acSmall };

class TAsteroid;
typedef std::vector<TAsteroid> TVecAsteroids;
typedef std::vector<TAsteroid*> TVecPtrAsteroids;

class TAsteroid
{
	public:
        TAsteroid( TVideoManager* pVM,
        	TSoundManager* pSM,
        	enAsteroidClass nClass, TVector2 Pos,
            TVector2 Vel, double Radius );
		~TAsteroid();

	public:
        void Update(double DT);
        enAsteroidClass GetClass();

        double GetSize();
        TVector2 GetPos();
        TVector2 GetVel();

        void Explode();
        void DoExplosion();
        bool IsExploding();

        bool Collide(TVector2 Pt);
        void SetPos(TVector2 Pos);

        bool IsAlive();
        void SetAlive(bool bAlive);
	    void SetVisible(bool bVisible);

	protected:
        bool m_bAlive, m_bVisible;
        COLORREF m_Color;
        double m_Radius, m_Rot, m_DRot;
        TVecPoints m_Shape;
        TVector2 m_Pos, m_Vel, m_Acc;
		TSoundManager* m_pAudio;
        TVideoManager* m_pVideo;
        enAsteroidClass m_nClass;

	protected:
        void Crash();
        double GetRoughness();
		TVecPoints RandShape(double Size);

    protected:
										// debris
		TVector2 m_DebrisStartPos;
        int m_nDebris, m_nExplosionTicks;
        TVector2 m_Debris[ASTEROID_NDEBRIS];
        double m_DebrisScales[ASTEROID_NDEBRIS];
};

#endif


