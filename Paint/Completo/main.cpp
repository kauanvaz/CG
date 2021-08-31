//*********************************************************************
//  Codigo exemplo para aula pratica de Computacao Grafica 
//  Assunto: Rasterizacao e Transformacoes Geometricas
//
//  Autor: Kauan Vaz do Nascimento
//*********************************************************************
 
// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Biblioteca com funcoes matematicas
#include <cmath>
#include <cstdio>
#include <cstdlib>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>   
#include <glm/gtx/transform.hpp>   
#define maxVer 10000

// Variaveis Globais
bool click1 = false, click2 = false, click3 = false;
int opcao=1;

//------------------------ variaveis para desenho de poligono -------------------
int lados=0;
bool* clickPoli=NULL;
int desenhado=0;

double* xPoli=NULL;
double* yPoli=NULL;
// ------------------------------------------------------------------------------

int xTrans=0, yTrans=0, entrouTransl=0;

double x_1,y_1,x_2,y_2,x_3,y_3;

double xc, yc, raioEscolhido; 

int width = 512, height = 512; //Largura e altura da janela

// definição da struct cor
typedef struct{
	GLfloat r;
	GLfloat g;
	GLfloat b;
}cor;

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

//------------------------------------ código para preenchimento ----------------
typedef struct{
	int ymax; //y máximo da aresta
	float xdeymin; //x do y mínimo da aresta
	float m;
}Arestas;

typedef struct{
	int quantArestas; //quantidade de arestas
	Arestas arestas[maxVer];
}tabela;

tabela TA[512], TAA;

// ----------------------------------------------------------------------------------------------------

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
void especial(int key, int x, int y);
void mouse(int button, int state, int x, int y);

// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de retas
void reta(double x1,double y1,double x2,double y2);
// Funcao que desenha quadrilateros
void quadrilatero(double x1,double y1,double x2,double y2);
//Função que desenha todas retas necessarias para a formação do triangulo usando a fução da reta
void triangulo(double x1,double y1,double x2,double y2,double x3,double y3);
// Funcao que controi um poligono com o uso da funcao reta
void poligono(double* x, double* y);
// Funcao que implementa o Algoritmo de Bresenham para rasterizacao de circunferências
void circunferencia(double centroX, double centroY, double raio);
// Funcao que translada a figura na tela
void transladar(int vx, int vy);
// Função de preenchimento por scan
void preenchimento();
void inicializaTabelas();
void addTabela (int x1,int y1, int x2, int y2);
//Função floodFill para preenchimento
void FloodFill(int x, int y, cor corAntiga, cor novaCor);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();

// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (width, height);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (350, 250); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Rasterizacao"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); //funcao callback do teclado
    glutSpecialFunc(especial); //funcao para teclas especiais do teclado
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

void limparListaPontos(){
	while(pontos!=NULL){
		popPonto();
	}
}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla pressionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;    	
    	case 48: // codigo ASCII da tecla 0
    		printf("0 - LIMPAR TELA SELECIONADO.\n");
    		limparListaPontos();
    		glutPostRedisplay();
        break;    
    	case 49: // codigo ASCII da tecla 1
    		printf("1 - DESENHAR LINHAS SELECIONADO.\n");
            opcao=1;// alterna para desenhar linhas
        break;    	
    	case 50: // codigo ASCII da tecla 2
    		printf("2 - DESENHAR QUADRILATEROS SELECIONADO.\n");
            opcao=2;// alterna para desenhar quadrilateros
        break;    
    	case 51: // codigo ASCII da tecla 3
    		printf("3 - DESENHAR TRIANGULOS SELECIONADO.\n");
			opcao=3;// alterna para desenhar triangulos
        break;    
    	case 52: // codigo ASCII da tecla 4
    		printf("4 - DESENHAR CIRCUNFERENCIAS SELECIONADO.\n");
			opcao=4;// alterna para desenhar circunferencias
			printf("Insira o tamanho do raio da circunferencia: ");
    		scanf("%lf", &raioEscolhido);
        break;    
    	case 53: // codigo ASCII da tecla 5
    		printf("5 - DESENHAR POLIGONOS SELECIONADO.\n");
    		opcao=5;
			printf("Quantos lados voce quer que o poligono tenha? "); // alterna para desenhar poligonos;
			lados=0;
			while(lados<3){
				scanf("%d", &lados);
				if(lados<3) printf("Quantidade de lados invalida. Digite novamente: ");
			}
		
			free(clickPoli);
			free(xPoli);
			free(yPoli);
		
			clickPoli = (bool*)malloc(lados * sizeof(bool));
			xPoli = (double*)malloc(lados * sizeof(double));
			yPoli = (double*)malloc(lados * sizeof(double));
			for(int i=0; i<lados; i++) clickPoli[i]=false;		
        break;
    	case 54: // codigo ASCII da tecla 6
    		printf("6 - PREENCHIMENTO SELECIONADO.\n");
    		opcao=6; // alterna para o preechimento
    		printf("Quantos lados voce quer que o poligono tenha? "); // alterna para desenhar poligonos;
    		lados=0;
			while(lados<3){
				scanf("%d", &lados);
				if(lados<3) printf("Quantidade de lados invalida. Digite novamente: ");
			}
		
			free(clickPoli);
			free(xPoli);
			free(yPoli);
		
			clickPoli = (bool*)malloc(lados * sizeof(bool));
			xPoli = (double*)malloc(lados * sizeof(double));
			yPoli = (double*)malloc(lados * sizeof(double));
			for(int i=0; i<lados; i++) clickPoli[i]=false;	
        break;
    	case 55: // codigo ASCII da tecla 7
    		printf("7 - FLOODFILL SELECIONADO.\n");
			{
			opcao=7;// alterna para a tela de pintura com floodfill
			limparListaPontos();
			glClear(GL_COLOR_BUFFER_BIT); //Limpa o buffer de cores e reinicia a matriz
			glLoadIdentity();
    		glColor3f (0.0, 0.0, 0.0); // Seleciona a cor default como preto
         	circunferencia(250, 250, 50);
         	drawPontos();
         	cor antiga = {1.0f, 1.0f, 1.0f};
			cor nova = {0.0f, 1.0f, 1.0f};
         	FloodFill(250, 250, antiga, nova);
         	glutSwapBuffers();
        break;
			}
    }
}

