//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
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
	//here we set up the normal as a color to see them as a debug
	vec3 color;

	//here write the computations for PHONG.
	vec3 l = normalize(light_position-v_wPos);
     vec3 v = normalize(eye-v_wPos);
     vec3 r = reflect(-l,v_wNormal);

     vec3 la = ka*ia;

     vec3 ld = kd*clamp(dot(l,v_wNormal),0.0,1.0)*id;
 
     vec3 ls = ks*pow(clamp(dot(r,v),0.0,1.0),alpha)*is;

     color = la+ld+ls;

	//for GOURAUD you dont need to do anything here, just pass the color from the vertex shader
	//...

	//set the ouput color por the pixel
	gl_FragColor = vec4( color, 1.0 ) * 1.0;
}
