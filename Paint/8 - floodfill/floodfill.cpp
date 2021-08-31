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

// definição da struct cor
typedef struct{
	GLfloat r;
	GLfloat g;
	GLfloat b;
}cor;

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

// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de circunferências
void circunferencia(double x,double y,double r);
//Função floodFill para preenchimento
void FloodFill(int x, int y, cor corAntiga, cor novaCor);
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
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla pressionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
    }
}

void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
	 //  	  if ((state == GLUT_DOWN){	
		// }
		
         break;

      case GLUT_MIDDLE_BUTTON:
         if (state == GLUT_DOWN) {
         	cor antiga = {1.0f, 1.0f, 1.0f};
			cor nova = {0.0f, 1.0f, 1.0f};
         	FloodFill(x, y, antiga, nova);
         }
        break;
/*      case GLUT_RIGHT_BUTTON:
         if ((state == GLUT_DOWN) && (opcao==6)) {
         	cor antiga = {1.0f, 1.0f, 1.0f};
			cor nova = {0.0f, 1.0f, 1.0f};
         	FloodFill(250, 250, antiga, nova);
         	glFlush();
            //glutSwapBuffers();
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
    
	circunferencia(250, 250, 50); // chamada da função com os respectivos valores: coord. x do centro, coord. y do centro e raio
	drawPontos();
	//cor antiga = {1.0f, 1.0f, 1.0f};
	//cor nova = {0.0f, 1.0f, 1.0f};
	//FloodFill(250, 250, antiga, nova);

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

void addSimetricos(int xi, int yi, int centroX, int centroY){ // adiciona os pontos simetricos em relação aos outros octantes
	pontos = pushPonto(xi+centroX, yi+centroY);
	pontos = pushPonto(yi+centroY, xi+centroX);
	pontos = pushPonto(-yi+centroY, xi+centroX);
	pontos = pushPonto(xi+centroX, -yi+centroY);
	pontos = pushPonto(-xi+centroX, -yi+centroY);
	pontos = pushPonto(-yi+centroY,-xi+centroX);
	pontos = pushPonto(yi+centroY, -xi+centroX);
	pontos = pushPonto(-xi+centroX, yi+centroY);
}

 void circunferencia(double centroX, double centroY, double raio){
	//Armazenando os extremos para desenho
    pontos = pushPonto(centroX,(int)raio+centroY);
    pontos = pushPonto(centroX,(int)-raio+centroY);
	pontos = pushPonto((int)raio+centroX, centroY);
	pontos = pushPonto((int)-raio+centroX, centroY);
	
	double d=1-raio;
	double dE=3, dSE=-2*raio+5;
	double xi=0, yi=raio;
	
	while(yi>xi){
		xi++;
		if(d<0){
			d+=dE;
			dSE+=2;
		}else{
			d+=dSE;
			dSE+=4;
			yi--;
		}
		dE+=2;

		addSimetricos((int)xi, (int)yi, (int)centroX, (int)centroY);
	}
		
}

void FloodFill(int x, int y, cor corAntiga, cor novaCor){
	cor corFigura;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &corFigura);
        
    printf("%f - %f\n", corFigura.r, corAntiga.r);
    printf("%f - %f\n", corFigura.g, corAntiga.g);
    printf("%f - %f\n", corFigura.b, corAntiga.b);  
	    
	if((corFigura.r==corAntiga.r) && (corFigura.g==corAntiga.g) && (corFigura.b==corAntiga.b)){
		printf("AAA");
		glColor3f(novaCor.r, novaCor.g, novaCor.b);
		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
		//glFlush();
		//glutSwapBuffers();
		
		FloodFill(x+1, y, corAntiga, novaCor);
		FloodFill(x, y+1, corAntiga, novaCor);
		FloodFill(x-1, y, corAntiga, novaCor);
		FloodFill(x, y-1, corAntiga, novaCor);
	}
}