/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"
#include "Simple OpenGL Image Library/src/SOIL.h"

using namespace std ; 
#include "variables.h"
#include "readfile.h"
#include <vector>
#include <map>



GLuint load_texture(const char * filename) {
  GLuint tex2d = SOIL_load_OGL_texture(
    filename,
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_POWER_OF_TWO| SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y);
  if (tex2d == 0) {
	cout << SOIL_last_result() << "\n";
	exit(1);
  }
  return tex2d;
}

void load_obj(const char * filename, vector<glm::vec3> &face_vertices, vector<glm::vec3> &face_normals) {
  string str, cmd;
  ifstream in;
  in.open(filename);
  if (!in.is_open()) {
    cerr << "Unable to open OBJ file " << filename << "\n";
    throw 2;
  }
  vector<glm::vec3> vertices, normals;
  getline(in, str);
  while (in) {
    if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
      stringstream ss(str);
      ss >> cmd;
      // vertex
      if (cmd.compare("v") == 0) {
	float x, y, z;
	ss >> x >> y >> z;
	glm::vec3 vertex = glm::vec3(x, y, z);
	vertices.push_back(vertex);
      // vertex normal
      } else if (cmd.compare("vn") == 0) {
	float x, y, z;
	ss >> x >> y >> z;
	glm::vec3 normal = glm::vec3(x, y, z);
	normals.push_back(normal);

      // face
      } else if (cmd.compare("f") == 0) {
  int loop_count = 3;
	string index, temp[5];
	ss >> temp[0] >> temp[1] >> temp[2] >> temp[3] >> temp[4];
  if (temp[4] != "") {
    loop_count = 5;
  } else if (temp[3] != "") {
    loop_count = 4;
  }
	int i;
  vector<glm::vec3> temp_face;
  vector<glm::vec3> temp_norm;
	for (i = 0; i < loop_count; i++) {
	  stringstream temp_ss(temp[i]);
	  getline(temp_ss, index, '/');
	  glm::vec3 vertex = vertices[atoi(index.c_str()) - 1];
	  temp_face.push_back(vertex);
	  getline(temp_ss, index, '/');
	  // skip texture
	  getline(temp_ss, index, '/');
	  glm::vec3 normal = normals[atoi(index.c_str()) - 1];
	  temp_norm.push_back(normal);
	}
	if (temp_face.size() == 4) {
    face_vertices.push_back(temp_face[0]);
    face_vertices.push_back(temp_face[1]);
    face_vertices.push_back(temp_face[2]);
    face_normals.push_back(temp_norm[0]);
    face_normals.push_back(temp_norm[1]);
    face_normals.push_back(temp_norm[2]);
    
    face_vertices.push_back(temp_face[0]);
    face_vertices.push_back(temp_face[2]);
    face_vertices.push_back(temp_face[3]);
    face_normals.push_back(temp_norm[0]);
    face_normals.push_back(temp_norm[2]);
    face_normals.push_back(temp_norm[3]);
	}
	if (temp_face.size() == 5) {
    face_vertices.push_back(temp_face[0]);
    face_vertices.push_back(temp_face[1]);
    face_vertices.push_back(temp_face[2]);
    face_normals.push_back(temp_norm[0]);
    face_normals.push_back(temp_norm[1]);
    face_normals.push_back(temp_norm[2]);
    
    face_vertices.push_back(temp_face[0]);
    face_vertices.push_back(temp_face[2]);
    face_vertices.push_back(temp_face[3]);
    face_normals.push_back(temp_norm[0]);
    face_normals.push_back(temp_norm[2]);
    face_normals.push_back(temp_norm[3]);
    
    face_vertices.push_back(temp_face[0]);
    face_vertices.push_back(temp_face[3]);
    face_vertices.push_back(temp_face[4]);
    face_normals.push_back(temp_norm[0]);
    face_normals.push_back(temp_norm[3]);
    face_normals.push_back(temp_norm[4]);
	}
      }  else {
      }
    }
    getline(in, str);
  }
}

void draw_obj(vector<glm::vec3> &vertices, vector<glm::vec3> &normals) {
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
  glNormalPointer(GL_FLOAT, 0, &normals[0]);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

void draw_obj_with_texture(vector<glm::vec3> &vertices,
  vector<glm::vec3> &normals, vector<glm::vec2> &textures, GLuint texture) {
  glUniform1i(istex, true);

  glTexCoordPointer(2, GL_FLOAT, 0, &textures[0]); 
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLint texsampler;
  texsampler = glGetUniformLocation(shaderprogram, "tex");
  glUniform1i(texsampler, 0);
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, &textures[0]);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
  
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, &normals[0]);
  
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);

  glUniform1i(istex, false);
}

