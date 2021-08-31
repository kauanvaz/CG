//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica 
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Prof. Laurindo de Sousa Britto Neto
//*********************************************************************
 
// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Variaveis Globais
bool click1 = false, click2 = false;

double x_1,y_1,x_2,y_2;

int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
    int x;
    int y;
    ponto * prox;
};

// Lista encadeada de pontos
// indica o primeiro elemento da lista
ponto * pontos = NULL;

// Funcao para armazenar um ponto na lista
// Armazena como uma Pilha (empilha)
ponto * pushPonto(int x, int y){
	ponto * pnt;
	pnt = new ponto;
	pnt->x = x;
	pnt->y = y;
	pnt->prox = pontos;
	pontos = pnt;
	return pnt;
}

// Funcao para desarmazenar um ponto na lista
// Desarmazena como uma Pilha (desempilha)
ponto * popPonto(){
	ponto * pnt;
	pnt = NULL;
	if(pontos != NULL){
		pnt = pontos->prox;
		delete pontos;
		pontos = pnt;
	}
	return pnt;
}

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de retas
void reta(double x1,double y1,double x2,double y2);
// Funcao que desenha quadrilateros
void quadrilatero(double x1,double y1,double x2,double y2);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Rasterizacao"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutMouseFunc(mouse); //funcao callback do mouse
    glutDisplayFunc(display); //funcao callback de desenho
    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}

// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

void reshape(int w, int h)
{
	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
   // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);
    click1 = true; //Para redesenhar os pixels selecionados
    click2 = true;
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla pressionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
    }
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            if(click1){
                click2 = true;
                x_2 = x;
                y_2 = height - y;
                printf("x2y2(%.0f,%.0f)\n",x_2,y_2);
                glutPostRedisplay();
            }else{
                click1 = true;
                x_1 = x;
                y_1 = height - y;
                printf("x1y1(%.0f,%.0f)\n",x_1,y_1);
            }
         }
         break;
/*
      case GLUT_MIDDLE_BUTTON:
         if (state == GLUT_DOWN) {
            glutPostRedisplay();
         }
         break;
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
            glutPostRedisplay();
         }
         break;
*/
      default:
         break;
   }
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
    glLoadIdentity();
    
    glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
    

    if(click1 && click2){
        quadrilatero(x_1,y_1,x_2,y_2);
        drawPontos();
        click1 = false;
        click2 = false;
    }

    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas

}

//Funcao que desenha os pontos contidos em uma lista de pontos
void drawPontos(){
    ponto * pnt;
    pnt = pontos;
    glBegin(GL_POINTS); // Seleciona a primitiva GL_POINTS para desenhar
        while(pnt != NULL){
            glVertex2i(pnt->x,pnt->y);
            pnt = pnt->prox;
        }
    glEnd();  // indica o fim do desenho
}

 void reta(double x1,double y1,double x2,double y2){
	//Armazenando os extremos para desenho
    pontos = pushPonto((int)x1,(int)y1);
    pontos = pushPonto((int)x2,(int)y2);
	
	int declive = 0, simetrico = 0;
	double deltaX = x2-x1;
    double deltaY = y2-y1;
    
    if (deltaX*deltaY<0){
		   y1 = -y1;
		   y2 = -y2;
		   deltaY = -deltaY;
		   
		   simetrico=1;	
	} 
    if (abs(deltaX)<abs(deltaY)){
    	double aux1;
    	
    	aux1 = x1;
    	x1 = y1;
    	y1 = aux1;
    	
    	aux1 = x2;
    	x2 = y2;
    	y2 = aux1;
    	
    	aux1 = deltaX;
		deltaX = deltaY;
		deltaY = aux1;

		declive = 1;
	}
	if (x1>x2){
		double aux2; 
		
		aux2 = x1;
		x1 = x2;
		x2 = aux2;
		
		aux2 = y1;
		y1 = y2;
		y2 = aux2;

		deltaX = -deltaX;
		deltaY = -deltaY;
	}
	
    double yi=y1, auxY, auxX;
    double d = 2*deltaY - deltaX;
    double incE = 2*deltaY;
    double incNE = 2*(deltaY-deltaX);
	for (int x=x1+1;x<x2;x++) {
        if (d<=0) {
            d+=incE;
        } else {
            d+=incNE;
            yi++;
        }
        auxX=x;
        auxY = yi;
        
        if(declive==1){
			double aux3;	 
			
			aux3 = auxX;
			auxX = auxY;
			auxY = aux3;
		}
		if(simetrico==1){
			auxY=-auxY;
		}
        pontos = pushPonto((int)auxX,(int)auxY);
    }
}
    
void addRetas(int tex, int tey, int bdx, int bdy){ //Adiciona as retas necessarias para a formação do quadrilatero
	reta(tex, tey, bdx, tey);
	reta(bdx, tey, bdx, bdy);
	reta(bdx, bdy, tex, bdy);
	reta(tex, bdy, tex, tey);
}
	
void quadrilatero(double x1,double y1,double x2,double y2){
	double tex, tey, bdx, bdy; //Coordenadas do topo esquerdo e da base direita
	if (x1>x2 && y1>y2){
		tex = x1;
		tey = y1;
		bdx = x2;
		bdy = y2;
	}else{
		tex = x2;
		tey = y2;
		bdx = x1;
		bdy = y1;
	}
	addRetas((int)tex, (int)tey, (int)bdx, (int)bdy);
}