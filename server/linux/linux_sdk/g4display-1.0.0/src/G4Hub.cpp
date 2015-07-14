// G4Hub.cpp: implementation of the CG4Hub class.
//
//////////////////////////////////////////////////////////////////////


#include <GL/gl.h>
#include <GL/glu.h>
#include "G4Hub.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CG4Hub::CG4Hub()
{
	memset(m_col,0,sizeof(float)*3);	// black
	for (int i=0;i<3;i++)
		m_senActive[i]=false;
}

CG4Hub::~CG4Hub()
{
}

void CG4Hub::SetPnoData(float *pno,int sens)
{
  //	float scale=2.0f;
  //	float scale=0.01;
  float scale=1.0f;
	memcpy(m_pno[sens],pno,sizeof(float)*6);
	for (int i=0;i<3;i++)
		m_pno[sens][i]*=scale;
	m_senActive[sens]=true;
}

int CG4Hub::GetId()
{
	return m_id;
}



void CG4Hub::SetId(int id)
{
	m_id=id;
}
//#include <stdio.h>
void CG4Hub::Render(GLUquadric* quadric,float scale)
{
	for (int i=0;i<G4_SENSORS_PER_HUB;i++){

		if (!m_senActive[i])
			continue;

		//	printf("Rendering hub %d, sens %d\n",m_id,i);
		//	m_pno[i][0]+=(m_id*2);
// axes mapping
//  Trkr	OpenGl
//	 x		  -z
//	 y		   x
//	 z		  -y

	glColor3fv(m_col[i]);
	//	printf("%.3f %.3f %.3f\n",m_pno[i][0],m_pno[i][1],m_pno[i][2]);

	glPushMatrix();
		glTranslatef(m_pno[i][1],-m_pno[i][2],-m_pno[i][0]);	// ogl(x,y,z)-> trk(y,-z,-x)
		glRotatef(m_pno[i][3],0.0f,-1.0f,0.0f);		// az  rot about trk(z), ogl(-y)
		glRotatef(m_pno[i][4],1.0f,0.0f,0.0f);		// el  rot about trk(y), ogl(x)
		glRotatef(m_pno[i][5],0.0f,0.0f,-1.0f);		// rl  rot about trk(x), ogl(-z)

	glPushMatrix();
	//////////
	glScalef(scale,scale,scale);
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	glPushMatrix();

	glEnable(GL_LINE_SMOOTH);

	glPushMatrix();

	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(	quadric, 
					0.5f,			//GLdouble baseRadius,
					0.5f,			//GLdouble topRadius,
					3.0f,			//GLdouble height,
					40,				//GLint slices,
					2 );			//GLint stacks
	glTranslatef(0.0f, 0.0f, 3.0f);
	gluCylinder(	quadric,
					0.5,
					0.0,
					1.0,
					40,
					2 );
	glPopMatrix();

	glPushMatrix();
//	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(	quadric, 
					0.5f,			//GLdouble baseRadius,
					0.5f,			//GLdouble topRadius,
					3.0f,			//GLdouble height,
					40,				//GLint slices,
					2 );			//GLint stacks
	glTranslatef(0.0f, 0.0f, 3.0f);
	gluCylinder(	quadric,
					0.5,
					0.0,
					1.0,
					40,
					2 );
	glPopMatrix();

	// +X axis
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
///	glRotatef(180.0f, 0.0f, -1.0f, 0.0f);		// Flip z around to the pos dir -- JCF 1/20/05
	gluCylinder(	quadric, 
					0.5f,			//GLdouble baseRadius,
					0.5f,			//GLdouble topRadius,
					1.5f,			//GLdouble height,
					40,				//GLint slices,
					2 );			//GLint stacks
	glPushAttrib(GL_CURRENT_BIT);
	glColor3fv(m_invCol[i]);

	glTranslatef(0.0f, 0.0f, 1.5f);
	gluCylinder(	quadric, 
					0.5f,			//GLdouble baseRadius,
					0.5f,			//GLdouble topRadius,
					1.5f,			//GLdouble height,
					40,				//GLint slices,
					2 );			//GLint stacks

	glTranslatef(0.0f, 0.0f, 1.5f);
	gluCylinder(	quadric,
					0.5,
					0.0,
					1.0,
					40,
					2 );
	glPopAttrib();
	glPopMatrix();

//	glutSolidSphere( 0.5, 30, 30 );
	gluSphere(quadric, 0.5, 30, 30 );


	glPopMatrix();
	glPopAttrib();
 	//////////
	glPopMatrix();
	glPopMatrix();
	m_senActive[i]=false;	// reset
	}//end for i
}

void CG4Hub::SetColor(const float *col)
{
	memcpy(m_col,col,sizeof(float)*9);
	// get inverse for tip
	BYTE b;
	for (int i=0;i<3;i++){
		for (int j=0;j<3;j++){
			b=(BYTE)(col[i*3+j]*255.0);
			b=~b;
			m_invCol[i][j]=(float)b/255.0f;
		}
	}
}
