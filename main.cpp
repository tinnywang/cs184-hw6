/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "UCB/grader.h"

using namespace std ;

// Main variables in the program.
#define MAINPROGRAM
#include "variables.h"
#include "readfile.h" // prototypes for readfile.cpp  
void display(void) ;  // prototype for display function.

Grader grader;
bool allowGrader = false;
int oldx;
int oldy;
int angle_change;

// Uses the Projection matrices (technically deprecated) to set perspective
// We could also do this in a more modern fashion with glm.
void reshape(int width, int height) {
    w = width;
    h = height;
    mat4 mv ; // just like for lookat

    glMatrixMode(GL_PROJECTION);
    float aspect = w / (float) h, zNear = 0.1, zFar = 1000.0 ;
    // I am changing the projection stuff to be consistent with lookat
    if (useGlu) mv = glm::perspective(fovy,aspect,zNear,zFar) ;
    else {
        mv = Transform::perspective(fovy,aspect,zNear,zFar) ;
        mv = glm::transpose(mv) ; // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]) ;

    glViewport(0, 0, w, h);
}

void saveScreenshot(string fname) {
    int pix = w * h;
    BYTE pixels[3*pix];
    glReadBuffer(GL_FRONT);
    glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE, pixels);

    FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

    std::cout << "Saving screenshot: " << fname << "\n";

    FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}


void printHelp() {
    std::cout << "\npress 'h' to print this message again.\n"
              << "press '+' or '-' to change the amount of rotation that\noccurs with each arrow press.\n"
              << "press 'g' to switch between using glm::lookAt and glm::Perspective or your own LookAt.\n"
              << "press 'r' to reset the transformations.\n"
              << "press 'v' 's' to do view [default], scale.\n"
              << "press the arrow keys to move around the scene.\n"
              << "move the mouse to look around.\n"
              << "press 'z' to zoom in, 'x' to zoom out.\n"
              << "press 'o' to toggle outlines.\n"
              << "press 'w' to toggle wireframes.\n"
              << "press 'f' to toggle fragment shader.\n"
              << "press 't' to toggle textures.\n"
              << "press 'a' to toggle animation.\n"
	      << "press 'l' to toggle volumetric lighting.\n"
              << "press ESC to quit.\n" ;
}

void drag(int x, int y) {
    if (angle_change) {
        double diff_x = x - oldx;
        double diff_y = oldy - y;
        double x_ang = glm::degrees(glm::atan(diff_x * .01, 1.0));
        double y_ang = glm::degrees(glm::atan(diff_y*.01,1.0));
        Transform::up(y_ang, eye,  up, center);
        Transform::left(-x_ang, eye, up, center);
        oldx = x ;
        oldy = y ;
        glutPostRedisplay();
    }
}

bool up_direction = true;
bool slow = true;
double init_increment = .2;
double increment = init_increment;
int counter = 0;
bool waiting = false;
void animate_sword(void) {
    if (waiting) {
        if (counter == 30) {
            waiting = false;
        } else {
            counter++;
        }
        return;
    }
    if (sword_move >= 3) {
        if (counter != 30) {
            waiting = true;
            return;
        } else {
            counter = 0;
        }
        up_direction = false;
        slow = false;
    } else if (sword_move <= -3) {
        if (counter != 30) {
            waiting = true;
            return;
        } else {
            counter = 0;
        }
        up_direction = true;
        slow = false;
    }
    if (up_direction) {
        sword_move+=increment;
        if (slow) {
            increment = increment / 1.06;
        } else {
            increment = increment * 1.06;
        }
    } else {
        sword_move-=increment;
        if (slow) {
            increment = increment / 1.06;
        } else {
            increment = increment * 1.06;
        }
    }
    if (sword_move < 0 && up_direction == false && !slow) {
        increment = init_increment;
        slow = true;
    }
    if (sword_move > 0 && up_direction == true && !slow) {
        increment = init_increment;
        slow = true;
    }
}

void animate_crystals(void) {
    crystal_deg += 1;
    if (crystal_deg == 360) {
        crystal_deg = 0;
    }
}

void animation(void) {
    if (animate) {
        animate_sword();
        animate_crystals();
    }
    glutPostRedisplay();
}

// Mouse motion.  You need to respond to left clicks (to add points on curve)
// and right clicks (to delete points on curve)
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            oldx = x ;
            oldy = y ;
            angle_change = true;
        }
        if (state == GLUT_UP) {
            angle_change = false;
        }
    }
}


void keyboard(unsigned char key, int x, int y) {
    glUseProgram(shaderprogram);
    switch(key) {
    case '+':
        amount++;
        std::cout << "amount set to " << amount << "\n" ;
        break;
    case '-':
        if (amount > 0) {
            amount--;
        }
        std::cout << "amount set to " << amount << "\n" ;
        break;
    case 'g':
        useGlu = !useGlu;
        reshape(w,h) ;
        std::cout << "Using glm::LookAt and glm::Perspective set to: " << (useGlu ? " true " : " false ") << "\n" ;
        break;
    case 'h':
        printHelp();
        break;
    case 27:  // Escape to quit
        exit(0) ;
        break ;
    case 'r': // reset eye and up vectors, scale and translate.
        eye = eyeinit ;
        up = upinit ;
        center = centerinit;
        fovy = fovyinit;
        sx = sy = 1.0 ;
        reshape(w,h) ;
        break ;
    case 'v':
        transop = view ;
        std::cout << "Operation is set to View\n" ;
        break ;
    case 's':
        transop = scale ;
        std::cout << "Operation is set to Scale\n" ;
        break ;
    case 'x':
        if (fovy < 179) {
            fovy++;
        }
        reshape(w,h) ;
        break;
    case 'z':
        if (fovy > 1) {
            fovy--;
        }
        reshape(w,h) ;
        break;
    case 'o':
        outline = !outline;
        glutPostRedisplay();
        break;
    case 'f':
        celShade = !celShade;
        glUniform1i(isCelShaded, celShade);
        glutPostRedisplay();
        break;
    case 't':
        textured = !textured;
        glUniform1i(enableTextures, textured);
        glutPostRedisplay();
        break;
    case 'a':
        animate = !animate;
        glutPostRedisplay();
        break;
    case 'w':
        wireframe = !wireframe;
        glutPostRedisplay();
        break;
    case 'l':
        godray = !godray;
        glutPostRedisplay();
        break;
    }
    glutPostRedisplay();
}

