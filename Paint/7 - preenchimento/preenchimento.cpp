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
#define maxVer 10000

// Variaveis Globais
int lados=0;
bool* click;
int desenhado=0;

double* x_;
double* y_;

int width = 512, height = 512; //Largura e altura da janela

// Estrututa de dados para o armazenamento dinamico dos pontos
// selecionados pelos algoritmos de rasterizacao
struct ponto{
    int x;
    int y;
    ponto * prox;
};

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
// Funcao que controi um poligono com o uso da funcao reta
void poligono(double* x, double* y);
// Funcao que percorre a lista de pontos desenhando-os na tela
void drawPontos();
void inicializaTabelas();
void addTabela (int x1,int y1, int x2, int y2);
void preenchimento();

// Funcao Principal do C
int main(int argc, char** argv){
	printf("Quantos lados voce quer que o poligono tenha? ");
	while(lados<3){
		scanf("%d", &lados);
		if(lados<3) printf("Quantidade de lados invalida. Digite novamente: ");
	}
	click = (bool*)malloc(lados * sizeof(bool));
	x_ = (double*)malloc(lados * sizeof(double));
	y_ = (double*)malloc(lados * sizeof(double));
	for(int i=0; i<lados; i++) click[i]=false;
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
	//for(int i=0; i<lados; i++) click[i]=true; //Para redesenhar os pixels selecionados
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
         if ((state == GLUT_DOWN) && (desenhado==0)) {
         	for(int i=lados-2; i>=0; i--){
				if (click[i]){
					click[i+1]=true;
					x_[i+1] = x;
                	y_[i+1] = height - y;
                	printf("x%dy%d(%.0f,%.0f)\n",i+2,i+2,x_[i+1],y_[i+1]);
                	glutPostRedisplay();
                	break;
				}	 
			 }
			 if(click[0]==false){
				click[0]=true;
				x_[0] = x;
				y_[0] = height - y;
				printf("x1y1(%.0f,%.0f)\n",x_[0],y_[0]);
				glutPostRedisplay();	 
			 }
         }
         break;

/*      case GLUT_MIDDLE_BUTTON:
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
    
	int qtrue=0; //conta a quantidade de pontos que foram selecionados até o momento
	for(int i=0; i<lados; i++){
		if(click[i]==true) qtrue++;
	}
    if(qtrue==lados){
        poligono(x_, y_);
        drawPontos();
        for(int i=0; i<lados; i++) click[i]=false;
        desenhado = 1;
        
        inicializaTabelas();
        for(int j=0; j<lados; j++) addTabela(x_[j], y_[j], x_[(j+1)%lados], y_[(j+1)%lados]);
        
        preenchimento();
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

void poligono(double* x, double* y){
	for(int i=0; i<lados; i++){
		reta(x[i], y[i], x[(i+1)%lados], y[(i+1)%lados]);
	}
}

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