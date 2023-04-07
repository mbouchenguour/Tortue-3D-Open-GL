/****************************************************************************************/
/*                     cube.cpp                    */
/****************************************************************************************/
/*         Affiche a l'ecran un cube en 3D         */
/****************************************************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#include <cstdio>
#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h> /* Pour les autres systemes */
#endif
#include <cstdlib>
#include <cmath>
#include <GL/freeglut.h>
#include <jpeglib.h>


const int largimg = 256;
const int hautimg = 256;
unsigned char image[largimg*hautimg*3];
float zoom = 1.0, mouvement = 0.0;
double a = 0, b=0;
bool lumiere0 = false, lumiere1 = false, rotationQueue = true, patteAG = false, patteAD = false, patteDG = false, patteDD = false, premierMouvement = false; //AG = avant gauche, DG = derrière gauche, permet de savoir si une jambe à avancer ou non
float rPatteAG = 0.0, rPatteAD = 0.0, rPatteDG = 0.0, rPatteDD = 0.0; //Permet de faire une rotation aux pattes


unsigned char textureCarapace[largimg][hautimg][3];
unsigned char texturePlastron[largimg][hautimg][3];
unsigned char ecaille[largimg][hautimg][3];

/*class Point*/
class Point{
    public :
	//coordonnées x, y et z du point
	double x;
	double y;
	double z;
	// couleur r, v et b du point
	float r;
	float g;
	float b;
};



char presse;
int anglex,angley,anglez, x,y,z, xold,yold,zold;

/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche,int x,int y);
void clavierSpecial(int key, int x, int y);
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void sphere(double r, int NM, int NP);
void sphereCarapace(double r, int NM, int NP);
void spherePlastron(double r, int NM, int NP);
void cylindre(double r, int n, double h);
void loadJpegImage(char *fichier, unsigned char texture[largimg][hautimg][3]);


int main(int argc,char **argv)
{
/* Chargement de la texture */
  loadJpegImage("./carapace.jpg", textureCarapace);
  loadJpegImage("./plastron.jpg", texturePlastron);
  loadJpegImage("./ecaille.jpg", ecaille);



  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(500,500);
  glutCreateWindow("tortue");

  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);
  glEnable(GL_DEPTH_TEST);



  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(clavierSpecial);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);



  /* Parametrage du placage de textures */
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);


  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}


