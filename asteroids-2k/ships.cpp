/*!****************************************************************************

	@file	ships.h
	@file	ships.cpp

	@brief	Spaceships modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <math.h>

#include "maths.h"
#include "ships.h"
#include "game.h"
#include "vectors.h"
#include "commdefs.h"


#define SHIELDTICKS		100	///< Durata (in ticks) dello scudo difensivo


/*!****************************************************************************
* @brief	Constructor
* @param	pVM Pointer to the VideoManager
* @param	pSM Pointer to the SoundManager
* @param	nClass Ship class (small, medium, big)
* @param	Size Size of the ship
* @param	Pos Initial position of the ship
* @param	Vel Initial velocity of the ship
******************************************************************************/
TShip::TShip(TVideoManager *pVM, TSoundManager* pSM,
	enShipClass nClass, TVector2 Size, TVector2 Pos, TVector2 Vel)
{
	assert(pVM);
	assert(pSM);

	m_pVideo = pVM;
	m_pAudio = pSM;

	m_Pos = Pos;
	m_Vel = Vel;
	m_Size = Size;
	m_bAlive = true;
	m_bVisible = false;
	m_Rot = m_Impulse = 0;
	m_Color = RGB(255, 255, 255);
	m_nImpulseTicks = m_nExplosionTicks = 0;

	m_bShield = false;
	m_nShieldTick = 0;

	m_nClass = nClass;

	BuildTheShip();
}

/*!****************************************************************************
* @brief	Builds the ship
******************************************************************************/
void TShip::BuildTheShip()
{

	if( GetClass() == scHuman )
	{
												// human ship
												// build the frame
		{
			m_Shape.clear();
			TVecPoints Ship;

			Ship.push_back(TVector2(0.5, -0.5));
			Ship.push_back(TVector2(0, 0.5));
			Ship.push_back(TVector2(-0.5, -0.5));
			Ship.push_back(TVector2(0, -0.25));
			Ship.push_back(TVector2(0.5, -0.5));

			for (int i = 0; i < Ship.size(); i++)
			{
				Ship[i].X *= m_Size.X;
				Ship[i].Y *= m_Size.Y;
			}

			m_Shape.push_back(Ship);
		}
												// build the engine
		{
			m_Engine.clear();
			TVecPoints Engine;

			Engine.push_back(TVector2(-0.25, -0.25));
			Engine.push_back(TVector2(-0.1, -0.3));
			Engine.push_back(TVector2(0, -0.75));
			Engine.push_back(TVector2(0.1, -0.3));
			Engine.push_back(TVector2(0.25, -0.25));

			for (int i = 0; i < Engine.size(); ++i)
			{
				Engine[i].X *= m_Size.X;
				Engine[i].Y *= m_Size.Y;
			}
		
			m_Engine.push_back(Engine);
		}
												// build the shield
		{
			m_Shield.clear();
			TVecPoints Shield;

			int nItems = 64;

			double DAngle = 2.0*M_PI/double(nItems);
			double ShieldSize = m_Size.X * 1.25;

			for(int i=0; i<nItems; i++)
			{
				Shield.push_back(TVector2 ( cos(i*DAngle)*ShieldSize, sin(i*DAngle)*ShieldSize ) );
			}

			Shield.push_back( Shield[0] );

			m_Shield.push_back(Shield);
		}
	}
	else
	{
											// alien ship
											// build the frame
		{
			m_Shape.clear();
			TVecPoints Ship;

			Ship.push_back(TVector2(0.5, 0));
			Ship.push_back(TVector2(0.25, 0.25));
			Ship.push_back(TVector2(-0.25, 0.25));
			Ship.push_back(TVector2(-0.5, 0));
			Ship.push_back(TVector2(-0.25, -0.25));
			Ship.push_back(TVector2(0.25, -0.25));
			Ship.push_back(TVector2(0.5, 0));
			Ship.push_back(TVector2(-0.5, 0));

			for (int i = 0; i < Ship.size(); ++i)
			{
				Ship[i].X *= m_Size.X;
				Ship[i].Y *= -m_Size.Y;
			}
												// windshield
			TVecPoints WShield;
			WShield.push_back(TVector2(0.2, 0.25));
			WShield.push_back(TVector2(0.15, 0.4));
			WShield.push_back(TVector2(-0.15, 0.4));
			WShield.push_back(TVector2(-0.2, 0.25));

			for (int i = 0; i < WShield.size(); ++i)
			{
				WShield[i].X *= m_Size.X;
				WShield[i].Y *= -m_Size.Y;
			}

			m_Shape.push_back(Ship);
			m_Shape.push_back(WShield);
		}
	}
}

