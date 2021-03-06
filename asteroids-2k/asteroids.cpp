/*!****************************************************************************

	@file	asteroids.h
	@file	asteroids.cpp

	@brief	Asteroids modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <math.h>

#include "maths.h"
#include "utils.h"
#include "asteroids.h"
#include "commdefs.h"


#define ASTEROID_MAXVERTS	16


/*!****************************************************************************
* @brief	Constructor
* @param	pVM Pointer to the video manager object
* @param	pSM Pointer to the sound manager object
* @param	nClass Class of the asteroid (e.g.: big, medium, small)
* @param	Pos The initial position for the asteroid
* @param	Vel The initial velocity for the asteroid
* @param	Radius The size (e.g. radius) of the asteroid
******************************************************************************/
TAsteroid::TAsteroid(TVideoManager* pVM, TSoundManager* pSM,
	enAsteroidClass nClass, TVector2 Pos, TVector2 Vel, double Radius)
{
	assert(pVM);
	assert(pSM);

	m_pVideo = pVM;
	m_pAudio = pSM;
	m_nClass = nClass;
	m_Pos = Pos;
	m_Vel = Vel;
	m_bAlive = true;
	m_Radius = Radius;
	m_nClass = nClass;
    m_bVisible = true;
	m_Radius = Radius;
	m_nExplosionTicks = 0;
	m_Color = RGB(255,255,255);

	m_Shape = RandShape(Radius);

	m_Rot = 0;
	m_DRot = rand()/double(RAND_MAX) * Vel.Length() * 0.25 * maths::RandSign();
}

/*!****************************************************************************
* @brief	Destructor
******************************************************************************/
TAsteroid::~TAsteroid()
{
	;
}

/*!****************************************************************************
* @brief	Checks for the asteroid "aliveness"
* @return	True if the asteroid is "alive", false otherwise
******************************************************************************/
bool TAsteroid::IsAlive()
{
	return m_bAlive;
}

/*!****************************************************************************
* @brief	Checks if the asteroid is colliding in a specified position
* @return	True if the asteroid is colliding in Pos, false otherwise
******************************************************************************/
bool TAsteroid::Collide(TVector2 Pos)
{
	return bool ( Distance(Pos, this->m_Pos) <= this->m_Radius );
}

/*!****************************************************************************
* @brief	Sets the position of the asteroid
* @param	Pos The position to be set for the asteroid
******************************************************************************/
void TAsteroid::SetPos(TVector2 Pos)
{
	m_Pos = Pos;
}

/*!****************************************************************************
* @brief	Gets the position of the asteroid
* @return	The position of the asteroid
******************************************************************************/
TVector2 TAsteroid::GetPos()
{
	return m_Pos;
}

/*!****************************************************************************
* @brief	Gets the veclocity of the asteroid
* @return	The velocity of the asteroid
******************************************************************************/
TVector2 TAsteroid::GetVel()
{
	return this->m_Vel;
}

/*!****************************************************************************
* @brief	Gets the class of the asteroid
* @return	The class of the asteroid (e.g. small, medium or big)
******************************************************************************/
enAsteroidClass TAsteroid::GetClass()
{
	return this->m_nClass;
}

/*!****************************************************************************
* @brief	Generates randomly an asteroid shape
* @param	Size The size of the asteroid to be generated
******************************************************************************/
TVecPoints TAsteroid::RandShape(double Size)
{
	TVecPoints Pts;
	double Angle = 0;
	double DAngle = 360.0/ASTEROID_MAXVERTS;

	for(int i=0; i<ASTEROID_MAXVERTS; ++i)
	{
		Pts.push_back( TVector2(
        	Size * cos( DEG2RAD(Angle)) + maths::Rand(GetRoughness()),
			Size * sin( DEG2RAD(Angle)) + maths::Rand(GetRoughness()) ));

		Angle += DAngle;
	}

	return Pts;
}

/*!****************************************************************************
* @brief	Gets the "roughness" of asteroid
* @return	Return the roughness by mean of the "class" (size) of asteroid
******************************************************************************/
double TAsteroid::GetRoughness()
{
    double Roughness = 5.0;

    if( m_nClass == acBig ) Roughness = 5.0;
    else if( m_nClass == acMedium ) Roughness = 3.0;
    else Roughness = 2.0;

    return Roughness;
}

