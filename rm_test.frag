#version 330 core

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

uniform vec2 iResolution;
uniform float iTime;
uniform float focal_scale;
uniform vec4 ball_pos;
uniform vec3 light_pos;
uniform float static_yes;
out vec4 fragColor;

//random hash
vec4 hash42(vec2 p){
  vec4 p4 = fract(vec4(p.xyxy) * vec4(443.8975,397.2973, 491.1871, 470.7827));
  p4 += dot(p4.wzxy, p4+19.19);
  return fract(vec4(p4.x * p4.y, p4.x*p4.z, p4.y*p4.w, p4.x*p4.w));
}


float hash( float n ){
  return fract(sin(n)*43758.5453123);
}

float n( in vec3 x ){
  vec3 p = floor(x);
  vec3 f = fract(x);
  f = f*f*(3.0-2.0*f);
  float n = p.x + p.y*57.0 + 113.0*p.z;
  float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
		      mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
		  mix(mix( hash(n+113.0), hash(n+114.0),f.x),
		      mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
  return res;
}

//tape noise
float nn(vec2 p){
  float y = p.y;
  float s = iTime*2.;
  
  float v = (n( vec3(y*.01 +s, 1., 1.0) ) + .0)
           *(n( vec3(y*.011+1000.0+s, 1., 1.0) ) + .0) 
           *(n( vec3(y*.51+421.0+s, 1., 1.0) ) + .0);

  v*= hash42(   vec2(p.x +iTime*0.01, p.y) ).x +.3 ;
  
  v = pow(v+.3, 1.);
  if(v<.7) v = 0.;  //threshold
  return v;
}


/* 
   This gets the minimum distance from a point to every object
   in the scene, 
   used in RayMarch() and Normal()
 */
float GetDistance(vec3 p) {
    vec4 sphere = ball_pos;
   
    float sphereDist = length(p-sphere.xyz)-sphere.w;
    float floorDist = p.y;
    
    return min(sphereDist, floorDist);
}

float RayMarch(vec3 ro, vec3 rd) {
    float distanceFromOrigin = 0.; // our return value
    
    for(int i = 0; i < MAX_STEPS; ++i) {
        vec3 p = ro + rd*distanceFromOrigin; //new march point
        float ds = GetDistance(p);
        distanceFromOrigin += ds;
        
        // Break out of march if distance is too high, or is low
        // enough to be a desired value
        if(distanceFromOrigin > MAX_DIST || ds<SURF_DIST) break;
    }

    return distanceFromOrigin;
}

vec3 Normal(vec3 p) {
    float d = GetDistance(p);
    vec2 n = vec2(.01, 0.);

	// Hint: Scalar (operator) Vector does the operation component-wise
    vec3 norm = d - vec3(
        // Swizzle!
        GetDistance(p - n.xyy),
        GetDistance(p - n.yxy),
        GetDistance(p - n.yyx)
    );

    return normalize(norm);
}

float DiffuseLight(vec3 p) {
    // Position of light source
    vec3 lPos = light_pos;
    
    // Normal of light
    vec3 l = normalize(lPos - p);
    vec3 norm = Normal(p);
    
    // Angle between light vector and normal
    float dif = dot(norm, l); // -1 <> 1
    
    // Clamp light value between 0 and 1
    dif = clamp(dif, 0., 1.);
    //dif = dif+1./2.; // 0 <> 1
    
    /* 
       Shadow Detection: If we raymarch to object instead of light
       source, we are behind an object, meaning we are in a shadow 
    */
    float shadow = RayMarch(p+norm*SURF_DIST*2., l);
    if (shadow < length(lPos - p)) dif *= .1;
    
    return dif;
}

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 globalUV = (gl_FragCoord.xy -.5*iResolution.xy) /iResolution.y;
    vec2 uv = (focal_scale >= 1.0) ? fract(globalUV*focal_scale) - .5 :  globalUV ;
    
    // Camera Model
    vec3 ro = vec3(0., 1., 0.);
    vec3 rd = normalize(vec3(uv.x, uv.y, 1.));
    
    float d = RayMarch(ro, rd);
    
    vec3 p = ro + rd*d;
    
    float diffuse = DiffuseLight(p);

    float noise = 0.0;
    if (static_yes >= 1.0) {
	float linesN = 240.; //fields per seconds
	float one_y = iResolution.y / linesN; //field line
	uv = floor(uv*iResolution.xy/one_y)*one_y;
	noise =  nn(uv);
    }

   
    // Output to screen
    fragColor = vec4(vec3(diffuse+noise),1.0);
}
