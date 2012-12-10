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
varying mat3 mytbn;
varying vec4 shadowcoord;
uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D shadowmap;
uniform bool istex;
uniform bool isbump;
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
        
        float shadow = 1.0;
        if ( texture2D( shadowmap, shadowcoord.xy ).z  <  shadowcoord.z){
          shadow = 0.5;
        }
        
        vec4 _mypos = gl_ModelViewMatrix * myvertex;
        vec3 mypos = _mypos.xyz / _mypos.w;
        // not sure about this
        vec3 eyepos = vec3(0, 0, 0);
        vec3 eyedir = normalize(eyepos - mypos);
        vec3 normal = normalize(gl_NormalMatrix * mynormal);
        if (isbump && gl_TexCoord[0].s >= 0) {
          mypos = mytbn * mypos;
          eyedir = normalize(mytbn * eyedir);
          normal = normalize(vec3(texture2D(bump, gl_TexCoord[0].st).rgb * 2.0 - 1.0));
        }
        vec3 direction, halfAngle;

        for (int i = 0; i < numused; i++) {
            vec4 lightcolor = lightcolor[i];
            if (lightposn[i].w == 0) {
                direction = normalize(lightposn[i].xyz);
                if (isbump && gl_TexCoord[0].s >= 0) {
                  direction = normalize(mytbn * direction);
                }
                halfAngle = normalize(direction + eyedir);
            } else {
                vec3 position = lightposn[i].xyz / lightposn[i].w;
                if (isbump && gl_TexCoord[0].s >=0) {
                  position = mytbn * position;
                }
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
        if (istex && enableTextures && gl_TexCoord[0].s >= 0) {
        
          gl_FragColor = shadow * texture2D(tex, gl_TexCoord[0].st) * finalcolor;
        } else {
          gl_FragColor = shadow * finalcolor;
        }
    } else {
        if (istex && enableTextures && gl_TexCoord[0].s >= 0) {
          gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * color;
        } else {
          gl_FragColor = color ;
        }
    }
}