void affichage()
{


    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    //Activation lumiere
    if(lumiere0 == false && lumiere1==false)
        glDisable(GL_LIGHTING);
    else
        glEnable(GL_LIGHTING);


   //Lumiere ambient + diffuse + specular
    GLfloat position0[]={-2,2,3,1},direction0[]={0,0,0},ambient0[]={0.1,0.1,0.1,1}, diffuse0[]={0.7,0.7,0.2,1}, specular0[]={0.5,0.2,0.0,1};
    glLightfv(GL_LIGHT0,GL_POSITION,position0);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,direction0);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient0);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse0);
    glLightfv(GL_LIGHT0,GL_SPECULAR,specular0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular0);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 125.0);

    if(lumiere0 == true)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //lumiere specular
    GLfloat position1[]={-2,2,3,1}, direction1[]={0,0,0}, specular1[]={1.0,1.0,1.0,1};
    glLightfv(GL_LIGHT1,GL_POSITION,position1);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,direction1);
    glLightfv(GL_LIGHT1,GL_SPECULAR,specular1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular1);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 125.0);


    if(lumiere1 == true)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);


    //Gestion du zoom
    glScalef(zoom,zoom,1.0f);

    //Permet de tourner autour de l'objet
    glRotatef(angley,1.0,0.0,0.0);
    glRotatef(anglex,0.0,1.0,0.0);


    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,textureCarapace);  //on charge la texture de la carapace
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix(); //On met tout dans une seul matrice pour pouvoir avancer la tortue
    glTranslatef(mouvement,0,0); //Permet d'avancer toute la tortue

    /* Carapace */
    glPushMatrix();
    glColor3f(0.7, 0.5, 0.1); //On applique du marron pour avoir un effet plus réaliste
    glRotatef(90,0,1,0);
    glScalef(1,1.1,1.3);
    sphereCarapace(0.5, 8,7); // On génère la carapace de la tortue
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,texturePlastron);  //on charge la texture du plastron
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    /* Plastron */
    glPushMatrix();
    glColor3f(0.7, 0.5, 0.1);
    glRotatef(180,1,0,0);
    glRotatef(-90,0,1,0);
    glScalef(1,0.4,1.3);
    spherePlastron(0.5, 8, 7);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,largimg,hautimg,0,GL_RGB,GL_UNSIGNED_BYTE,ecaille);  //On charge la texture ecaille
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glEnable(GL_TEXTURE_2D);

    //Cou
    glPushMatrix();
    glColor3f(0, 0.3, 0); //On applique la texture sur du vert foncé pour avoir un effet plus réaliste

    glTranslatef(0.7, 0.0 ,0);
    glRotatef(90,0,0,1);
    glRotatef(90,0,1,0);
    glScalef(1.5,1.0,1.0);
    cylindre(0.075,20,0.25);
    glPopMatrix();

    //On utilise la même texture pour le cou et la tête donc pas de glDisable(GL_TEXTURE_2D)

    //Tête
    glPushMatrix();
    glColor3f(0, 0.3, 0);
    glTranslatef(0.9, 0.0 ,0);
    glScalef(1.5,1,1);
    sphere(0.125,20,20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //Oeil droit
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(1.0, 0.07 ,0.07);
    glutSolidSphere(0.03,20,20);
    glPopMatrix();

    //Oeil gauche
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(1.0, 0.07 ,-0.07);
    glutSolidSphere(0.03,20,20);
    glPopMatrix();





    //Pate avant gauche

    glPushMatrix(); //On met toute la patte dans une matrice pour pouvoir faire avancer la tortue

    glTranslatef(0.40, -0.225 ,0.20);
    glRotatef(rPatteAG,0,0,1);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    cylindre(0.1, 15, 0.35);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    glTranslatef(0.0, -0.16 ,0.0);
    glScalef(4,1,4);
    sphere(0.03,19,12);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150, -0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();


    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,-0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.0);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPopMatrix();


    //Pate avant droite


    glPushMatrix(); //On met toute la patte dans une matrice pour pouvoir la faire avancer


    glTranslatef(0.40, -0.225 ,-0.20);
    glRotatef(rPatteAD,0,0,1);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    cylindre(0.1, 15, 0.35);
    glPopMatrix();



    glPushMatrix();
    glColor3f(0, 0.3, 0);
    glTranslatef(0.0, -0.16 ,0.0);
    glScalef(4,1,4);
    sphere(0.03,19,12);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150, -0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,-0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.0);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPopMatrix();


    //Pate arriere droite

    glPushMatrix(); //On met toute la patte dans une matrice pour pouvoir la faire avancer

    glTranslatef(-0.35, -0.225 ,-0.20);
    glRotatef(rPatteDD,0,0,1);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    cylindre(0.1, 15, 0.35);
    glPopMatrix();



    glPushMatrix();
    glColor3f(0, 0.3, 0);
    glTranslatef(0.0, -0.16 ,0.0);
    glScalef(4,1,4);
    sphere(0.03,19,12);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150, -0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,-0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.0);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPopMatrix();

    //Pate arriere gauche

    glPushMatrix(); //On met toute la patte dans une matrice pour pouvoir la faire avancer

    glTranslatef(-0.35, -0.225 ,0.20);
    glRotatef(rPatteDG,0,0,1);

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    cylindre(0.1, 15, 0.35);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor3f(0, 0.3, 0);
    glTranslatef(0.0, -0.16 ,0.0);
    glScalef(4,1,4);
    sphere(0.03,19,12);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150, -0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,-0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.0);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.025);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.5, 0.2, 0.0);
    glTranslatef(0.070, -0.150 ,0.05);
    glRotatef(90,0,1,0);
    glScalef(1,1,0.3);
    glutSolidCone(0.010, 0.35, 20, 20);
    glPopMatrix();

    glPopMatrix();


    //queue

    glPushMatrix();
    glColor3f(0, 0.3, 0);
    glTranslatef(-0.63, 0 ,0);
    glRotatef(-90,0,1,0);
    glRotatef(a,0,1,0); //a permet de faire pivoter la queue pour l'animation automatique
    glutSolidCone(0.020, 0.20, 20, 20);
    glPopMatrix();

    glPopMatrix(); //On referme la matrice de la tortue

    glutIdleFunc(idle);


    //Repère
    //axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(1, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 1,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 0,1.0);
    glEnd();

  glFlush();

  //On echange les buffers
  glutSwapBuffers();
}


