# version 120

void main (void)
{
  gl_FragColor = vec4(vec3(.5,.5,.5) * (gl_FragCoord.z+1)/2, 1); 
}