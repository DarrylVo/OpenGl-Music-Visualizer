/*
 * ParticleSystem.hpp
 *
 *  Created on: Mar 10, 2016
 *      Author: darryl
 */

#ifndef SRC_PARTICLESYSTEM_HPP_
#define SRC_PARTICLESYSTEM_HPP_

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
#include <iostream>     // std::cout
#include <algorithm>    // std::sort

#define MAXPARTICLES FFT_WINDOW_SIZE/12

#define MAXRANDOM 6000

using namespace std;
using namespace Eigen;

struct Particle{
	Vector4f pos, speed;
	float r,g,b,a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f
	float fag;

};

class ParticleSystem {


public:
	void init();
	void draw();
	void setEye(Vector3f eye);
	GLuint getText();
	void createParticle(float x, float y, float z, float s,float r, float g, float b);
	void sortParticles();
	void explode(float x, float y, float z);
	void fire();
	void set_explode();

	int findUnusedParticle();
	int findUnusedRandomParticle();
private:

	GLuint Texture;

	Particle ParticlesContainer[MAXPARTICLES];
	Particle randomContainer[MAXRANDOM];
	Vector3f eye;
	GLfloat g_vertex_buffer_data[12];

	GLfloat* g_particule_position_size_data = new GLfloat[MAXPARTICLES* 4];
	GLfloat* g_particule_color_data         = new GLfloat[MAXPARTICLES * 4];

	GLfloat* r_pos_data = new GLfloat[MAXRANDOM * 4];
	GLfloat* r_color_data = new GLfloat[MAXRANDOM * 4];

	int lastUsedParticle = 0;
	int lastUsedRandomParticle = 0 ;
	float delta = 0.04;

	int flag_fire = 0;
	int flag_explode = 0;
	int newparticles = MAXRANDOM;

	GLuint vao;
	GLuint billboard_vertex_buffer ;
	GLuint particles_position_buffer ;
	GLuint particles_color_buffer ;



};





#endif /* SRC_PARTICLESYSTEM_HPP_ */