void idle()
{
    if(rotationQueue == true){
        a += 1.5;
        if(a == 45)
            rotationQueue = false;
    }
    else
    {
        a -= 1.5;
        if(a == -45)
            rotationQueue = true;
    }

    glutPostRedisplay();
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    break;

    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    break;

    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
    break;

    case 'd':
      glEnable(GL_DEPTH_TEST);
    break;

    case 'D':
      glDisable(GL_DEPTH_TEST);
    break;

    case 'a'://Les faces à l'envers s'affichent en fil de fer
      glPolygonMode(GL_FRONT,GL_FILL);
      glPolygonMode(GL_FRONT,GL_LINE);
    break;

    case 'l':   //Active ou desactive la lumiere1
      lumiere0 = !lumiere0;
    break;
    case 'L':   //Active ou desactive la lumiere2
      lumiere1 = !lumiere1;
    break;

    case 'r':  //Avance la tortue
      if(patteAG == false){
        rPatteAG = 20;
        patteAG =true;
      }
      else if(patteDD == false){
        rPatteDD = 20;
        patteDD =true;
      }
      else if(premierMouvement == false){
        rPatteAG = 0;
        rPatteDD = 0;
        mouvement += 0.25;
        premierMouvement = true;
      }
      else if(patteAD == false){
        rPatteAD = 20;
        patteAD = true;
      }
      else if(patteDG == false){
        rPatteDG = 20;
        patteDG =true;
      }
      else
      {
        mouvement += 0.25;
        rPatteAD = 0;
        rPatteDG = 0;
        patteAG = false;
        patteAD = false;
        patteDG = false;
        patteDD = false;
        premierMouvement = false;
      }
      break;

    case 'R':  //Recule la tortue

      if(patteDD == false){
        rPatteDD = -20;
        patteDD =true;
      }
      else if(patteAG == false){
        rPatteAG = -20;
        patteAG =true;
      }
      else if(premierMouvement == false){
        rPatteAG = 0;
        rPatteDD = 0;
        mouvement -= 0.25;
        premierMouvement = true;
      }
      else if(patteDG == false){
        rPatteDG = -20;
        patteDG =true;
      }
      else if(patteAD == false){
        rPatteAD = -20;
        patteAD = true;
      }
      else
      {
        mouvement -= 0.25;
        rPatteAD = 0;
        rPatteDG = 0;
        patteAG = false;
        patteAD = false;
        patteDG = false;
        patteDD = false;
        premierMouvement = false;
      }
    break;

    case 'z' : /* Zoomer sur l'objet */
      if(zoom<3.0)
        zoom+=0.1;
    break;

    case 'Z' : /* DÃ©zoomer sur l'objet */
      if(zoom>0.1)
        zoom-=0.1;
    break;

    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
    }
    glutPostRedisplay();
}

void clavierSpecial (int key, int x, int y)
{
    switch(key){
        case GLUT_KEY_LEFT: // Tourne à droite de de la tortue
            anglex-=2;
        break;

        case GLUT_KEY_RIGHT: // Tourne à gauche de de la tortue
            anglex+=2;
        break;

        case GLUT_KEY_UP:   // Tourne en bas de de la tortue
            angley-=2;
        break;

        case GLUT_KEY_DOWN: // Tourne en haut de de la tortue
            angley+=2;
        break;
    }
    glutPostRedisplay();
}




void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }

//Genere une sphere dont la texture sera enroulé sur toute la sphere
void sphere(double r, int NM, int NP){

    float tempI;
    float tempJ;
    float tempNM = (float)NM;
    float tempNP = (float)NP;

    Point pSphere[NM*NP];
    int fSphere[NM*(NP-1)][4];

    for(int j = 0; j < NP; j++){
        for(int i = 0; i < NM; i++){

            float theta = (0 + i*((2*M_PI)/NM));
            float phi = (-M_PI/2) + j*(M_PI/(NP-1));

            pSphere[i+j*NM].x = r*cos(theta)*cos(phi);
            pSphere[i+j*NM].y = r*sin(theta)*cos(phi);
            pSphere[i+j*NM].z = r*sin(phi);

        }
    }

    for(int j = 0; j < (NP-1); j++){
        for(int i = 0; i < NM; i++){
            fSphere[i+j*NM][0] = i+j*NM;
            fSphere[i+j*NM][1] = ((i+1) % NM) + j*NM;
            fSphere[i+j*NM][2] = ((j+1) * NM) + ((i+1) % NM);
            fSphere[i+j*NM][3] = i + (j+1)*NM;
        }
    }

    for(int j = 0; j < (NP-1); j++){
        for(int i = 0; i < NM; i++){

          glBegin(GL_POLYGON);
          tempI = (float)i;
          tempJ = (float)j;
          glTexCoord2f(tempI/tempNM,(tempNP-j)/tempNP);
          glVertex3f(pSphere[fSphere[i+j*NM][0]].x, pSphere[fSphere[i+j*NM][0]].y, pSphere[fSphere[i+j*NM][0]].z);
          glTexCoord2f((tempI+1)/tempNM,(tempNP-j)/tempNP);
          glVertex3f(pSphere[fSphere[i+j*NM][1]].x, pSphere[fSphere[i+j*NM][1]].y, pSphere[fSphere[i+j*NM][1]].z);
          glTexCoord2f((tempI+1)/tempNM,(tempNP-j-1)/tempNP);
          glVertex3f(pSphere[fSphere[i+j*NM][2]].x, pSphere[fSphere[i+j*NM][2]].y, pSphere[fSphere[i+j*NM][2]].z);
          glTexCoord2f(tempI/tempNM,(tempNP-j-1)/tempNP);
          glVertex3f(pSphere[fSphere[i+j*NM][3]].x, pSphere[fSphere[i+j*NM][3]].y, pSphere[fSphere[i+j*NM][3]].z);


          glEnd();
          }
    }
  }

