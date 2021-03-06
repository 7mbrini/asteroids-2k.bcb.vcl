/*!****************************************************************************

	@file	weapons.h
	@file	weapons.cpp

	@brief	Weapons modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>

#include "weapons.h"

/*!****************************************************************************
* @brief	Builds the missile
* @param	pVM Pointer to the video manager data structure
******************************************************************************/
TWeapon::TWeapon(TVideoManager* pVM)
{
	assert(pVM);

	m_pVM = pVM;
	m_bArmed = false;

	m_Pos = TVector2(-10,-10);
	m_Vel = TVector2(0,0);
}

/*!****************************************************************************
* @brief	Builds the missile
* @param	pVM Pointer to the video manager data structure
* @param	Pos Initial position of the missile
* @param	Vel Initial velocity of the missile
******************************************************************************/
TWeapon::TWeapon(TVideoManager* pVM, TVector2 Pos, TVector2 Vel)
{
	assert(pVM);

	m_Pos = Pos;
	m_Vel = Vel;
	m_pVM = pVM;
	m_bArmed = false;
}

/*!****************************************************************************
* @brief	Gets the missile position
* @return	The position of the missile
******************************************************************************/
TVector2 TWeapon::GetPos()
{
	return m_Pos;
}

/*!****************************************************************************
* @brief	Gets the status of missile
* @return	Returns true if the missile is armed, false otherwise
******************************************************************************/
bool TWeapon::IsArmed()
{
	return m_bArmed;
}

/*!****************************************************************************
* @brief	Arms the missile
* @param	Pos The position of the missile
* @param	Vel The velocity of the missile
******************************************************************************/
void TWeapon::Arm(TVector2 Pos, TVector2 Vel)
{
	m_Pos = Pos;
	m_Vel = Vel;
	m_bArmed = true;
	m_Color = RGB(255,255,255);
}

/*!****************************************************************************
* @brief	Builds the missile
* @param	pVM Pointer to the video manager data structure
******************************************************************************/
TMissile::TMissile(TVideoManager* pVM):TWeapon(pVM)
{
	m_pShip = NULL;
}

/*!****************************************************************************
* @brief	Builds the missile
* @param	pVM Pointer to the video manager data structure
* @param	Pos Initial position of the missile
* @param	Vel Initial velocity of the missile
******************************************************************************/
TMissile::TMissile(TVideoManager* pVM, TVector2 Pos, TVector2 Vel)
: TWeapon(pVM, Pos, Vel)
{
	m_pShip = NULL;
}

/*!****************************************************************************
* @brief	Set the pointer to the ship
* @param	pShip The pointer to the TShip object
******************************************************************************/
void TMissile::SetShip(TShip* pShip)
{
	m_pShip = pShip;
}

/*!****************************************************************************
* @brief	Gett the pointer to the ship
* @return	Return the pointer to the TShip object
******************************************************************************/
TShip* TMissile::GetShip()
{
	return m_pShip;
}

/*!****************************************************************************
* @brief	Updates by time the missile status
* @param	Dt The value for the delta time
******************************************************************************/
void TMissile::Update(double Dt)
{
	assert(m_pVM);

	m_Pos.X += m_Vel.X * Dt;
	m_Pos.Y += m_Vel.Y * Dt;

	m_pVM->DrawPoint(m_Pos, m_Color);
}