/*!****************************************************************************
* @brief	Set the ship class (small, medium, big)
* @param	nClass The shiip class
******************************************************************************/
void TShip::SetClass(enShipClass nClass)
{
	m_nClass = nClass;
}

/*!****************************************************************************
* @brief	Return the ship class
* @return	The ship class
******************************************************************************/
enShipClass TShip::GetClass()
{
	return m_nClass;
}

/*!****************************************************************************
* @brief	Reset the ship to the default properties
******************************************************************************/
void TShip::Reset()
{
	m_Pos = TVector2(0, 0);
	m_Vel = TVector2(0, 0);
	m_Rot = 0;
	m_nExplosionTicks = -1;
	m_nImpulseTicks = 0;

	m_bShield = false;
	m_nShieldTick = 0;

	SetAlive(true);
	SetVisible(false);
}

/*!****************************************************************************
* @brief	Return true if the ship is exploding
* @return	True if the ship is exploding, false otherwise
******************************************************************************/
bool TShip::IsExploding()
{
	return bool(m_nExplosionTicks > 0);
}

/*!****************************************************************************
* @brief	Set the ship position
* @param	Pos The position to be set for the ship
******************************************************************************/
void TShip::SetPos(TVector2 Pos)
{
	m_Pos = Pos;
}

/*!****************************************************************************
* @brief	Return the ship position
* @return	The ship position
******************************************************************************/
TVector2 TShip::GetPos()
{
	return m_Pos;
}

/*!****************************************************************************
* @brief	Set the ship rotation
* @param	Rot Rotation angle in radians
******************************************************************************/
void TShip::SetRot(double Rot)
{
	m_Rot = Rot;
}

/*!****************************************************************************
* @brief	Return the ship rotation
* @return	Return the rotation angle in radians
******************************************************************************/
double TShip::GetRot()
{
	return m_Rot;
}

/*!****************************************************************************
* @brief	Set the ship velocity
* @param	Vel The velocity value to be set for the ship
******************************************************************************/
void TShip::SetVel(TVector2 Vel)
{
	m_Vel = Vel;
}

/*!****************************************************************************
* @brief	Return the ship velocity
* @return	Returns the velocity value of the ship
******************************************************************************/
TVector2 TShip::GetVel()
{
	return m_Vel;
}

/*!****************************************************************************
* @brief	Set the color for the ship
* @param	Color The color to be set for the ship
******************************************************************************/
void TShip::SetColor(COLORREF Color)
{
	m_Color = Color;
}

/*!****************************************************************************
* @brief	Rotate the ship to the left
* @param	DAngleDeg The rotation value in degrees
******************************************************************************/
void TShip::RotateLeft(double DAngleDeg)
{
	m_Rot += DAngleDeg;
}

/*!****************************************************************************
* @brief	Rotate the ship to the right
* @param	DAngleDeg The rotation value in degrees
******************************************************************************/
void TShip::RotateRight(double DAngleDeg)
{
	m_Rot -= DAngleDeg;
}

/*!****************************************************************************
* @brief	Return the ship visibility
* @return	True if the ship is visible, false otherwise
******************************************************************************/
bool TShip::IsVisible()
{
	return m_bVisible;
}

/*!****************************************************************************
* @brief	Returns the ship status, alive or not
* @return	True if the ship is alive, false otherwise
******************************************************************************/
bool TShip::IsAlive()
{
	return m_bAlive;
}

/*!****************************************************************************
* @brief	Set the ship visiblity
* @param	bVisible True for ship visiblitiy, false otherwise
******************************************************************************/
void TShip::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;


	if( bVisible )
	{
		if( GetClass() == scAlienBig )
		{
			m_pAudio->PlayTheSound("saucer_big", true);
		}
		else if( GetClass() == scAlienSmall )
		{
			m_pAudio->PlayTheSound("saucer_small", true);
		}
	}
	else
	{
		if( GetClass() == scAlienBig )
		{
			m_pAudio->StopTheSound("saucer_big");
		}
		else if( GetClass() == scAlienSmall )
		{
			m_pAudio->StopTheSound("saucer_small");
		}
	}
}

