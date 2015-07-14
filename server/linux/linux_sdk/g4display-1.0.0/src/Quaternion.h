/************************************************************************

		POLHEMUS PROPRIETARY

		Polhemus 
		P.O. Box 560
		Colchester, Vermont 05446
		(802) 655-3159



        		
	    Copyright © 2005 by Polhemus
		All Rights Reserved.


*************************************************************************/

// Quaternion.h: interface for the CQuaternion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUATERNION_H__D76D8B96_F74D_41A4_869E_C3A397F9AA7A__INCLUDED_)
#define AFX_QUATERNION_H__D76D8B96_F74D_41A4_869E_C3A397F9AA7A__INCLUDED_


class CQuaternion  
{
private:
	void Normalize();
	float q0;
	float q1;
	float q2;
	float q3;
public:
	CQuaternion Slerp(const CQuaternion& q, float t) const;
	CQuaternion operator+(const CQuaternion& q1) const;
	void GetAngle(float& a,bool deg=true);
	void SetFromEulers(float* eul, bool deg=true);
	void MakeLargestElementPos();
	static void Quats2Eul(float* eul,float* quats,bool isDeg=true);
	static void Eul2Quat(float* quat,float* eul,bool isDeg=true);
	bool IsIdentity();
	void SetQuatVals(const float w,const float x,const float y,const float z);
	void SetQuatVals(const float val[4]);
	float GetQuatVal(int ind) const;
	void GetQuatVal(float vals[4]) const;
	CQuaternion operator-(const CQuaternion& quat)const;
	CQuaternion GetDeltaQuat(const CQuaternion& quat) const;
	void GetAxisAngle(float vect[3],float& angle,bool deg=true) const;
	CQuaternion operator -() const;
	CQuaternion operator*(const CQuaternion& quat2) const;
	CQuaternion operator*(const float ) const;
	void operator *=(const CQuaternion &quat);
	void GetAttMat(float mat[3][3]) const;
	void GetEuler(float aer[3],bool deg=true) const;
	void GetEuler(float& az,float& el,float& rl,bool deg=true) const;
	CQuaternion(const float vect[3],float angle,bool deg=true);
	CQuaternion(float q0,float q1,float q2,float q3);
	CQuaternion(const float q[4]);
	CQuaternion(float az, float el,float roll,bool deg=true);
	CQuaternion();
	virtual ~CQuaternion();

};

#endif // !defined(AFX_QUATERNION_H__D76D8B96_F74D_41A4_869E_C3A397F9AA7A__INCLUDED_)
