//*********************************************************************
//  Jogo: Bomberman 3D
//  Autores: Francisco Caio Feitosa Silva
//			 Kauan Vaz do Nascimento
//*********************************************************************

/* Inclui os headers do OpenGL, GLU, e GLUT */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <mmsystem.h>

#include "desenhaBloco.h"
#include "textureRGB.h"
#include "texturaBlocoMadeira.h"
#include "texturaBlocoPedra.h"
#include "texturaGramaClara.h"
#include "texturaGramaEscura.h"
#include "desenhaExitDoor.h"

// TECLAS
#define ESC 27

struct Bomba{
	int timer;
	int x, y;
};

struct Personagem{
	float x, y; // posicao
	float w, h; // largura, altura
};

GLfloat tam = 1.0; // determina o tamanho do lado de um quadrilátero/cubo etc

// Variaveis
bool ganhou = false;
bool morreu = false;
int x = 0, y = 0;

// variaveis para animar a exit door
float altura_exit_door = 0.0;
bool sobe_exit_door = true;
int gira_exit = 0;

// ----------- variaveis relacionadas a bomba  --------------
int corBomba = 0;
int raioBomba = 1;
float tbomb = 0.2;
int limiteBombas = 1;
int bombasColocadas = 0;

Bomba bomba;

#define QUANT_TEX 5
unsigned int id_texturas[QUANT_TEX]; //nomes identificadores de textura

void init(void);
void keyboard (unsigned char key, int x, int y);
void special_keyboard (int key, int x, int y);
void display(void);
void reshape (int w, int h);
void idle(void);
void colisaoInimigos(Personagem *p, char eixo, int *dir);
void reinicia();
void verificaSeGanhou();
void background();
void desenhaMapa();
void colisao(unsigned char key);
void colisaoSom(int a);

float velocidade = 0.25;
int olha_lado = 1;
Personagem p1 = {2, 1, 0.8, 0.8};

float velocInim = 0.01;
Personagem inim1 = {1, 4, tam, tam};
int dirInim1 = 1;
bool m1 = false;

Personagem inim2 = {5, 5, tam, tam};
int dirInim2 = 1;
bool m2 = false;

Personagem inim3 = {8, 9, tam, tam};
int dirInim3 = 1;
bool m3 = false;

void mostraControles(){
	printf("\n   CONTROLES\n\n");
	printf("       UP\n LEFT      RIGHT\n      DOWN");
	printf("\n\n B: Bomba");
	printf("\n\n R: Reiniciar");
}

/*
0 - Quadrilateros verde claro
1 - Quadrilateros verde escuro
2 - Blocos indestrutiveis cinza
3 - Blocos destrutiveis marrom
4 - Bomba + bloco verde claro
5 - Bomba + bloco verde escuro
*/
#define tamMapa 13
int mapa[tamMapa][tamMapa] = {
							{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
							{2, 0, 1, 3, 1, 0, 1, 3, 1, 0, 3, 3, 2},
							{2, 1, 2, 1, 2, 3, 2, 1, 2, 1, 2, 1, 2},
							{2, 0, 3, 0, 3, 3, 3, 0, 1, 3, 1, 0, 2}, 
							{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2},
							{2, 0, 3, 3, 1, 0, 1, 3, 1, 3, 1, 0, 2},
							{2, 3, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2},
							{2, 0, 3, 0, 3, 0, 3, 3, 1, 3, 1, 0, 2},
							{2, 3, 2, 1, 2, 3, 2, 1, 2, 1, 2, 1, 2},
							{2, 0, 1, 3, 1, 0, 1, 3, 1, 0, 1, 0, 2},
							{2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2, 3, 2},
							{2, 3, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 2},
							{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

void iluminacao(){
	/* Inicia a iluminação e as características gerais dos materiais */
    GLfloat light_pos[] = {2.0, 7.0, 5.0, 0.0};
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

void textura(){
	
	/* Prepara a textura */
  glGenTextures(QUANT_TEX, id_texturas); //gera nomes identificadores de texturas
  
  glBindTexture(GL_TEXTURE_2D, id_texturas[0]); //Textura background
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, id_texturas[1]); //Textura bloco de madeira
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_madeira, TEXTURE_madeira, 0, GL_RGB, GL_UNSIGNED_BYTE, bloco_madeira_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, id_texturas[2]); //Textura bloco de pedra
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_pedra, TEXTURE_pedra, 0, GL_RGB, GL_UNSIGNED_BYTE, bloco_pedra_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, id_texturas[3]); //Textura grama clara
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_grama_clara, TEXTURE_grama_clara, 0, GL_RGB, GL_UNSIGNED_BYTE, grama_clara_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glBindTexture(GL_TEXTURE_2D, id_texturas[4]); //Textura grama escura
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_grama_escura, TEXTURE_grama_escura, 0, GL_RGB, GL_UNSIGNED_BYTE, grama_escura_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);
}

