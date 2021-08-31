//*********************************************************************
//  Iluminação usando OpenGL
//  Codigo Exemplo
//*********************************************************************

#include <GL/glut.h>
#include <stdio.h>

GLfloat angle=45;
GLfloat fAspect;

void init();
void display();
void Viewing();
void reshape(GLsizei w, GLsizei h);
void mouse(int button, int state, int x, int y);

// Programa Principal
int main(void){
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400,350);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Visualizacao 3D");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	init();
	glutMainLoop();
	return 0;
}

// Inicialização
void init(){
	GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0};
	GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};		 // "cor"
	GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho"
	GLfloat posicaoLuz[4]={0.0, 50.0, 50.0, 1.0};

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0};
	GLint especMaterial = 5;

 	// Especifica que a cor de fundo da janela será preta
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Habilita o modelo de tonalização de Gouraud
	glShadeModel(GL_SMOOTH);
//    glShadeModel(GL_FLAT);

	// Define a refletância do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

	// Habilita a definição da cor do material
	// a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
}

// Função usada para especificar o volume de visualização
void Viewing(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle,fAspect,0.1,500);
    printf("angle = %f\n",angle);
	gluLookAt(0,80,200, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}


// Chamada pela GLUT quando a janela é redimensionada
void reshape(GLsizei w, GLsizei h){
	if ( h == 0 ) h = 1;
	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w/(GLfloat)h;
	Viewing();
}

// Chamada para fazer o desenho
void display()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glColor3f(0.0f, 0.0f, 1.0f);
	// Desenha o teapot com a cor corrente (solid)

//	glutSolidTeapot(50.0f);
//    glutWireTeapot(50.0f);

//	glutSolidCube(50.0f);

//    glutSolidSphere(50.f,20.0f,20.0f);
//    glutWireSphere(50.f,10.0f,10.0f);
//    glRotatef(-65,1,0,0);
//    glutSolidCone(50,50,10,10);
//	glutWireCone(50,50,10,10);

//    glRotatef(-65,1,0,0);
	  glutSolidTorus(10.0,50.0,50,50); //glutSolidTorus(raio_total_ate_raio_menor,raio_total,num_divisoes_vert,num_div_hor);
//	glutWireTorus(10.0,50.0,50,50);
	
//    glRotatef(-90,1,0,0);
//	  glutSolidCone(50.0f,80.0,30.0,10); //glutSolidCone(base,altura,fatias,pilha);

//    glScalef(3.0,3.0,3.0);
//    glTranslatef(0,20,50);
//    glRotatef(45,1,0,0);
//    glRotatef(-45,0,0,1);
//    glRotatef(60,0,1,0);
//    glutSolidTetrahedron();


//    glScalef(3.0,3.0,3.0);
//    glTranslatef(0,20,50);
//	  glutSolidOctahedron();


//    glScalef(3.0,3.0,3.0);
//    glTranslatef(0,20,50);
//    glutWireDodecahedron();

//    glScalef(3.0,3.0,3.0);
//    glTranslatef(0,20,50);
//    glutSolidIcosahedron();


	glutSwapBuffers();
}

// Callback para gerenciar eventos do mouse
void mouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON)
		if (state == GLUT_DOWN)
			if (angle >= 10)	angle -= 5;
	if (button == GLUT_RIGHT_BUTTON)
		if (state == GLUT_DOWN)
			if (angle <= 130)	angle += 5;
	Viewing();
	glutPostRedisplay();
}