/*!****************************************************************************
* @brief	Set the aliving status for the ship
* @param	bAlive True for ship alivenes, false otherwise
******************************************************************************/
void TShip::SetAlive(bool bAlive)
{
	m_bAlive = bAlive;
}

/*!****************************************************************************
* @brief	Give an impulse to the ship
* @param	Impulse The value of the impulse to be given to the spaceship
******************************************************************************/
void TShip::Impulse(double Impulse)
{
	assert(m_pAudio);

	m_nImpulseTicks = SHIP_IMPULSETICKS;

											// gives the impulse in the same
											// direction of the ship heading
	m_Vel.X += cos( DEG2RAD(m_Rot - 90.0) ) * Impulse;
	m_Vel.Y += sin( DEG2RAD(m_Rot + 90.0) ) * Impulse;

											// limit the speed
	if (m_Vel.X > SHIP_MAXVEL) m_Vel.X = SHIP_MAXVEL;
	if (m_Vel.Y > SHIP_MAXVEL) m_Vel.Y = SHIP_MAXVEL;

											// plays the thrust sound
	unsigned nDelay = 250;
	static unsigned nCurTick = nDelay;

	if( (::GetTickCount() - nCurTick ) >= nDelay )
	{
		nCurTick = GetTickCount();

		m_pAudio->PlayTheSound("ship_thrust");
	}
}

/*!****************************************************************************
* @brief	Manages the explosion of the spaceship
******************************************************************************/
void TShip::Explode()
{
	assert(m_pAudio);

	SetAlive(false);
	SetVisible(false);

	m_pAudio->PlayTheSound("ship_explosion");
	m_nExplosionTicks = SHIP_EXPLOSIONTICKS;

									// debris initial conditions
    Split(m_Shape, m_Debris);
	m_DebrisStartPos = TVector2(0,0);

	m_DebrisRotations.clear();
    m_DebrisTranslations.clear();

	double RotVal = 2.5; // in degrees
    double ShiftVal = GetSize().Length() * 0.01;

    for(int i=0;i<m_Debris.size();i++)
    {
		m_DebrisRotations.push_back( maths::Rand(RotVal) );
		m_DebrisTranslations.push_back(
        	MidPoint(m_Debris[i][0], m_Debris[i][1]) * maths::AbsRand(ShiftVal));
    }
}

/*!****************************************************************************
* @brief	Handles the explosion of the spaceship
******************************************************************************/
void TShip::DoExplosion()
{
	assert(m_pVideo);

	m_nExplosionTicks--;

	int nCurTick = SHIP_EXPLOSIONTICKS - m_nExplosionTicks;

	if( m_nExplosionTicks > 0 )
    {
		BYTE Brightness = 255.0/ double(SHIP_EXPLOSIONTICKS) * m_nExplosionTicks;

		for(int i=0; i<m_Debris.size(); i++)
        {
											// local transformations
			Rotate(m_Debris[i], m_DebrisRotations[i]);
            Translate(m_Debris[i], m_DebrisTranslations[i]);
        }

		TVecVecPoints Shape = m_Debris;

											// global transformations
        Rotate(Shape, m_Rot);
											// no!: appare "innaturale"
        //Translate(Shape, m_Pos);
											// si!: tien conto della quantita'
                                            // di moto che possiede l'astronave
                                            // al momento dell'esplosione.
                                            // ( moltiplica per un fattore
                                            // limitativo, ad esempio 0.5 )
		double LimitingFactor = 0.5;
        m_DebrisStartPos.X += m_Vel.X * DT * LimitingFactor;
        m_DebrisStartPos.Y += m_Vel.Y * DT * LimitingFactor;
        Translate(Shape, m_Pos + m_DebrisStartPos);

											// draw the ship debris
		m_pVideo->DrawLines(Shape, 0, RGB(Brightness, Brightness, Brightness));
    }
}

/*!****************************************************************************
* @brief	Collision detection
* @param	Pos Position to check for collision
******************************************************************************/
bool TShip::IsColliding(TVector2 Pos)
{
	return bool( Distance(GetPos(), Pos) <= m_Size.X);
	//return bool( GetPos(pShip).Distance(Pos) <= m_Size.X);
}