vector<glm::vec3> crystal_vertices, crystal_normals;
void draw_crystal() {
  if (crystal_vertices.size() == 0) {
    load_obj("crystal.obj", crystal_vertices, crystal_normals);
  }
  draw_obj(crystal_vertices, crystal_normals);
}

vector<glm::vec3> window_vertices, window_normals;
void draw_window() {
  if (window_vertices.size() == 0) {
    load_obj("window.obj", window_vertices, window_normals);
  }
  draw_obj(window_vertices, window_normals);
}

vector<glm::vec3> glass_vertices_left, glass_vertices_right, glass_normals_right, glass_normals_left;
vector<glm::vec2> glass_textures_left, glass_textures_right;
void draw_glass(GLuint texture, GLfloat direction) {

  if (glass_normals_left.size() == 0 || glass_normals_right.size() == 0) {
    glm::vec3 v1, v2, v3, v4, v5;
    glm::vec2 t1, t2, t3, t4, t5;
    v1 = glm::vec3(-1.75, 1.9, 0);
    v2 = glm::vec3(0, 4, 0);
    v3 = glm::vec3(1.75, 1.9, 0);
    v4 = glm::vec3(1.75, -3.9, 0);
    v5 = glm::vec3(-1.75, -3.9, 0);
    
    t1 = glm::vec2(0, 0.9);
    t2 = glm::vec2(0.5, 1);
    t3 = glm::vec2(1, 0.9);
    t4 = glm::vec2(1, 0);
    t5 = glm::vec2(0, 0);
    glm::vec3 normal = vec3(0, 0, 1);
    if (direction == -1) {
      for (int i = 0; i < 18; i++) {
        glass_vertices_left.push_back(v3);
        glass_vertices_left.push_back(v2);
        glass_vertices_left.push_back(v1);
        glass_textures_left.push_back(t3);
        glass_textures_left.push_back(t2);
        glass_textures_left.push_back(t1);

        glass_vertices_left.push_back(v3);
        glass_vertices_left.push_back(v1);
        glass_vertices_left.push_back(v5);
        glass_textures_left.push_back(t3);
        glass_textures_left.push_back(t1);
        glass_textures_left.push_back(t5);

        glass_vertices_left.push_back(v4);
        glass_vertices_left.push_back(v3);
        glass_vertices_left.push_back(v5);
        glass_textures_left.push_back(t4);
        glass_textures_left.push_back(t3);
        glass_textures_left.push_back(t5);
	      glass_normals_left.push_back(normal);
      }
    } else {
      normal *= -1;
      glass_vertices_right.push_back(v1);
      glass_vertices_right.push_back(v2);
      glass_vertices_right.push_back(v3);
      glass_textures_right.push_back(t1);
      glass_textures_right.push_back(t2);
      glass_textures_right.push_back(t3);

      glass_vertices_right.push_back(v5);
      glass_vertices_right.push_back(v1);
      glass_vertices_right.push_back(v3);
      glass_textures_right.push_back(t5);
      glass_textures_right.push_back(t1);
      glass_textures_right.push_back(t3);

      glass_vertices_right.push_back(v5);
      glass_vertices_right.push_back(v3);
      glass_vertices_right.push_back(v4);
      glass_textures_right.push_back(t5);
      glass_textures_right.push_back(t3);
      glass_textures_right.push_back(t4);
      for (int i = 0; i < 18; i++) {
	glass_normals_right.push_back(normal);
      }
    }
  }

  if (direction == -1) {
    draw_obj_with_texture(glass_vertices_left, glass_normals_left, glass_textures_left, texture);
  } else {
    draw_obj_with_texture(glass_vertices_right, glass_normals_right, glass_textures_right, texture);
  }
}

void draw_claptrap() {
  vector<glm::vec3> vertices, normals;
  load_obj("claptrap/Clap Trap.obj", vertices, normals);
  draw_obj(vertices, normals);
}

void draw_sword() {
  vector<glm::vec3> vertices, normals;
  load_obj("sword.obj", vertices, normals);
  draw_obj(vertices, normals);
}

vector<glm::vec3> arch_vertices, arch_normals;
void draw_arch() {
  if (arch_vertices.size() == 0) {
    load_obj("arch.obj", arch_vertices, arch_normals);
  }
  draw_obj(arch_vertices, arch_normals);
}

vector<glm::vec3> bench_vertices, bench_normals;
void draw_bench() {
  if (bench_vertices.size() == 0) {
    load_obj("bench.obj", bench_vertices, bench_normals);
  }
  draw_obj(bench_vertices, bench_normals);
}

