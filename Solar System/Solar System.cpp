// Solar System.cpp
// 許笙杰 E14075304 機械三乙
// BGM : Space Ambience (Credit: Vilkas Sound)

#include <windows.h>
#include <gl/GLee.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <iostream>
#include <cstring>
#include "LoadTGA.h"
#include "glFrame.h"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")
#pragma comment(lib,"glut32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#define SUN 0
#define MERCURY 1
#define VENUS 2
#define EARTH 3
#define MARS 4
#define JUPITER 5
#define SATURN 6
#define URANUS 7
#define NEPTUNE 8
#define PLUTO 9
#define MOON 10
#define OBJECT_NUM 11

using namespace std;

void SunMaterial();
void PlanetMaterial();
void LabelMaterial();
void OrbitMaterial();

class Planet
{
public:
	float radius;		// 行星半徑
	float distance;		// 行星與太陽距離
	float orbit;		// 在軌道上的位置
	float speed;		// 公轉速度
	float axisTilt;		// 自轉軸傾角
	float axisAni;		// 自轉角位置

	Planet(float _radius, float _distance, float _orbit, float _speed, float _axisTilt, float _axisAni)	// 建構子
	{
		radius = _radius;
		distance = _distance;
		orbit = _orbit;
		speed = _speed;
		axisTilt = _axisTilt;
		axisAni = _axisAni;
	}

	void DrawSatelliteOrbit(void)	// 畫衛星軌道
	{
		glPushMatrix();
		OrbitMaterial();
		glColor3f(0.3f, 0.3f, 0.3f);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutWireTorus(0.001, distance, 100.0, 100.0);
		glPopMatrix();
	}

	void DrawMoon(void)	// 畫衛星
	{
		GLUquadricObj* quadric;
		quadric = gluNewQuadric();
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		glRotatef(orbit, 0.0, 1.0, 0.0);
		glTranslatef(distance, 0.0, 0.0);
		gluSphere(quadric, radius, 40.0, 40.0);
		glPopMatrix();

	}
};

// 天體定義
// radius(1:10000km) (太陽被縮小10倍)
// distance 20 = 1AU
Planet sun(6.96, 0, 0, 0, 0, 0);
Planet mercury(0.24397, 7.74, 0, 0.479, 2.11, 0);
Planet venus(0.60518, 14.46, 0, 0.350, 177.36, 0);
Planet earth(0.6371, 20, 0, 0.298, 23.441, 0);
Planet mars(0.3390, 30.48, 0, 0.241, 25.19, 0);
Planet jupiter(6.9911, 104.08, 0, 0.1306, 3.13, 0);
Planet saturn(5.8232, 191.64, 0, 0.0964, 26.73, 0);
Planet uranus(2.5362, 384.58, 0, 0.0681, 97.77, 0);
Planet neptune(2.4622, 602.08, 0, 0.0543, 28.32, 0);
Planet pluto(0.1185, 790.8, 0, 0.047, 122.53, 0);
Planet moon(0.17371, 3.0, 0, 1.2, 1.5, 0);

bool Simulate = false;
bool bigOrbit = true;
bool smallOrbit = true;
bool ShowSatellite = true;
int changeCamera = 0;
bool ShowLabels = false;
GLFrame frameCamera;

GLuint textureObjects[OBJECT_NUM];

float lightPos[] = { 0.0, 0.0, 0.0, 1.0 }; // 光源位置 (太陽中心)

const char *szTextureFiles[] = { "Sun.tga","Mercury.tga","Venus.tga","Earth.tga","Mars.tga",
								"Jupiter.tga","Saturn.tga","Uranus.tga","Neptune.tga","Pluto.tga",
								"Moon.tga" };

//const char *szCubeFaces[6] = { "pos_x.tga", "neg_x.tga", "pos_y.tga", "neg_y.tga", "pos_z.tga", "neg_z.tga" };

//GLenum  cube[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
//					 GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
//					 GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
//					 GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
//					 GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
//					 GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