//Génère une demi sphere représentant la carapace avec la texture enroulé sur toute la demi sphere
void sphereCarapace(double r, int NM, int NP){

    Point pSphere[NM*NP];
    int fSphere[NM*(NP-1)][4];

    float tempI;
    float tempJ;
    float tempNM = (float)NM;
    float tempNP = (float)NP;

    for(int j = 0; j < NP; j++){
        for(int i = 0; i < NM; i++){

            float theta = (0 + i*((2*M_PI)/NM));
            float phi = (-M_PI/2) + j*(M_PI/(NP-1));

            pSphere[i+j*NM].x = r*cos(theta)*cos(phi);
            pSphere[i+j*NM].y = r*sin(theta)*cos(phi);
            pSphere[i+j*NM].z = r*sin(phi);

        }
    }

    for(int j = 0; j < (NP-1); j++){
        for(int i = 0; i < NM; i++){
            fSphere[i+j*NM][0] = i+j*NM;
            fSphere[i+j*NM][1] = ((i+1) % NM) + j*NM;
            fSphere[i+j*NM][2] = ((j+1) * NM) + ((i+1) % NM);
            fSphere[i+j*NM][3] = i + (j+1)*NM;
        }
    }

    for(int j = 0; j < NP-1; j++){
        for(int i = 0; i < NM/2; i++){


          glBegin(GL_POLYGON);
          tempI = (float)i;
          tempJ = (float)j;
          glTexCoord2f((tempI/tempNM)*2,((tempNP-j)/tempNP)*2);
          glVertex3f(pSphere[fSphere[i+j*NM][0]].x, pSphere[fSphere[i+j*NM][0]].y, pSphere[fSphere[i+j*NM][0]].z);
          glTexCoord2f(((tempI+1)/tempNM)*2,((tempNP-j)/tempNP)*2);
          glVertex3f(pSphere[fSphere[i+j*NM][1]].x, pSphere[fSphere[i+j*NM][1]].y, pSphere[fSphere[i+j*NM][1]].z);
          glTexCoord2f(((tempI+1)/tempNM)*2,((tempNP-j-1)/tempNP)*2);
          glVertex3f(pSphere[fSphere[i+j*NM][2]].x, pSphere[fSphere[i+j*NM][2]].y, pSphere[fSphere[i+j*NM][2]].z);
          glTexCoord2f((tempI/tempNM)*2,((tempNP-j-1)/tempNP)*2);
          glVertex3f(pSphere[fSphere[i+j*NM][3]].x, pSphere[fSphere[i+j*NM][3]].y, pSphere[fSphere[i+j*NM][3]].z);
          glEnd();
          }
    }
  }