vector<glm::vec3> door_vertices, door_normals;
vector<glm::vec2> door_textures;
void draw_door(double width, double length, GLuint texture) {
  door_vertices.push_back(glm::vec3(width/2, -length/2, 0));
  door_vertices.push_back(glm::vec3(-width/2, length/2, 0));
  door_vertices.push_back(glm::vec3(width/2, length/2, 0));
  
  door_vertices.push_back(glm::vec3(-width/2, length/2, 0));
  door_vertices.push_back(glm::vec3(width/2, -length/2, 0));
  door_vertices.push_back(glm::vec3(-width/2, -length/2, 0));
  
  door_normals.push_back(glm::vec3(0, 0, -1));
  door_normals.push_back(glm::vec3(0, 0, -1));
  door_normals.push_back(glm::vec3(0, 0, -1));
  door_normals.push_back(glm::vec3(0, 0, -1));
  door_normals.push_back(glm::vec3(0, 0, -1));
  door_normals.push_back(glm::vec3(0, 0, -1));
  
  door_textures.push_back(glm::vec2(1, 0));
  door_textures.push_back(glm::vec2(0, 1));
  door_textures.push_back(glm::vec2(1, 1));
  
  door_textures.push_back(glm::vec2(0, 1));
  door_textures.push_back(glm::vec2(1, 0));
  door_textures.push_back(glm::vec2(0, 0));
  draw_obj_with_texture(door_vertices, door_normals, door_textures, texture);
}

void init_cube(double width, double length, double height, double y_start, bool inverse_norm, vector<glm::vec3> & vertices, vector<glm::vec3> & normals) {
  //left  
  if (inverse_norm) {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    normals.push_back(glm::vec3(1,0,0));
    normals.push_back(glm::vec3(1,0,0));
    normals.push_back(glm::vec3(1,0,0));
    normals.push_back(glm::vec3(1,0,0));
    normals.push_back(glm::vec3(1,0,0));
    normals.push_back(glm::vec3(1,0,0));
  } else {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));
    normals.push_back(-glm::vec3(1,0,0));
    normals.push_back(-glm::vec3(1,0,0));
    normals.push_back(-glm::vec3(1,0,0));
    normals.push_back(-glm::vec3(1,0,0));
    normals.push_back(-glm::vec3(1,0,0));
    normals.push_back(-glm::vec3(1,0,0));
  }
  
  //right
  if (inverse_norm) {
    vertices.push_back(glm::vec3(width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));

    vertices.push_back(glm::vec3(width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    normals.push_back(glm::vec3(-1,0,0));
    normals.push_back(glm::vec3(-1,0,0));
    normals.push_back(glm::vec3(-1,0,0));
    normals.push_back(glm::vec3(-1,0,0));
    normals.push_back(glm::vec3(-1,0,0));
    normals.push_back(glm::vec3(-1,0,0));
  } else {
    vertices.push_back(glm::vec3(width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));

    vertices.push_back(glm::vec3(width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    normals.push_back(-glm::vec3(-1,0,0));
    normals.push_back(-glm::vec3(-1,0,0));
    normals.push_back(-glm::vec3(-1,0,0));
    normals.push_back(-glm::vec3(-1,0,0));
    normals.push_back(-glm::vec3(-1,0,0));
    normals.push_back(-glm::vec3(-1,0,0));
  }
  
  //top
  if (inverse_norm) {
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));

    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
  } else {
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));

    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2)); 
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2)); 
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    normals.push_back(-glm::vec3(0,-1,0));
    normals.push_back(-glm::vec3(0,-1,0));
    normals.push_back(-glm::vec3(0,-1,0));
    normals.push_back(-glm::vec3(0,-1,0));
    normals.push_back(-glm::vec3(0,-1,0));
    normals.push_back(-glm::vec3(0,-1,0));
  }
  
  //bottom
  if (inverse_norm) {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));

    vertices.push_back(glm::vec3(width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));  
    vertices.push_back(glm::vec3(width/2,y_start,length/2));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
  } else {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));

    vertices.push_back(glm::vec3(width/2,y_start,-length/2)); 
    vertices.push_back(glm::vec3(width/2,y_start,length/2)); 
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));  
    normals.push_back(-glm::vec3(0,1,0));
    normals.push_back(-glm::vec3(0,1,0));
    normals.push_back(-glm::vec3(0,1,0));
    normals.push_back(-glm::vec3(0,1,0));
    normals.push_back(-glm::vec3(0,1,0));
    normals.push_back(-glm::vec3(0,1,0));
  }
  
  //near
  if (inverse_norm) {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(width/2,y_start,length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,length/2));  
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    normals.push_back(glm::vec3(0,0,-1));
    normals.push_back(glm::vec3(0,0,-1));
    normals.push_back(glm::vec3(0,0,-1));
    normals.push_back(glm::vec3(0,0,-1));
    normals.push_back(glm::vec3(0,0,-1));
    normals.push_back(glm::vec3(0,0,-1));
  } else {
    vertices.push_back(glm::vec3(-width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start,length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,length/2));  
    normals.push_back(-glm::vec3(0,0,-1));
    normals.push_back(-glm::vec3(0,0,-1));
    normals.push_back(-glm::vec3(0,0,-1));
    normals.push_back(-glm::vec3(0,0,-1));
    normals.push_back(-glm::vec3(0,0,-1));
    normals.push_back(-glm::vec3(0,0,-1));
  }
  
  //far
  if (inverse_norm) {
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));  
    normals.push_back(glm::vec3(0,0,1));
    normals.push_back(glm::vec3(0,0,1));
    normals.push_back(glm::vec3(0,0,1));
    normals.push_back(glm::vec3(0,0,1));
    normals.push_back(glm::vec3(0,0,1));
    normals.push_back(glm::vec3(0,0,1));
  } else {
    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    vertices.push_back(glm::vec3(width/2,y_start,-length/2));

    vertices.push_back(glm::vec3(-width/2,y_start,-length/2));  
    vertices.push_back(glm::vec3(-width/2,y_start+height,-length/2));  
    vertices.push_back(glm::vec3(width/2,y_start+height,-length/2));
    normals.push_back(-glm::vec3(0,0,1));
    normals.push_back(-glm::vec3(0,0,1));
    normals.push_back(-glm::vec3(0,0,1));
    normals.push_back(-glm::vec3(0,0,1));
    normals.push_back(-glm::vec3(0,0,1));
    normals.push_back(-glm::vec3(0,0,1));
  }
}

