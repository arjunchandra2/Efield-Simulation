/* 

term_project_driver.c - Driver code for the term project

This code prompts the user to enter parameters needed for the simulation of a test charge placed in the electric field generated by a discrete charge distribution of electrons. It will then call a function to evolve the test charge appropriately and communicate with the OpenGL 

Compile with)
cc term_project_driver.c -o term_project_driver -lm -lglut -lGLU -lGL -lXext -lX11

 */


#define WIDTH  700
#define HEIGHT 700 

/* #define FOVY  45.0 */
#define FOVY  55.0 
#define ZNEAR  1.0
#define ZFAR  150.0

#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

GLuint Distcharge = 1;
GLuint Testcharge = 2;


//PARAMETERS OF THE SIMULATION

//distribution charge locations (only on the x-axis)
double* d_charge_locations;
//n - the number of charges in the distribution 
int n;
//dt - the timestep used in the evolution
double dt;
//t_x. t_y - (x,y) coordinates of the test charge (also interpreted as a vector)
double t_x;
double t_y;


void draw_sphere(float color[4])
{
  /* This function renders the sphere with a triangulation consisting
     of two triangle fans for the polar caps and triangle strips for
     the rest of the sphere.

     This function is only invoked once, when a GL-list with the
     sphere elements is compiled.  The actual rendering is done
     with a glCallList function.  For this reason we are not using
     here any special time saving procedure, such as storing into
     arrays values of trigonometric functions which are used
     repeatedly.  We prefer to call these functions explicitly
     for all triangulations, beacuse their direct use makes the 
     geometry more transparent.
  */

  /* NTHETA and NPHI are the numbers of subdivisions of longitude
     and latitude in the triangulations.  EPS is used to introduce
     a small overlap in the triangulations, to avoid the occurence
     of occasional black pixels at the joining of the triangles.
  */

  /* #define NTHETA 12 
     #define NPHI 16 */
#define NTHETA 6 
#define NPHI 10
#define EPS 0.01

  int m,i,j;
  double theta,phi,phi0,dtheta,dphi,st0,ct0,st1,ct1,sp,cp;
  float xyz[3],nm[3];
  float radius;

  radius=0.5;
  dtheta=M_PI/NTHETA;
  dphi=M_PI/NPHI;

  /* Northern polar cap: */

  st0=sin(dtheta*(1+EPS));
  ct0=cos(dtheta*(1+EPS));

  glBegin(GL_TRIANGLE_FAN);

  nm[0]=0;
  nm[1]=0;
  nm[2]=1;  
  for(j=0;j<=2;j++) xyz[j]=radius*nm[j];

  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glNormal3fv(nm);
  glVertex3fv(xyz);

  phi0=0;
  phi=phi0;

  for(i=0;i<=NPHI;i++){
    
    sp=sin(phi);
    cp=cos(phi);
    nm[0]=st0*cp;
    nm[1]=st0*sp;
    nm[2]=ct0;
    for(j=0;j<=2;j++) xyz[j]=radius*nm[j];      

    glMaterialfv(GL_FRONT, GL_DIFFUSE, color); 
    glNormal3fv(nm);
    glVertex3fv(xyz);

    phi=phi+2*dphi;
  }
  
  glEnd();
  
  /* Strips around the sphere: */

  for(m=1;m<NTHETA-1;m++){  

    theta=m*dtheta;
    st0=sin(theta-EPS*dtheta);
    ct0=cos(theta-EPS*dtheta);
    st1=sin(theta+dtheta*(1+EPS));
    ct1=cos(theta+dtheta*(1+EPS));

    phi=phi0;

    glBegin(GL_TRIANGLE_STRIP);
    for(i=0;i<=NPHI;i++){
    
      sp=sin(phi);
      cp=cos(phi);
      nm[0]=st0*cp;
      nm[1]=st0*sp;
      nm[2]=ct0;
      for(j=0;j<=2;j++) xyz[j]=radius*nm[j];
      
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
      glNormal3fv(nm);
      glVertex3fv(xyz);

      phi=phi+dphi;
      
      sp=sin(phi);
      cp=cos(phi);
      nm[0]=st1*cp;
      nm[1]=st1*sp;
      nm[2]=ct1;
      for(j=0;j<=2;j++) xyz[j]=radius*nm[j];
      
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
      glNormal3fv(nm);
      glVertex3fv(xyz);

      phi=phi+dphi;
      
    }

  phi0=(m%2)*dphi;
  }

  glEnd();

  /* Southern polar cap: */

  st0=sin(dtheta*(1+EPS));
  ct0=-cos(dtheta*(1+EPS));

  phi=phi0; 

  glBegin(GL_TRIANGLE_FAN);

  nm[0]=0;
  nm[1]=0;
  nm[2]=-1;  
  for(j=0;j<=2;j++) xyz[j]=radius*nm[j];
  color[0]=0; color[1]=0; color[2]=1; color[3]=1;

  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glNormal3fv(nm);
  glVertex3fv(xyz);

  for(i=0;i<=NPHI;i++){
    
    sp=sin(phi);
    cp=cos(phi);
    nm[0]=st0*cp;
    nm[1]=st0*sp;
    nm[2]=ct0;
    for(j=0;j<=2;j++) xyz[j]=radius*nm[j];      

    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glNormal3fv(nm);
    glVertex3fv(xyz);

    phi=phi+2*dphi;
  }
  
  glEnd();
  
}

