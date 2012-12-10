/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif 

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 
EXTERN vec3 center;
EXTERN double crystal_deg;
EXTERN double sword_move;

#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 centerinit(0.0,0.0,0.0) ; // Center look at point 
int w = 600, h = 400 ; // width and height 
float fovy = 90.0 ; // For field of view
float fovyinit = 90.0 ;
#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 centerinit ; 
EXTERN int w, h ; 
EXTERN float fovy ; 
EXTERN float fovyinit ;
#endif 

EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user 
EXTERN GLuint vertexshader, fragmentshader, godrayshader, shaderprogram, godrayshaderprogram; // shaders
static enum {view, scale} transop ; // which operation to transform 
enum shape {cube, sphere, teapot, pillar, room, cylinder, sword, arch, bench,
	barrel_vault, window, glass, textured_cube, crystal, door, triforce} ;
EXTERN float sx, sy ; // the scale in x and y
EXTERN GLuint textures[10];
EXTERN GLuint carpet;
EXTERN GLuint wood;
EXTERN GLuint wall_normal_map;
EXTERN GLuint wall_texture;
EXTERN GLuint texsampler;
EXTERN GLuint bumpsampler;
EXTERN GLuint shadowmapsampler;
EXTERN GLuint tangent_loc;
EXTERN GLuint bitangent_loc;

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN GLfloat lightposn [4*numLights] ; // Light Positions
EXTERN GLfloat lightcolor[4*numLights] ; // Light Colors
EXTERN GLfloat lightscreen[2*numLights] ;
EXTERN int numused ;                     // How many lights are used 
EXTERN int num_glass;			 // How many glass panes

// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN GLfloat ambient[4] ; 
EXTERN GLfloat diffuse[4] ; 
EXTERN GLfloat specular[4] ; 
EXTERN GLfloat emission[4] ; 
EXTERN GLfloat shininess ; 

// For multiple objects, read from a file.  
const int maxobjects = 1000 ; 
EXTERN int numobjects ; 
EXTERN struct object {
  shape type ; 
  GLfloat size ;
  GLfloat width;
  GLfloat length;
  GLfloat height;
  GLfloat inner_radius;
  GLfloat outer_radius;
  GLfloat depth;
  GLfloat ambient[4] ; 
  GLfloat diffuse[4] ; 
  GLfloat specular[4] ;
  GLfloat emission[4] ; 
  GLfloat shininess ;
  GLuint texture;
  
  mat4 transform ; 
} objects[maxobjects] ;

// Variables to set uniform params for lighting fragment shader 
EXTERN GLuint lightcol ; 
EXTERN GLuint lightpos ;
EXTERN GLuint lightscreenLoc ; 
EXTERN GLuint numusedcol ;
EXTERN GLuint numusedGodray ; 
EXTERN GLuint enablelighting ; 
EXTERN GLuint ambientcol ; 
EXTERN GLuint diffusecol ; 
EXTERN GLuint specularcol ; 
EXTERN GLuint emissioncol ; 
EXTERN GLuint shininesscol ;
EXTERN GLuint istex ;
EXTERN GLuint tex;
EXTERN int occlusionMapLoc ;
EXTERN GLuint occlusionMap ;
EXTERN GLuint occlusionFramebuffer ;
EXTERN GLuint isbump;
EXTERN bool godray;
EXTERN bool bumpmap;

// Variables to toggle different lighting/shading/texture/animation features
EXTERN GLuint isCelShaded;	// phong is the default
EXTERN bool celShade;
EXTERN bool outline;
EXTERN bool wireframe;
EXTERN GLuint enableTextures;
EXTERN bool textured;
EXTERN bool animate;

// Needed for shadowmap
EXTERN GLuint shadowvertexshader, shadowfragshader, shadowprogram;
EXTERN GLuint f_id;
EXTERN GLuint dt_id;
EXTERN GLuint depthmatrix;
EXTERN GLuint depthmatrix2;
EXTERN GLuint shadowmap;
EXTERN vec3 lightInvDir;
EXTERN bool usingShadow;

// Needed for displacement
EXTERN GLuint heightsampler;
EXTERN GLuint floor_texture;
EXTERN GLuint floor_normal_map;
EXTERN GLuint floor_height_map;
EXTERN bool displacementmap;
EXTERN GLuint isdisplace;

// animated texture
EXTERN GLuint triforce1;
EXTERN GLuint triforce2;
EXTERN GLuint triforce3;

