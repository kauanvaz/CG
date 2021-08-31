//*********************************************************************
//  Texturas usando OpenGL
//  Codigo Exemplo
//*********************************************************************

/* Inclui os headers do OpenGL, GLU, e GLUT */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

/* Inclui o ficheiro com a textura */
#include "textureRGB.h"

/**********************************************************************/
/*                       Declaração de constantes                     */
/**********************************************************************/

float angulo = 0.0;
int mseg = 10;

#define QUANT_TEX 1
unsigned int id_texturas[QUANT_TEX]; //nomes identificadores de textura

/* Coordenadas dos vértices do objeto: */
#define N_VERTICES 8
const GLfloat vertices[N_VERTICES][3] = {
    { -0.5, -0.5, -0.5 }, //0
    { -0.5, -0.5,  0.5 }, //1
    {  0.5, -0.5,  0.5 }, //2
    {  0.5, -0.5, -0.5 }, //3
 	{ -0.5,  0.5,  0.5 }, //4
    {  0.5,  0.5,  0.5 }, //5
    {  0.5,  0.5, -0.5 }, //6
    { -0.5,  0.5, -0.5 }  //7
};

/* Triângulos do objeto (vértices que os constituem) */
#define N_FACES 12
const GLuint faces[N_FACES][3] = {
    {0, 4, 7},
    {0, 1, 4},
    {1, 5, 4},
 	{1, 2, 5},
    {2, 6, 5},
    {2, 3, 6},
  	{3, 7, 6},
    {3, 0, 7},
    {7, 5, 6},
 	{7, 4, 5},
	{3, 1, 0},
	{3, 2, 1}
};