/*!****************************************************************************
* @brief	Gets the size of the asteroid
* @return	The Size (radius) of the asteroid
******************************************************************************/
double TAsteroid::GetSize()
{
 	return m_Radius;
}

/*!****************************************************************************
* @brief	Sets the asteroid "aliveness"
* @return	True for alive, false otherwise
******************************************************************************/
void TAsteroid::SetAlive(bool bAlive)
{
 	m_bAlive = bAlive;
}

/*!****************************************************************************
* @brief	Sets the asteroid visibility
******************************************************************************/
void TAsteroid::SetVisible(bool bVisible)
{
 	m_bVisible = bVisible;
}

/*!****************************************************************************
* @brief	Handles the asteroid crashing
******************************************************************************/
void TAsteroid::Crash()
{
    SetAlive(false);
}

/*!****************************************************************************
* @brief	Check if the asteroid is exploding
* @return	True if the asteroid is exploding, false otherwise
******************************************************************************/
bool TAsteroid::IsExploding()
{
	return bool(m_nExplosionTicks > 0);
}

/*!****************************************************************************
* @brief	Starts the asteroid explosion
******************************************************************************/
void TAsteroid::Explode()
{
	assert(m_pAudio);

	SetAlive(false);
	SetVisible(false);

    if( GetClass() == acBig ) { m_pAudio->PlayTheSound("bang_large"); }
    else if( GetClass() == acMedium ) { m_pAudio->PlayTheSound("bang_medium"); }
    else { m_pAudio->PlayTheSound("bang_small"); }

	m_nExplosionTicks = ASTEROID_EXPLOSIONTICKS;

	m_DebrisStartPos = m_Pos;

												// builds the debris
	{
        m_nDebris = ASTEROID_NDEBRIS/2.0 + maths::AbsRand(ASTEROID_NDEBRIS)/2.0;

        double Scale = 8.0;
        int nSize = GetSize();
        double DAngle = (2.0 * M_PI) / m_nDebris;

        for(int i=0; i < m_nDebris; ++i)
        {
            double Radius = nSize / 4.0 + abs(maths::Rand(nSize));

            m_Debris[i].X = m_Pos.X + cos(i*DAngle) * Radius;
            m_Debris[i].Y = m_Pos.Y + sin(i*DAngle) * Radius;

            m_DebrisScales[i] = maths::AbsRand(Scale);
            m_DebrisScales[i] = maths::AbsRand(Scale);
        }
	}
}

/*!****************************************************************************
* @brief 	Handles the asteroid explosion
******************************************************************************/
void TAsteroid::DoExplosion()
{
	assert(m_pVideo);

	double Scale = 16;
	m_nExplosionTicks--;

	int nCurTick = ASTEROID_EXPLOSIONTICKS - m_nExplosionTicks;

											// Tien conto della quantita'
                                            // di moto che ha l'asteroide
                                            // al momento dell'esplosione.
    m_DebrisStartPos.X += m_Vel.X * DT;
    m_DebrisStartPos.Y += m_Vel.Y * DT;

	if( m_nExplosionTicks > 0 )
	{
		BYTE Brightness = 255.0/ double(ASTEROID_EXPLOSIONTICKS) * m_nExplosionTicks;

		for(int i=0; i<m_nDebris; i++)
		{
			double X = m_DebrisStartPos.X
            	+ (m_Debris[i].X - m_Pos.X)
                * (Scale + m_DebrisScales[i]) / 100.0 * nCurTick ;
			double Y = m_DebrisStartPos.Y
            	+ (m_Debris[i].Y - m_Pos.Y)
            	* (Scale + m_DebrisScales[i]) / 100.0 * nCurTick ;

			TVector2 Pos( X, Y );
			m_pVideo->DrawPoint(Pos, RGB(Brightness, Brightness, Brightness) );
		}
	}
}

/*!****************************************************************************
* @brief	Updates the asteroid status
* @param	Dt The value for the delta time
******************************************************************************/
void TAsteroid::Update(double Dt)
{
	assert(m_pVideo);

	if( IsAlive() )
    {
        m_Pos.X += m_Vel.X * Dt;
        m_Pos.Y += m_Vel.Y * Dt;

        TVecPoints Shape = this->m_Shape;

        m_Rot += m_DRot;

        Rotate(Shape, m_Rot);
        Translate(Shape, m_Pos);

        m_pVideo->DrawLines(Shape, 0, m_Color, true);
	}
    else if( IsExploding() )
    {
		DoExplosion();
    }
}