void myinit(void)
{
  float color[4];
  
  GLfloat mat_specular[] = {0.02,0.02,0.02,0.5}; 
  GLfloat mat_shininess[] = {5.0}; 
  GLfloat light_position[] = {7.0,10.0,40.0,0.0};
 

  glClearColor (0.0, 0.0, 0.0, 1.0);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); 

  
  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);     

  /* Compiling Sphere callLists for distribution charges and test charges */

  glNewList(Distcharge, GL_COMPILE);
  color[0]=0; color[1]=0; color[2]=1; color[3]=1;
  draw_sphere(color);
  glEndList();

  glNewList(Testcharge, GL_COMPILE);
  color[0]=1; color[1]=0; color[2]=0; color[3]=1;
  draw_sphere(color);
  glEndList();

  dt=0;
  
}


void display(void)
{
  int i;
  static float side=25;

  double x_coord;

  //variables for computing total E field contribution
  double rvec_x;
  double rvec_y;
  double rvec_cubed;
  double totalrvec_x = 0;
  double totalrvec_y = 0;
  double mag;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //NOT WORKING - WRITE 'X' 'Y' AXIS ON WINDOW

  glColor3f(1.0, 1.0, 1.0); // Set text color to white
    
  // Set the position for the text
  glRasterPos2f(5,- 5);

  char axis[] = {'x','y'};
 
  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, axis[0]);

  
  //Draw  all of the distribution charges 

  for(int i=0; i<n; i++){
    x_coord = d_charge_locations[i];

    glPushMatrix();
    glTranslated(x_coord,0.0,-10);
    glCallList(Distcharge);
  
    glPopMatrix();
    
  }


  //draw the test charge
  glPushMatrix();
  glTranslated(t_x, t_y,-10);
  glCallList(Testcharge);
  
  glPopMatrix();

  glDisable(GL_LIGHTING);

  glColor3f(1.0,1.0,1.0);
  glBegin(GL_LINE_STRIP);
  glVertex3f(-side/2,0.0,-15);
  glVertex3f(side/2,0.0,-15);
  glEnd();

  glColor3f(1.0,1.0,1.0);
  glBegin(GL_LINE_STRIP);
  glVertex3f(0,-side/2,-15);
  glVertex3f(0,side/2,-15);
  glEnd();

  glEnable(GL_LIGHTING);


  glutSwapBuffers();

  
  //advance t_x and t_y by summing all electric field contributions and taking a step dt
  //in the net direction
  
  for(int i=0; i<n; i++){
    x_coord = d_charge_locations[i];
    rvec_x = x_coord - t_x;
    rvec_y = 0 - t_y;
    
    rvec_cubed = pow(rvec_x*rvec_x + rvec_y * rvec_y, 1.5);

    rvec_x = rvec_x / rvec_cubed;
    rvec_y = rvec_y / rvec_cubed;

    totalrvec_x += rvec_x;
    totalrvec_y += rvec_y;
  }

  //When we get too close to the source distribution the E field blows up
  //and the simulation breaks down because we essentially try to take a step
  //of infinite size. Here we remedy this by using a special vector when the
  //step size is too large

  //the special vector is a unit vector scaled by 0.2 in the same direction as the
  //net E field. The scale can be adjusted.
  if(abs(totalrvec_x) > 1 || abs(totalrvec_y) > 1){
    mag = pow(totalrvec_x*totalrvec_x + totalrvec_y * totalrvec_y, 0.5);
    totalrvec_x = 0.2 * totalrvec_x/mag;
    totalrvec_y = 0.2 *totalrvec_y/mag;

  }

  t_x += totalrvec_x*dt;
  t_y += totalrvec_y*dt;

  //stopping condition - within 0.05 of x-axis
  if(fabs(t_y) < 0.05){
    dt = 0.0;
  }
  
  
}

void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (FOVY, (GLdouble)w/(GLdouble)h, ZNEAR, ZFAR); 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated (0.0,0.0,-8.5); 
}

void start(void)
{
  dt=0.05;  // adjust to adapt the speed of the particle motion to the processor 
}

void stop(void)
{
  dt=0.0;
}
void choice(int menuitem)
{
  switch(menuitem){
  case 1:
    start();
    break;
  case 2:
    stop();
    break;
  case 3:
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush(); 
    glutSwapBuffers();
    printf("\n");
    exit(0);
    break;
  default:
    exit(0);
  }
}




int main(int argc, char *argv[])
{

  //d_x - x coordinate of the distribution charge
  double d_x;

  //Gathering parameters from the user  
  printf("\n");
  printf("Enter the number of charges in the distribution: ");
  scanf("%d",&n);

  d_charge_locations = (double*)malloc(n*sizeof(double));
  
  printf("\nYou will now enter the charge locations. Please keep values in the range [-8.0,8.0]\n");

  //gather the distribution charge locations
  for(int i = 0; i<n; i+=1){ 
    printf("Enter the x coordinate of charge %d: ", i); 
    scanf("%lf", &d_x);
    d_charge_locations[i] = d_x;
  
  }

  printf("\n");
  //gather test charge location
  printf("Enter the x coordinate of the test charge: ");
  scanf("%lf", &t_x);
  printf("Enter the y coordinate of the test charge: ");
  scanf("%lf", &t_y);
  
 
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WIDTH,HEIGHT);
  glutInitWindowPosition(50,50);
  glutCreateWindow("2D PFGM Simulation");

  myinit();

  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0);

  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutCreateMenu(choice);
  glutAddMenuEntry("start", 1);
  glutAddMenuEntry("stop", 2);
  glutAddMenuEntry("exit", 3);
  glutAttachMenu(GLUT_LEFT_BUTTON);
  glutMainLoop();

  return 0; 

}
