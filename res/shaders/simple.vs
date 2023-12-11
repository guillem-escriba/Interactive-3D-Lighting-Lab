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
uniform vec3 ambient_light;
uniform vec3 diffuse_color;
uniform vec3 specular_color;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

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
	vec3 l = normalize(light_position-v_wPos);
     vec3 v = normalize(eye-v_wPos);
     vec3 l2 = normalize(v_wPos-light_position);
     vec3 r = reflect(l2,v_wNormal);

     vec3 la = ambient_light*ambient;
     vec3 ld = diffuse*clamp(dot(l,v_wNormal),0.0,1.0)*diffuse_color;

     vec3 rv = specular*clamp(max(0.0,dot(r,v)),0.0,1.0);
     rv.x = pow(max(0.0,rv.x),shininess);
     rv.y = pow(max(0.0,rv.y),shininess);
     rv.z = pow(max(0.0,rv.z),shininess);
     
     vec3 ls = rv*specular_color;

     vertexcolor = la+ld+ls;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}