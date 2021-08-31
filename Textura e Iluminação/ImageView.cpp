//*********************************************************************
//  Image View - Visualizador de Imagens 
//  Copyright (C) 2012/01 Laurindo de Sousa Britto Neto
//*********************************************************************


// Bibliotecas utilizadas pelo OpenGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "textureRGB.h"

// Declaracoes forward das funcoes utilizadas
void init(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void display(void);

// Funcao Principal do C
int main(int argc, char** argv){
    glutInit(&argc, argv); // Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // Selecao do Modo do Display e do Sistema de cor utilizado
    glutInitWindowSize (512, 512);  // Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); //Posicao inicial da janela do OpenGL
    glutCreateWindow ("Visualizador de Imagens"); // Da nome para uma janela OpenGL
    init(); // Chama funcao init();
    glutReshapeFunc(reshape); //funcao callback para redesenhar a tela
    glutDisplayFunc(display); //funcao callback de desenho
    glutKeyboardFunc(keyboard); //funcao callback do teclado
//    glutSpecialFunc(keyboard_special);	//funcao callback do teclado especial
    glutMainLoop(); // executa o loop do OpenGL
    return 0; // retorna 0 para o tipo inteiro da funcao main();
}


// Funcao com alguns comandos para a inicializacao do OpenGL;
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void reshape(int w, int h)
{
	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);

	// Reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho (0, 512, 0, 512, -1 ,1);  // Define o volume de projecao ortografica
                                      // neste caso o ponto (0,0,0) encontra-se na
                                      // canto inferior esquerdo da tela e o viewport
                                      // tem 0 a 512 pixel no eixo x, 0 a 512 pixel no
                                      // eixo y. Como sera trabalhado imagens 2D definimos
                                      // o eixo z com tamanho -1 a 1;

//   glOrtho (-256, 256, -256, 256, -1 ,1);  // Neste caso, continuamos com 512 pixels só que o
                                            // centro dos eixos passa a se localizar no centro
                                            // da tela.

//   glOrtho (-(w/2), (w/2),-(h/2),(h/2), -1 ,1); // Neste caso, ficamos com a quantidade de pixels variavel,
                                                 // dependente da largura e altura da janela, e o
                                                 // centro dos eixos continua a se localizar no centro
                                                 // da tela.

    // muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
    // quando estivermos a desenhar na tela)
	glMatrixMode(GL_MODELVIEW);

}

// Funcao usada na funcao callback para utilizacao das teclas normais do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla precionada
        case 27: // codigo ASCII da tecla ESC
            exit(0); // comando pra finalizacao do programa
        break;
    }
}

//Função para dar funções as teclas especiais do teclado
/*void keyboard_special(int key, int x, int y){
   switch(key){
      case GLUT_KEY_F1:
         break;

      case GLUT_KEY_F2:
         break;

      case GLUT_KEY_F3:
         break;

      default:
         break;
   }
}
*/

// Funcao usada na funcao callback para desenhar na tela
void display(void){

    glClear(GL_COLOR_BUFFER_BIT); //Limpa o Buffer de Cores
	glLoadIdentity();

    glRasterPos2d(0,0);
    glPixelZoom(7,7);
    glDrawPixels(TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,texture_data);


    glutSwapBuffers(); // manda o OpenGl renderizar as primitivas
}