void limpaCampos(int ax, int ay){
	if((ax%2 != 0 && ay%2 == 0) || (ax%2 == 0 && ay%2 != 0))
		mapa[ax][ay] = 1;
	else
		mapa[ax][ay] = 0;
}

void animaBomba(int a){
	if(bombasColocadas != 0){
		tbomb -= 0.003;
		
		if(tbomb <= 0.08) corBomba = 1;
		if(tbomb <= 0.03){  // Bomba explode
			PlaySound("explosion.wav", NULL, SND_ASYNC);
			tbomb = 0.2;
			corBomba = 0;
			bombasColocadas = 0;
			int ax = bomba.x;
			int ay = bomba.y;
			int px = (int)floor(p1.y);
			int py = (int)floor(p1.x);
			
			limpaCampos(ax, ay);
			
			 // ------------  explodindo blocos  --------------
			for (int i = 1; i <= raioBomba; i++){  // buscando pra direita
				if(mapa[ax + i][ay] == 2)
					break;
				
				px = (int)floor(p1.y);
				py = (int)floor(p1.x);
				if((ax + i == px) && (ay == py)){
					PlaySound("Lose sound.wav", NULL, SND_ASYNC);
				 	morreu = true;
				}
				
				px = (int)floor(inim1.y);
				py = (int)floor(inim1.x);
				if((ax + i == px) && (ay == py))
					m1 = true;
				
				px = (int)floor(inim2.y);
				py = (int)floor(inim2.x);
				if((ax + i == px) && (ay == py))
					m2 = true;
				
				px = (int)floor(inim3.y);
				py = (int)floor(inim3.x);
				if((ax + i == px) && (ay == py))
					m3 = true;
				
				if(mapa[ax + i][ay] == 3) {
					limpaCampos(ax + i, ay);
					break;
				}
			}
			
			for (int i = 1; i <= raioBomba; i++){  // buscando pra esquerda
				if(mapa[ax - i][ay] == 2)
					break;
				
				px = (int)floor(p1.y);
				py = (int)floor(p1.x);
				if((ax - i == px) && (ay == py)){
					morreu = true;
					PlaySound("Lose sound.wav", NULL, SND_ASYNC);
				}
				
				px = (int)floor(inim1.y);
				py = (int)floor(inim1.x);
				if((ax - i == px) && (ay == py))
					m1 = true;
				
				px = (int)floor(inim2.y);
				py = (int)floor(inim2.x);
				if((ax - i == px) && (ay == py))
					m2 = true;
				
				px = (int)floor(inim3.y);
				py = (int)floor(inim3.x);
				if((ax - i == px) && (ay == py))
					m3 = true;
				
				if(mapa[ax - i][ay] == 3) {
					limpaCampos(ax - i, ay);
					break;
				}
			}
			for (int i = 1; i <= raioBomba; i++){  // buscando pra cima
				if(mapa[ax][ay + i] == 2)
					break;
				
				px = (int)floor(p1.y);
				py = (int)floor(p1.x);
				if((ax == px) && (ay + i == py)){
					morreu = true;
					PlaySound("Lose sound.wav", NULL, SND_ASYNC);
				}
				
				px = (int)floor(inim1.y);
				py = (int)floor(inim1.x);
				if((ax == px) && (ay + i == py))
					m1 = true;
				
				px = (int)floor(inim2.y);
				py = (int)floor(inim2.x);
				if((ax == px) && (ay + i == py))
					m2 = true;
				
				px = (int)floor(inim3.y);
				py = (int)floor(inim3.x);
				if((ax == px) && (ay + i == py))
					m3 = true;
				
				if(mapa[ax][ay + i] == 3) {
					limpaCampos(ax, ay + i);
					break;
				}
			}
			for (int i = 1; i <= raioBomba; i++){  // buscando pra baixo
				if(mapa[ax][ay - i] == 2)
					break;
				
				px = (int)floor(p1.y);
				py = (int)floor(p1.x);
				if((ax == py) && (ay - i == px)){
					PlaySound("Lose sound.wav", NULL, SND_ASYNC);
					morreu = true;
				}
				
				px = (int)floor(inim1.y);
				py = (int)floor(inim1.x);
				if((ax == px) && (ay - i == py))
					m1 = true;
				
				px = (int)floor(inim2.y);
				py = (int)floor(inim2.x);
				if((ax == px) && (ay - i == py))
					m2 = true;
				
				px = (int)floor(inim3.y);
				py = (int)floor(inim3.x);
				if((ax == px) && (ay - i == py))
					m3 = true;
				
				if(mapa[ax][ay - i] == 3) {
					limpaCampos(ax, ay - i);
					break;
				}
			}
			
			px = (int)floor(p1.y);
			py = (int)floor(p1.x);
			if((ax == px) && (ay == py)){
				PlaySound("Lose sound.wav", NULL, SND_ASYNC);
				morreu = true;
			}
			
			px = (int)floor(inim1.y);
			py = (int)floor(inim1.x);
			if((ax == px) && (ay == py))
				m1 = true;
				
			px = (int)floor(inim2.y);
			py = (int)floor(inim2.x);
			if((ax == px) && (ay == py))
				m2 = true;
			
			px = (int)floor(inim3.y);
			py = (int)floor(inim3.x);
			if((ax == px) && (ay == py))
				m3 = true;
		}
	}
	
	glutPostRedisplay();
	glutTimerFunc(70, animaBomba, 1);
}

