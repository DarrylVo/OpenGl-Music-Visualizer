/* P3 materials example - see SetMaterial and fragment shader
CPE 471 Cal Poly Z. Wood + S. Sueda
 */
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include <string>
#include "Fft.hpp"
#include "ParticleSystem.hpp"
#include "texture.hpp"


using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
string SONG = "";
shared_ptr<Program> prog;
shared_ptr<Program> phong_prog;
shared_ptr<Program> particle_prog;
shared_ptr<ParticleSystem> particle_system;
shared_ptr<Shape> shape;
shared_ptr<Shape> light_shape;
shared_ptr<Shape> pyramid_shape;
shared_ptr<Shape> sphere_shape;
shared_ptr<Shape> sponge_shape;

int pausef = 0;

float spectrum[FFT_WINDOW_SIZE/2];

shared_ptr<Fft> fft;


int g_width, g_height;
int flag = 0;

int gMat =2;
double pos_x = 0;
double pos_y = 0;
float l_x=1,l_y=1,l_z=-2;
float c_x = 0.2, c_z = 0.2;
float rads = 0;

Vector3f u = Vector3f(1,0,0);
Vector3f w = Vector3f(0,0,1);
Vector3f v = Vector3f(0,1,0);
Vector3f eye = Vector3f(-5.753,21.378044,53.101517);
Vector3f lookAtP = Vector3f(-5.139922,21.018873,52.197746);
Vector3f up = Vector3f(0,1,0);

double alpha = 0.52;
double beta = 0.52;
int right_f = 0;
float b_r,b_g,b_b;
GLuint Texture;

int lastSphere= 0;
Vector4f sphere_pos[10];
int c = 0;
double lastTime, curTime;

static void createSphere();

void rgb_hsl(float r, float g, float b, float *H, float *S, float *L) {
	float var_Min = min( {r, g, b} );    //Min. value of RGB
	float var_Max = max( {r, g, b} );    //Max. value of RGB
	float del_Max = var_Max - var_Min;          //Delta RGB value

	*L = ( var_Max + var_Min ) / 2;

	if ( del_Max == 0 )                     //This is a gray, no chroma...
	{
	   *H = 0;                                //HSL results from 0 to 1
	   *S = 0;
	}
	else                                    //Chromatic data...
	{
	   if ( *L < 0.5 )
		   *S = del_Max / ( var_Max + var_Min );
	   else
		   *S = del_Max / ( 2 - var_Max - var_Min );

	   float del_R = ( ( ( var_Max - r ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
	   float del_G = ( ( ( var_Max - g ) / 6 ) + ( del_Max / 2 ) ) / del_Max;
	   float del_B = ( ( ( var_Max - b ) / 6 ) + ( del_Max / 2 ) ) / del_Max;

	   if ( r == var_Max )
		   *H = del_B - del_G;
	   else if ( g == var_Max )
		   *H = ( 1 / 3 ) + del_R - del_B;
	   else if ( b == var_Max )
		   *H = ( 2 / 3 ) + del_G - del_R;
	   if ( *H < 0 )
		   *H += 1;
	   if ( *H > 1 )
		   *H -= 1;
	}

}

float hue_rgb(float v1, float v2, float vH) {
	 if ( vH < 0 )
		 vH += 1;
	   if ( vH > 1 )
		   vH -= 1;
	   if ( ( 6 * vH ) < 1 )
		   return ( v1 + ( v2 - v1 ) * 6 * vH );
	   if ( ( 2 * vH ) < 1 )
		   return ( v2 );
	   if ( ( 3 * vH ) < 2 )
		   return ( v1 + ( v2 - v1 ) * ( ( 2 / 3 ) - vH ) * 6 );
	   return ( v1 );
}


void hsl_rgb(float *r, float *g, float *b, float H, float S, float L) {
	float var_2;
	if ( S == 0 )                       //HSL from 0 to 1
	{
	   *r= L;                  //RGB results from 0 to 255
	   *g=L;
	   *b = L;
	}
	else
	{
	   if ( L < 0.5 )
		   var_2 = L * ( 1 + S );
	   else
		   var_2 = ( L + S ) - ( S * L );

	   float var_1 = 2 * L - var_2;

	   *r = hue_rgb( var_1, var_2, H + ( 1 / 3 ) );
	   *g = hue_rgb( var_1, var_2, H );
	   *b = hue_rgb( var_1, var_2, H - ( 1 / 3 ) );
	}
}

void getNewColor(float r, float g, float b, float *r2, float *g2, float *b2, int degrees) {
	float H,S,L;
	rgb_hsl(r, g, b, &H, &S, &L);
	H+=degrees*3.14/180;
	if( H >= 1.85*3.14 )
		H -= 2*3.14;
	/*
	cout << "H:" << H << endl;
	cout << "S:" << S << endl;
	cout << "L:" << L << endl;*/
	hsl_rgb(r2, g2, b2, H, S, L);


}


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void updateCameraVector() {

	w = -(lookAtP - eye);


	w = w.normalized();


	u = up.cross(w).normalized();


	v = w.cross(u).normalized();



}


static void calcLAMouse() {
	double radius = 1;
	alpha = (pos_y-240)*3.14/180*180*0.003;
	beta = (pos_x-320)*3.14/180*360*0.008;

	if(alpha*180/3.14>= 80) {
		alpha = 80*3.14/180;
	}
	else if (alpha*180/3.14<=- 80) {
		alpha = -80*3.14/180;
	}
	//printf("alpha:%f beta:%f  pos_x:%f pos_Y:%f \n",alpha*180/3.14,beta*180/3.14,pos_x,pos_y);

	double lax = radius * cos(alpha) * cos(beta);
	double lay = radius * sin(alpha);
	double laz = radius * cos(alpha) *cos(90-beta);


	updateCameraVector();

	lookAtP = Vector3f(lax,lay,laz) +eye;



	//printV(lookAtP,"lookAtP MOUSE");
/*
	printV(u,"u vector");

	printV(w,"w vector");

	printV(v,"v vector");
*/

}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{





	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	} else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		gMat = (gMat+1);
	}
	if(key == 68) {
		updateCameraVector( );
		lookAtP+= u*c_x;
		eye += u*c_x;
	}
	if(key == 65) {
		updateCameraVector( );
		lookAtP -= u*c_x;
		eye -=u*c_x;

	}
	if(key == 87) {
		updateCameraVector( );
		eye -=w*c_z;
		lookAtP-=w*c_z;
	}

	if(key == 83) {
		updateCameraVector();
		eye +=w*c_z;
		lookAtP+=w*c_z;
	}
	if(key == 78 && action == GLFW_PRESS) {
		particle_system->fire();
		if(!c)
			c = 1;
		else
			c=0;
	}
	if(key == 66 && action == GLFW_PRESS) {
			//particle_system->set_explode();
		createSphere();

		}
	if(key == 80 && action == GLFW_PRESS) {
		if(pausef == 0) {
			fft->pauseSong();
			pausef = 1;
		}
		else {
			fft->playSong();
			pausef = 0;
		}



	}
	//printV(eye,"eye");
	//printV(lookAtP,"lookAtP");
}



static void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

	pos_x = xpos;
	pos_y = ypos;
	//calcLAMouse();
}

