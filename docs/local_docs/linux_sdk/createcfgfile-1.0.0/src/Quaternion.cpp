/************************************************************************

		POLHEMUS PROPRIETARY

		Polhemus 
		P.O. Box 560
		Colchester, Vermont 05446
		(802) 655-3159



        		
	    Copyright © 2005 by Polhemus
		All Rights Reserved.


*************************************************************************/

// Quaternion.cpp: implementation of the CQuaternion class.
//
//////////////////////////////////////////////////////////////////////

#include "Quaternion.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




// Function name	: CQuaternion::CQuaternion
// Description	    : Default Constructor -- Orientaton 0,0,0
// Return type		: 
CQuaternion::CQuaternion()
{
	q0=1.0f;
	q1=q2=q3=0.0f;
}

CQuaternion::~CQuaternion()
{

}


// Function name	: CQuaternion::CQuaternion
// Description	    : Constructs class from array of four floats
// Argument         : float q[]
CQuaternion::CQuaternion(const float q[])
{
	q0=q[0];
	q1=q[1];
	q2=q[2];
	q3=q[3];
	Normalize();

}


// Function name	: CQuaternion::CQuaternion
// Description	    : Constructs class from 4 individual floats
// Argument         : float quat0
// Argument         : float quat1
// Argument         : float quat2
// Argument         : float quat3
CQuaternion::CQuaternion(float quat0, float quat1, float quat2, float quat3)
{
	q0=quat0;
	q1=quat1;
	q2=quat2;
	q3=quat3;
	Normalize();
}


// Function name	: CQuaternion::CQuaternion
// Description	    : Constructs class from az,el,roll in either deg or radians
// Argument         : float az
// Argument         : float el
// Argument         : float rl
// Argument         : bool deg - bool to indicate that angles are in degrees.  Default is true
CQuaternion::CQuaternion(float az, float el, float rl,bool deg)
{
	float eul[3]={az,el,rl};
	SetFromEulers(eul,deg);

}


// Function name	: CQuaternion::CQuaternion
// Description	    : Constructs class from an axis and an angle of rotation about that axis
// Argument         : float vect[] - 3 values representing the vector which is the axis of rotation
// Argument         : float angle - The angle to rotate about vect.  Degrees or Radians
// Argument         : bool deg - bool to indicate that angles are in degrees.  Default is true
CQuaternion::CQuaternion(const float vect[], float angle, bool deg)
{
	if (deg)
		angle*=DEG2RADS;

	float halfAng=angle/2.0f;

	q0=(float)cos(halfAng);
	q1=vect[0]*(float)sin(halfAng);
	q2=vect[1]*(float)sin(halfAng);
	q3=vect[2]*(float)sin(halfAng);

	Normalize();
}



// Function name	: CQuaternion::GetEuler
// Description	    : Returns the Euler angles associated with this quaternion in deg or rads
// Return type		: void 
// Argument         : float& az - azimuth returned here.
// Argument         : float& el - elevation returned here.
// Argument         : float& rl - roll returned here.
// Argument         : bool deg - If true returned angles are in degrees, otherwise radians.  Default is true
void CQuaternion::GetEuler(float& az, float& el, float& rl,bool deg) const
{
	float mat[3][3];
	float sinAz,sinEl,sinRl,cosAz,cosEl,cosRl;

	// create orthogonal Attitude matrix
	GetAttMat(mat);

	sinEl=-mat[2][0];
	if (sinEl>1.0f)   // protect from round errors causing nans
	  sinEl=1.0f;
	else if (sinEl<-1.0f)
	  sinEl=-1;

	cosEl=(float)sqrt(1.0f-(sinEl*sinEl));

	if (fabs(cosEl)<0.001f){
		sinAz=0.0f;
		cosAz=1.0f;
	}

	else {
		sinAz=mat[1][0]/cosEl;
		cosAz=mat[0][0]/cosEl;
	}

	sinRl=sinAz*mat[0][2]-cosAz*mat[1][2];
	cosRl=-sinAz*mat[0][1]+cosAz*mat[1][1];
	
	az=(float)atan2((double)sinAz,(double)cosAz);
	el=(float)atan2((double)sinEl,(double)cosEl);
	rl=(float)atan2((double)sinRl,(double)cosRl);

	if (deg){			// convert to degrees
		az/=DEG2RADS;
		el/=DEG2RADS;
		rl/=DEG2RADS;
	}

}