string PlanetLabelName[] = { "Sun","Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune","Pluto","Moon" };

void writeBitmapString(void* font, string string)	// 繪製文字
{
	for (int i = 0; string[i] != '\0'; i++)
		glutBitmapCharacter(font, string[i]);
}

void SunMaterial(void)	// 太陽材質
{
	GLfloat SunAmbient[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SunDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SunSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SunEmission[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SunShininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, SunAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, SunDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SunSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, SunEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, SunShininess);
	glEnable(GL_COLOR_MATERIAL);
}

void PlanetMaterial(void)	// 行星+衛星材質
{
	GLfloat AmbientLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat DiffuseLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SpecularLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat EmissionLight[] = { 0.0f,0.0f,0.0f,0.0f };
	GLfloat Shininess = 32.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, AmbientLight);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, DiffuseLight);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularLight);
	glMaterialfv(GL_FRONT, GL_EMISSION, EmissionLight);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
	glEnable(GL_COLOR_MATERIAL);
}

void LabelMaterial(void)	// 文字標籤材質
{
	GLfloat AmbientLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat DiffuseLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SpecularLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat EmissionLight[] = { 0.0f,0.0f,0.0f,0.0f };
	GLfloat Shininess = 32.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, AmbientLight);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, DiffuseLight);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularLight);
	glMaterialfv(GL_FRONT, GL_EMISSION, EmissionLight);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
	glEnable(GL_COLOR_MATERIAL);
}

void OrbitMaterial(void)	// 軌道材質
{
	GLfloat AmbientLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat DiffuseLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat SpecularLight[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat EmissionLight[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat Shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, AmbientLight);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, DiffuseLight);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularLight);
	glMaterialfv(GL_FRONT, GL_EMISSION, EmissionLight);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
	glEnable(GL_COLOR_MATERIAL);
}

//void DrawSkyBox(void)	// 繪製天空盒(不成功)
//{
//	GLfloat fExtent = 10.0f;
//
//	glBegin(GL_QUADS);
//	//////////////////////////////////////////////
//	// Negative X
//	glTexCoord3f(-1.0f, -1.0f, 1.0f);
//	glVertex3f(-fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(-1.0f, -1.0f, -1.0f);
//	glVertex3f(-fExtent, -fExtent, -fExtent);
//
//	glTexCoord3f(-1.0f, 1.0f, -1.0f);
//	glVertex3f(-fExtent, fExtent, -fExtent);
//
//	glTexCoord3f(-1.0f, 1.0f, 1.0f);
//	glVertex3f(-fExtent, fExtent, fExtent);
//
//
//	///////////////////////////////////////////////
//	//  Postive X
//	glTexCoord3f(1.0f, -1.0f, -1.0f);
//	glVertex3f(fExtent, -fExtent, -fExtent);
//
//	glTexCoord3f(1.0f, -1.0f, 1.0f);
//	glVertex3f(fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, 1.0f);
//	glVertex3f(fExtent, fExtent, fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, -1.0f);
//	glVertex3f(fExtent, fExtent, -fExtent);
//
//
//	////////////////////////////////////////////////
//	// Negative Z 
//	glTexCoord3f(-1.0f, -1.0f, -1.0f);
//	glVertex3f(-fExtent, -fExtent, -fExtent);
//
//	glTexCoord3f(1.0f, -1.0f, -1.0f);
//	glVertex3f(fExtent, -fExtent, -fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, -1.0f);
//	glVertex3f(fExtent, fExtent, -fExtent);
//
//	glTexCoord3f(-1.0f, 1.0f, -1.0f);
//	glVertex3f(-fExtent, fExtent, -fExtent);
//
//
//	////////////////////////////////////////////////
//	// Positive Z 
//	glTexCoord3f(1.0f, -1.0f, 1.0f);
//	glVertex3f(fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(-1.0f, -1.0f, 1.0f);
//	glVertex3f(-fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(-1.0f, 1.0f, 1.0f);
//	glVertex3f(-fExtent, fExtent, fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, 1.0f);
//	glVertex3f(fExtent, fExtent, fExtent);
//
//
//	//////////////////////////////////////////////////
//	// Positive Y
//	glTexCoord3f(-1.0f, 1.0f, 1.0f);
//	glVertex3f(-fExtent, fExtent, fExtent);
//
//	glTexCoord3f(-1.0f, 1.0f, -1.0f);
//	glVertex3f(-fExtent, fExtent, -fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, -1.0f);
//	glVertex3f(fExtent, fExtent, -fExtent);
//
//	glTexCoord3f(1.0f, 1.0f, 1.0f);
//	glVertex3f(fExtent, fExtent, fExtent);
//
//
//	///////////////////////////////////////////////////
//	// Negative Y
//	glTexCoord3f(-1.0f, -1.0f, -1.0f);
//	glVertex3f(-fExtent, -fExtent, -fExtent);
//
//	glTexCoord3f(-1.0f, -1.0f, 1.0f);
//	glVertex3f(-fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(1.0f, -1.0f, 1.0f);
//	glVertex3f(fExtent, -fExtent, fExtent);
//
//	glTexCoord3f(1.0f, -1.0f, -1.0f);
//	glVertex3f(fExtent, -fExtent, -fExtent);
//	glEnd();
//}

