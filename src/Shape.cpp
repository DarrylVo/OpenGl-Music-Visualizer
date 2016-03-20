#include "Shape.h"
#include <iostream>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "GLSL.h"
#include "Program.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

unsigned char * data;

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
   vaoID(0)
{
}

Shape::~Shape()
{
}

void Shape::calcNormal(){

	int idx1, idx2, idx3;


	assert((eleBuf.size() % 3) == 0);
	for (int f = 0; f < eleBuf.size()/3; f++) {
		idx1 = eleBuf[3*f+0];
		idx2 = eleBuf[3*f+1];
		idx3 = eleBuf[3*f+2];

		double v1x = posBuf[3*idx1+0];
		double v1y = posBuf[3*idx1+1];
		double v1z = posBuf[3*idx1+2];

		double v2x = posBuf[3*idx2+0];
		double v2y = posBuf[3*idx2+1];
		double v2z = posBuf[3*idx2+2];

		double v3x = posBuf[3*idx3+0];
		double v3y = posBuf[3*idx3+1];
		double v3z = posBuf[3*idx3+2];

		double u1 = v1x-v2x;
		double u2 = v1y-v2y;
		double u3 = v1z-v2z;

		double v1 = v2x-v3x;
		double v2 = v2y-v3y;
		double v3 = v2z-v3z;

		double x = u2 * v3 - v2 * u3;
		double y = v1 * u3 - u1 * v3;
		double z = u1 * v2 - v1 * u2;

		double mag = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
		x/=mag;
		y/=mag;
		z/=mag;


		norBuf[3*idx1+0]+=x;
		norBuf[3*idx1+1]+=y;
		norBuf[3*idx1+2]+=z;

		norBuf[3*idx2+0]+=x;
		norBuf[3*idx2+1]+=y;
		norBuf[3*idx2+2]+=z;

		norBuf[3*idx3+0]+=x;
		norBuf[3*idx3+1]+=y;
		norBuf[3*idx3+2]+=z;
	}
	/*
	for (int f = 0; f < eleBuf.size()/3; f++) {
		idx1 = eleBuf[3*f+0];
		idx2 = eleBuf[3*f+1];
		idx3 = eleBuf[3*f+2];

		//printf("section\n");
		double v1x = norBuf[3*idx1+0];
		double v1y = norBuf[3*idx1+1];
		double v1z = norBuf[3*idx1+2];

		double mag1 = sqrt(pow(v1x,2) + pow(v1y,2) + pow(v1z,2));

		norBuf[3*idx1+0]/=mag1;
		norBuf[3*idx1+1]/=mag1;
		norBuf[3*idx1+2]/=mag1;
		//printf("%f:%f:%f\n",v1x,v1y,v1z);

		double v2x = norBuf[3*idx2+0];
		double v2y = norBuf[3*idx2+1];
		double v2z = norBuf[3*idx2+2];

		double mag2 = sqrt(pow(v2x,2) + pow(v2y,2) + pow(v2z,2));
		norBuf[3*idx2+0]/=mag2;
		norBuf[3*idx2+1]/=mag2;
		norBuf[3*idx2+2]/=mag2;

		//printf("%f:%f:%f\n",v2x,v2y,v2z);

		double v3x = norBuf[3*idx3+0];
		double v3y = norBuf[3*idx3+1];
		double v3z = norBuf[3*idx3+2];

		double mag3 = sqrt(pow(v3x,2) + pow(v3y,2) + pow(v3z,2));
		norBuf[3*idx3+0]/=mag3;
		norBuf[3*idx3+1]/=mag3;
		norBuf[3*idx3+2]/=mag3;
	}*/

}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {

		// tryna figure out vector add
		//posBuf = shapes[0].mesh.positions;
		for (size_t v = 0; v < shapes[0].mesh.positions.size(); v++) {

			posBuf.push_back(shapes[0].mesh.positions[v]);

		}


		//renable to get normals back
		//norBuf = shapes[0].mesh.normals;

		//texBuf = shapes[0].mesh.texcoords;
		for (size_t v = 0; v < shapes[0].mesh.texcoords.size(); v++) {

			texBuf.push_back(shapes[0].mesh.texcoords[v]);

		}




		//eleBuf = shapes[0].mesh.indices;
		for (size_t v = 0; v < shapes[0].mesh.indices.size(); v++) {

			eleBuf.push_back(shapes[0].mesh.indices[v]);

		}
		//comment out below to switchback

		for (size_t v = 0; v < posBuf.size(); v++) {

			norBuf.push_back(0);

		}
		calcNormal();

	}
}

void Shape::loadBMP_custom(const char * imagepath) {
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	 unsigned int dataPos;     // Position in the file where the actual data begins
	 unsigned int width, height;

	 unsigned int imageSize;   // = width*height*3
	 // Actual RGB data

	FILE * file = fopen(imagepath,"rb");
	 if (!file){printf("Image could not be opened\n"); }
	 if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
	      printf("Not a correct BMP file\n");

	 }
	 if ( header[0]!='B' || header[1]!='M' ){
	      printf("Not a correct BMP file\n");

	  }
	 dataPos    = *(int*)&(header[0x0A]);
	  imageSize  = *(int*)&(header[0x22]);
	  width      = *(int*)&(header[0x12]);
	  height     = *(int*)&(header[0x16]);
	  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	   if (dataPos==0)      dataPos=54; // The BMP header is done that way
	   data = new unsigned char [imageSize];

	    // Read the actual data from the file into the buffer
	   fread(data,1,imageSize,file);

	    //Everything is in memory now, the file can be closed
	    fclose(file);
}



void Shape::resize() {
  float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
	for (size_t v = 0; v < posBuf.size() / 3; v++) {
		posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
		assert(posBuf[3*v+0] >= -1.0 - epsilon);
		assert(posBuf[3*v+0] <= 1.0 + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
		assert(posBuf[3*v+1] >= -1.0 - epsilon);
		assert(posBuf[3*v+1] <= 1.0 + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
		assert(posBuf[3*v+2] >= -1.0 - epsilon);
		assert(posBuf[3*v+2] <= 1.0 + epsilon);
	}
}

void Shape::init()
{
   // Initialize the vertex array object
   glGenVertexArrays(1, &vaoID);
   glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	if(norBuf.empty()) {
		norBufID = 0;
	} else {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the texture array to the GPU
	if(texBuf.empty()) {
		texBufID = 0;
	} else {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
	
	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//texture stuff
	glGenTextures(1,&textID);
	glBindTexture(GL_TEXTURE_2D,textID);
	/*GLuint data = SOIL_load_OGL_texture
			(
				"liquid.bmp",
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);*/
	loadBMP_custom("src/grass.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 256, 256, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

   glBindVertexArray(vaoID);
	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}

	if (texBufID != 0) {	
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if(h_tex != -1 && texBufID != 0) {
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}
	
	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
	
	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