void draw_cube(double width, double length, double height, double y_start, bool inverse_norm, GLuint texture_file) {
  vector<glm::vec3> vertices, normals;
  vector<glm::vec2> textures;
  init_cube(width, length, height, y_start, inverse_norm, vertices, normals);
  if (texture_file != -1) {
    //left
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));
    //right
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));
    //top
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 0));
    //bottom
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(8, 0));
    //near
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(8, 8));
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));
    //far
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 0));
    textures.push_back(glm::vec2(0, 8));
    textures.push_back(glm::vec2(8, 8));

    draw_obj_with_texture(vertices, normals, textures, texture_file);
  }
  draw_obj(vertices, normals);
}

vector<glm::vec3> barrel_vertices, barrel_normals;
void draw_barrel_vault(double outer_radius, double inner_radius, double depth) {
  if (barrel_vertices.size() == 0) {
    glm::vec3 z_axis(0, 0, 1);
    glm::vec3 inner_near(inner_radius, 0, -depth/2);
    glm::vec3 outer_near(outer_radius, 0, -depth/2);
    glm::vec3 inner_far(inner_radius, 0, depth/2);
    glm::vec3 outer_far(outer_radius, 0, depth/2);
    glm::vec3 center_near(0, 0, -depth/2);
    glm::vec3 center_far(0, 0, depth/2);
    glm::vec3 norm1, norm2;

    for (int i = 0; i < 180; i += 10) {
      glm::vec3 v1 = inner_near * Transform::rotate(i, z_axis);
      glm::vec3 v2 = outer_near * Transform::rotate(i, z_axis);
      glm::vec3 v3 = inner_near * Transform::rotate(i+10, z_axis);
      glm::vec3 v4 = outer_near * Transform::rotate(i+10, z_axis);

      glm::vec3 v5 = inner_far * Transform::rotate(i, z_axis);
      glm::vec3 v6 = outer_far * Transform::rotate(i, z_axis);
      glm::vec3 v7 = inner_far * Transform::rotate(i+10, z_axis);
      glm::vec3 v8 = outer_far * Transform::rotate(i+10, z_axis);

      // near faces
      barrel_vertices.push_back(v4);
      barrel_vertices.push_back(v2);
      barrel_vertices.push_back(v1);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);

      barrel_vertices.push_back(v3);
      barrel_vertices.push_back(v4);
      barrel_vertices.push_back(v1);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);

      // far faces
      barrel_vertices.push_back(v5);
      barrel_vertices.push_back(v6);
      barrel_vertices.push_back(v8);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);

      barrel_vertices.push_back(v5);
      barrel_vertices.push_back(v8);
      barrel_vertices.push_back(v7);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);

      // inner face
      barrel_vertices.push_back(v1);
      barrel_vertices.push_back(v7);
      barrel_vertices.push_back(v3);
      norm1 = glm::vec3(-1, 0, 0) * Transform::rotate(i, z_axis);
      norm2 = glm::vec3(-1, 0, 0) * Transform::rotate(i+10, z_axis);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm2);
      barrel_normals.push_back(norm2);
      barrel_vertices.push_back(v1);
      barrel_vertices.push_back(v5);
      barrel_vertices.push_back(v7);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm2);

      // outer face
      barrel_vertices.push_back(v8);
      barrel_vertices.push_back(v4);
      barrel_vertices.push_back(v2);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm2);
      barrel_normals.push_back(norm2);
      barrel_vertices.push_back(v8);
      barrel_vertices.push_back(v6);
      barrel_vertices.push_back(v2);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm1);
      barrel_normals.push_back(norm2);

      // near cap
      barrel_vertices.push_back(center_near);
      barrel_vertices.push_back(v1);
      barrel_vertices.push_back(v3);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);
      barrel_normals.push_back(z_axis);
    
      // far cap
      barrel_vertices.push_back(v7);
      barrel_vertices.push_back(v5);
      barrel_vertices.push_back(center_far);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);
      barrel_normals.push_back(-z_axis);
    }
  }
  draw_obj(barrel_vertices, barrel_normals);
}
    
