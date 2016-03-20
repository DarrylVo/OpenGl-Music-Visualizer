/*
 * ParticleSystem.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: darryl
 */
#include "ParticleSystem.hpp"
#include "texture.hpp"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort


void ParticleSystem::fire() {
	if (flag_fire == 0)
		flag_fire = 1;
	else
		flag_fire = 0;

}

void ParticleSystem::init() {
	g_vertex_buffer_data[2] = g_vertex_buffer_data[5] = g_vertex_buffer_data[8] = g_vertex_buffer_data[11] = 0.0f;
	g_vertex_buffer_data[3] = g_vertex_buffer_data[7] = g_vertex_buffer_data[9] = g_vertex_buffer_data[10] = 0.5f;
	g_vertex_buffer_data[0] = g_vertex_buffer_data[1] = g_vertex_buffer_data[4] = g_vertex_buffer_data[6] = -0.5f;

	for(int i=0; i<MAXPARTICLES; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
		randomContainer[i].life = -1.0f;
		randomContainer[i].cameradistance = -1.0f;
/*
		ParticlesContainer[i].pos[0] =static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/50));
		ParticlesContainer[i].pos[1] =static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/50));
		ParticlesContainer[i].pos[2] =static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/50));
		ParticlesContainer[i].pos[3] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/50));
		ParticlesContainer[i].r = ((float)(rand()%100))/100;
		ParticlesContainer[i].g = ((float)(rand()%100))/100;
		ParticlesContainer[i].b = ((float)(rand()%100))/100;
		ParticlesContainer[i].a = ((float)(rand()%100))/100;

		g_particule_position_size_data[4*i+0] = ParticlesContainer[i].pos[0];
		g_particule_position_size_data[4*i+1] = ParticlesContainer[i].pos[1];
		g_particule_position_size_data[4*i+2] = ParticlesContainer[i].pos[2];
		g_particule_position_size_data[4*i+3] = ParticlesContainer[i].pos[3];

		g_particule_color_data[4*i+0] = ParticlesContainer[i].r;
		g_particule_color_data[4*i+1] = ParticlesContainer[i].g;
		g_particule_color_data[4*i+2] = ParticlesContainer[i].b;
		g_particule_color_data[4*i+3] = ParticlesContainer[i].a;*/
	}




	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

}

void ParticleSystem::setEye(Vector3f eye_p) {
	eye=eye_p;
}

//create particles
void ParticleSystem::createParticle(float x, float y, float z, float s,float r, float g, float b) {
	int i = findUnusedParticle();
	//cout << i <<endl;


	ParticlesContainer[i].life = 1.0f;
	ParticlesContainer[i].pos[0] = x;
	ParticlesContainer[i].pos[1] = y;
	ParticlesContainer[i].pos[2] = z;
	ParticlesContainer[i].pos[3] = s;

	ParticlesContainer[i].r = r;
	ParticlesContainer[i].g = g;
	ParticlesContainer[i].b = b;
	ParticlesContainer[i].a = ((float)(rand()%100))/100;
	ParticlesContainer[i].speed = Vector4f(0,((float)(rand()%100))/100,0,0);
}

