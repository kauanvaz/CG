#include <math.h>

//x e y para que se possa definir o centro da circunferência
void desenhaCirculo(int x, int y, float raio, int num_linhas,bool preenchido)
{
    int i;
    GLfloat angulo;
   
    angulo = 2 * M_PI / num_linhas; //divide 360° pela quantidade de linhas que serao desenhadas
 
    glBegin(GL_TRIANGLE_FAN); // Primitiva que liga os pontos gerando segmentos de reta em um loop
	glColor3f(0.0f, 1.0f, 0.0f); //cor verde        
    
    for(i = 1; i <= num_linhas; i++) // for usado para o calculo de cada ponto pertencente aos extremos das retas
    {
		// comando que calcula as coord. da rotação e desenha as retas na tela    	
		glVertex2f( x + raio*cos(i * angulo) , y + raio*sin(i * angulo) );
	}
    glEnd(); 
}


