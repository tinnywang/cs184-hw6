# version 120

/* This is the fragment shader for reading in a scene description, including
   lighting. Uniform lights are specified from the main program, and used in
   the shader. As well as the material parameters of the object. */

// Mine is an old machine. For version 130 or higher, do
// in vec4 color ;
// in vec4 mynormal ;
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ;
varying vec4 myvertex ;

uniform sampler2D tex;
uniform bool istex;
uniform bool enableTextures;

uniform bool isCelShaded;
const float interval1 = 0.1;
const float interval2 = 0.3;
const float interval3 = 0.6;
const float interval4 = 1.0;

const int numLights = 10 ;
uniform bool enablelighting ; // are we lighting at all (global).
uniform vec4 lightposn[numLights] ; // positions of lights
uniform vec4 lightcolor[numLights] ; // colors of lights
uniform int numused ; // number of lights used

// Now, set the material parameters. These could be varying and/or bound to
// a buffer. But for now, I'll just make them uniform.
// I use ambient, diffuse, specular, shininess as in OpenGL.
// But, the ambient is just additive and doesn't multiply the lights.

uniform vec4 ambient ;
uniform vec4 diffuse ;
uniform vec4 specular ;
uniform vec4 emission ;
uniform float shininess ;

vec4 phongIllumination(vec3 normal, vec3 direction, vec3 halfAngle, vec4 lightcolor) {
    float nDotL = dot(normal, direction);
    vec4 diffuseTerm = diffuse * lightcolor * max(nDotL, 0.0);
    float nDotH = dot(normal, halfAngle);
    vec4 specularTerm = specular * lightcolor * pow(max(nDotH, 0.0), shininess);
    return diffuseTerm + specularTerm;
}

vec4 celShade(vec3 normal, vec3 direction, vec3 halfAngle, vec4 lightcolor) {
    float df = max(dot(normal, direction), 0.0);
    float epsilon = fwidth(df);
    if (df > interval1 - epsilon && df < interval1 + epsilon) {
        df = mix(interval1, interval2, smoothstep(interval1 - epsilon, interval1 + epsilon, df));
    } else if (df > interval2 - epsilon && df < interval2 + epsilon) {
        df = mix(interval2, interval3, smoothstep(interval2 - epsilon, interval2 + epsilon, df));
    } else if (df > interval3 - epsilon && df < interval3 + epsilon) {
        df = mix(interval3, interval4, smoothstep(interval3 - epsilon, interval3 + epsilon, df));
    } else if (df < interval1) {
        df = 0.0;
    } else if (df < interval2) {
        df = interval2;
    } else if (df < interval3) {
        df = interval3;
    } else {
        df = interval4;
    }
    vec4 diffuseTerm = diffuse * lightcolor * df;

    float sf = pow(max(dot(normal, halfAngle), 0.0), shininess);
    epsilon = fwidth(sf);
    if(sf > 0.5 - epsilon && sf < 0.5 + epsilon) {
        sf = smoothstep(0.5 - epsilon, 05 + epsilon, sf);
    } else {
        sf = step(0.5, sf);
    }
    vec4 specularTerm = specular * lightcolor * sf;
    return diffuseTerm + specularTerm;
}

void main (void)
{
    if (enablelighting) {

        vec4 finalcolor = vec4(0, 0, 0, 0);

        // YOUR CODE FOR HW 2 HERE
        // A key part is implementation of the fragment shader
        vec4 _mypos = gl_ModelViewMatrix * myvertex;
        vec3 mypos = _mypos.xyz / _mypos.w;
        // not sure about this
        vec3 eyepos = vec3(0, 0, 0);
        vec3 eyedir = normalize(eyepos - mypos);
        vec3 normal = normalize(gl_NormalMatrix * mynormal);
        vec3 direction, halfAngle;

        for (int i = 0; i < numused; i++) {
            vec4 lightcolor = lightcolor[i];
            if (lightposn[i].w == 0) {
                direction = normalize(lightposn[i].xyz);
                halfAngle = normalize(direction + eyedir);
            } else {
                vec3 position = lightposn[i].xyz / lightposn[i].w;
                direction = normalize(position - mypos);
                halfAngle = normalize(direction + eyedir);
            }
            if (isCelShaded) {
                finalcolor += celShade(normal, direction, halfAngle, lightcolor);
            } else {
                finalcolor += phongIllumination(normal, direction, halfAngle, lightcolor);
            }
        }
        finalcolor += (color * ambient) + emission;
        if (istex && enableTextures) {
          gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * finalcolor;
        } else {
          gl_FragColor = finalcolor;
        }
    } else {
        if (istex && enableTextures) {
          gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * color;
        } else {
          gl_FragColor = color ;
        }
    }
}

/*
vec4 crepuscular() {
  vec4 final_color = vec4(0), color;
  float illuminationDecay;
  vec2 textCoord, deltaTextCoord;
  for (int i=0; i < numused; i++) {
    illuminationDecay = 1.0;
    textCoord = gl_TexCoord[0].st;
    deltaTextCoord = textCoord - lightScreenCoord[i];
    deltaTexCoord *= 1.0/(float(num_samples) * density);
    vec4 sample;
    for (int i=0; i < num_samples; i++) {
      textCoord -= deltaTextCoord;
      sample = texture2D(occlusionMap, textCoord);
      sample *= illuminationDecay * weight;
      color += sample;
      illuminationDecay *= decay;
    }
    color *= exposure;
    final_color += color;
  }
  return final_color;
}
*/