int main(int argc, char** argv){
    /* inicia o GLUT */
    glutInit(&argc,argv);
    
    mostraControles();

    /* inicia o display usando RGB e double-buffering */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE| GLUT_DEPTH);
    glutInitWindowSize(700,512);
    glutInitWindowPosition(350,100);
    glutCreateWindow("Bomberman 3D");

    /* Funcao com alguns comandos para a inicializacao do OpenGL; */
    init ();

    /* define as funcões de callback */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(200, animaBomba, 1);
    glutTimerFunc(33, colisaoSom, 1);
    glutSpecialFunc(special_keyboard); // teclado especial
	glutIdleFunc(idle); // faz a animação
		
    glutMainLoop();

  return 0;
}

/* Funcao com alguns comandos para a inicializacao do OpenGL; */
void init(void){
  glClearColor (1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST); // Habilita o algoritmo Z-Buffer
  
  iluminacao();
  textura();
  
  /* Ativa o modelo de sombreagem de "Gouraud". */
  glShadeModel( GL_SMOOTH );
}

// Função para animação
void idle(void){
	glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y){
	switch (key) {
	case 'R':	
		case 'r':
			reinicia();
		break;
	
		case 'B':
		case 'b':
			if(bombasColocadas < limiteBombas && !morreu){
				if(mapa[(int)floor(p1.y)][(int)floor(p1.x)] == 0)
					mapa[(int)floor(p1.y)][(int)floor(p1.x)] = 4;
				
				if(mapa[(int)floor(p1.y)][(int)floor(p1.x)] == 1)
					mapa[(int)floor(p1.y)][(int)floor(p1.x)] = 5;
				
				bombasColocadas++;
				
				if(bombasColocadas == 1){
					PlaySound("colocaBomba.wav", NULL, SND_ASYNC);
					bomba.timer = 0;
					bomba.x = p1.y;
					bomba.y = p1.x;
				}
			}
			glutPostRedisplay();
		break;
	
		case ESC:
			exit(0);
		break;
		default:
		break;
	}
}