void init_cylinder(double top_radius, double bottom_radius, double height, double y_start, vector<glm::vec3> & vertices, vector<glm::vec3> & normals) {
  for (int i = 0; i < 360; i += 20) {
    // main cylinder
    glm::vec3 point1(-bottom_radius,y_start,0);
    glm::vec3 point2(-top_radius,y_start+height,0);
    glm::vec3 v1 = point1 * Transform::rotate(i, glm::vec3(0,1,0));
    glm::vec3 v2 = point2 * Transform::rotate(i, glm::vec3(0,1,0));
    glm::vec3 v3 = point1 * Transform::rotate(i+20, glm::vec3(0,1,0));
    glm::vec3 v4 = point2 * Transform::rotate(i+20, glm::vec3(0,1,0));
    
    glm::vec3 norm1 = glm::vec3(-1,0,0) * Transform::rotate(i, glm::vec3(0,1,0));
    glm::vec3 norm2 = glm::vec3(-1,0,0) * Transform::rotate(i+20, glm::vec3(0,1,0));
    
    // curve part
    vertices.push_back(v1); 
    vertices.push_back(v3);  
    vertices.push_back(v4);  
    
    vertices.push_back(v1);  
    vertices.push_back(v4);  
    vertices.push_back(v2);  
    
    normals.push_back(norm1);
    normals.push_back(norm2);
    normals.push_back(norm2);

    normals.push_back(norm1);
    normals.push_back(norm2);
    normals.push_back(norm1);   
    
    // top face
    vertices.push_back(v2);
    vertices.push_back(v4);
    vertices.push_back(glm::vec3(0,y_start+height,0));
    
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
    normals.push_back(glm::vec3(0,1,0));
        
    // bottom face
    vertices.push_back(v1);
    vertices.push_back(glm::vec3(0,y_start,0));
    vertices.push_back(v3);
    
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
    normals.push_back(glm::vec3(0,-1,0));
  }
}

void draw_cylinder(double top_radius, double bottom_radius, double height, double y_start) {
  vector <glm::vec3> vertices, normals;
  init_cylinder(top_radius, bottom_radius, height, y_start, vertices, normals);
  draw_obj(vertices, normals);
}

