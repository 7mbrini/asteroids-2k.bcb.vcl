/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _MATHS_H_
#define _MATHS_H_

#include <vector>
#include <math.h>

#include <assert.h>


#define RAD2DEG(X) ((X)*180.0/M_PI)
#define DEG2RAD(X) ((X)*M_PI/180.0)

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

typedef std::vector<int> TVecIntegers;

namespace maths
{
    int RandSign();
    int Sign(double Val);
    double Rand(double Val);
    double AbsRand(double Val);

    void Sort(TVecIntegers& VecInts);
}

#endif