/*!****************************************************************************
* @brief Activate the shield to protect the ship against alien missiles.
* @param pShip Pointer to the ship data structure
******************************************************************************/
void TShip::ActivateTheShield()
{
	if( m_nShieldTick > SHIELDTICKS )
	{
		m_nShieldTick = 0;
		m_bShield = true;

		m_pAudio->PlayTheSound("shield");
	}
}

/*!****************************************************************************
* @brief Return the status of the shield
* @param pShip Pointer to the ship
******************************************************************************/
bool TShip::IsShieldActive()
{
	return m_bShield;
}

/*!****************************************************************************
* @brief	Splits a vector of polylines in a vector of segments
* @param[in] Pts Reference to a vector of polylines
* @param[out] Splits Reference to a vector of segments
******************************************************************************/
void TShip::Split(TVecVecPoints& Pts, TVecVecPoints& Splits)
{
 	Splits.clear();

    for(int i=0; i<Pts.size(); i++)
    {
        for(int j=0; j<Pts[i].size()-1; j++)
        {
            TVecPoints Temp;
            Temp.push_back(Pts[i][j]);
            Temp.push_back(Pts[i][j+1]);

            Splits.push_back(Temp);
        }
    }
}

/*!****************************************************************************
* @brief	Updates by time the spaceship status
* @param	Dt The value for the delta time
******************************************************************************/
void TShip::Update(double Dt)
{
	assert(m_pVideo);
	assert(m_Shape.size() > 0);

	if ( IsAlive() )
	{
		if( GetClass() == scHuman )
		{
			m_nShieldTick++;
			if( m_nShieldTick > SHIELDTICKS )
			{
				m_bShield = false;
			}

			TVecVecPoints Shape = m_Shape;

			Rotate(Shape, m_Rot);
			TVector2 Vel = GetVel();
			m_Pos.X += Vel.X * Dt;
			m_Pos.Y += Vel.Y * Dt;
			Translate(Shape, m_Pos);

			m_pVideo->DrawLines(Shape, 0, m_Color);

												// draw the engine
			m_nImpulseTicks--;

			if (m_nImpulseTicks > 0)
			{
				TVecVecPoints Engine = m_Engine;

				Rotate(Engine, m_Rot);
				Translate(Engine, m_Pos);

				m_pVideo->DrawLines(Engine, 0, m_Color);
			}

											// draw the shield
			if( IsShieldActive() )
			{
				TVecVecPoints Shield = m_Shield;

				Translate(Shield, m_Pos);

											// some special effects ...
				{
					static int nCounter = 0, nMaxCount = 4;
					if( nCounter++ > nMaxCount ) nCounter = 0;
					double ShadeLevel = double(nCounter) / double(nMaxCount);

											// ... blink the shield when time is running out
					if( m_nShieldTick > SHIELDTICKS*3.0/4.0)
					{
						COLORREF nColor = RGB(m_Color * ShadeLevel,
                        	m_Color * ShadeLevel, m_Color * ShadeLevel);

						//m_pVideo->DrawLines(Shield, 0, m_Color * ShadeLevel);
						m_pVideo->DrawLines(Shield, 0, nColor);
					}
					else
					{
						//m_pVideo->DrawLines(Shield, 0, m_Color);
						m_pVideo->DrawLines(Shield, 0, RGB(m_Color, m_Color, m_Color));
					}
				}
			}
		}
		else
		{
			static int nCounter = 0;
			nCounter++;

			TVecVecPoints Shape = m_Shape;

			TVector2 Vel = GetVel();

			double Module = 5.0;

			if( nCounter > 25 )
			{
				nCounter = 0;

				Vel.Y += maths::Rand(2.0*Module);
				Vel.X += maths::AbsRand(Module);
			}

			SetVel(Vel);

			m_Pos.X += Vel.X * Dt;
			m_Pos.Y += Vel.Y * Dt;
			Translate(Shape, m_Pos);

			m_pVideo->DrawLines(Shape, 0, m_Color);

												// draw the engine
			m_nImpulseTicks--;

			if (m_nImpulseTicks > 0)
			{
				TVecVecPoints Engine = m_Engine;

				Rotate(Engine, m_Rot);
				Translate(Engine, m_Pos);

				m_pVideo->DrawLines(Engine, 0, m_Color);
			}
		}
	}
	else if ( IsExploding() )
	{
		DoExplosion();
	}
}