vector <glm::vec3> room_vertices, room_normals;
void draw_room(double width, double length, double height) {
  if (room_vertices.size() == 0) {
    glm::vec3 v1 = glm::vec3(width/2, -height/2, length/2);
    glm::vec3 v2 = glm::vec3(width/2, -height/2, -length/2);
    glm::vec3 v3 = glm::vec3(-width/2, -height/2, -length/2);
    glm::vec3 v4 = glm::vec3(-width/2, -height/2, length/2);
    glm::vec3 v5 = glm::vec3(width/2, height/2, length/2);
    glm::vec3 v6 = glm::vec3(width/2, height/2, -length/2);
    glm::vec3 v7 = glm::vec3(-width/2, height/2, -length/2);
    glm::vec3 v8 = glm::vec3(-width/2, height/2, length/2);

    // bottom face
    room_vertices.push_back(v3);
    room_vertices.push_back(v2);
    room_vertices.push_back(v1);
    room_vertices.push_back(v4);
    room_normals.push_back(glm::vec3(0, 1, 0));
    room_normals.push_back(glm::vec3(0, 1, 0));
    room_normals.push_back(glm::vec3(0, 1, 0));
    room_normals.push_back(glm::vec3(0, 1, 0));

    // right face
    room_vertices.push_back(v5);
    room_vertices.push_back(v6);
    room_vertices.push_back(v2);
    room_vertices.push_back(v1);
    room_normals.push_back(glm::vec3(-1, 0, 0));
    room_normals.push_back(glm::vec3(-1, 0, 0));
    room_normals.push_back(glm::vec3(-1, 0, 0));
    room_normals.push_back(glm::vec3(-1, 0, 0));

    // left face
    room_vertices.push_back(v8);
    room_vertices.push_back(v7);
    room_vertices.push_back(v3);
    room_vertices.push_back(v4);
    room_normals.push_back(glm::vec3(1, 0, 0));
    room_normals.push_back(glm::vec3(1, 0, 0));
    room_normals.push_back(glm::vec3(1, 0, 0));
    room_normals.push_back(glm::vec3(1, 0, 0));

    // near face
    room_vertices.push_back(v8);
    room_vertices.push_back(v5);
    room_vertices.push_back(v1);
    room_vertices.push_back(v4);
    room_normals.push_back(glm::vec3(0, 0, -1));
    room_normals.push_back(glm::vec3(0, 0, -1));
    room_normals.push_back(glm::vec3(0, 0, -1));
    room_normals.push_back(glm::vec3(0, 0, -1));

    // far face
    room_vertices.push_back(v2);
    room_vertices.push_back(v6);
    room_vertices.push_back(v7);
    room_vertices.push_back(v3);
    room_normals.push_back(glm::vec3(0, 0, 1));
    room_normals.push_back(glm::vec3(0, 0, 1));
    room_normals.push_back(glm::vec3(0, 0, 1));
    room_normals.push_back(glm::vec3(0, 0, 1));

    glm::vec3 vault1 = glm::vec3(-width/4 + 1.5, height/2, length/2);
    glm::vec3 vault2 = glm::vec3(width/4 - 1.5, height/2, length/2);
    glm::vec3 vault3 = glm::vec3(-width/4 + 1.5, height/2, -length/2);
    glm::vec3 vault4 = glm::vec3(width/4 - 1.5, height/2, -length/2);

    // left half of top face
    room_vertices.push_back(v8);
    room_vertices.push_back(vault1);
    room_vertices.push_back(vault3);
    room_vertices.push_back(v7);
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
 
    // right half of top face
    room_vertices.push_back(vault2);
    room_vertices.push_back(v5);
    room_vertices.push_back(v6);
    room_vertices.push_back(vault4);
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
    room_normals.push_back(glm::vec3(0, -1, 0));
  }
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &room_vertices[0]);
  glNormalPointer(GL_FLOAT, 0, &room_normals[0]);
  glDrawArrays(GL_QUADS, 0, room_vertices.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

vector<glm::vec3> pillar_vertices, pillar_normals;
void draw_pillar() {
  if (pillar_vertices.size() == 0) {
    float cyl_height = 5;
    float normalize_side = 1.4;
    float normalize_radius = 2.8;
    float normalize_height = cyl_height + 1;
    float normalized_offset = .5 - ((cyl_height/2+.2)/ normalize_height + .1 / normalize_height);
    init_cube(1.4/normalize_side, 1.4/normalize_side, .1 / normalize_height, (cyl_height/2+.2)/ normalize_height + normalized_offset, false, pillar_vertices, pillar_normals);
    init_cube(1.2/normalize_side, 1.2/normalize_side, .1/ normalize_height, (cyl_height/2+.1)/ normalize_height + normalized_offset, false, pillar_vertices, pillar_normals);
    init_cylinder(1.2/normalize_radius, 1.2/normalize_radius, .1/ normalize_height, (cyl_height/2)/ normalize_height + normalized_offset, pillar_vertices, pillar_normals);
    init_cylinder(1/normalize_radius, 1/normalize_radius,  cyl_height/ normalize_height, -(cyl_height/2)/ normalize_height + normalized_offset, pillar_vertices, pillar_normals);
    init_cylinder(1.2/normalize_radius, 1.2/normalize_radius, .1/ normalize_height, (-cyl_height/2 - .1)/ normalize_height + normalized_offset, pillar_vertices, pillar_normals);
    init_cylinder(1.2/normalize_radius, 1.4/normalize_radius, .2/ normalize_height, (-cyl_height/2 - .3)/ normalize_height + normalized_offset, pillar_vertices, pillar_normals);
    init_cube(1.4/normalize_side, 1.4/normalize_side, .4/ normalize_height, (-cyl_height/2 - .7)/ normalize_height + normalized_offset, false, pillar_vertices, pillar_normals);
  }
  draw_obj(pillar_vertices, pillar_normals);
}

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  

void transformvec (const GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) { 
      output[i] += modelview[4*j+i] * input[j] ;
    }
  }
}

