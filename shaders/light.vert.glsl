# version 120


// Mine is an old machine.  For version 130 or higher, do
// out vec4 color ;
// out vec4 mynormal ;
// out vec4 myvertex ;
// That is certainly more modern
uniform mat4 depthmatrix;
uniform sampler2D displace;
uniform bool isdisplace;

attribute vec3 tangent;
attribute vec3 bitangent;
varying vec4 color ;
varying vec3 mynormal ;
varying vec4 myvertex ;
varying mat3 mytbn;
varying vec4 shadowcoord;

void main() {
    vec4 newVertexPos;
	  vec4 dv;
	  float df;
	  
	  gl_TexCoord[0] = gl_MultiTexCoord0;
		
		if (isdisplace) {
      vec4 pixel = texture2D(displace, gl_TexCoord[0].xy);
      float dv = (pixel.r + pixel.g + pixel.b) / 3.0;
      dv = dv *.5;

      // calculate height map from brightness of texture sample
      vec4 displace = vec4(normalize(gl_Normal) * dv, 0.0);
      gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + displace);
    } else {
      gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex ;
      
    }
    myvertex = gl_Vertex ;
    
    color = gl_Color ;
    mynormal = gl_Normal ;
    
    shadowcoord = depthmatrix * gl_Vertex;
    mytbn = transpose(mat3(
        normalize((gl_NormalMatrix * normalize(tangent)).xyz),
        normalize((gl_NormalMatrix * normalize(bitangent)).xyz),
        normalize((gl_NormalMatrix * normalize(gl_Normal)).xyz)
    )); 
}

