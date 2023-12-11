#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

// Constantes máximas 
#define MAX_MESHES 50
#define MAX_LIGHTS 10

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;
Shader* gouraud = NULL;
Shader* phong = NULL;

//might be useful...
Material* material = NULL;
Light* light = NULL;


Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

// Variables de inicialización de luces
Vector3 l_pos(0, 0, 0);
Vector3 l_dif(0.6f, 0.6f, 0.6f);
Vector3 l_spec(0.6f, 0.6f, 0.6f);

float angle = 0;

int N_MESHES = 1; // Número de meshes
int N_LIGHTS = 1; // Número de luces 

bool setLight = false; // Boleano para establecer el editor de luces
bool isPhong = true; // Boleano para alternar shader mode



Light* lights[MAX_LIGHTS]; // Lista de luces

Matrix44 matrixs[MAX_MESHES]; // Lista de matrices de las meshes



Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ( "../res/meshes/lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	//shader = Shader::Get( "../res/shaders/simple.vs", "../res/shaders/simple.fs" );

	//load your Gouraud and Phong shaders here and stored them in some glbal variables
	printf("Hello");
	gouraud = Shader::Get("../res/shaders/gouraud.vs", "../res/shaders/gouraud.fs");
	phong = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");

	shader = phong; // Sirve para alternar de shaders
	
	//CODE HERE:
	//create a light (or several) and and some materials
	light = new Light();
	lights[0] = light;
	

	material = new Material();
}

//render one frame
void Application::render(void)
{
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z will pass if the Z is LESS or EQUAL to the Z of the pixel
	

	//choose a shader and enable it
	shader->enable();

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader

	shader->setUniform3("ia", ambient_light);
	shader->setUniform3("ka", material->ambient);
	shader->setUniform3("ks", material->specular);
	shader->setUniform3("kd", material->diffuse);
	shader->setUniform1("alpha", material->shininess);
	shader->setUniform3("eye", camera->eye);

	
	
	for (int i = 0; i < N_LIGHTS; i++) { // Itera tantas veces como luces se añadan MULTIPASS
		if (i == 0) { // La primera iteración lo desactiva
			glDisable(GL_BLEND);
		}
		else { 
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}

		shader->setUniform3("light_position", lights[i]->position);
		shader->setUniform3("id", lights[i]->diffuse_color);
		shader->setUniform3("is", lights[i]->specular_color);

		//do the draw call into the GPU
		
		for (int i = 0; i < N_MESHES; i++) { // Itera tantas veces como meshes
			matrixs[i].setIdentity(); 
			int dist = N_MESHES / 2; // Para que aparezcan a ambos lados se resta la mitad del N_MESHES para que vaya alternando
			matrixs[i].translate(12 * (i - dist), 0, 12 * (-i + dist)); // La ubica a una distancia constante en función del N_MESHES activos
			matrixs[i].rotate(angle, Vector3(0, 1, 0));
			shader->setMatrix44("model", matrixs[i]); //upload the transform matrix to the shader
			shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
			mesh->render(GL_TRIANGLES);
		}
	}
	

	//disable shader when we do not need it any more

	shader->disable();
	

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window); 
}