void draw(object * obj) {
  if (obj -> type == pillar) {
    draw_pillar();
  } else if (obj -> type == room) {
    draw_room(obj->width, obj->length, obj->height);
  } else if (obj -> type == barrel_vault) {
    draw_barrel_vault(obj->outer_radius, obj->inner_radius, obj->depth);
  } else if (obj -> type == sword) {
    draw_sword();
  } else if (obj -> type == window) {
    draw_window();
  } else if (obj -> type == glass) {
    draw_glass(obj -> texture, obj -> size);
  } else if (obj -> type == bench) {
    draw_bench();
  } else if (obj -> type == arch) {
    draw_arch();
  } else if (obj -> type == crystal) {
    draw_crystal();
  } else if (obj -> type == cylinder) {
    draw_cylinder(obj->width/2, obj->length/2, obj->height, -obj->height/2);
  } else if (obj -> type == textured_cube) {
    draw_cube(1, 1, 1, 0, false, obj -> texture);
  } else if (obj -> type == door) {
    draw_door(obj -> width, obj -> length, wood);
  } else if (obj -> type == cube) {
    glutSolidCube(obj->size) ;
  } else if (obj -> type == sphere) {
    const int tessel = 20 ;
    glutSolidSphere(obj->size, tessel, tessel) ;
  } else if (obj -> type == teapot) {
    glutSolidTeapot(obj->size) ;
  }
}

void drawOcclusionMap() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, occlusionBuffer);
  glUseProgram(0); // don't use any shaders
  glViewport(0, 0, w/2, h/2);
  glClear(GL_COLOR_BUFFER_BIT);

  mat4 mv, pm, mvp;
  float aspect = w / (float) h, zNear = 0.1, zFar = 1000.0 ;
  if (useGlu) {
    pm = glm::perspective(fovy,aspect,zNear,zFar);
    mv = glm::lookAt(eye, center, up); 
  } else {
    pm = Transform::perspective(fovy,aspect,zNear,zFar) ; 
    pm = glm::transpose(mv) ; // accounting for row major
    mv = Transform::lookAt(eye,center,up) ;
    mv = glm::transpose(mv) ; // accounting for row major
  }
  mvp = pm * mv;
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(&pm[0][0]);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(&mv[0][0]);
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport); 
  float x, y, z, w;
  glm::vec4 pos;
  glm::vec2 screen_pos;
  for (int i = 0; i < numused; i++) {
    // calculate screen coordinates of light
    pos = glm::vec4(lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3]) * mvp;
    if (pos.w != 0) {
      pos.x /= pos.w; pos.y /= pos.w; pos.z /= pos.w; pos.w = 1;
    }
    GLfloat screenCoord[] = {viewport[0] + viewport[2] * (pos.x+1)/2,
			   viewport[1] + viewport[3] * (pos.y+1)/2};
    glUniform2fv(lightScreenCoord+i, 1, screenCoord);

    // draw light as a point
    GLfloat color[] = {lightcolor[4*i],
		       lightcolor[4*i+1],
		       lightcolor[4*i+2],
		       lightcolor[4*i+3]};
    GLfloat posn[] = {pos.x, pos.y, pos.z};
    glColor4fv(&color[0]);
    glBegin(GL_POINTS);
    glVertex3fv(&posn[0]);
    glEnd();
  }

  // draw occluding objects
  mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
  sc = Transform::scale(sx,sy,1.0) ; 
  glLoadMatrixf(&transf[0][0]) ; 
  transf = glm::transpose(mv) * transf;	
  transf = sc * transf;
  for (int i = 0 ; i < numobjects ; i++) {
    object * obj = &(objects[i]) ; 
    {
      mat4 transform = obj -> transform;
        if (obj -> type == sword) {
	  mat4 transf_new = Transform::translate(0, sword_move,0) * transf;
	  glLoadMatrixf(&glm::transpose(transform * transf_new)[0][0]);
	} else {
	  glLoadMatrixf(&glm::transpose(transform * transf)[0][0]);
        }
    }
    glColor3f(0, 0, 0);
    draw(obj);
  }
  glGenerateMipmap(GL_TEXTURE_2D);
}