//  You will need to enter code for the arrow keys
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key, int x, int y) {
    switch(key) {
    case 100: //left
        if (transop == view) Transform::side(-amount * .1, eye,  up, center);
        else if (transop == scale) sx -= amount * 0.01 ;
        break;
    case 101: //up
        if (transop == view) Transform::forward(amount * .1,  eye,  center);
        else if (transop == scale) sy += amount * 0.01 ;
        break;
    case 102: //right
        if (transop == view) Transform::side(amount* .1, eye,  up, center);
        else if (transop == scale) sx += amount * 0.01 ;
        break;
    case 103: //down
        if (transop == view) Transform::forward(-amount* .1,  eye,  center);
        else if (transop == scale) sy -= amount * 0.01 ;
        break;
    }
    glutPostRedisplay();
}

void init() {
    // Initialize shaders
    vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
    godrayshader = initshaders(GL_FRAGMENT_SHADER, "shaders/godray.frag.glsl");
    shaderprogram = initprogram(vertexshader, fragmentshader) ;
    godrayshaderprogram = initprogram(vertexshader, godrayshader) ;
    enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
    lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;
    lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;
    numusedcol = glGetUniformLocation(shaderprogram,"numused") ;
    ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;
    diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;
    specularcol = glGetUniformLocation(shaderprogram,"specular") ;
    emissioncol = glGetUniformLocation(shaderprogram,"emission") ;
    shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;
    istex = glGetUniformLocation(shaderprogram, "istex");
    isbump = glGetUniformLocation(shaderprogram, "isbump");
    isCelShaded = glGetUniformLocation(shaderprogram, "isCelShaded");
    enableTextures = glGetUniformLocation(shaderprogram, "enableTextures");
    texsampler = glGetUniformLocation(shaderprogram, "tex");
    bumpsampler = glGetUniformLocation(shaderprogram, "bump");
    tangent_loc = glGetAttribLocation(shaderprogram, "tangent");
    bitangent_loc = glGetAttribLocation(shaderprogram, "bitangent");
    glUseProgram(shaderprogram);
    glUniform1i(enableTextures, true);

    carpet = load_texture("textures/carpet.jpg");
    wood = load_texture("textures/wood.jpg");
    textures[0] = load_texture("textures/glass.jpg");
    textures[1] = load_texture("textures/glass2.jpg");
    textures[2] = load_texture("textures/glass3.jpg");
    textures[3] = load_texture("textures/glass4.jpg");
    textures[4] = load_texture("textures/glass5.jpg");
    textures[5] = load_texture("textures/glass6.jpg");
    textures[6] = load_texture("textures/glass7.jpg");
    textures[7] = load_texture("textures/glass8.jpg");
    textures[8] = load_texture("textures/glass9.jpg");
    textures[9] = load_texture("textures/glass10.jpg");
    wall_normal_map = load_texture("textures/wall/MetallicAssembly-NormalMap.png");
    wall_texture = load_texture("textures/wall/MetallicAssembly-ColorMap.png");

    outline = false; // off by default
    celShade = false; // use phong by default
    textured = true;
    animate = true;
    wireframe = false;
    godray = true;

    // variables for godrays
    occlusionMapLoc = glGetUniformLocation(godrayshaderprogram, "occlusionMap");
    lightscreenLoc = glGetUniformLocation(godrayshaderprogram, "lightscreen");
    numusedGodray = glGetUniformLocation(godrayshaderprogram, "numused");

    // Create a texture for the occlusion map
    glGenTextures(1, &occlusionMap);
    glBindTexture(GL_TEXTURE_2D, occlusionMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w/2, h/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Create a framebuffer for the occlusion map
    glGenFramebuffersEXT(1, &occlusionFramebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, occlusionFramebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, occlusionMap, 0);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Usage: transforms scenefile [grader input (optional)]\n";
        exit(-1);
    }

    FreeImage_Initialise();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("HW2: Scene Viewer");
    init();
    readfile(argv[1]) ;
    glutDisplayFunc(display);
    glutSpecialFunc(specialKey);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);
    glutReshapeWindow(w, h);
    glutMotionFunc(drag);
    glutIdleFunc(animation);

    if (argc > 2) {
        allowGrader = true;
        stringstream tcid;
        tcid << argv[1] << "." << argv[2];
        grader.init(tcid.str());
        grader.loadCommands(argv[2]);
        grader.bindDisplayFunc(display);
        grader.bindSpecialFunc(specialKey);
        grader.bindKeyboardFunc(keyboard);
        grader.bindScreenshotFunc(saveScreenshot);
    }

    printHelp();
    glutMainLoop();
    FreeImage_DeInitialise();
    return 0;
}
