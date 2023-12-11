//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 vertexcolor;

//here create uniforms for all the data we need here
uniform vec3 light_position;
uniform vec3 eye;
uniform vec3 ia;
uniform vec3 id;
uniform vec3 is;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float alpha;

void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;
	//convert local normal to world coordinates
	vec3 wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	//pass them to the pixel shader interpolated
	v_wPos = wPos;
	v_wNormal = wNormal;

	//in GOURAUD compute the color here and pass it to the pixel shader
	vec3 l = normalize(light_position-wPos);
     vec3 v = normalize(eye-wPos);
     vec3 r = reflect(-l,wNormal);

     vec3 la = ka*ia;

     vec3 ld = kd*clamp(dot(l,wNormal),0.0,1.0)*id;
 
     vec3 ls = ks*pow(clamp(dot(r,v),0.0,1.0),alpha)*is;

     vertexcolor = la+ld+ls;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}