// Function name	: CQuaternion::GetEuler
// Description	    : Returns the Euler angles associated with this quaternion in deg or rads
// Return type		: void 
// Argument         : float aer[] - array of 3 floats where az,el,roll are returned.
// Argument         : bool deg - If true returned angles are in degrees, otherwise radians.  Default is true
void CQuaternion::GetEuler(float aer[],bool deg) const
{
	GetEuler(aer[0],aer[1],aer[2],deg);
}


// Function name	: CQuaternion::Normalize
// Description	    : normalizes the quat to a unit quaternion
// Return type		: void 
void CQuaternion::Normalize()
{
	float mag=(float)sqrt(q0*q0+q1*q1+q2*q2+q3*q3);

	if (q0<0.0f)
		mag*=-1.0f;		// make first entry pos
	
		q0/=mag;
		q1/=mag;
		q2/=mag;
		q3/=mag;

}


// Function name	: CQuaternion::GetAttMat
// Description	    : Returns the attitude matrix associated with this quaternion
// Return type		: void 
// Argument         : float mat[][3] - 3x3 float array where att matrix is returned.
void CQuaternion::GetAttMat(float mat[][3]) const
{
	// create orthogonal Attitude matrix
	mat[0][0]=q0*q0+q1*q1-q2*q2-q3*q3;
	mat[0][1]=2*(q1*q2-q0*q3);
	mat[0][2]=2*(q1*q3+q0*q2);
	mat[1][0]=2*(q0*q3+q1*q2);
	mat[1][1]=q0*q0-q1*q1+q2*q2-q3*q3;
	mat[1][2]=2*(q2*q3-q0*q1);
	mat[2][0]=2*(q1*q3-q0*q2);
	mat[2][1]=2*(q0*q1+q2*q3);
	mat[2][2]=q0*q0-q1*q1-q2*q2+q3*q3;

}






// Function name	: *
// Description	    : multiplies two quaternion.
// Return type		: CQuaternion - The product of the multiplication. 
// Argument         : const CQuaternion& quat2 - The quat to mult this quat by.
CQuaternion CQuaternion::operator *(const CQuaternion& quat2) const
{

	CQuaternion prod;

	prod.q0 = q0*quat2.q0 - q1*quat2.q1 - q2*quat2.q2 - q3*quat2.q3;
	prod.q1 = q0*quat2.q1 + q1*quat2.q0 + q2*quat2.q3 - q3*quat2.q2;
	prod.q2 = q0*quat2.q2 - q1*quat2.q3 + q2*quat2.q0 + q3*quat2.q1;
	prod.q3 = q0*quat2.q3 + q1*quat2.q2 - q2*quat2.q1 + q3*quat2.q0;
	
	prod.Normalize();
	return prod;
}

// Function name	: *
// Description	    : Multiplies quaternion by a scaler
// Return type		: CQuaternion -- the product 
// Argument         : const float scaler
CQuaternion CQuaternion::operator *(const float scaler) const
{
	CQuaternion prod;
	prod.q0=q0*scaler;
	prod.q1=q1*scaler;
	prod.q2=q2*scaler;
	prod.q3=q3*scaler;

	return prod;
}



// Function name	: *=
// Description	    : Multiply and assign eg.  thisQuat*=thatQuat--->thisQuat=thisQuat*thatQuat
// Return type		: void  
// Argument         : const CQuaternion &quat - The quat to mult this quat by.
void CQuaternion::operator *=(const CQuaternion &quat)
{
	CQuaternion prod=*this * quat;
	*this=prod;
}


