/*!****************************************************************************

	@file	vectors.h
	@file	vectors.cpp

	@brief	Vectors routines

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <assert.h>
#include <math.h>

#include "maths.h"
#include "vectors.h"

namespace maths
{

/*!****************************************************************************
* @brief	Default constructor
******************************************************************************/
TVector2::TVector2()
{
    this->X = this->Y = 0;
}

/*!****************************************************************************
* @brief	Constructor
* @param	V Referernce to a vector
******************************************************************************/
TVector2::TVector2(const TVector2& V)
{
 	this->X = V.X;
    this->Y = V.Y;
}

/*!****************************************************************************
* @brief	Constructor
* @param	X Value for X
* @param	Y Value for Y
******************************************************************************/
TVector2::TVector2(double X, double Y)
{
    this->X = X;
    this->Y = Y;
}

/*!****************************************************************************
* @brief	Returns the length (module) of the vector
* @param	Pt Referernce to a vector
******************************************************************************/
double TVector2::Length()
{
	return sqrt(this->X * this->X + this->Y * this->Y);
}

/*!****************************************************************************
* @brief	Normalizes the vector
* @param	Pt Referernce to a vector
******************************************************************************/
void TVector2::Normalize()
{
	double Len = Length();

	if (Len!= 0)
	{
		this->X /= Len;
		this->Y /= Len;
	}
}

/*!****************************************************************************
* @brief	Calculates the Sum of two vectors
* @param	A Referernce to a first vector to sum
* @param	B Referernce to a second vector to sum
* @return	The sum of vectors
******************************************************************************/
TVector2 Add(TVector2& A, TVector2& B)
{
	TVector2 Result;

	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;

	return Result;
}

/*!****************************************************************************
* @brief	Sum operator ovverride
* @param	V Referernce to a vector to sum
* @return	The vector sum with V
******************************************************************************/
TVector2 TVector2::operator + (TVector2& V)
{
    return TVector2(this->X + V.X, this->Y + V.Y);
}

/*!****************************************************************************
* @brief	Sum operator ovverride
* @param	V Referernce to a vector to sum
* @return	The resulting sum vector with V
******************************************************************************/
TVector2 TVector2::operator += (TVector2& V)
{
	this->X += V.X;
    this->Y += V.Y;

    return *this;
}

/*!****************************************************************************
* @brief	Difference operator ovverride
* @param	V Referernce to a vector to sum
* @return	The vector difference with V
******************************************************************************/
TVector2 TVector2::operator - (TVector2& V)
{
    return TVector2(V.X - this->X, V.Y - this->Y);
}

/*!****************************************************************************
* @brief	Scalar multiplication operator ovveride
* @param	V Scalar multiplier
* @return	The actual vector multiplied by scalar V
******************************************************************************/
TVector2 TVector2::operator * (double V)
{
	this->X *= V;
    this->Y *= V;

    return *this;
}

/*!****************************************************************************
* @brief	Calculates the euclidean distance between two points
* @param	Pt1 Referernce to a vector data structure
* @param	Pt2 Referernce to a vector data structure
* @return	The euclidean distance between two poins
******************************************************************************/
double Distance(TVector2 Pt1, TVector2 Pt2)
{
	return sqrt((Pt2.X - Pt1.X) * (Pt2.X - Pt1.X) + (Pt2.Y - Pt1.Y) * (Pt2.Y - Pt1.Y));
}

/*!****************************************************************************
* @brief	Calculates the middle point bewteen two vectors
* @param	Pt1 Referernce to a first point
* @param	Pt2 Referernce to a second point
* @return	The middle point between input points
******************************************************************************/
TVector2 MidPoint(TVector2 Pt1, TVector2 Pt2)
{
	return TVector2( (Pt1.X + Pt2.X)/2.0, (Pt1.Y + Pt2.Y)/2.0 );
}

/*!****************************************************************************
* @brief	Rotates the vector around the axis origin
* @param	ThetaDeg The angle of rotation, in degrees
* @return	The vector rotated by ThetaDeg around the origin
******************************************************************************/
TVector2 TVector2::Rotate(double ThetaDeg)
{
	double SinTheta = sin(DEG2RAD(ThetaDeg));
	double CosTheta = cos(DEG2RAD(ThetaDeg));

	double X = this->X;
    double Y = this->Y;

	this->X = X * CosTheta + Y * SinTheta;
	this->Y = -X * SinTheta + Y * CosTheta;

    return *this;
}

/*!****************************************************************************
* @brief			Rotates a list of points around the axis origin
* @param[in,out]	VecPts Referernce to a list of points data structures
* @param			ThetaDeg The angle of rotation, in degrees
******************************************************************************/
void Rotate(TVecPoints& VecPts, double ThetaDeg)
{
	for (int i = 0; i < VecPts.size(); ++i)
	{
        VecPts[i] = VecPts[i].Rotate(ThetaDeg);
	}
}

/*!****************************************************************************
* @brief			Rotates a list of a list of points around the axis origin
* @param[in,out]	VecPts Referernce to a list of list of points data structures
* @param			ThetaDeg The angle of rotation, in degrees
******************************************************************************/
void Rotate(TVecVecPoints& VecPts, double ThetaDeg)
{
	for(int i=0; i<VecPts.size(); ++i)
	{
		Rotate(VecPts[i], ThetaDeg);
	}
}

/*!****************************************************************************
* @brief	Translates a point 
* @param	Translation The value for translation
* @return	The input vector translated
******************************************************************************/
TVector2 TVector2::Translate(TVector2 Translation)
{
	this->X += Translation.X;
    this->Y += Translation.Y;

    return *this;
}

/*!****************************************************************************
* @brief	Translates a list of points
* @param	VecPts Referernce to a list of points data structures
* @param	Translation The value for translation
******************************************************************************/
void Translate(TVecPoints& VecPts, TVector2 Translation)
{
	for (int i = 0; i < VecPts.size(); ++i)
	{
        VecPts[i] = VecPts[i].Translate(Translation);
	}
}

/*!****************************************************************************
* @brief	Translates a list of a list of points
* @param	VecPts Referernce to a list of list of points data structures
* @param	Translation The value for translation
******************************************************************************/
void Translate(TVecVecPoints& VecPts, TVector2 Translation)
{
	for(int i=0; i<VecPts.size(); ++i)
	{
		Translate(VecPts[i], Translation);
	}
}

} // namespace maths;
