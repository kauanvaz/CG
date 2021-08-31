//*********************************************************************
//  Modelagem Geométrica 3D usando OpenGL
//  Codigo Exemplo
//*********************************************************************

/* Inclui os headers do OpenGL, GLU, e GLUT */
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

/* Coordenadas dos vértices do objeto: */
#define VERTICES_exit 6
GLfloat vertices_exit[VERTICES_exit][3] = {
	    {-0.5, 0.0,  0.5},
	    { 0.5, 0.0,  0.5},
	    { 0.0, 1.0,  0.0},
	    { 0.5, 0.0, -0.5},
	 	{-0.5, 0.0, -0.5},
		{ 0.0, -1.0, 0.0}
};

/* Triângulos do objeto (vértices que os constituem) */
#define FACES_exit 8
GLuint faces_exit[FACES_exit][3] = {
	    {0, 1, 2},
	    {1, 3, 2},
	    {3, 4, 2},
	    {4, 0, 2},
	 	{0, 5, 1},
	    {5, 3, 1},
	    {3, 5, 4},
	    {4, 5, 0}
};


/* Vetores normais aos vértices do objeto: */
GLfloat vertex_normals_exit[VERTICES_exit][3];

/* Vetores normais aos triângulos do objeto: */
GLfloat face_normals_exit[FACES_exit][3];

//Estrutura criada para facilitar o entendimento
struct ponto_exit{
    float x;
    float y;
    float z;
};
  
void normalizar_exit(ponto_exit * n){
	GLfloat length;
	
	length = (GLfloat) sqrt((n->x * n->x) + (n->y * n->y) + (n->z * n->z));
		
	if (length == 0.0f)
		length =  1.0f;

	n->x = n->x / length;
	n->y = n->y / length;
	n->z = n->z / length;
}

void calcularNormaisFaces_exit(){
    float x[3], y[3], z[3];
    ponto_exit a, b, n;
        
    for(int i = 0; i < FACES_exit; i++){
        x[0] = vertices_exit[faces_exit[i][0]][0];
        y[0] = vertices_exit[faces_exit[i][0]][1];
        z[0] = vertices_exit[faces_exit[i][0]][2];
    
        x[1] = vertices_exit[faces_exit[i][1]][0];
        y[1] = vertices_exit[faces_exit[i][1]][1];
        z[1] = vertices_exit[faces_exit[i][1]][2];
    
        x[2] = vertices_exit[faces_exit[i][2]][0];
        y[2] = vertices_exit[faces_exit[i][2]][1];
        z[2] = vertices_exit[faces_exit[i][2]][2];
        
        a.x = x[2]- x[0];
        a.y = y[2]- y[0];
        a.z = z[2]- z[0];
        
        b.x = x[2]- x[1];
        b.y = y[2]- y[1];
        b.z = z[2]- z[1];
        
        n.x = a.y * b.z - a.z * b.y;
        n.y = a.z * b.x - a.x * b.z;
        n.z = a.x * b.y - a.y * b.x;
       
        normalizar_exit(&n);
        
        face_normals_exit[i][0] = n.x;
        face_normals_exit[i][1] = n.y;
        face_normals_exit[i][2] = n.z;
    }
}
    
void calcularNormaisVertices_exit(){
    ponto_exit n;
    for(unsigned int i = 0; i < VERTICES_exit; i++){
        n.x = 0.0;
        n.y = 0.0;
        n.z = 0.0;
        for(int j = 0; j < FACES_exit; j++){
            if(faces_exit[j][0] == i || faces_exit[j][1] == i || faces_exit[j][2] == i){
                n.x += face_normals_exit[j][0];
                n.y += face_normals_exit[j][1];
                n.z += face_normals_exit[j][2];
            }
        }
        
        n.x /= 3.0;
        n.y /= 3.0;
        n.z /= 3.0;        
        
        normalizar_exit(&n);
        
        vertex_normals_exit[i][0] = n.x;
        vertex_normals_exit[i][1] = n.y;
        vertex_normals_exit[i][2] = n.z;        

    }
    
}        

/*
 * Desenha o objeto, triângulo por triângulo, definindo os vetores
 * normais de cada vértice (ideal para usar com Gouraud shading).
 */
void desenha_exit_door(){
    GLuint i;
    
    calcularNormaisFaces_exit();
	calcularNormaisVertices_exit();

    /* Desenha todos os triângulos do objeto */
    glBegin(GL_TRIANGLES);
        for (i=0; i<FACES_exit; i++){
            glNormal3fv(vertex_normals_exit[faces_exit[i][0]]);
            glVertex3fv(vertices_exit[faces_exit[i][0]]);

            glNormal3fv(vertex_normals_exit[faces_exit[i][1]]);
            glVertex3fv(vertices_exit[faces_exit[i][1]]);

            glNormal3fv(vertex_normals_exit[faces_exit[i][2]]);
            glVertex3fv(vertices_exit[faces_exit[i][2]]);
        }
    glEnd();
}