int ParticleSystem::findUnusedRandomParticle() {
	for(int i=lastUsedRandomParticle; i<MAXPARTICLES; i++){

		if (randomContainer[i].life < 0){
			lastUsedRandomParticle = i;

			return i;
		}
	}

	for(int i=0; i<lastUsedRandomParticle; i++){
		if (randomContainer[i].life < 0){
			lastUsedRandomParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one


}

int ParticleSystem::findUnusedParticle() {
	for(int i=lastUsedParticle; i<MAXPARTICLES; i++){

		if (ParticlesContainer[i].life < 0){
			lastUsedParticle = i;

			return i;
		}
	}

	for(int i=0; i<lastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one


}



int compare (const void * a, const void * b) {
	Particle *first = (Particle *) a;
	Particle *second = (Particle*) b;

	return (int)( second->cameradistance> first->cameradistance );
}


void ParticleSystem::sortParticles(){
	qsort(ParticlesContainer,MAXPARTICLES, sizeof(Particle),compare);
	qsort(randomContainer,MAXRANDOM, sizeof(Particle),compare);
	/*
	cout << "after sorted" <<endl;
	for(int i = 0; i < 20; i ++) {
		cout << ParticlesContainer[i].cameradistance  << endl;
	}
	*/
 }



GLuint ParticleSystem::getText() {
	return Texture;
}
void ParticleSystem::set_explode() {
	flag_explode = 1;
}

void ParticleSystem::explode(float x, float y, float z) {

	Vector4f center = Vector4f(x,y,z,0);
	for(int i =0; i < 20 ; i ++) {
		//int index = findUnusedRandomParticle();
		int index = rand() %MAXRANDOM;
		float x = (float)(rand()%5)/10;
		float y = (float)(rand()%5)/10;
		float z = (float)(rand()%5)/10;

		randomContainer[index].pos = Vector4f(center[0]+x,center[1]+y,center[2]+z,-42);

		randomContainer[index].life = 0.3;
		randomContainer[index].speed = (randomContainer[index].pos - center)*20;
		randomContainer[index].r = 1;
		randomContainer[index].g = 0;
		randomContainer[index].b = 0;
		randomContainer[index].a = 1;
	}
	flag_explode = 0;

}

void ParticleSystem::draw() {

	int particleCount = 0;
	int randomParticleCount = 0;

	for(int i=0; i<newparticles; i++){
		int particleIndex = findUnusedRandomParticle();
		randomContainer[particleIndex].life = 50.0f; // This particle will live 5 seconds.

		randomContainer[particleIndex].pos = Vector4f((float)(rand()%50),0,0,0);






		float spread = 1.5f;
		Vector4f maindir = Vector4f(0.0f, 10.0f, 0.0f,0);
		// Very bad way to generate a random direction;
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		Vector4f randomdir = Vector4f(
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,
				(rand()%2000 - 1000.0f)/1000.0f,0
		);

		randomContainer[particleIndex].speed = maindir + randomdir*spread;


		// Very bad way to generate a random color
		randomContainer[particleIndex].r = ((float)(rand()%100))/100;
		randomContainer[particleIndex].g = ((float)(rand()%100))/100;
		randomContainer[particleIndex].b = ((float)(rand()%100))/100;
		randomContainer[particleIndex].a = ((float)(rand()%100))/100;

		randomContainer[particleIndex].size = -69;

	}
	/*
	if(flag_explode) {
		cout << "explode" <<endl;
		explode();
	}*/

	// simulate random
	for(int i=0; i<MAXRANDOM; i++){


			if(randomContainer[i].life > 0.0f){

				// Decrease life
				randomContainer[i].life -= delta;
				if (randomContainer[i].life > 0.0f){

					// Simulate simple physics : gravity only, no collisions REPLACE WITH MY OWN EIGEN STUFF
					//p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
					//p.pos += p.speed * (float)delta;
					//p.cameradistance = glm::length2( p.pos - CameraPosition );
					if(randomContainer[i].pos[3] == -42) {

					}
					else {
						randomContainer[i].speed += Vector4f(0.0f,-9.81f, 0.0f,0) *0.01f;
					}


					randomContainer[i].pos += randomContainer[i].speed*0.1;
					Vector3f pos = Vector3f(randomContainer[i].pos[0],randomContainer[i].pos[1],randomContainer[i].pos[2]);
					//float dist = (pos-eye).norm();
					randomContainer[i].cameradistance = (pos-eye).norm();

					// Fill the GPU buffer
					r_pos_data[4*i+0] = randomContainer[i].pos[0];
					r_pos_data[4*i+1] = randomContainer[i].pos[1];
					r_pos_data[4*i+2] = randomContainer[i].pos[2];
					r_pos_data[4*i+3] = randomContainer[i].size;

					r_color_data[4*i+0] =randomContainer[i].r;
					r_color_data[4*i+1] = randomContainer[i].g;
					r_color_data[4*i+2] = randomContainer[i].b;
					r_color_data[4*i+3] = randomContainer[i].a;
					randomParticleCount++;

				}else{
					// Particles that just died will be put at the end of the buffer in SortParticles();
					randomContainer[i].cameradistance = -1.0f;
					/*
					r_color_data[4*i+0] =0;
					r_color_data[4*i+1] = 0;
					r_color_data[4*i+2] = 0;
					r_color_data[4*i+3] = 0;
					r_pos_data[4*i+0] =0;
					r_pos_data[4*i+1] = 0;
					r_pos_data[4*i+2] = 0;*/
				}



			}
	}

	//simulate particles
	for(int i=0; i<MAXPARTICLES; i++){


		if(ParticlesContainer[i].life > 0.0f){

			// Decrease life
			ParticlesContainer[i].life -= delta;
			if (ParticlesContainer[i].life > 0.0f){

				// Simulate simple physics : gravity only, no collisions REPLACE WITH MY OWN EIGEN STUFF
				//p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
				//p.pos += p.speed * (float)delta;
				//p.cameradistance = glm::length2( p.pos - CameraPosition );

					ParticlesContainer[i].pos += ParticlesContainer[i].speed;
				Vector3f pos = Vector3f(ParticlesContainer[i].pos[0],ParticlesContainer[i].pos[1],ParticlesContainer[i].pos[2]);
				//float dist = (pos-eye).norm();
				ParticlesContainer[i].cameradistance = (pos-eye).norm();

				// Fill the GPU buffer
				g_particule_position_size_data[4*i+0] = ParticlesContainer[i].pos[0];
				g_particule_position_size_data[4*i+1] = ParticlesContainer[i].pos[1];
				g_particule_position_size_data[4*i+2] = ParticlesContainer[i].pos[2];

				g_particule_position_size_data[4*i+3] = ParticlesContainer[i].size;

				g_particule_color_data[4*i+0] =ParticlesContainer[i].r;
				g_particule_color_data[4*i+1] = ParticlesContainer[i].g;
				g_particule_color_data[4*i+2] = ParticlesContainer[i].b;
				g_particule_color_data[4*i+3] = ParticlesContainer[i].a;
				particleCount++;

			}else{
				// Particles that just died will be put at the end of the buffer in SortParticles();
				ParticlesContainer[i].cameradistance = -1.0f;
				g_particule_color_data[4*i+0] =0;
				g_particule_color_data[4*i+1] = 0;
				g_particule_color_data[4*i+2] = 0;
				g_particule_color_data[4*i+3] = 0;
				g_particule_position_size_data[4*i+0] =0;
								g_particule_position_size_data[4*i+1] = 0;
								g_particule_position_size_data[4*i+2] = 0;
			}



		}

	}

	sortParticles();

	//texture stuff

	if(flag_fire) {
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat)+MAXRANDOM * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount  * sizeof(GLfloat) * 4  , g_particule_position_size_data);

		glBufferSubData(GL_ARRAY_BUFFER, particleCount * sizeof(GLfloat) * 4, randomParticleCount  * sizeof(GLfloat) * 4  , r_pos_data);

		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES* 4 * sizeof(GLfloat) + MAXRANDOM* 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0,particleCount  * sizeof(GLfloat) * 4, g_particule_color_data);

		glBufferSubData(GL_ARRAY_BUFFER, particleCount  * sizeof(GLfloat) * 4,randomParticleCount  * sizeof(GLfloat) * 4, r_color_data);


	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
			glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount  * sizeof(GLfloat) * 4, g_particule_position_size_data);


			glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
			glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES* 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0,MAXPARTICLES  * sizeof(GLfloat) * 4, g_particule_color_data);
			randomParticleCount = 0;
	}





	glEnableVertexAttribArray(0);



	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);


	glVertexAttribPointer(
			0, // attribute. No particular reason for 0, but must match the layout in the shader.
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
	);



	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
			1, // attribute. No particular reason for 1, but must match the layout in the shader.
			4, // size : x + y + z + size => 4
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0, // stride
			(void*)0 // array buffer offset
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(
			2, // attribute. No particular reason for 1, but must match the layout in the shader.
			4, // size : r + g + b + a => 4
			GL_FLOAT, // type
			GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0, // stride
			(void*)0 // array buffer offset
	);



	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1


	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount + randomParticleCount);



	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	//DO I HAVE TO UNBIND MORE STUFFF???????



	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}



