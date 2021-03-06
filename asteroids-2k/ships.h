/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _SHIPS_H_
#define _SHIPS_H_

#include <windows.h>
#include <vector>
#pragma hdrstop

#include "vectors.h"
#include "audio.h"
#include "video.h"


#define SHIP_SIZE				16
#define SHIP_ROTSTEP			10.0
#define SHIP_IMPULSE			4.0
#define SHIP_VELSTEP			0.5
#define SHIP_MAXVEL				250.0
#define SHIP_IMPULSETICKS		20
//#define SHIP_EXPLOSIONTICKS	64
#define SHIP_EXPLOSIONTICKS		32
#define SHIP_NDEBRIS			16


enum enShipClass { scHuman, scAlienSmall, scAlienBig };

struct TShip;
typedef std::vector<TShip*> TVecPtrShips;


class TShip
{
    public:
        TShip(TVideoManager *pVM, TSoundManager* pSM,
            enShipClass nClass, TVector2 Size, TVector2 Pos, TVector2 Vel);

    public:
        void Reset();

        enShipClass GetClass();
        void SetClass(enShipClass nClass);

        void ActivateTheShield();
        bool IsShieldActive();
        bool IsExploding();

        void Update(double Dt);

        TVector2 GetPos();
        void SetPos(TVector2 Pos);
        double GetRot();
        void SetRot(double Rot);
        TVector2 GetVel();
        void SetVel(TVector2 Vel);

        void SetColor(COLORREF Color);
        TVector2 GetSize() { return m_Size; }

        void RotateLeft(double DAngleDeg);
        void RotateRight(double DAngleDeg);
        void SetVisible(bool bVisible);
        void SetAlive(bool bAlive);

        void Impulse(double Impulse);

        void Explode();
        void DoExplosion();

        bool IsAlive();
        bool IsVisible();
        bool IsColliding(TVector2 Pos);

    protected:
        enShipClass m_nClass;
        TSoundManager *m_pAudio;
        TVideoManager *m_pVideo;

        COLORREF m_Color;
        int m_nImpulseTicks;
        double m_Rot, m_Impulse;
        bool m_bAlive, m_bVisible;
        TVector2 m_Size, m_Pos, m_Vel;
        TVecVecPoints m_Shape, m_Engine, m_Shield;

        bool m_bShield;
        unsigned m_nShieldTick;

        int m_nExplosionTicks;
                                      		// debris
        TVecVecPoints m_Debris;
		TVector2 m_DebrisStartPos;
        TVecPoints m_DebrisTranslations;
        std::vector<double> m_DebrisRotations;

    protected:
		void BuildTheShip();
        void Split(TVecVecPoints& Pts, TVecVecPoints& Splits);

};

#endif



