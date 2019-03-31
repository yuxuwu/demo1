#version 330 core

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

uniform vec2 iResolution;
uniform float iTime;
out vec4 fragColor;

/* 
	This gets the minimum distance from a point to every object
	in the scene, 
	used in RayMarch() and Normal()
 */
float GetDistance(vec3 p) {
    vec4 sphere = vec4(0., 1., 6., 1.);
   
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
    vec3 lPos = vec3(3.+sin(iTime), 3.+sin(iTime), 3.);
    
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
    vec2 uv = (gl_FragCoord.xy -.5*iResolution.xy) /iResolution.y;
    
    // Camera Model
    vec3 ro = vec3(0., 1., 0.);
    vec3 rd = normalize(vec3(uv.x, uv.y, 1.));
    
    float d = RayMarch(ro, rd);
    
    vec3 p = ro + rd*d;
    
    float diffuse = DiffuseLight(p);
   
    // Output to screen
    fragColor = vec4(vec3(diffuse),1.0);
}