//Génère une demi sphere représentant le plastron avec la texture plaqué sur chaque face de la demi sphere
void spherePlastron(double r, int NM, int NP){

    Point pSphere[NM*NP];
    int fSphere[NM*(NP-1)][4];


    for(int j = 0; j < NP; j++){
        for(int i = 0; i < NM; i++){

            float theta = (0 + i*((2*M_PI)/NM));
            float phi = (-M_PI/2) + j*(M_PI/(NP-1));

            pSphere[i+j*NM].x = r*cos(theta)*cos(phi);
            pSphere[i+j*NM].y = r*sin(theta)*cos(phi);
            pSphere[i+j*NM].z = r*sin(phi);

        }
    }

    for(int j = 0; j < (NP-1); j++){
        for(int i = 0; i < NM; i++){
            fSphere[i+j*NM][0] = i+j*NM;
            fSphere[i+j*NM][1] = ((i+1) % NM) + j*NM;
            fSphere[i+j*NM][2] = ((j+1) * NM) + ((i+1) % NM);
            fSphere[i+j*NM][3] = i + (j+1)*NM;
        }
    }

    for(int j = 0; j < NP-1; j++){
        for(int i = 0; i < NM/2; i++){
          glBegin(GL_POLYGON);
          glTexCoord2f(0.0,1.0);
          glVertex3f(pSphere[fSphere[i+j*NM][0]].x, pSphere[fSphere[i+j*NM][0]].y, pSphere[fSphere[i+j*NM][0]].z);
          glTexCoord2f(1.0,1.0);
          glVertex3f(pSphere[fSphere[i+j*NM][1]].x, pSphere[fSphere[i+j*NM][1]].y, pSphere[fSphere[i+j*NM][1]].z);
          glTexCoord2f(1.0,0.0);
          glVertex3f(pSphere[fSphere[i+j*NM][2]].x, pSphere[fSphere[i+j*NM][2]].y, pSphere[fSphere[i+j*NM][2]].z);
          glTexCoord2f(0.0,0.0);
          glVertex3f(pSphere[fSphere[i+j*NM][3]].x, pSphere[fSphere[i+j*NM][3]].y, pSphere[fSphere[i+j*NM][3]].z);
          glEnd();
          }
    }
  }

//Génére un cylindre à partir de sa représentation paramétrique avec la texture enroulé sur toute la primitive
void cylindre(double r, int n, double h){
    Point pCylindre[n*2];
    int fCylindre[n][4];
    for(int i = 0; i < n; i++){

        pCylindre[i].x = r*cos((2*i*M_PI)/(float)n);
        pCylindre[i].y = -h/(double)2;
        pCylindre[i].z = r*sin((2*i*M_PI)/(float)n);

        pCylindre[i+n].x = r*cos((2*i*M_PI)/(float)n);
        pCylindre[i+n].y = h/(double)2;
        pCylindre[i+n].z = r*sin((2*i*M_PI)/(float)n);

        pCylindre[i].r = (float)rand() / (float)RAND_MAX;
        pCylindre[i].g = (float)rand() / (float)RAND_MAX;
        pCylindre[i].b = (float)rand() / (float)RAND_MAX;

        pCylindre[i+n].r = (float)rand() / (float)RAND_MAX;
        pCylindre[i+n].g = (float)rand() / (float)RAND_MAX;
        pCylindre[i+n].b = (float)rand() / (float)RAND_MAX;

        fCylindre[i][0] = i;

        fCylindre[i][1] = (i+1) % n;

        fCylindre[i][2] = n + ((i+1) % n);

        fCylindre[i][3] = i+n;

    }

    for (int i=0;i<n;i++)
    {
      glBegin(GL_POLYGON);
      float tempI = (float)i;
      float tempN = (float)n;
      glTexCoord2f(tempI/tempN,1.0);
      glVertex3f(pCylindre[fCylindre[i][0]].x, pCylindre[fCylindre[i][0]].y, pCylindre[fCylindre[i][0]].z);
      glTexCoord2f((tempI+1)/tempN,1.0);
      glVertex3f(pCylindre[fCylindre[i][1]].x, pCylindre[fCylindre[i][1]].y, pCylindre[fCylindre[i][1]].z);
      glTexCoord2f((tempI+1)/tempN,0.0);
      glVertex3f(pCylindre[fCylindre[i][2]].x, pCylindre[fCylindre[i][2]].y, pCylindre[fCylindre[i][2]].z);
      glTexCoord2f(tempI/tempN,0.0);
      glVertex3f(pCylindre[fCylindre[i][3]].x, pCylindre[fCylindre[i][3]].y, pCylindre[fCylindre[i][3]].z);

      glEnd();
    }


  }


void loadJpegImage(char *fichier, unsigned char texture[largimg][hautimg][3])
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  if ((cinfo.image_width!=largimg)||(cinfo.image_height!=hautimg)) {
    fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
    exit(1);
  }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*largimg*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);


  for(int i =0; i<hautimg; i++)
  {
    for(int j=0; j<largimg; j++)
    {
        texture[i][j][0] = image[i*largimg*3+j*3];
        texture[i][j][1] = image[i*largimg*3+j*3+1];
        texture[i][j][2] = image[i*largimg*3+j*3+2];
    }
  }
}