void special_keyboard (int key, int x, int y){
	switch (key) {
		case GLUT_KEY_UP:
			p1.y += velocidade;
			olha_lado = 180;
			glutPostRedisplay();
		break;
	
		case GLUT_KEY_DOWN:
			p1.y -= velocidade;
			olha_lado = 0;
			glutPostRedisplay();
		break;
	
		case GLUT_KEY_LEFT:
			p1.x -= velocidade;
			olha_lado = -90;
			glutPostRedisplay();
		break;
	
		case GLUT_KEY_RIGHT:
			p1.x += velocidade;
			olha_lado = 90;
			glutPostRedisplay();
		break;
		default:
		break;
	}
	colisao(key);
	verificaSeGanhou();
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
    gluPerspective(40, (float)w/(float)h, 1.0, 50.0);
    gluLookAt(6.0,-5.0,15.0, 	// posição da câmera (olho) 
			  6.0,6.0,0.0, 	// centro da cena
			  0.0,1.0,0.0); // direção de cima 
    /* muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
     * quando estivermos a desenhar a tela) */
    glMatrixMode (GL_MODELVIEW);
}

void display(void){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o Buffer de Cores
    glLoadIdentity();
	
	background();
	desenhaMapa();
	
    /* Troca os buffers, mostrando o que acabou de ser desenhado */
    glutSwapBuffers();
}

bool colidir(float ax, float ay, float aComp, float aAlt, float bx, float by, float bComp, float bAlt){
	
	if(ay + aAlt <= by) return false;
	else if(ay >= by + bAlt) return false;
	else if(ax + aComp <= bx) return false;
	else if(ax >= bx + bComp) return false;
	
	return true;
}

void colisao(unsigned char key){
	for(int i = 0; i < tamMapa; i++){
		for(int j = 0; j < tamMapa; j++){
			if (colidir(p1.y, p1.x, p1.h, p1.w, i * tam, j * tam, tam, tam) == true){
				if((mapa[i][j] == 2) || (mapa[i][j] == 3)){
					if(key==GLUT_KEY_UP) p1.y -= velocidade;
				    else if(key==GLUT_KEY_DOWN) p1.y += velocidade;
				    else if(key==GLUT_KEY_LEFT) p1.x += velocidade;
				    else if(key==GLUT_KEY_RIGHT) p1.x -= velocidade;
				}
			}
		}
	}
}

void colisaoInimigos(Personagem *p, char eixo, int *dir){
	for(int i = 0; i < tamMapa; i++){
		for(int j = 0; j < tamMapa; j++){
			if (colidir((*p).y, (*p).x, (*p).h, (*p).w, i * tam, j * tam, tam, tam) == true){
				if((mapa[i][j] == 2) || (mapa[i][j] == 3) || (mapa[i][j] == 4) || (mapa[i][j] == 5)){
					if(eixo == 'x'){
						(*p).x -= (*dir)*velocInim;
					}else{
						(*p).y -= (*dir)*velocInim;
					}
					(*dir)*=-1;				
				}
			}
			
			if (colidir(p1.y, p1.x, p1.h, p1.w, (*p).y, (*p).x, (*p).h, (*p).w)){
				//PlaySound("Lose sound.wav", NULL, SND_ASYNC|SND_FILENAME);
				morreu = true;
			}
		}
	}
}