// Function name	: CQuaternion::GetAxisAngle
// Description	    : Returns the axis and angle of rotation that is associated with this quaternion 
// Return type		: void 
// Argument         : float vect[] - array of 3 floats to receive vector representing the axis
// Argument         : float &angle - reference to received the angle.
// Argument         : bool deg - if true angle will be in degrees, otherwise radians.  Default is true.
void CQuaternion::GetAxisAngle(float vect[], float &angle, bool deg) const
{
	angle=2.0f*(float)acos(q0);
	if (deg)
		angle/=DEG2RADS;

	float scale=(float)sqrt(q1*q1+q2*q2+q3*q3);

	if (fabs(scale)<0.001f){		// infinite axis, set to no rotation
		angle=0.0f;
		vect[0]=1.0f;
		vect[1]=vect[2]=0.0f;
		return;
	}

	vect[0]=q1/scale;
	vect[1]=q2/scale;
	vect[2]=q3/scale;

	scale=(float)sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]);
	for (int i=0;i<3;i++)
		vect[i]/=scale;
}


// Function name	: - 
// Description	    : Returns inverse of quaternion.  eg -thisQuat --> InvthisQuat
// Return type		: CQuaternion - The inverse of this quaternion 
CQuaternion CQuaternion::operator -() const
{
	CQuaternion inv=*this;
	inv.q1*=-1;
	inv.q2*=-1;
	inv.q3*=-1;
	return inv;
}



// Function name	: CQuaternion::GetDeltaQuat
// Description	    : Returns the quaternion that represents the difference between two quaternions
// Return type		: CQuaternion - the delta quaternion
// Argument         : const CQuaternion &quat - reference to a quaternion to measure the difference between
CQuaternion CQuaternion::GetDeltaQuat(const CQuaternion &quat) const
{
/*	CQuaternion inv=-(*this);
	return inv*quat;
*/
	CQuaternion inv=-quat;
	return *this * inv;

}


// Function name	: - 
// Description	    : Also retrieves the delta quat. eg.  thisQuat-thatQuat --> GetDeltaQuat(thatQuat)
//					  Yes this is really a multiplicaton process, but it's kind of intuitive.
// Return type		: CQuaternion CQuaternion::operator 
// Argument         : const CQuaternion &quat
CQuaternion CQuaternion::operator -(const CQuaternion &quat) const
{
	return GetDeltaQuat(quat);
}


// Function name	: CQuaternion::GetQuatVal
// Description	    : returns the four values of the quaternion
// Return type		: void 
// Argument         : float vals[]	- array of 4 floats to recieve the quat values
void CQuaternion::GetQuatVal(float vals[]) const
{
	vals[0]=q0;
	vals[1]=q1;
	vals[2]=q2;
	vals[3]=q3;
	
}


// Function name	: CQuaternion::GetQuatVal
// Description	    : Returns an individual value of the quaternion
// Return type		: float - the requested value of the quaternion
// Argument         : int ind - the index (0-3) of the value to return. Invalid values return q0.
float CQuaternion::GetQuatVal(int ind) const
{
	float retVal;
	switch (ind) {
	case 0:
	default:
		retVal=q0;
		break;
	case 1:
		retVal=q1;
		break;
	case 2:
		retVal=q2;
		break;
	case 3:
		retVal=q3;
	}

	return retVal;
}


// Function name	: CQuaternion::SetQuatVals
// Description	    : Sets the values of the quaternion as indicated.
// Return type		: void 
// Argument         : float val[] - Array of 4 floats that indicate the values to set the quaternion members to.
void CQuaternion::SetQuatVals(const float val[])
{
	SetQuatVals(val[0],val[1],val[2],val[3]);
}


// Function name	: CQuaternion::SetQuatVals
// Description	    : Sets the values of the quaternion as indicated by the individual parameters
// Return type		: void 
// Argument         : float w	- set first quaternion value to this
// Argument         : float x	- set second quaternion value to this
// Argument         : float y	- set third quaternion value to this
// Argument         : float z	- set fourth quaternion value to this
void CQuaternion::SetQuatVals(const float w, const float x, const float y, const float z)
{
	q0=w;
	q1=x;
	q2=y;
	q3=z;
	Normalize();

}

bool CQuaternion::IsIdentity()
{
	return ((q0==1.0f) && (q1==0.0f) && (q2==0.0f) && (q3==0.0f));
}

