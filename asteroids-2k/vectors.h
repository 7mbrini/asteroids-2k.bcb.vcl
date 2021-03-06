/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _VECTORS_H_
#define _VECTORS_H_

#include <vector>
#include <math.h>

#include <assert.h>

#include "maths.h"

namespace maths
{

class TVector2;

typedef std::vector<TVector2> TVecPoints;
typedef std::vector<TVector2*> TVecPtrPoints;
typedef std::vector<TVecPoints> TVecVecPoints;

class TVector2
{
	public:
		TVector2();
    	TVector2(const TVector2& Pt);
        TVector2(double X, double Y);

        double Length();
		void Normalize();

        TVector2 Rotate(double ThetaDeg);
        TVector2 Translate(TVector2 Translation);

        TVector2 operator + (TVector2& V);
        TVector2 operator += (TVector2& V);
        TVector2 operator * (double V);

        TVector2 operator - (TVector2& V);

	public:
		double X, Y;
};

TVector2 Add(TVector2& A, TVector2& B);
double Distance(TVector2 Pt1, TVector2 Pt2);
TVector2 MidPoint(TVector2 Pt1, TVector2 Pt2);

void Rotate(TVecPoints& VecPts, double ThetaDeg);
void Translate(TVecPoints& Src, TVector2 Translation);

void Rotate(TVecVecPoints& VecPts, double ThetaDeg);
void Translate(TVecVecPoints& VecPts, TVector2 Translation);

}	// namespace maths

#endif