void colisaoSom(int a){
	if (colidir(p1.y, p1.x, p1.h, p1.w, inim1.y, inim1.x, inim1.h, inim1.w))
		PlaySound("Lose sound.wav", NULL, SND_ASYNC);
	
	if (colidir(p1.y, p1.x, p1.h, p1.w, inim2.y, inim2.x, inim2.h, inim2.w))
		PlaySound("Lose sound.wav", NULL, SND_ASYNC);
	
	if (colidir(p1.y, p1.x, p1.h, p1.w, inim3.y, inim3.x, inim3.h, inim3.w))
		PlaySound("Lose sound.wav", NULL, SND_ASYNC);
		
	glutTimerFunc(1, colisaoSom, 1);
}

// Plano de fundo
void background(){
	glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
		glDepthMask(GL_FALSE);
		
		glBindTexture(GL_TEXTURE_2D, id_texturas[0]);
        glBegin(GL_QUADS);
        	glColor3f(1.0, 1.0, 1.0);
	        // glTexCoord2i(-1,-1); glVertex2i(-1,-1);
	        // glTexCoord2i(1,-1); glVertex2i(1,-1);
	        // glTexCoord2i(1, 1); glVertex2i(1, 1);
	        // glTexCoord2i(-1, 1); glVertex2i(-1, 1);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
        	glTexCoord2i(1, 0); glVertex2i( 1,-1);
        	glTexCoord2i(1, 1); glVertex2i( 1, 1);
        	glTexCoord2i(0, 1); glVertex2i(-1, 1);
        glEnd();
        
        glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glDepthMask(GL_TRUE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void exit_door(){
	float altMin = 1;
	float altMax = altMin + 0.6;

    if((altMin+altura_exit_door<altMax) && sobe_exit_door) altura_exit_door+=0.005;
    if((altMin+altura_exit_door>altMin) && !(sobe_exit_door)) altura_exit_door-=0.005;
    if(altMin+altura_exit_door>=altMax) sobe_exit_door = false;
    if(altMin+altura_exit_door<=altMin) sobe_exit_door = true;
    
    gira_exit++;
    if (gira_exit> 360.0) gira_exit -= 360.0;
    
    if(!ganhou){
		glPushMatrix();
	    glTranslatef (11, 11, altMin+altura_exit_door);
	    glRotatef (90, 1.0, 0.0, 0.0);
	    glRotatef(gira_exit, 0.0, 1.0, 0.0);
  		glScalef (0.5, 0.5, 0.5);
		glColor3f(0.4, 0.88, 0.84); 
		desenha_exit_door();
	glPopMatrix();
	}
}

void pers_principal(){
	if(!morreu){		
		glPushMatrix();
			glTranslatef (p1.x, p1.y, 1.1);
			glRotatef (90, 1.0, 0.0, 0.0);
			glRotatef ((GLfloat)olha_lado, 0.0, 1.0, 0.0);
			glScalef (0.7, 0.7, 0.7);
	    	
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
	} else
		p1.x = 0;
}

void corpoInimigo(){
	glPushMatrix();
		glColor3f(0.96, 0.37, 0.64);
	   	glutSolidSphere(tam/2,16,16);
	glPopMatrix();	
			
  	glPushMatrix();
   		glColor3f(0.96, 0.37, 0.64);
   		glTranslatef (0.0, 0.0, 0.15);
   		glScalef (0.9, 1.0, 1.0);
   		glutSolidCone(tam/2, 0.9, 16,16);
  	glPopMatrix();
	    	
  	glPushMatrix();
   		glColor3f(1.0, 1.0, 1.0);
   		glTranslatef (0.0, 0.0, 1.1);
   		glScalef (0.25, 0.25, 0.25);
   		glutSolidCube(tam);
  	glPopMatrix();
}

void inimigos(){
	if(!m1){ // se não morreu
		//Inimigo 1
		inim1.y+=(dirInim1)*velocInim;
		colisaoInimigos(&inim1, 'y', &dirInim1);
		
		glPushMatrix();
		    glTranslatef (inim1.x, inim1.y, 1.0);
		    glRotatef (180, 1.0, 0.0, 0.0);
			glScalef (0.8, 0.8, 0.8);
			
			corpoInimigo();
		glPopMatrix();
	} else
		inim1.x = 0;
	
	if(!m2){ // se não morreu
		//Inimigo 2
		inim2.y+=(dirInim2)*velocInim;
		colisaoInimigos(&inim2, 'y', &dirInim2);
		 
		glPushMatrix();
		    glTranslatef (inim2.x, inim2.y, 1.0);
		    glRotatef (180, 1.0, 0.0, 0.0);
			glScalef (0.8, 0.8, 0.8);
			
			corpoInimigo();
		glPopMatrix();
	} else
		inim2.x = 0;
		
	if(!m3){ // se não morreu
		//Inimigo 3
		inim3.x+=(dirInim3)*velocInim;
		colisaoInimigos(&inim3, 'x', &dirInim3);
		 
		glPushMatrix();
		    glTranslatef (inim3.x, inim3.y, 1.0);
			glRotatef (180, 1.0, 0.0, 0.0);
			glScalef (0.8, 0.8, 0.8);
			
			corpoInimigo();
		glPopMatrix();
	} else
		inim3.x = 0;
}

void DesenhaTexto(void *font, char *string){
	// Exibe caractere a caractere
	while(*string)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++); 
}

void desenhaMapa(){
	GLfloat w = 1.0, h = 1.0, x = 0, y = 0;
		
	for(int i = 0; i < tamMapa; i++){ // coluna
		for(int j = 0; j < tamMapa; j++){ // linha
			if(mapa[i][j]==0){
				glBindTexture(GL_TEXTURE_2D, id_texturas[3]);
				glPushMatrix();
					glScalef (1.0, 1.0, 0.1);
					glTranslatef (j*h+x, i*w+y, 0.0);
    				//glColor3f(0.17, 0.87, 0.13);
					glColor3f(1.0, 1.0, 1.0); 
            		desenha_bloco();
    			glPopMatrix();
			}else if(mapa[i][j]==1){
				glBindTexture(GL_TEXTURE_2D, id_texturas[4]);
				glPushMatrix();
					glScalef (1.0, 1.0, 0.1);
					glTranslatef (j*h+x, i*w+y, 0.0);
    				//glColor3f(0.06, 0.44, 0.22);
					glColor3f(1.0, 1.0, 1.0);  
            		desenha_bloco();
    			glPopMatrix();
			}else if(mapa[i][j]==2){
				glBindTexture(GL_TEXTURE_2D, id_texturas[2]);
				glPushMatrix();
					glTranslatef (j*h+x, i*w+y, 0.5);
    				//glColor3f(0.43,0.43,0.43);
					glColor3f(1.0, 1.0, 1.0);  
            		desenha_bloco();
    			glPopMatrix();
			}else if(mapa[i][j]==3){
				glBindTexture(GL_TEXTURE_2D, id_texturas[1]);
				glPushMatrix();
					glTranslatef (j*h+x, i*w+y, 0.5);
    				//glColor3f(0.6,0.3,0.0);
					glColor3f(1.0, 1.0, 1.0);   
            		desenha_bloco();
    			glPopMatrix();
			}else if(mapa[i][j]==4){
				glBindTexture(GL_TEXTURE_2D, id_texturas[3]);
				glPushMatrix();
					glScalef (1.0, 1.0, 0.1);
					glTranslatef (j*h+x, i*w+y, 0.0);
    				glColor3f(0.17, 0.87, 0.13); 
            		desenha_bloco();
    			glPopMatrix();
				glPushMatrix();
					glDisable(GL_TEXTURE_2D);
					if(corBomba == 1) glColor3f(1.0,0.0,0.0);
					else glColor3f(0.0,0.0,0.0);
					glTranslatef(j*h+x, i*w+y, 0.5);
					glutSolidSphere(tam/2 - tbomb,16,16);
					glEnable(GL_TEXTURE_2D);
				glPopMatrix();
			}else if(mapa[i][j]==5){
				glBindTexture(GL_TEXTURE_2D, id_texturas[4]);
				glPushMatrix();
					glScalef (1.0, 1.0, 0.1);
					glTranslatef (j*h+x, i*w+y, 0.0);
    				glColor3f(0.06, 0.44, 0.22);  
            		desenha_bloco();
    			glPopMatrix();
				glPushMatrix();
					glDisable(GL_TEXTURE_2D);
					if(corBomba == 1) glColor3f(1.0,0.0,0.0);
					else glColor3f(0.0,0.0,0.0);
					if(corBomba == 1) glColor3f(1.0,0.0,0.0);
					glTranslatef(j*h+x, i*w+y, 0.5);
					glutSolidSphere(tam/2 - tbomb,16,16);
					glEnable(GL_TEXTURE_2D);
				glPopMatrix();
			}
		}
	}
	glDisable(GL_TEXTURE_2D);
	
	
	// PERSONAGEM PRINCIPAL
	pers_principal();
	
	//Inimigo 1
	inimigos();
	
	//Lugar pra "passar de fase"
	exit_door();
	
	// Desenha texto na tela
	if(ganhou){
		glPushMatrix();
			glColor3f(0,1,1);
			glTranslatef (6.0, 5.5, 1.5);
			glScalef(3.0, 1.2, 0.1);
			glutSolidCube(tam);
		glPopMatrix();
				
		glPushMatrix();
			glColor3f(0,0.0,1);
			glTranslatef (4.6, 4.5, 2.0);
    		glRasterPos2f((tam/2)+x, (tam/2)+y);
			    
			char str[] = "PARABENS!";
			DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,str);
		glPopMatrix();
	}
		
	if(morreu){
		glPushMatrix();
			glColor3f(1,1,1);
			glTranslatef (6.0, 5.5, 1.5);
			glScalef(3.0, 1.2, 0.1);
			glutSolidCube(tam);
		glPopMatrix();
				
		glPushMatrix();
			glColor3f(1,1,1);
			glTranslatef (4.8, 4.5, 2.0);
			glRasterPos2f((tam/2)+x, (tam/2)+y);
			    
			char str[] = "PERDEU!";
			DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,str);
		glPopMatrix();
	}

	glEnable(GL_TEXTURE_2D);
}

