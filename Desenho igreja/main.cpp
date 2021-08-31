#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "circulo.h"
#include <stdlib.h>

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void display(void);

// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametros C para o glut
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (512, 512);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Igreja"); // Da nome para uma janela OpenGL
    init(); // Chama a funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
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

	glOrtho (-(w/2), (w/2),-(h/2),(h/2), -1 ,1); // Nesse caso, ficamos com a quantidade de pixels variavel,
                                                 // dependente da largura e altura da janela, e o
                                                 // centro dos eixos continua a se localizar no centro
                                                 // da tela.
   // muda para o modo GL_MODELVIEW (nao pretendemos alterar a projecao
   // quando estivermos desenhando na tela)
	glMatrixMode(GL_MODELVIEW);
}

// Funcao para plotar triangulos
void desenhaTriangulo(float R, float G, float B, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y){
	glBegin(GL_TRIANGLES);
        glColor3f(R, G, B);
		glVertex2f(p1x, p1y);
		glVertex2f(p2x, p2y);
		glVertex2f(p3x, p3y);
    glEnd();
}

// Funcao para plotar quadrilateros
void desenhaQuadrilatero(float R, float G, float B, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int p4x, int p4y){
	glBegin(GL_QUADS);
        glColor3f(R, G, B);
		glVertex2f(p1x, p1y);
		glVertex2f(p2x, p2y);
		glVertex2f(p3x, p3y);
		glVertex2f(p4x, p4y);
    glEnd();
}

// Funcao para plotar linhas
void desenhaLinhas(int p1x, int p1y, int p2x, int p2y){
	glBegin(GL_LINES);
		glVertex2f(p1x, p1y);
		glVertex2f(p2x, p2y);
    glEnd();
}

// Funcao usada na funcao callback para desenhar na tela
void display(void){
	//Limpa o Buffer de Cores e reinicia a matriz
    glClear(GL_COLOR_BUFFER_BIT); 
	glLoadIdentity();

	// TRIANGULOS
	desenhaTriangulo(1.0f, 0.0f, 0.0f, -150, 100, -200, 0, -100, 0);
	desenhaTriangulo(1.0f, 0.0f, 0.0f, 0, 200, -100, 0, 100, 0);
	desenhaTriangulo(1.0f, 0.0f, 0.0f, 150, 100, 100, 0, 200, 0);
    
    // QUADRILATEROS
    desenhaQuadrilatero(0.0f, 0.0f, 1.0f, -200, -200, -200, 0, -100, 0, -100, -200);
    desenhaQuadrilatero(1.0f, 1.0f, 0.0f, -100, -200, -100, 0, 100, 0, 100, -200);
    desenhaQuadrilatero(0.0f, 0.0f, 1.0f, 100, -200, 100, 0, 200, 0, 200, -200);
    desenhaQuadrilatero(0.0f, 0.0f, 0.0f, -50, -200, -50, -75, 50, -75, 50, -200);
    
    //LINHAS
    desenhaLinhas(-150, 100, -150, 160);
    desenhaLinhas(-175, 140, -125, 140);
    desenhaLinhas(0, 200, 0, 260);
    desenhaLinhas(-25, 240, 25, 240);
    desenhaLinhas(150, 100, 150, 160);
    desenhaLinhas(125, 140, 175, 140);

	//CIRCULOS
	desenhaCirculo(-150,-50,40,30,true);
	
	desenhaCirculo(150,-50,40,30,true);
	
	
    glFlush(); // manda o OpenGl renderizar as primitivas

}