void SetupRC(void)
{
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	PlaySound(TEXT("Space Ambience.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);	// 播放音效
	/*glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);*/
	glEnable(GL_DEPTH_TEST);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//for (int i = 0; i < 6; i++)
	//{
	//	// Load this texture map
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
	//	pBytes = gltLoadTGA(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
	//	glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
	//	free(pBytes);
	//}

	//// Enable cube mapping, and set texture environment to decal
	//glEnable(GL_TEXTURE_CUBE_MAP);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(sizeof textureObjects / sizeof GLuint, textureObjects);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (int i = 0; i < sizeof textureObjects / sizeof GLuint; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureObjects[i]);

		// Load this texture map
		pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);

		gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	GLfloat AmbientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat DiffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat SpecularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat GlobalAmbient[] = { 0.4f,0.4f,0.4f,1.0f };
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GlobalAmbient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_NORMALIZE);
}

void DrawOrbitalTrails(void)	// 繪製軌道
{
	glPushMatrix();
	OrbitMaterial();
	glColor3f(0.3f, 0.3f, 0.3f);
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutWireTorus(0.001, mercury.distance, 100.0, 100.0);
	glutWireTorus(0.001, venus.distance, 100.0, 100.0);
	glutWireTorus(0.001, earth.distance, 100.0, 100.0);
	glutWireTorus(0.001, mars.distance, 100.0, 100.0);
	glutWireTorus(0.001, jupiter.distance, 100.0, 100.0);
	glutWireTorus(0.001, saturn.distance, 100.0, 100.0);
	glutWireTorus(0.001, uranus.distance, 100.0, 100.0);
	glutWireTorus(0.001, neptune.distance, 100.0, 100.0);
	glutWireTorus(0.001, pluto.distance, 100.0, 100.0);
	glPopMatrix();
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glPushMatrix();
	frameCamera.ApplyCameraTransform();
	
	if (changeCamera == 0)
		gluLookAt(0.0, 40, 80, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	if (changeCamera == 1)
		gluLookAt(0.0, 0.0, 50, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	if (changeCamera == 2)
		gluLookAt(0.0, 60, 0.00001, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	if (bigOrbit)
		DrawOrbitalTrails();

	glColor3f(1.0f, 1.0f, 1.0f);

	GLUquadric* quadric;
	quadric = gluNewQuadric();
	
	// Sun
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glRotatef(sun.orbit, 0.0, 1.0, 0.0);
	glTranslatef(sun.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(-1.0, 8.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[SUN]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(sun.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(sun.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	
	glEnable(GL_TEXTURE_2D);
	SunMaterial();
	glBindTexture(GL_TEXTURE_2D, textureObjects[SUN]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, sun.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glPopMatrix();
	glPopMatrix();

	//Mercury
	glPushMatrix();
	PlanetMaterial();
	glRotatef(mercury.orbit, 0.0, 1.0, 0.0);
	glTranslatef(mercury.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 0.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[MERCURY]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(mercury.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(mercury.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[MERCURY]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, mercury.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Venus
	glPushMatrix();
	PlanetMaterial();
	glRotatef(venus.orbit, 0.0, 1.0, 0.0);
	glTranslatef(venus.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 0.8, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[VENUS]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(venus.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(venus.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[VENUS]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, venus.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Earth
	glPushMatrix();
	PlanetMaterial();
	glRotatef(earth.orbit, 0.0, 1.0, 0.0);
	glTranslatef(earth.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 0.8, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[EARTH]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(earth.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(earth.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[EARTH]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, earth.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbit)
		moon.DrawSatelliteOrbit();
	if (ShowSatellite)
	{
		glPushMatrix();
		PlanetMaterial();
		glColor3f(1.0f, 1.0f, 1.0f);
		glRotatef(moon.orbit, 0.0, 1.0, 0.0);
		glTranslatef(moon.distance, 0.0, 0.0);
		if (ShowLabels)
		{
			glPushMatrix();
			LabelMaterial();
			glRasterPos3f(0.0, 0.8, 0.0);
			glColor3ub(255, 255, 255);
			writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[MOON]);
			glPopMatrix();
		}
		glPushMatrix();
		glRotatef(moon.axisTilt, 1.0, 0.0, 0.0);
		glRotatef(moon.axisAni, 0.0, 1.0, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureObjects[MOON]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		gluQuadricTexture(quadric, 1);
		gluSphere(quadric, moon.radius, 40.0, 40.0);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();

	//Mars
	glPushMatrix();
	PlanetMaterial();
	glRotatef(mars.orbit, 0.0, 1.0, 0.0);
	glTranslatef(mars.distance, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0); // ***
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(-0.5, 0.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[MARS]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(mars.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(mars.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[MARS]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, mars.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Jupiter
	glPushMatrix();
	PlanetMaterial();
	glRotatef(jupiter.orbit, 0.0, 1.0, 0.0);
	glTranslatef(jupiter.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 7.5, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[JUPITER]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(jupiter.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(jupiter.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[JUPITER]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, jupiter.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Saturn
	glPushMatrix();
	PlanetMaterial();
	glRotatef(saturn.orbit, 0.0, 1.0, 0.0);
	glTranslatef(saturn.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 7.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[SATURN]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(saturn.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(saturn.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[SATURN]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, saturn.radius, 40, 40);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glColor3ub(158, 145, 137);
	glRotatef(-63.0, 1.0, 0.0, 0.0);
	glutWireTorus(0.4, 12.0, 30.0, 30.0);
	glutWireTorus(0.8, 10.0, 30.0, 30.0);
	glPopMatrix();
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);

	//Uranus
	glPushMatrix();
	PlanetMaterial();
	glRotatef(uranus.orbit, 0.0, 1.0, 0.0);
	glTranslatef(uranus.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 3.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[URANUS]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(uranus.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(uranus.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[URANUS]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, uranus.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Neptune
	glPushMatrix();
	PlanetMaterial();
	glRotatef(neptune.orbit, 0.0, 1.0, 0.0);
	glTranslatef(neptune.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 3.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[NEPTUNE]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(neptune.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(neptune.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[NEPTUNE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, neptune.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();

	//Pluto
	glPushMatrix();
	PlanetMaterial();
	glRotatef(pluto.orbit, 0.0, 1.0, 0.0);
	glTranslatef(pluto.distance, 0.0, 0.0);
	if (ShowLabels)
	{
		glPushMatrix();
		LabelMaterial();
		glRasterPos3f(0.0, 0.3, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, PlanetLabelName[PLUTO]);
		glPopMatrix();
	}
	glPushMatrix();
	glRotatef(pluto.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(pluto.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureObjects[PLUTO]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, pluto.radius, 40, 40);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	fAspect = (float)w / (float)h;
	gluPerspective(60.0, fAspect, 1.0, 3000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Simulation(int n)	// 模擬函式(TimerFunction)
{
	if (Simulate)
	{
		mercury.orbit += mercury.speed;
		venus.orbit += venus.speed;
		earth.orbit += earth.speed;
		mars.orbit += mars.speed;
		jupiter.orbit += jupiter.speed;
		saturn.orbit += saturn.speed;
		uranus.orbit += uranus.speed;
		neptune.orbit += neptune.speed;
		pluto.orbit += pluto.speed;
		moon.orbit += moon.speed;
	}

	if (mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto.orbit > 360.0)
		mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto.orbit -= 360.0;
	
	mercury.axisAni += 5.0;
	venus.axisAni += 5.0;
	earth.axisAni += 5.0;
	mars.axisAni += 5.0;
	jupiter.axisAni += 5.0;
	saturn.axisAni += 5.0;
	uranus.axisAni += 5.0;
	neptune.axisAni += 5.0;
	pluto.axisAni += 5.0;
	moon.axisAni += 5.0;

	glutPostRedisplay();
	glutTimerFunc(10, Simulation, 1);
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		frameCamera.MoveForward(0.5f);

	if (key == GLUT_KEY_DOWN)
		frameCamera.MoveForward(-0.5f);

	if (key == GLUT_KEY_LEFT)
		frameCamera.RotateLocalY(0.1f);

	if (key == GLUT_KEY_RIGHT)
		frameCamera.RotateLocalY(-0.1f);

	// Refresh the Window
	glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		frameCamera.RotateLocalX(-0.1f);
		break;
	case 's':
		frameCamera.RotateLocalX(0.1f);
		break;
	case 'a':
		frameCamera.RotateLocalZ(-0.1f);
		break;
	case 'd':
		frameCamera.RotateLocalZ(0.1f);
		break;
	case 27:
		exit(0);
		break;
	case ' ':
		if (Simulate)
			Simulate = false;
		else
		{
			Simulate = true;
			Simulation(1);
		}
		break;
	case 'o':
		if (smallOrbit)
			smallOrbit = 0;
		else
			smallOrbit = 1;
		break;
	case'O':
		if (bigOrbit)
			bigOrbit = false;
		else
			bigOrbit = true;
		break;
	case 'm':
	case 'M':
		if (ShowSatellite)
			ShowSatellite = 0;
		else
			ShowSatellite = 1;
		break;
	case 'l':
	case 'L':
		if (ShowLabels)
			ShowLabels = false;
		else
			ShowLabels = true;
		break;
	case '1':
		frameCamera.SetOrigin(0.0, 0.0, 0.0);
		changeCamera = 0;
		break;
	case '2':
		frameCamera.SetOrigin(0.0, 0.0, 0.0);
		changeCamera = 1;
		break;
	case '3':
		frameCamera.SetOrigin(0.0, 0.0, 0.0);
		changeCamera = 2;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	cout << "Instructions:" << endl
		<< "Arrow up/down: Move forward/backward" << endl
		<< "Arrow left/right: Turn your head to left/right" << endl
		<< "w/a/s/d: Rotate the view" << endl
		<< "Spacebar: Start simulation" << endl
		<< "1/2/3: Special view perspective" << endl
		<< "o: Switch satellites' orbit" << endl
		<< "O: Switch planets' orbit" << endl
		<< "M/m: Show/hide the satellite" << endl
		<< "L/l: Show/hide labels" << endl
		<< "Esc: Close window" << endl << endl
		<< "BGM: Space Ambience (Credit: Vilkas Sound)" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Solar System");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(keyInput);
	glutTimerFunc(10, Simulation, 0);
	SetupRC();
	glutMainLoop();
	return 0;
}