static void resize_callback(GLFWwindow *window, int width, int height) {
	g_width = width;
	g_height = height;
	glViewport(0, 0, width, height);
}

//helper function to set materials
void SetMaterial(int i) {

	switch (i) {
	case 0: //shiny blue plastic

		glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
		glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
		glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
		glUniform1f(prog->getUniform("MatShine"), 120);


		glUniform3f(phong_prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
		glUniform3f(phong_prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
		glUniform3f(phong_prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
		glUniform1f(phong_prog->getUniform("MatShine"), 120);

		glUniform3f(phong_prog->getUniform("colorC"), 0.14, 0.2, 0.8);




		//set specular to: (0.14, 0.2, 0.8);
		//set shine to: (120.0);
		break;
	case 1: // flat grey


		glUniform3f(phong_prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
		glUniform3f(phong_prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
		glUniform3f(phong_prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(phong_prog->getUniform("MatShine"), 40);
		glUniform3f(phong_prog->getUniform("colorC"), 0.1, 0.5, 0.2);
		//set specular to: (0.3, 0.3, 0.4);
		//set shine to: (4.0);
		break;
	case 2: //brass


		glUniform3f(phong_prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
		glUniform3f(phong_prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		glUniform3f(phong_prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		glUniform1f(phong_prog->getUniform("MatShine"), 27.9);
		glUniform3f(phong_prog->getUniform("colorC"), 0.3, 0.6, 0.2);
		//set specualr to: (0.9922, 0.941176, 0.80784);
		//set shine to: (27.9);
		break;
	case 3: //copper

		glUniform3f(phong_prog->getUniform("MatAmb"), 0.135f, 0.2225f, 0.1575f);
		glUniform3f(phong_prog->getUniform("MatDif"), 0.54f, 0.89f, 0.63f);
		glUniform3f(phong_prog->getUniform("MatSpec"), 0.316228f, 0.316228f, 0.316228f);
		glUniform1f(phong_prog->getUniform("MatShine"), 12.8);
		glUniform3f(phong_prog->getUniform("colorC"), 0.4, 0.7, 0.2);
		//set specualr to: (0.257, 0.1376, 0.08601);
		//set shine to: (12.8);
		break;

	case 4: //chrome

		glUniform3f(phong_prog->getUniform("MatAmb"), 0.25, 0.25, 0.25);
		glUniform3f(phong_prog->getUniform("MatDif"), 0.4, 0.4, 0.4);
		glUniform3f(phong_prog->getUniform("MatSpec"), 0.774597, 0.774597, 0.774597);
		glUniform1f(phong_prog->getUniform("MatShine"), 76.8);
		glUniform3f(phong_prog->getUniform("colorC"), 0.5, 0.3, 0.1);

		break;
	case 5:
		glUniform3f(phong_prog->getUniform("MatAmb"), 0, 0, 0);
				glUniform3f(phong_prog->getUniform("MatDif"), 0.64, 0.64, 0.64);
				glUniform3f(phong_prog->getUniform("MatSpec"), 0, 0, 0);
				glUniform1f(phong_prog->getUniform("MatShine"), 96.078341);
				glUniform3f(phong_prog->getUniform("colorC"), 0.4, 0.1, 0.6);
		break;


	}
}



static void init()
{
	GLSL::checkVersion();


	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	//init fmod
	fft = make_shared<Fft>();


	//particle system
		particle_system = make_shared<ParticleSystem>();
		particle_system->init();


	//particle vertex/shader program
	particle_prog = make_shared<Program>();
	particle_prog->setVerbose(true);
	particle_prog->setShaderNames(RESOURCE_DIR + "particle_vert.glsl", RESOURCE_DIR + "particle_frag.glsl");
	particle_prog->init();
	particle_prog->addAttribute("billboard");
	particle_prog->addAttribute("pos");
	particle_prog->addAttribute("color");
	particle_prog->addUniform("P");
	particle_prog->addUniform("V");
	particle_prog->addUniform("Up");
	particle_prog->addUniform("Right");
	particle_prog->addTexture("myTextureSampler");

	Texture = loadDDS("/home/darryl/workspace/Final Solution/resources/particle.DDS");







	//init T
	light_shape = make_shared<Shape>();
	light_shape->loadMesh(RESOURCE_DIR+ "cube.obj");
	light_shape->resize();
	light_shape->init();




	//init light_shape
	sphere_shape = make_shared<Shape>();
	sphere_shape->loadMesh(RESOURCE_DIR+ "sphere.obj");
	sphere_shape->resize();
	sphere_shape->init();


	phong_prog = make_shared<Program>();
	phong_prog->setVerbose(true);
	phong_prog->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	phong_prog->init();
	phong_prog->addUniform("P");
	phong_prog->addUniform("V");
	phong_prog->addUniform("M");
	phong_prog->addUniform("MatAmb");

	phong_prog->addUniform("MatDif");
	phong_prog->addUniform("MatSpec");
	phong_prog->addUniform("MatShine");
	phong_prog->addUniform("flag_nc");
	phong_prog->addUniform("lightPos");
	phong_prog->addUniform("colorC");
	phong_prog->addUniform("ti");
	phong_prog->addAttribute("vertPos");
	phong_prog->addAttribute("vertNor");
	phong_prog->addAttribute("vertTex");

}

static void createSphere() {
	int flag = 0;
	for(int i = 0; i < 10; i ++) {
		if(sphere_pos[i][3]<=0) {
			lastSphere = i;
			flag = 1;
		}
	}
	if(flag != 0) {
		sphere_pos[lastSphere] = Vector4f((float)(rand()%50),0,0,5);

	}
	//eXPLODE
	else {

	}



}

static void drawAllSpheres(shared_ptr<MatrixStack> M) {
	for(int i = 0; i < 10; i ++) {
		if(sphere_pos[i][3]> 0) {
			sphere_pos[i][3]-=0.7;
			sphere_pos[i][1]+=0.4;
			M->pushMatrix();
			M->translate(Vector3f(sphere_pos[i][0],sphere_pos[i][1],0));
			M->scale(2);
			glUniformMatrix4fv(phong_prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
			sphere_shape->draw(phong_prog);
			M->popMatrix();
		}
		else if((float)sphere_pos[i][0]>0)  {

			particle_system->explode((float)sphere_pos[i][0],(float)sphere_pos[i][1],0);
			sphere_pos[i][0]=0;
		}
	}
}


static void drawBar(shared_ptr<MatrixStack> M, float x, float y, float z, float mag) {
	M->pushMatrix();
	M->translate(Vector3f(x,y,z));
	M->scale(Vector3f(0.2,mag*200,0.2));
	glUniform1f(phong_prog->getUniform("flag_nc"), 0);
	glUniformMatrix4fv(phong_prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
	light_shape->draw(phong_prog);
	M->popMatrix();
}

static void drawAllBars(shared_ptr<MatrixStack> M, shared_ptr<MatrixStack> V) {
	if(fft->beat()) {
		b_r = ((float)(rand()%100))/100;
		b_g = ((float)(rand()%100))/100;
		b_b = ((float)(rand()%100))/100;
		glClearColor(b_r,b_g,b_b,1);
		gMat++;
		float c_r,c_g,c_b;
		getNewColor(b_r, b_g, b_b, &c_r, &c_g, &c_b, 180);
		glUniform3f(phong_prog->getUniform("colorC"), c_r, c_g, c_b);
		//SetMaterial(gMat%6);
	}


	M->pushMatrix();
	M->translate(Vector3f(0,-3,0));
	int i2 = 0;
	for(int i = 0; i < FFT_WINDOW_SIZE/3; i+=4) {
		drawBar(M,(i2)*0.3,0,0,spectrum[i]);
		/*	camera distance calculations
		Vector3f pos = Vector3f(i2*0.5,spectrum[i]*185,0);
		float dist = (pos-eye).norm();
		*/
		float size = 2.0;
		// only do this when yu want to shoot a particle out
		if(fft->beat()) {
			float p_r,p_g,p_b;
			getNewColor(b_r, b_g, b_b, &p_r, &p_g, &p_b, 90);
			particle_system->createParticle(i2*0.3,spectrum[i]*185,0,size,p_r,p_g,p_b);
		}


		i2++;


	}

	M->popMatrix();


}


static void render()
{
	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*
	printV(u,"u");

	printV(w,"w");

	printV(v,"v");*/
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
	float aspect = width/(float)height;

	// Create the matrix stacks - please leave these alone for now
	auto P = make_shared<MatrixStack>();
	auto M = make_shared<MatrixStack>();
	auto V = make_shared<MatrixStack>();
	// Apply perspective projection.
	P->pushMatrix();
	P->perspective(45.0f, aspect, 0.01f, 100.0f);

	// Draw a stack of cubes with indiviudal transforms
	phong_prog->bind();
	glUniform1i(phong_prog->getUniform("ti"),0);

	glUniform3f(phong_prog->getUniform("lightPos"), l_x,l_y,l_z);

	//glUniform3f(phong_prog->getUniform("lightCol"), 1.0f,1.0f,1.0f);
	glUniformMatrix4fv(phong_prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());



	V->pushMatrix();
	V->loadIdentity();
	calcLAMouse();
	V->lookAt(eye,lookAtP,up);
	glUniformMatrix4fv(phong_prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());

/*
	//plane
	M->pushMatrix();
	M->loadIdentity();
	M->translate(Vector3f(0, -3, 0));
	M->scale(Vector3f(23,0.5,20));
	glUniform1f(phong_prog->getUniform("flag_nc"), 0);
	glUniformMatrix4fv(phong_prog->getUniform("M"), 1, GL_FALSE, M->topMatrix().data());
	//light_shape->draw(phong_prog);

	M->popMatrix();
*/
	//test bar
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	drawAllBars(M,V);
	curTime = glfwGetTime();
	if(c==1&&curTime - lastTime > 0.6) {
		lastTime=curTime;
		createSphere();
	}

	drawAllSpheres(M);

	phong_prog->unbind();

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	particle_prog->bind();
	glUniformMatrix4fv(particle_prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(particle_prog->getUniform("V"), 1, GL_FALSE, V->topMatrix().data());

	glUniform3f(particle_prog->getUniform("Up"), v[0],v[1],v[2]);
	glUniform3f(particle_prog->getUniform("Right"), u[0],u[1],u[2]);

	//moe this to draw?????
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(particle_prog->getTextureID("myTextureSampler"), 0);


	particle_system->setEye(eye);
	particle_system->draw();


	particle_prog->unbind();

	V->popMatrix();
	P->popMatrix();

}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");
	SONG = RESOURCE_DIR + argv[2];
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	//request the highest possible version of OGL - important for mac
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
	glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	//set the mouse call back
	glfwSetCursorPosCallback(window, mouse_callback);
	//set the window resize call back
	glfwSetFramebufferSizeCallback(window, resize_callback);

	if(argc!=3) {
		cerr <<"wrong args" <<endl;
		exit(-1);
	}


	init();
	fft->init(SONG);
	fft->playSong();

	//SetMaterial(gMat%6);
	lastTime = glfwGetTime();
	while(!glfwWindowShouldClose(window)) {
		fft->analyze();
		fft->getData(spectrum);

		// Render scene.
		render();

		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