//called after render
void Application::update(double seconds_elapsed)
{
	
	
	if (keystate[SDL_SCANCODE_A])
		camera->orbit(seconds_elapsed, Vector3(0, 1, 0));
	else if (keystate[SDL_SCANCODE_D])
		camera->orbit(seconds_elapsed, Vector3(0, -1, 0));
	if (keystate[SDL_SCANCODE_W])
		camera->orbit(seconds_elapsed, Vector3(1, 0, 0));
	else if (keystate[SDL_SCANCODE_S])
		camera->orbit(seconds_elapsed, Vector3(-1, 0, 0));
	
	
	if (setLight) { // EDITOR DE LUCES
		if (keystate[SDL_SCANCODE_RIGHT]) // Luz aumenta x 
			l_pos = l_pos + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
		else if (keystate[SDL_SCANCODE_LEFT]) // Luz disminuye x
			l_pos = l_pos + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
		if (keystate[SDL_SCANCODE_UP]) // Luz aumenta y
			l_pos = l_pos + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
		else if (keystate[SDL_SCANCODE_DOWN]) // Luz disminuye y
			l_pos = l_pos + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
		if (keystate[SDL_SCANCODE_K]) // Luz aumenta z
			l_pos = l_pos + Vector3(0, 0, 1) * seconds_elapsed * 10.0;
		else if (keystate[SDL_SCANCODE_L]) // Luz disminuye z
			l_pos = l_pos + Vector3(0, 0, -1) * seconds_elapsed * 10.0;

		if (keystate[SDL_SCANCODE_R]) { // Aumenta intensidad rojo
			l_dif = l_dif + Vector3(0.5f, 0, 0) * seconds_elapsed;
			l_spec = l_spec + Vector3(0.5f,0, 0) * seconds_elapsed;
			l_dif.x = min(l_dif.x, 1);
			l_spec.x = min(l_spec.x, 1);
		}
		else if (keystate[SDL_SCANCODE_T]) { // Disminuye intensidad rojo
			l_dif = l_dif + Vector3(-0.5f, 0, 0) * seconds_elapsed;
			l_spec = l_spec + Vector3(-0.5f, 0, 0) * seconds_elapsed;
			l_dif.x = max(l_dif.x, 0);
			l_spec.x = max(l_spec.x, 0);
		}
		if (keystate[SDL_SCANCODE_G]) {// Aumenta intensidad verde
			l_dif = l_dif + Vector3(0, 0.5f, 0) * seconds_elapsed;
			l_spec = l_spec + Vector3(0, 0.5f, 0) * seconds_elapsed;
			l_dif.y = min(l_dif.y, 1);
			l_spec.y = min(l_spec.y, 1);
		}
		else if (keystate[SDL_SCANCODE_H]) { // Disminuye intensidad verde
			l_dif = l_dif + Vector3(0, -0.5f, 0) * seconds_elapsed;
			l_spec = l_spec + Vector3(0, -0.5f, 0) * seconds_elapsed;
			l_dif.y = max(l_dif.y, 0);
			l_spec.y = max(l_spec.y, 0);
		}
		if (keystate[SDL_SCANCODE_B]) { // Aumenta intensidad azul
			l_dif = l_dif + Vector3(0, 0, 0.5f) * seconds_elapsed;
			l_spec = l_spec + Vector3(0, 0, 0.5f) * seconds_elapsed;
			l_dif.z = min(l_dif.z, 1);
			l_spec.z = min(l_spec.z, 1);
		}
		else if (keystate[SDL_SCANCODE_N]) { // Disminuye intensidad azul
			l_dif = l_dif + Vector3(0, 0, -0.5f) * seconds_elapsed;
			l_spec = l_spec + Vector3(0, 0, -0.5f) * seconds_elapsed;
			l_dif.z = max(l_dif.z, 0);
			l_spec.z = max(l_spec.z, 0);
		}
		if (N_LIGHTS > 0) {
			lights[N_LIGHTS - 1]->addlight(l_pos, l_dif, l_spec); // Actualiza los valores de la luz actual a tiempo real
		}
	}
	
	
	 
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
		case SDLK_z: // Añade meshes
			if (N_MESHES < MAX_MESHES) {
				N_MESHES++;
				printf("Number of meshes: %d/%d.\n", N_MESHES, MAX_MESHES);
			}
			else
			{
				printf("Max number of meshes reached: %d/%d.\n", N_MESHES, MAX_MESHES);
			}
			break; //ESC key, kill the app
		case SDLK_x: // Quita meshes
			if (N_MESHES > 1) {
				N_MESHES--;
				printf("Number of meshes: %d/%d.\n", N_MESHES, MAX_MESHES);
			}
			else
			{
				printf("Min number of meshes reached: %d/%d.\n", N_MESHES, MAX_MESHES);
			}
			break; //ESC key, kill the app
		
		case SDLK_p: // Alterna modos
			if (isPhong) {
				isPhong = false;
				shader = gouraud;
				printf("Shader mode: Gouraud shading.\n");
			}
			else
			{
				isPhong = true;
				shader = phong;
				printf("Shader mode: Phong Shading.\n");
			}
			break; //ESC key, kill the app
		
		
		
		case SDLK_i:
			if (setLight) { // Activa el editor de luces
				setLight = false;
				printf("Light mode: Disabled.\n");
			}
			else
			{	
				if (N_LIGHTS > 0) {
					l_pos = lights[N_LIGHTS - 1]->position;
					l_dif = lights[N_LIGHTS - 1]->diffuse_color;
					l_spec = lights[N_LIGHTS - 1]->specular_color;
				}
				setLight = true;
				printf("Light mode: Enabled.\n");

			}
			break; //ESC key, kill the app

		case SDLK_q: // Borra luces
			if (N_LIGHTS < 1) {
				printf("All lights deleted, add a new one with 'I' (to enter to light mode) and 'Y' (to add a new light) (%d/%d).\n", N_LIGHTS, MAX_LIGHTS);
			}
			else
			{
				// Se encargan de almacenar los valores para que no se sobreescriban
				if (N_LIGHTS > 1) {
					l_pos = lights[N_LIGHTS - 2]->position;
					l_dif = lights[N_LIGHTS - 2]->diffuse_color;
					l_spec = lights[N_LIGHTS - 2]->specular_color;
				}
				N_LIGHTS--;
				lights[N_LIGHTS] = NULL; // Borra la última luz creada
				printf("Number of lights: %d/%d.\n", N_LIGHTS, MAX_LIGHTS);
			}
			break; //ESC key, kill the app
		case SDLK_e: // Crea luces
			if (N_LIGHTS >= MAX_LIGHTS) {
				printf("Max number of lights reached, delete last light added with 'U': % d / % d.\n", N_LIGHTS, MAX_LIGHTS);
			}
			else
			{
				// Añade una nueva luz
				Light* l = new Light();
				l_pos = Vector3(0, 0, 0);
				l_dif = Vector3(0.6f, 0.6f, 0.6f);
				l_spec = Vector3(0.6f, 0.6f, 0.6f);
				l->addlight(l_pos, l_dif, l_spec);
				lights[N_LIGHTS] = l;
				N_LIGHTS++;
				printf("Number of lights: %d/%d.\n", N_LIGHTS, MAX_LIGHTS);
			}
			break; //ESC key, kill the app


	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		
	}

}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{
		
	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