void verificaSeGanhou(){
	if(p1.x == 11 && p1.y == 11 && !ganhou){
		PlaySound("Win sound.wav", NULL, SND_ASYNC);
		ganhou = true;
	}
}

void reinicia(){
	p1.x = 2;
	p1.y = 1;
	inim1.x = 1;
	inim1.y = 4;
	inim2.x = 5;
	inim2.y = 5;
	inim3.x = 8;
	inim3.y = 9;
	m1 = false;
	m2 = false;
	m3 = false;
	tbomb = 0.1;
	corBomba = 0;
	ganhou = false;
	morreu = false;
	bombasColocadas = 0;
	mapa = {
			{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
			{2, 0, 1, 3, 1, 0, 1, 3, 1, 0, 3, 3, 2},
			{2, 1, 2, 1, 2, 3, 2, 1, 2, 1, 2, 1, 2},
			{2, 0, 3, 0, 3, 3, 3, 0, 1, 3, 1, 0, 2}, 
			{2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2},
			{2, 0, 3, 3, 1, 0, 1, 3, 1, 3, 1, 0, 2},
			{2, 3, 2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2},
			{2, 0, 3, 0, 3, 0, 3, 3, 1, 3, 1, 0, 2},
			{2, 3, 2, 1, 2, 3, 2, 1, 2, 1, 2, 1, 2},
			{2, 0, 1, 3, 1, 0, 1, 3, 1, 0, 1, 0, 2},
			{2, 1, 2, 1, 2, 1, 2, 1, 2, 3, 2, 3, 2},
			{2, 3, 3, 0, 1, 0, 3, 0, 1, 0, 3, 0, 2},
			{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
	};
}