void CQuaternion::Eul2Quat(float *quat, float *eul, bool isDeg)
{
	CQuaternion q(eul[0],eul[1],eul[2],isDeg);
	q.MakeLargestElementPos();
	q.GetQuatVal(quat);
}

void CQuaternion::Quats2Eul(float *eul, float *quats, bool isDeg)
{
	CQuaternion q(quats);
	q.GetEuler(eul,isDeg);
}

void CQuaternion::MakeLargestElementPos()
{
	float max=(float)fabs(q0);
	bool bChgSign=q0<0;


	if ((float)fabs(q1)>max){
		max=(float)fabs(q1);
		bChgSign=q1<0;
	}

	if ((float)fabs(q2)>max){
		max=(float)fabs(q2);
		bChgSign=q2<0;
	}

	if ((float)fabs(q3)>max){
		max=(float)fabs(q3);
		bChgSign=q3<0;
	}

	if (bChgSign){
		q0*=-1;
		q1*=-1;
		q2*=-1;
		q3*=-1;
	}
}

void CQuaternion::SetFromEulers(float *eul, bool deg/*=true*/)
{
	float azHalf=eul[0]/2.0f;
	float elHalf=eul[1]/2.0f;
	float rollHalf=eul[2]/2.0f;

	if (deg){
		azHalf*=DEG2RADS;
		elHalf*=DEG2RADS;
		rollHalf*=DEG2RADS;
	}

	q0=(float)(cos(azHalf)*cos(elHalf)*cos(rollHalf)+sin(azHalf)*sin(elHalf)*sin(rollHalf));
	q1=(float)(cos(azHalf)*cos(elHalf)*sin(rollHalf)-sin(azHalf)*sin(elHalf)*cos(rollHalf));
	q2=(float)(cos(azHalf)*sin(elHalf)*cos(rollHalf)+sin(azHalf)*cos(elHalf)*sin(rollHalf));
	q3=(float)(sin(azHalf)*cos(elHalf)*cos(rollHalf)-cos(azHalf)*sin(elHalf)*sin(rollHalf));


	Normalize();
	
}

// Function name	: CQuaternion::Slerp
// Description	    : Spherical interpolation of quaternions.  Interpolates between
//						this quaternion and the quaternion passed in as a parameter
// Return type		: CQuaternion -- The interpolated quaternion
// Argument         : const CQuaternion &q
// Argument         : float t -- value between 0 and 1 which indicates the distance between 
//						quaternions to interpolate
CQuaternion CQuaternion::Slerp(const CQuaternion &q, float t) const
{

	//    qa*sin((1-t)theta)+qb*sin(t*theta)
	// q= ----------------------------------
	//                sin(theta)

	// 2*theta = qa.q0*qb.q0+qa.q1*qb.q1+qa.q2*qb.q2+qa.q3*qb.q3




	float angle;

	if (t>=1.0f)
		return CQuaternion(q);

	if (t<=0.0f)
		return CQuaternion(*this);


	float dot=q0*q.q0+q1*q.q1+q2*q.q2+q3*q.q3;
	angle=(float)(acos(dot))/2.0f;
	if (angle<0.0f)
		angle*=-1;

	float coeff1,coeff2;

	if (angle==0.0f){
		coeff1=1-t;
		coeff2=t;
	}
	else {

		coeff1=(float)sin((1.0f-t)*angle)/(float)sin(angle);
		coeff2=(float)sin(t*angle)/(float)sin(angle);
	}

	CQuaternion res=*this*coeff1+q*coeff2;
	res.Normalize();

	return res;
}

// Function name	: +
// Description	    : Adds to quaternions
// Return type		: CQuaternion -- the sum
// Argument         : const CQuaternion &q1
// Argument         : const CQuaternion &q2
CQuaternion CQuaternion::operator +(const CQuaternion &q) const
{
	CQuaternion sum;
	sum.q0=q0+q.q0;
	sum.q1=q1+q.q1;
	sum.q2=q2+q.q2;
	sum.q3=q3+q.q3;

	return sum;

}




void CQuaternion::GetAngle(float &a,bool deg)
{
	a=2.0f*(float)acos(q0);
	if (deg)
		a/=DEG2RADS;

}