void especial(int key, int x, int y){
	switch (key) {
		case GLUT_KEY_UP: // codigo ASCII da tecla seta para cima
			xTrans=0; 
			yTrans=10; 
			entrouTransl=1;
			glutPostRedisplay();
        break;
    	case GLUT_KEY_LEFT: // codigo ASCII da tecla seta para esquerda
			xTrans=-10; 
			yTrans=0; 
			entrouTransl=1;
			glutPostRedisplay();			
        break;
		case GLUT_KEY_DOWN: // codigo ASCII da tecla seta para baixo
			xTrans=0; 
			yTrans=-10; 
			entrouTransl=1;
			glutPostRedisplay();
        break;
    	case GLUT_KEY_RIGHT: // codigo ASCII da tecla seta para direita
    		xTrans=10; 
			yTrans=0; 
			entrouTransl=1;
			glutPostRedisplay();
        break;
	}
}

//Funcao usada na funcao callback para a utilizacao do mouse
void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
	  	  if ((state == GLUT_DOWN) && ((opcao==1)||(opcao==2))) {
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
		
		if ((state == GLUT_DOWN) && (opcao==3)) {
         	if(click2){
				click3 = true;
                x_3 = x;
                y_3 = height - y;
                printf("x3y3(%.0f,%.0f)\n",x_3,y_3);
                glutPostRedisplay();
			 }else if(click1){
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
         
         if ((state == GLUT_DOWN) && (opcao==4)){
			 click1=true;
			 xc = x;
			 yc = height - y;
			 printf("Centro(%.0f,%.0f)\n",xc,yc);
			 glutPostRedisplay();
		 }
		 
		 if (state == GLUT_DOWN && ((opcao==5)||(opcao==6))) {
         	for(int i=lados-2; i>=0; i--){
				if (clickPoli[i]){
					clickPoli[i+1]=true;
					xPoli[i+1] = x;
                	yPoli[i+1] = height - y;
                	printf("x%dy%d(%.0f,%.0f)\n",i+2,i+2,xPoli[i+1],yPoli[i+1]);
                	glutPostRedisplay();
                	break;
				}	 
			 }
			 if(clickPoli[0]==false){
				clickPoli[0]=true;
				xPoli[0] = x;
				yPoli[0] = height - y;
				printf("x1y1(%.0f,%.0f)\n",xPoli[0],yPoli[0]);
				glutPostRedisplay();	 
			 }
         }
			
         break;
/*
      case GLUT_MIDDLE_BUTTON:
         if (state == GLUT_DOWN) {
         	
         }
        break;
      case GLUT_RIGHT_BUTTON:
         if ((state == GLUT_DOWN) && (opcao==6)) {
         	
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
	
    if((opcao==1) && click1 && click2){
        reta(x_1,y_1,x_2,y_2);
        drawPontos();
        click1 = false;
        click2 = false;
    }else if((opcao==2) && click1 && click2){
        quadrilatero(x_1,y_1,x_2,y_2);
        drawPontos();
        click1 = false;
        click2 = false;
    }else if((opcao==3) && click1 && click2 && click3){
        triangulo(x_1,y_1,x_2,y_2, x_3, y_3);
        drawPontos();
        click1 = false;
        click2 = false;
        click3 = false;
    }else if((opcao==4) && click1){
		circunferencia(xc, yc, raioEscolhido); // chamada da função com os respectivos valores: 
		drawPontos();							// coord. x do centro, coord. y do centro e raio
		click1 = false;	
	}else if((opcao==5)||(opcao==6)){
		int qtrue=0; //conta a quantidade de pontos que foram selecionados até o momento
		for(int i=0; i<lados; i++){
			if(clickPoli[i]==true) qtrue++;
		}
    	if(qtrue==lados){
        	poligono(xPoli, yPoli);
        	drawPontos();
        	for(int i=0; i<lados; i++) clickPoli[i]=false;
        	
        	if(opcao==6){
        		inicializaTabelas();
        		for(int j=0; j<lados; j++) addTabela(xPoli[j], yPoli[j], xPoli[(j+1)%lados], yPoli[(j+1)%lados]);
        		preenchimento();
			}
    	}
	}
	
	if(entrouTransl==1){
		transladar(xTrans, yTrans);
		drawPontos();
		entrouTransl=0;
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

//------------------------------------------------------------- RETA -------------------------------------------------------------------

 // Função que implementa o algoritmo de Bresenham
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

//---------------------------------------------------- QUADRILATERO -------------------------------------------------------------------

// Função de auxílio para a função quadrilatero
void addRetasQuadrilatero(int tex, int tey, int bdx, int bdy){ //Adiciona as retas necessarias para a formação do quadrilatero
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
	addRetasQuadrilatero((int)tex, (int)tey, (int)bdx, (int)bdy);
}

//------------------------------------------------------ TRIANGULO -------------------------------------------------------------------

//Adiciona as retas necessarias para a formação do triangulo	
void triangulo(double x1,double y1,double x2,double y2,double x3,double y3){
	reta(x1, y1, x2, y2);
	reta(x1, y1, x3, y3);
	reta(x2, y2, x3, y3);
}

//------------------------------------------------------ POLIGONO -------------------------------------------------------------------

void poligono(double* x, double* y){
	for(int i=0; i<lados; i++){
		reta(x[i], y[i], x[(i+1)%lados], y[(i+1)%lados]);
	}
}

//--------------------------------------------------- CIRCUNFERENCIA -------------------------------------------------------------------

void addSimetricos(int xi, int yi, int centroX, int centroY){ // adiciona os pontos simetricos em relação aos outros octantes
	pontos = pushPonto(xi+centroX, yi+centroY);
	pontos = pushPonto(yi+centroX, xi+centroY);
	pontos = pushPonto(-yi+centroX, xi+centroY);
	pontos = pushPonto(xi+centroX, -yi+centroY);
	pontos = pushPonto(-xi+centroX, -yi+centroY);
	pontos = pushPonto(-yi+centroX,-xi+centroY);
	pontos = pushPonto(yi+centroX, -xi+centroY);
	pontos = pushPonto(-xi+centroX, yi+centroY);
}

 void circunferencia(double centroX, double centroY, double raio){
	//Armazenando os extremos para desenho
    pontos = pushPonto((int)centroX,(int)(raio+centroY));
    pontos = pushPonto((int)centroX,(int)(-raio+centroY));
	pontos = pushPonto((int)(raio+centroX), (int)centroY);
	pontos = pushPonto((int)(-raio+centroX), (int)centroY);
	
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

//--------------------------------------------------- TRANFORMAÇÕES -------------------------------------------------------------

void transladar(int vx, int vy){
	ponto * aux;
	aux = pontos;
	
	glm::mat3 t(
		1, 0, 0,
		0, 1, 0,
		vx, vy, 1
	);
	while(aux != NULL){
		glm::vec3 v(aux->x, aux->y, 1);
		glm::vec3 novo = t*v;
		aux->x = novo[0];
		aux->y = novo[1];
		aux = aux->prox;
	}
}

//----------------------------------------------------PREENCHIMENTO -------------------------------------------------------------

void inicializaTabelas(){
	int i;
	for (i=0; i<512; i++){
		TA[i].quantArestas = 0;
	}
	
	TAA.quantArestas = 0;
}

// Função para ordenar as tabelas de acordo com o valor do x do y min de uma aresta
void insertionSort(tabela *ta){
	int i,j;
	Arestas temp;

	for (i = 1; i < ta->quantArestas; i++){
		temp.ymax = ta->arestas[i].ymax;
		temp.xdeymin = ta->arestas[i].xdeymin;
		temp.m = ta->arestas[i].m;
		j = i - 1;

		while ((temp.xdeymin < ta->arestas[j].xdeymin) && (j >= 0)){
			ta->arestas[j + 1].ymax = ta->arestas[j].ymax;
			ta->arestas[j + 1].xdeymin = ta->arestas[j].xdeymin;
			ta->arestas[j + 1].m = ta->arestas[j].m;
			j = j - 1;
		}
		ta->arestas[j + 1].ymax = temp.ymax;
		ta->arestas[j + 1].xdeymin = temp.xdeymin;
		ta->arestas[j + 1].m = temp.m;
	}
}

void formarTuplas (tabela *recebe,int ym,int xm,float m){
	(recebe->arestas[(recebe)->quantArestas]).ymax = ym;
	(recebe->arestas[(recebe)->quantArestas]).xdeymin = (float)xm;
	(recebe->arestas[(recebe)->quantArestas]).m = m;
			
	insertionSort(recebe);
		
	(recebe->quantArestas)++;	
}

void addTabela (int x1,int y1, int x2, int y2){
	float m,minv;
	int yMaxTabela,xDeyMinTabela, scanline;
	
	if (x2==x1){
		minv=0.000000;
	}else{
	m = ((float)(y2-y1))/((float)(x2-x1));
	
	if (y2==y1) // não adiciona arestas horizontais
		return;
		
	minv = (float)1.0/m;
	}
	
	if (y1>y2){
		scanline=y2;
		yMaxTabela=y1;
		xDeyMinTabela=x2;
	}else{
		scanline=y1;
		yMaxTabela=y2;
		xDeyMinTabela=x1;	
	}
	formarTuplas(&TA[scanline],yMaxTabela,xDeyMinTabela,minv);	
}

void remove(tabela *ta, int yy){
	int i,j;
	for (i=0; i< ta->quantArestas; i++){
		if (ta->arestas[i].ymax == yy){		
			for ( j = i ; j < ta->quantArestas -1 ; j++ ){
				ta->arestas[j].ymax = ta->arestas[j+1].ymax;
				ta->arestas[j].xdeymin = ta->arestas[j+1].xdeymin;
				ta->arestas[j].m = ta->arestas[j+1].m;
		}
				ta->quantArestas--;
			i--;
		}
	}
}

void atualizax(tabela *ta){
	int i;
	
	for (i=0; i<ta->quantArestas; i++){
		(ta->arestas[i]).xdeymin = (ta->arestas[i]).xdeymin + (ta->arestas[i]).m;
	}
}

void preenchimento(){
	int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
	
	//Scanline em toda a altura da tela
	for (i=0; i<height; i++){
		//Arestas da TA para TAA
		for (j=0; j<TA[i].quantArestas; j++){
			formarTuplas(&TAA, TA[i].arestas[j].ymax, TA[i].arestas[j].xdeymin, TA[i].arestas[j].m);
		}
		
		//Remoção das arestas com y=i
		remove(&TAA, i);
		
		insertionSort(&TAA);
		
		j = 0;
		FillFlag = 0;
		coordCount = 0;
		x1 = 0;
		x2 = 0;
		ymax1 = 0;
		ymax2 = 0;
		while (j<TAA.quantArestas){
			if (coordCount%2==0){
				x1 = (int)(TAA.arestas[j].xdeymin);
				ymax1 = TAA.arestas[j].ymax;
				if (x1==x2){
					if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
						x2 = x1;
						ymax2 = ymax1;
					}else{
						coordCount++;
					}
				}else{
					coordCount++;
				}
			}else{
				x2 = (int)TAA.arestas[j].xdeymin;
				ymax2 = TAA.arestas[j].ymax;
			
				FillFlag = 0;
				
				if (x1==x2){
					if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
						x1 = x2;
						ymax1 = ymax2;
					}else{
						coordCount++;
						FillFlag = 1;
					}
				}else{
					coordCount++;
					FillFlag = 1;
				}
			
			
			if(FillFlag){
				glColor3f(1.0f,0.0f,0.0f);

				glBegin(GL_LINES);
				glVertex2i(x1,i);
				glVertex2i(x2,i);
				glEnd();
				glFlush();		
			}
		}			
		j++;
	}		
	atualizax(&TAA);
	}

}

//----------------------------------------------------- FloodFill -------------------------------------------------------------

void FloodFill(int x, int y, cor corAntiga, cor novaCor){
	cor corFigura;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &corFigura);
        
	if((corFigura.r==corAntiga.r) && (corFigura.g==corAntiga.g) && (corFigura.b==corAntiga.b)){
		
		glColor3f(novaCor.r, novaCor.g, novaCor.b);
		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
		glFlush();
		
		FloodFill(x+1, y, corAntiga, novaCor);
		FloodFill(x, y+1, corAntiga, novaCor);
		FloodFill(x-1, y, corAntiga, novaCor);
		FloodFill(x, y-1, corAntiga, novaCor);
	}
}