/* Coordenadas de textura dos vérices de cada triângulo */
const GLfloat tex_coords[N_FACES][3][2] = {
    {{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
    {{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
 	{{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
 	{{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
 	{{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
 	{{0.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}},
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}},
};

/* Vectores normais aos vértices do objecto: */
GLfloat vertex_normals[N_VERTICES][3];

/* Vetores normais aos triângulos do objeto: */
GLfloat face_normals[N_FACES][3];

//Estrutura criada para facilitar o entendimento
struct ponto{
    float x;
    float y;
    float z;
};
  
void normalizar(ponto * n){
	GLfloat length;
	
	length = (GLfloat) sqrt((n->x * n->x) + (n->y * n->y) + (n->z * n->z));
		
	if (length == 0.0f)
		length =  1.0f;

	n->x = n->x / length;
	n->y = n->y / length;
	n->z = n->z / length;
}
    
void calcularNormaisFaces(){
    float x[3], y[3], z[3];
    ponto a, b, n;
        
    for(int i = 0; i < N_FACES; i++){
        x[0] = vertices[faces[i][0]][0];
        y[0] = vertices[faces[i][0]][1];
        z[0] = vertices[faces[i][0]][2];
    
        x[1] = vertices[faces[i][1]][0];
        y[1] = vertices[faces[i][1]][1];
        z[1] = vertices[faces[i][1]][2];
    
        x[2] = vertices[faces[i][2]][0];
        y[2] = vertices[faces[i][2]][1];
        z[2] = vertices[faces[i][2]][2];
        
        a.x = x[2]- x[0];
        a.y = y[2]- y[0];
        a.z = z[2]- z[0];
        
        b.x = x[2]- x[1];
        b.y = y[2]- y[1];
        b.z = z[2]- z[1];
        
        n.x = a.y * b.z - a.z * b.y;
        n.y = a.z * b.x - a.x * b.z;
        n.z = a.x * b.y - a.y * b.x;
       
        normalizar(&n);
        
        face_normals[i][0] = n.x;
        face_normals[i][1] = n.y;
        face_normals[i][2] = n.z;
    }
}

void calcularNormaisVertices(){
    ponto n;
    for(unsigned int i = 0; i < N_VERTICES; i++){
        n.x = 0.0;
        n.y = 0.0;
        n.z = 0.0;
        for(int j = 0; j < N_FACES; j++){
            if(faces[j][0] == i || faces[j][1] == i || faces[j][2] == i){
                n.x += face_normals[j][0];
                n.y += face_normals[j][1];
                n.z += face_normals[j][2];
            }
        }
        
        n.x /= 3.0;
        n.y /= 3.0;
        n.z /= 3.0;        
        
        normalizar(&n);
        
        vertex_normals[i][0] = n.x;
        vertex_normals[i][1] = n.y;
        vertex_normals[i][2] = n.z;        

    }
    
}  


/**********************************************************************/
/*                        Declaração de funções                       */
/**********************************************************************/
void init_glut(const char *window_name, int argc, char** argv);
void draw_object(void);
void display_callback(void);
void reshape_callback(int w, int h);
void timer_callback(int value);
void keyboard_callback(unsigned char key, int x, int y);
void keyboard_callback_special(int key, int x, int y);
void menu_callback(int value);

/**********************************************************************/
/*                                                                    */
/*                       Função principal (main)                      */
/*                                                                    */
/**********************************************************************/
int main(int argc, char** argv){
	
	calcularNormaisFaces();
    calcularNormaisVertices();
	/* inicia o GLUT e alguns parâmetros do OpenGL */
	init_glut("Computação Gráfica: Exemplo usando Textura", argc, argv);

	/* função de controlo do GLUT */
	glutMainLoop();
	
	return 0;
}

/**********************************************************************/
/*                                                                    */
/*                         Definição de funções                       */
/*                                                                    */
/**********************************************************************/

/*
 * inicia o GLUT e alguns parâmetros do OpenGL
 */
void init_glut(const char *nome_janela, int argc, char** argv){
    /* inicia o GLUT */
    glutInit(&argc,argv);

    /* inicia o display usando RGB e double-buffering */
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(100,100);
    glutCreateWindow(nome_janela);

    /* define as funcões de callback */
    glutKeyboardFunc(keyboard_callback);
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutSpecialFunc(keyboard_callback_special);
    glutTimerFunc(mseg,timer_callback,mseg);

    /* define o menu */
    glutCreateMenu(menu_callback);
    glutAddMenuEntry("Desenhar a azul", 0);
    glutAddMenuEntry("Desenhar a verde", 1);
    glutAddMenuEntry("Desenhar a vermelho", 2);
    glutAddMenuEntry("Sair", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    /* Inicia a iluminação e as características gerais dos materiais */
    GLfloat light_pos[] = {-1.0, 1.0, 1.0, 0.0};
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

    /* Prepara a textura */
    glGenTextures(QUANT_TEX, id_texturas); //gera nomes identificadores de texturas
    glBindTexture(GL_TEXTURE_2D, id_texturas[0]); //Ativa a textura atual
    
    //Cria a textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    /* Activa o modelo de sombreagem de "Gouraud". */
    glShadeModel( GL_SMOOTH );

    /* Activa o z-buffering, de modo a remover as superfícies escondidas */
    glEnable(GL_DEPTH_TEST);

    /* Define a côr de fundo (preto) e a cor de desenho inicial (azul) */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
}

/*
 * Processa o reshape da janela
 */
void reshape_callback(int w, int h){
    /* muda para o modo GL_PROJECTION e reinicia a projecção */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    
    /* define a zona da janela onde se vai desenhar */
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    
    /* Define a forma do "viewing volume" para termos *
     * uma projecção de perspectiva (3D).             */
    gluPerspective(30, (float)w/(float)h, 1.0, 100.0);
	
	/* Transformação de visão */
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    /* muda para o modo GL_MODELVIEW (não pretendemos alterar a projecção
     * quando estivermos a desenhar no ecrã) */
    glMatrixMode (GL_MODELVIEW);
}


/*
 * Desenha o objecto, triângulo por triângulo, definindo os vectores
 * normais e as coordenadas de textura de cada vértice.
 */
void draw_object(void){
    GLuint i, j;

    /* Desenha todos os triângulos do objecto */
    glBegin(GL_TRIANGLES);
        for (i=0; i<N_FACES; i++){
            for (j=0; j<3; j++){
               /* Define o vértice j da face i */
               glTexCoord2fv(tex_coords[i][j]);
               glNormal3fv(vertex_normals[faces[i][j]]);
               glVertex3fv(vertices[faces[i][j]]);
            }
        }
    glEnd();

}


/*
 * função para controlar o display (desenhar no ecrã em cada frame)
 */
void display_callback(void){
    /* Apaga o ecrã e o depth buffer, e reinicia a matriz */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* faz variar o ângulo de rotação entre 0 e 360 graus, e roda o *
     * referencial segundo o eixo (1, 1, 0).                        */
    angulo++;
    
    if (angulo> 360.0) angulo -= 360.0;
    glRotatef(angulo, 1.0, 1.0, 0.0);

    /* Desenha o objecto */
    draw_object();

    /* Troca os buffers, mostrando o que acabou de ser desenhado */
    glutSwapBuffers();
}

/*
 * Função necessária para a animação
 */
void timer_callback(int value){
    glutTimerFunc(value, timer_callback, value);
    glutPostRedisplay(); /* Manda o redesenhar o ecrã em cada frame */
}

/*
 * Controle das teclas normais
 */
void keyboard_callback(unsigned char key, int x, int y){
    if (key==27) exit(0); /* Esc: sai do programa */
}

/*
 * Controle das teclas especiais (F1 a F12, etc...)
 */
void keyboard_callback_special(int key, int x, int y){
    switch(key){
        case 1:
            glColor3f(0.5, 0.5, 1.0);      /* F1: muda a cor para azul */
            break;

        case 2:
            glColor3f(0.5, 1.0, 0.5);      /* F2: muda a cor para verde */
            break;

        case 3:
            glColor3f(1.0, 0.5, 0.5);      /* F3: muda a cor para vermelho */
            break;

    }

}

/*
 * Função callback para o controle do menu pop-up
 */
void menu_callback(int value){
    switch(value){
        case 0:
            glColor3f(0.5, 0.5, 1.0);   /* Opção 1: muda a cor para azul */
            break;

        case 1:
            glColor3f(0.5, 1.0, 0.5);   /* Opção 2: muda a cor para verde */
            break;

        case 2:
            glColor3f(1.0, 0.5, 0.5);   /* Opção 3: muda a cor para vermelho */
            break;

        case 3:
            exit(0);                    /* Opção 4: sai do programa */
            break;

        default:
            break;
    }

    /* Manda o redesenhar a tela quando o menu pop-up for desativado */
    glutPostRedisplay();

}


