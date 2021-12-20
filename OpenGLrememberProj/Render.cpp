#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
double s = 1;
double A[] = { 1, 4, 0 };
double B[] = { 6, 1, 0 };
double C[] = { 10,8, 0 };
double D[] = { 16,7, 0 };
double E[] = { 15, 15, 0 };
double F[] = { 11, 15, 0 };
double G[] = { 9,10, 0 };
double H[] = { 4,10, 0 };
//======================
double A1[] = { 1, 4, s };
double B1[] = { 6, 1, s };
double C1[] = { 10,8, s };
double D1[] = { 16,7, s };
double E1[] = { 15, 15, s };
double F1[] = { 11, 15, s };
double G1[] = { 9,10, s };
double H1[] = { 4,10, s };
double qq[] = { (A[0] + H[0]) / 2, (A[1] + H[1]) / 2, 0 };
double r = sqrt(pow((H[0] - A[0]), 2) + pow((H[1] - A[1]), 2)) / 2;
double angleA = acos((A[0] - (A[0] + H[0]) / 2) / r);
double angleH = acos((H[0] - (A[0] + H[0]) / 2) / r);
double tmp[] = { 0,0,0 };


//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void normal(double A[], double B[], double C[], double* tmp)
{
	double a[] = { 0,0,0 };
	double b[] = { 0,0,0 };
	double len;
	for (int i = 0; i < 3; i++)
	{
		a[i] = A[i] - B[i];
		b[i] = C[i] - B[i];
	}
	tmp[0] = a[1] * b[2] - b[1] * a[2];
	tmp[1] = -a[0] * b[2] + b[0] * a[2];
	tmp[2] = a[0] * b[1] - b[0] * a[1];
	len = sqrt(pow(tmp[0], 2) + pow(tmp[1], 2) + pow(tmp[2], 2));
	tmp[0] /= len;
	tmp[1] /= len;
	tmp[2] /= len;
}

void flor(double z)
{
	if (z == 0) {
		glColor3f(1.0f, 0.1f, 0.0f);
		glNormal3d(0, 0, -1);
	}
	else {
		glColor4d(1.0, 0.1, 0.66,0.9);
		glNormal3d(0, 0, 1);
	}

	for (double a = -angleA; a <= angleH; a += 0.01) {
		glBegin(GL_TRIANGLES);
		glVertex3d(qq[0], qq[1], z);
		glVertex3f(2.5 - cos(a) * r, 7 - sin(a) * r, z);
		glVertex3f(2.5 - cos(a + 0.01) * r, 7 - sin(a + 0.01) * r, z);
		glEnd();
	}

	glBegin(GL_QUADS);
	glVertex3d(A[0], A[1], z);
	glVertex3d(B[0], B[1], z);
	glVertex3d(C[0], C[1], z);
	glVertex3d(H[0], H[1], z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3d(C[0], C[1], z);
	glVertex3d(H[0], H[1], z);
	glVertex3d(G[0], G[1], z);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(C[0], C[1], z);
	glVertex3d(D[0], D[1], z);
	glVertex3d(E[0], E[1], z);
	glVertex3d(F[0], F[1], z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3d(F[0], F[1], z);
	glVertex3d(C[0], C[1], z);
	glVertex3d(G[0], G[1], z);
	glEnd();
}

void wall(double z)
{
	
	glBegin(GL_QUADS);
	glColor3d(0.2, 0.7, 0.7);
	normal(A, B1,B, tmp);
	glNormal3dv(tmp);
	glVertex3d(A[0], A[1], 0);
	glVertex3d(A1[0], A1[1], z);
	glVertex3d(B1[0], B1[1], z);
	glVertex3d(B[0], B[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(1.0, 0.0, 1.0);
	normal(C, B, C1, tmp);
	glNormal3dv(tmp);
	glVertex3d(B[0], B[1], 0);
	glVertex3d(C[0], C[1], 0);
	glVertex3d(C1[0], C1[1], z);
	glVertex3d(B1[0], B1[1], z);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(1.0, 1.0, 0.0);
	normal(C, C1, D, tmp);
	glNormal3dv(tmp);
	glVertex3d(C1[0], C1[1], z);
	glVertex3d(D1[0], D1[1], z);
	glVertex3d(D[0], D[1], 0);
	glVertex3d(C[0], C[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(1.0, 1.0, 0.0);
	normal(E1, E, D, tmp);
	glNormal3dv(tmp);
	glVertex3d(D1[0], D1[1], z);
	glVertex3d(E1[0], E1[1], z);
	glVertex3d(E[0], E[1], 0);
	glVertex3d(D[0], D[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.0, 0.0, 1.0);
	normal(E, E1, F, tmp);
	glNormal3dv(tmp);
	glVertex3d(E[0], E[1], 0);
	glVertex3d(E1[0], E1[1], z);
	glVertex3d(F1[0], F1[1], z);
	glVertex3d(F[0], F[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.0, 0.0, 1.0);
	normal(G1, G, F, tmp);
	glNormal3dv(tmp);
	glVertex3d(G[0], G[1], 0);
	glVertex3d(G1[0], G1[1], z);
	glVertex3d(F1[0], F1[1], z);
	glVertex3d(F[0], F[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.0, 0.0, 1.0);
	normal(H, G, H1, tmp);
	glNormal3dv(tmp);
	glVertex3d(G[0], G[1], 0);
	glVertex3d(G1[0], G1[1], z);
	glVertex3d(H1[0], H1[1], z);
	glVertex3d(H[0], H[1], 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 1);
	for (double a = -angleA; a <= angleH; a += 0.01) {
		double x1 = 2.5 - cos(a) * r;
		double y1 = 7.0 - sin(a) * r;
		double x2 = 2.5 - cos(a + 0.01) * r;
		double y2 = 7.0 - sin(a + 0.01) * r;
		double xx1[3], xx2[3], xx3[3], xx4[3];
		xx1[0] = x1;
		xx1[1] = y1;
		xx1[2] = 0;

		xx2[0] = x1;
		xx2[1] = y1;
		xx2[2] = z;

		xx3[0] = x2;
		xx3[1] = y2;
		xx3[2] = 0;

		xx4[0] = x2;
		xx4[1] = y2;
		xx4[2] = z;

		normal(xx1, xx2, xx3, tmp);
		glNormal3dv(tmp);
		glVertex3dv(xx1);
		glVertex3dv(xx2);
		glVertex3dv(xx4);
		glVertex3dv(xx3);
	}
	glEnd();

}


void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//альфаналожение
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//рисуем фигуру 
	wall(2);
	flor(0);
	flor(2);



   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	/*ss << "T - вкл/выкл текстур" << std::endl;*/
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}


