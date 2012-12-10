# version 120


// Mine is an old machine.  For version 130 or higher, do
// out vec4 color ;
// out vec4 mynormal ;
// out vec4 myvertex ;
// That is certainly more modern
uniform mat4 depthmatrix;
attribute vec3 tangent;
attribute vec3 bitangent;
varying vec4 color ;
varying vec3 mynormal ;
varying vec4 myvertex ;
varying mat3 mytbn;
varying vec4 shadowcoord;

void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex ;
    color = gl_Color ;
    mynormal = gl_Normal ;
    myvertex = gl_Vertex ;
    
    shadowcoord = depthmatrix * gl_Vertex;
    mytbn = transpose(mat3(
        normalize((gl_NormalMatrix * normalize(tangent)).xyz),
        normalize((gl_NormalMatrix * normalize(bitangent)).xyz),
        normalize((gl_NormalMatrix * normalize(gl_Normal)).xyz)
    )); 
}

