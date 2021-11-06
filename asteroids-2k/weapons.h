/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _WEAPONS_H_
#define _WEAPONS_H_

#include <math.h>
#include <vector>

#include "maths.h"
#include "video.h"
#include "vectors.h"


class TShip;

class TWeapon;
typedef std::vector<TWeapon*> TVecPtrWeapons;

class TWeapon
{
	public:
    	TWeapon(TVideoManager* pVM);
        TWeapon(TVideoManager* pVM, TVector2 Pos, TVector2 Vel);

        TVector2 GetPos();
        virtual void Update(double Dt) = 0;

        bool IsArmed();
        void Arm(TVector2 Pos, TVector2 Vel);

	protected:
		bool m_bArmed;
        TVector2 m_Pos, m_Vel;
        COLORREF m_Color;
        TVideoManager *m_pVM;
};

class TMissile : public TWeapon
{
	public:
        TMissile(TVideoManager* pVM);
        TMissile(TVideoManager* pVM, TVector2 Pos, TVector2 Vel);

		TShip* GetShip();
		void SetShip(TShip* pShip);

        void Update(double Dt);

	protected:
        TShip* m_pShip;
};

#endif