void display() {
  /*
  drawOcclusionMap();

  glGenerateMipmapEXT(GL_TEXTURE_2D);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glUseProgram(shaderprogram);
  glClearColor(0, 0, 1, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  send occlusion map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, occlusionMap);
  glUniform1i(occlusionMapLocation, 0);
  */

  glUseProgram(shaderprogram);
  glClearColor(0, 0, 1, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // I'm including the basic matrix setup for model view to 
        // give some sense of how this works.  

	glMatrixMode(GL_MODELVIEW);
	mat4 mv ; 

        if (useGlu) mv = glm::lookAt(eye,center,up) ; 
	else {
          mv = Transform::lookAt(eye,center,up) ;
          mv = glm::transpose(mv) ; // accounting for row major
        }
        glLoadMatrixf(&mv[0][0]) ; 

        // Set Light and Material properties for the teapot
        // Lights are transformed by current modelview matrix. 
        // The shader can't do this globally. 
        // So we need to do so manually.  
        if (numused) {
          glUniform1i(enablelighting,true) ;

          // YOUR CODE FOR HW 2 HERE.  
          // You need to pass the lights to the shader. 
          // Remember that lights are transformed by modelview first.
	  for (int i = 0; i < numused; i++) {
	    const GLfloat _light[] = {lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3]};
	    GLfloat light[4];
	    transformvec(_light, light);
	    GLfloat color[] = {lightcolor[4*i], lightcolor[4*i+1], lightcolor[4*i+2], lightcolor[4*i+3]};
	    glUniform4fv(lightpos+i, 1, light);
	    glUniform4fv(lightcol+i, 1, color);
	  }
	  glUniform1i(numusedcol, numused);
        }
        else glUniform1i(enablelighting,false) ; 
     
        // Transformations for objects, involving translation and scaling 
        mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
        sc = Transform::scale(sx,sy,1.0) ; 

        // YOUR CODE FOR HW 2 HERE.  
        // You need to use scale, translate and modelview to 
        // set up the net transformation matrix for the objects.  
        // Account for GLM issues etc.  
        glLoadMatrixf(&transf[0][0]) ; 

	transf = glm::transpose(mv) * transf;	
	transf = sc * transf;
	for (int i = 0 ; i < numobjects ; i++) {
          object * obj = &(objects[i]) ; 

          {
          // YOUR CODE FOR HW 2 HERE. 
          // Set up the object transformations 
          // And pass in the appropriate material properties
	    mat4 transform = obj -> transform;
	    if (obj -> type == sword) {
        mat4 transf_new = Transform::translate(0, sword_move,0) * transf;
        glLoadMatrixf(&glm::transpose(transform * transf_new)[0][0]);
      } else if (obj ->type == crystal) {
        mat3 rotation = Transform::rotate(crystal_deg, up);
        mat4 crys_rotate(rotation[0][0],rotation[0][1],rotation[0][2],0,
                         rotation[1][0],rotation[1][1],rotation[1][2],0,
                         rotation[2][0],rotation[2][1],rotation[2][2],0,
                         0,0,0,1);
        mat4 transf_new =  sc * transform * Transform::translate(0, 0, 45) * crys_rotate * Transform::translate(0, 0, -85) * glm::transpose(mv);
        glLoadMatrixf(&glm::transpose(transf_new)[0][0]);
      } else {
	      glLoadMatrixf(&glm::transpose(transform * transf)[0][0]);
      }
	    glUniform4fv(ambientcol, 1, obj -> ambient);
            glUniform4fv(diffusecol, 1, obj -> diffuse);
            glUniform4fv(specularcol, 1, obj -> specular);
            glUniform4fv(emissioncol, 1, obj -> emission);
            glUniform1f(shininesscol, obj -> shininess);
          }
  glColor3f(0, 0, 0); // make things default black to get a darker scene.    	
	if (wireframe) {
          glEnable(GL_LINE_SMOOTH);
          glLineWidth(1.0);
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          glUniform1i(enablelighting, false);
	  draw(obj);
	  glDisable(GL_LINE_SMOOTH);
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          glUniform1i(enablelighting, true);
          continue; 
	} else if (outline) {
	  // Draw the outline of objects
	  glEnable(GL_CULL_FACE);
	  glCullFace(GL_FRONT);
	  glEnable(GL_LINE_SMOOTH);
	  glLineWidth(4.0);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  glUniform1i(enablelighting, false);

          draw(obj);
	  // Undo wireframe rendering.
	  glDisable(GL_CULL_FACE);
	  glDisable(GL_LINE_SMOOTH);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  glUniform1i(enablelighting, true);
	}    
	draw(obj);
  }   
  glutSwapBuffers();
}
