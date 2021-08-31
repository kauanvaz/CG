//*********************************************************************
//  Minicurso: Computacao Grafica usando OpenGL
//  Autor: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************

/* Inclui os headers do OpenGL, GLU, e GLUT */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>

#define ESC 27

static int ombro = 0, cotovelo = 0, indicador=0, medio=0, polegar=0, giro=0;

void init(void);
void keyboard (unsigned char key, int x, int y);
void display(void);
void reshape (int w, int h);

void iluminacao(){
	/* Inicia a iluminação e as características gerais dos materiais */
    GLfloat light_pos[] = {-5.0, -5.0, 5.0, 0.0}; //-1.0, 1.0, 1.0, 0.0
	GLfloat light_Ka[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_Kd[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_Ks[] = {1.0, 1.0, 1.0, 1.0};		
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);		    
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
	GLfloat mat_Ka[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_Kd[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_Ks[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_Se[] = {25.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_Ka);    
   	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_Ks);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_Se);
    
    /* Para usar a cor definida pelo glColor*/
    glEnable(GL_COLOR_MATERIAL);
}

int main(int argc, char** argv){
    /* inicia o GLUT */
    glutInit(&argc,argv);

    /* inicia o display usando RGB e double-buffering */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(700,512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Braco Robotico");

    /* Funcao com alguns comandos para a inicializacao do OpenGL; */
    init ();

    /* define as funcões de callback */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

  return 0;
}

/* Funcao com alguns comandos para a inicializacao do OpenGL; */
void init(void){
  glClearColor (0.0, 0.0, 0.0, 1.0); //Limpa a tela com a cor branca;
  iluminacao();
  glEnable(GL_DEPTH_TEST); // Habilita o algoritmo Z-Buffer
}

void reshape (int w, int h){
    /* muda para o modo GL_PROJECTION e reinicia a projecção */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    /* define a zona da janela onde se vai desenhar */
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);

    /* Define a forma do "viewing volume" para termos               *
     * uma projecção de perspectiva (3D).                           *
     * gluPerpective(angulo,aspecto,ponto_proximo, ponto distante); */
    gluPerspective(60, (float)w/(float)h, 1.0, 20.0);
    gluLookAt(0.0,0.0,5.0, 	// posição da câmera (olho) 
			  0.0,0.0,0.0, 	// centro da cena
			  0.0,1.0,0.0); // direção de cima 
    /* muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
     * quando estivermos a desenhar a tela) */
    glMatrixMode (GL_MODELVIEW);
}

void keyboard (unsigned char key, int x, int y){
	switch (key) {
		case 'o': // sentido anti-horário
			ombro = (ombro + 5) % 360;
			glutPostRedisplay();
		break;
		case 'O': // sentido horário
			ombro = (ombro - 5) % 360;
			glutPostRedisplay();
		break;
		case 'c': // sentido anti-horário
			cotovelo = (cotovelo + 5) % 360;
			glutPostRedisplay();
		break;
		case 'C': // sentido horário
			cotovelo = (cotovelo - 5) % 360;
			glutPostRedisplay();
		break;
		case 'i': // sentido anti-horário
			indicador = (indicador + 5) % 360;
			glutPostRedisplay();
		break;
		case 'I': // sentido horário
			indicador = (indicador - 5) % 360;
			glutPostRedisplay();
		break;
		case 'm': // sentido anti-horário
			medio = (medio + 5) % 360;
			glutPostRedisplay();
		break;
		case 'M': // sentido horário
			medio = (medio - 5) % 360;
			glutPostRedisplay();
		break;
		case 'p': // sentido anti-horário
			polegar = (polegar + 5) % 360;
			glutPostRedisplay();
		break;
		case 'P': // sentido horário
			polegar = (polegar - 5) % 360;
			glutPostRedisplay();
		break;
		case 'y': // sentido anti-horário
			giro = (giro + 5) % 360;
			glutPostRedisplay();
		break;
		case 'Y': // sentido horário
			giro = (giro - 5) % 360;
			glutPostRedisplay();
		break;
		case ESC:
			exit(0);
		break;
		default:
		break;
	}
}

void display(void){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o Buffer de Cores
    glLoadIdentity();

    glPushMatrix();
			glRotatef ((GLfloat) giro, 0.0, 1.0, 0.0);
	    	
	    	//antena fio
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glTranslatef (0.0, 0.45, -0.45);
		    	glRotatef (-45, 1.0, 0.0, 0.0);
		    	glScalef (0.3, 0.5, 0.3);
				glutSolidCube (0.4);
	    	glPopMatrix();
	    	
	    	//antena bola
	    	glPushMatrix();
		    	glColor3f(0.96, 0.37, 0.64);
		    	glTranslatef (0.0, 0.6, -0.6);
				glutSolidSphere(0.2,16,16);
	    	glPopMatrix();
	    	
	    	//cabeça
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glutSolidSphere(0.55,16,16);
	    	glPopMatrix();
	    	
	    	//tronco
	    	glPushMatrix();
		    	glColor3f(0.31, 0.31, 0.96);
		    	glTranslatef (0.0, -0.8, 0.0);
		    	glScalef (1.0, 1.5, 1.0);
		    	glutSolidSphere(0.25,16,16);
	    	glPopMatrix();
	    	
	    	//braco direito
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glTranslatef (-0.25, -0.8, 0.0);
		    	glRotatef (-23, 0.0, 0.0, 1.0);
		    	glScalef (0.3, 1.2, 0.3);
		    	glRotatef (-20, 0.0, 0.0, 1.0);
				glutSolidCube (0.4);
	    	glPopMatrix();
	    	
	    	//mão direita
	    	glPushMatrix();
		    	glColor3f(0.96, 0.37, 0.64);
		    	glTranslatef (-0.4, -1.0, 0.0);
				glutSolidSphere(0.15,16,16);
	    	glPopMatrix();
	    	
	    	//braco esquerdo
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glTranslatef (0.25, -0.8, 0.0);
		    	glRotatef (23, 0.0, 0.0, 1.0);
		    	glScalef (0.3, 1.2, 0.3);
		    	glRotatef (20, 0.0, 0.0, 1.0);
				glutSolidCube (0.4);
	    	glPopMatrix();
	    	
	    	//mão esquerda
	    	glPushMatrix();
		    	glColor3f(0.96, 0.37, 0.64);
		    	glTranslatef (0.4, -1.0, 0.0);
				glutSolidSphere(0.15,16,16);
	    	glPopMatrix();
	    	
	    	//perna direita
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glTranslatef (-0.1, -1.2, 0.0);
		    	glScalef (0.3, 1.0, 0.3);
				glutSolidCube (0.4);
	    	glPopMatrix();
	    	
	    	//pé direito
	    	glPushMatrix();
		    	glColor3f(0.96, 0.37, 0.64);
		    	glTranslatef (-0.1, -1.4, 0.05);
		    	glScalef (1.0, 1.0, 1.5);
				glutSolidSphere(0.15,16,16);
	    	glPopMatrix();
	    	
	    	//perna esquerda
	    	glPushMatrix();
		    	glColor3f(1.0,1.0,1.0);
		    	glTranslatef (0.1, -1.2, 0.0);
		    	glScalef (0.3, 1.0, 0.3);
				glutSolidCube (0.4);
	    	glPopMatrix();
	    	
	    	//pé esquerdo
	    	glPushMatrix();
		    	glColor3f(0.96, 0.37, 0.64);
		    	glTranslatef (0.1, -1.4, 0.05);
		    	glScalef (1.0, 1.0, 1.5);
				glutSolidSphere(0.15,16,16);
	    	glPopMatrix();
    	glPopMatrix();
    	
	/* Troca os buffers, mostrando o que acabou de ser desenhado */
    glutSwapBuffers();
}

