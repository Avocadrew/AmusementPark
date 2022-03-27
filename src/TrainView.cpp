/************************************************************************
	 File:        TrainView.cpp

	 Author:
				  Michael Gleicher, gleicher@cs.wisc.edu

	 Modifier
				  Yu-Chi Lai, yu-chi@cs.wisc.edu

	 Comment:
						The TrainView is the window that actually shows the
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within
						a TrainWindow
						that is the outer window with all the widgets.
						The TrainView needs
						to be aware of the window - since it might need to
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know
						about it (beware circular references)

	 Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>
#include <string>
// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glu.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"
#include "Object.H"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif


//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l)
	: Fl_Gl_Window(x, y, w, h, l)
	//========================================================================
{
	srand(time(NULL));
	mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);
	std::string skyboxPath = "";
	resetArcball();
	for (int i = 0; i < 6; i++)
	{
		skyboxPath = "Images/skybox/" + std::to_string(i) + ".jpg";
		skybox_textures.push_back(skyboxPath);
	}

	for (int i = 0; i < 6; i++)
	{
		tilebox_textures.push_back("Images/Brick.jpg");
		tbox_textures.push_back("Images/tiles.jpg");
	}
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event))
			return 1;

	// remember what button was used
	static int last_push;

	switch (event) {

		// Mouse button being pushed event
	case FL_PUSH:
		last_push = Fl::event_button();
		// if the left button be pushed is left mouse button
		if (last_push == FL_LEFT_MOUSE) {
			doPick();
			damage(1);
			return 1;
		};
		break;

		// Mouse button release event
	case FL_RELEASE: // button release
		damage(1);
		last_push = 0;
		return 1;

		// Mouse button drag event
	case FL_DRAG:

		// Compute the new control point position
		if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
			ControlPoint* cp = &m_pTrack->points[selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

			double rx, ry, rz;
			mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
				static_cast<double>(cp->pos.x),
				static_cast<double>(cp->pos.y),
				static_cast<double>(cp->pos.z),
				rx, ry, rz,
				(Fl::event_state() & FL_CTRL) != 0);

			cp->pos.x = (float)rx;
			cp->pos.y = (float)ry;
			cp->pos.z = (float)rz;
			damage(1);
		}
		break;

		// in order to get keyboard events, we need to accept focus
	case FL_FOCUS:
		return 1;

		// every time the mouse enters this window, aggressively take focus
	case FL_ENTER:
		focus(this);
		break;

	case FL_KEYBOARD:
		int k = Fl::event_key();
		int ks = Fl::event_state();
		if (k == 'p') {
			// Print out the selected control point information
			if (selectedCube >= 0)
				printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
					selectedCube,
					m_pTrack->points[selectedCube].pos.x,
					m_pTrack->points[selectedCube].pos.y,
					m_pTrack->points[selectedCube].pos.z,
					m_pTrack->points[selectedCube].orient.x,
					m_pTrack->points[selectedCube].orient.y,
					m_pTrack->points[selectedCube].orient.z);
			else
				printf("Nothing Selected\n");

			return 1;
		};
		if (k == 'b' || k == 'B')
		{
			tw->waveBrowser->select(1);
		}
		if (k == 'n' || k == 'N')
		{
			tw->waveBrowser->select(2);
		}
		if (k == 'm' || k == 'M')
		{
			tw->waveBrowser->select(3);
		}
		break;
	}

	return Fl_Gl_Window::handle(event);
}
//************************************************************************
//
// * This function updates the firework among calling
//   
//========================================================================
void TrainView::updateFirework() {
	for (int i = 0; i < firework.size(); i++)
	{
		if (firework[i].life >= 30)
		{
			firework[i].life -= 3.0f;
			firework[i].position += glm::vec3(0, 2.0f, 0);
		}
		else if (firework[i].life >= 0 && correct == true)
		{
			float ran = (rand() % 5 + 1) / 5.0f;
			firework[i].life -= ran;
			firework[i].position += glm::vec3(ran * firework[i].velocity.x*2, ran * firework[i].velocity.y*2, ran * firework[i].velocity.z*2);
		}
		else
		{
			drawGame(true);
			firework.clear();
			i--;
			
		}
	}
	for (int i = firework.size(); i < MAXPARTICLES + 100; i++)
	{
		sevenCount % 7;
		if (i == 0)
		{
			firework.push_back(CreateFirework(sevenCount, true));
		}
		else
		{
			firework.push_back(CreateFirework(sevenCount, false));
		}
		sevenCount++;
	}
}

//************************************************************************
//
// * This function creates a firework
//   
//========================================================================
Particle TrainView::CreateFirework(int count, bool t) {
	float life = 100;
	glm::vec3 pos = glm::vec3((float)(rand() % 5 + 1) / 5.0f, (float)(rand() % 5 + 1) / 5.0f, (float)(rand() % 5 + 1) / 5.0f);
	glm::vec3 vol;
	glm::vec3 col;
	switch ((count + rand()) % 7)
	{
	case 0:
		col = glm::vec3(1.0, 0, 0);
		break;
	case 1:
		col = glm::vec3(1.0, 0.5, 0);
		break;
	case 2:
		col = glm::vec3(1.0, 1.0, 0);
		break;
	case 3:
		col = glm::vec3(0, 1.0, 0);
		break;
	case 4:
		col = glm::vec3(0, 0, 1.0);
		break;
	case 5:
		col = glm::vec3(0.29, 0, 0.51);
		break;
	case 6:
		col = glm::vec3(0.56, 0, 1.0);
		break;
	}
	float theta = (count + rand()) % 15;
	theta *= (22.5f * 0.0174532925f);
	vol = glm::vec3(cos(theta), sin(theta), (float)(rand() % 5 + 1) / 10.0f);
	return Particle(pos, vol, col, life, t);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::
draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...

		if (!this->dotCubeShader)
			this->dotCubeShader = new
			Shader(
				"Shaders/cube.vert",
				nullptr, nullptr, nullptr,
				"Shaders/cube.frag");

		if (!this->skyboxShader)
			this->skyboxShader = new
			Shader(
				"Shaders/skybox.vert",
				nullptr, nullptr, nullptr,
				"Shaders/skybox.frag");

		if (!this->tileboxShader)
			this->tileboxShader = new
			Shader(
				"Shaders/tile.vert",
				nullptr, nullptr, nullptr,
				"Shaders/tile.frag");

		if (!this->billboardShader)
		{
			this->billboardShader = new Shader(
				"Shaders/billboard.vert",
				nullptr, nullptr, nullptr,
				"Shaders/billboard.frag");
		}

		if (!this->tboxShader)
			this->tboxShader = new
			Shader(
				"Shaders/tile.vert",
				nullptr, nullptr, nullptr,
				"Shaders/tile.frag");

		if (!this->sinWaveShader)
			this->sinWaveShader = new
			Shader(
				"Shaders/sinWave.vert",
				nullptr, nullptr, nullptr,
				"Shaders/sinWave.frag");
		
		if (!this->heightMapShader)
			this->heightMapShader = new
			Shader(
				"Shaders/heightmap.vert",
				nullptr, nullptr, nullptr,
				"Shaders/heightmap.frag");

		if (!this->interactiveShader)
			this->interactiveShader = new
			Shader(
				"Shaders/interactive.vert",
				nullptr, nullptr, nullptr,
				"Shaders/interactive.frag");

		if (!this->pixelationShader)
			this->pixelationShader = new
			Shader(
				"Shaders/pixelation.vert",
				nullptr, nullptr, nullptr,
				"Shaders/pixelation.frag");

		if (!this->particleShader)
		{
			this->particleShader = new Shader(
				"Shaders/firework.vert",
				nullptr, nullptr, nullptr,
				"Shaders/firework.frag");
		}

		if (!this->waterTexture)
			this->waterTexture = new Texture2D("Images/waterFace.png");

		if (!this->uvTexture)
			this->uvTexture = new Texture2D("Images/Grass.png");

		if (!this->concreteTexture)
			this->concreteTexture = new Texture2D("Images/Concrete.png");

		if (!this->billboardTexture)
			this->billboardTexture = new Texture2D("Images/sp.png");

		if (!this->commom_matrices)
			this->commom_matrices = new UBO();
		this->commom_matrices->size = 2 * sizeof(glm::mat4);
		glGenBuffers(1, &this->commom_matrices->ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->commom_matrices->size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		if (!this->skybox_matrices)
			this->skybox_matrices = new UBO();
		this->skybox_matrices->size = 2 * sizeof(glm::mat4);
		glGenBuffers(1, &this->skybox_matrices->ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, this->skybox_matrices->ubo);
		glBufferData(GL_UNIFORM_BUFFER, this->skybox_matrices->size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, .3f, 0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	
	glUseProgram(0);



	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	//glEnable(GL_LIGHTING);
	setupObjects();


	setUBO();
	glGetFloatv(GL_MODELVIEW_MATRIX, &origin_view_matrix[0][0]);
	glGetFloatv(GL_PROJECTION_MATRIX, &origin_projection_matrix[0][0]);
	glBindBufferRange(
		GL_UNIFORM_BUFFER, 0, this->commom_matrices->ubo, 0, this->commom_matrices->size);



	glBindBufferRange(
		GL_UNIFORM_BUFFER, 1, this->skybox_matrices->ubo, 0, this->skybox_matrices->size);


	//init skybox
	if (!this->skybox)
	{
		GLfloat  vertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		this->skybox = new VAO;
		glGenVertexArrays(1, &this->skybox->vao);
		glGenBuffers(1, this->skybox->vbo);

		glBindVertexArray(this->skybox->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->skybox->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Unbind VAO
		glBindVertexArray(0);
		if (!isSkyboxTextureLoaded)
		{
			isSkyboxTextureLoaded = true;
			skyboxTexture = loadCubemap(skybox_textures);
		}
	}

	//init tilebox
	if (!this->tilebox)
	{
		GLfloat  vertices[] = {
			-100.0f,  50.0f, -100.0f,
			-100.0f, 0.0f, -100.0f,
			 100.0f, 0.0f, -100.0f,
			 100.0f, 0.0f, -100.0f,
			 100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,

			-100.0f, 0.0f,  100.0f,
			-100.0f, 0.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f,  100.0f,
			-100.0f, 0.0f,  100.0f,

			 100.0f, 0.0f, -100.0f,
			 100.0f, 0.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f, -100.0f,
			 100.0f, 0.0f, -100.0f,

			-100.0f, 0.0f,  100.0f,
			-100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f, 0.0f,  100.0f,
			-100.0f, 0.0f,  100.0f,

			-100.0f,  50.0f, -100.0f,
			 100.0f,  50.0f, -100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			-100.0f,  50.0f,  100.0f,
			-100.0f,  50.0f, -100.0f
		};

		GLfloat  normal[] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
		};

		this->tilebox = new VAO;
		glGenVertexArrays(1, &this->tilebox->vao);
		glGenBuffers(2, this->tilebox->vbo);

		glBindVertexArray(this->tilebox->vao);

		glBindBuffer(GL_ARRAY_BUFFER, this->tilebox->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, this->tilebox->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal), &normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);
		if (!isTileCubeTextureLoad)
		{
			isTileCubeTextureLoad = true;
			tileCubeTexture = loadCubemap(tilebox_textures);
		}
	}

	//init tbox
	if (!this->tbox)
	{
		GLfloat  vertices[] = {
			-100.0f,  50.0f, -100.0f,
			-100.0f, -100.0f, -100.0f,
			 100.0f, -100.0f, -100.0f,
			 100.0f, -100.0f, -100.0f,
			 100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,

			-100.0f, -100.0f,  100.0f,
			-100.0f, -100.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f, -100.0f,
			-100.0f,  50.0f,  100.0f,
			-100.0f, -100.0f,  100.0f,

			 100.0f, -100.0f, -100.0f,
			 100.0f, -100.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f, -100.0f,
			 100.0f, -100.0f, -100.0f,

			-100.0f, -100.0f,  100.0f,
			-100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f, -100.0f,  100.0f,
			-100.0f, -100.0f,  100.0f,

			-100.0f,  50.0f, -100.0f,
			 100.0f,  50.0f, -100.0f,
			 100.0f,  50.0f,  100.0f,
			 100.0f,  50.0f,  100.0f,
			-100.0f,  50.0f,  100.0f,
			-100.0f,  50.0f, -100.0f,

			-100.0f, -100.0f, -100.0f,
			-100.0f, -100.0f,  100.0f,
			 100.0f, -100.0f, -100.0f,
			 100.0f, -100.0f, -100.0f,
			-100.0f, -100.0f,  100.0f,
			 100.0f, -100.0f,  100.0f
		};

		GLfloat  normal[] = {
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f
		};

		this->tbox = new VAO;
		glGenVertexArrays(1, &this->tbox->vao);
		glGenBuffers(2, this->tbox->vbo);

		glBindVertexArray(this->tbox->vao);

		glBindBuffer(GL_ARRAY_BUFFER, this->tbox->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, this->tbox->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal), &normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);
		if (!isTCubeTextureLoad)
		{
			isTCubeTextureLoad = true;
			tCubeTexture = loadCubemap(tbox_textures);
		}
	}

	//init cube
	if (!this->dotCube) {
		GLfloat  vertices[] = {
			-0.5f ,0.0f , -0.5f,
			-0.5f ,0.0f , 0.5f ,
			0.5f ,0.0f ,0.5f ,
			0.5f ,0.0f ,-0.5f,

			-0.5f, 0.0f, -0.5f,
			-0.5f, 1.0f, -0.5f,
			-0.5f, 1.0f, 0.5f,
			-0.5f, 0.0f, 0.5f,

			-0.5f, 1.0f, 0.5f,
			0.5f, 1.0f, 0.5f,
			0.5f, 0.0f, 0.5f,
			-0.5f, 0.0f, 0.5f,

			0.5f, 0.0f, -0.5f,
			0.5f, 1.0f, -0.5f,
			0.5f, 1.0f, 0.5f,
			0.5f, 0.0f, 0.5f,

			-0.5f, 1.0f, -0.5f,
			0.5f, 1.0f, -0.5f,
			0.5f, 0.0f, -0.5f,
			-0.5f, 0.0f, -0.5f,

			-0.5f ,1.0f , -0.5f,
			-0.5f ,1.0f , 0.5f ,
			0.5f ,1.0f ,0.5f ,
			0.5f ,1.0f ,-0.5f
		};
		GLfloat  normal[] = {
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,

			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,

			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		};
		GLfloat  texture_coordinate[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,

			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};
		GLuint element[] = {
			0, 1, 2,
			0, 2, 3,

			6, 7, 4,
			6, 4, 5,

			8, 9, 10,
			8, 10, 11,

			12, 13, 14,
			12, 14, 15,

			16, 17, 18,
			16, 18, 19,

			20, 21, 22,
			20, 22, 23
		};

		this->dotCube = new VAO;
		this->dotCube->element_amount = sizeof(element) / sizeof(GLuint);
		glGenVertexArrays(1, &this->dotCube->vao);
		glGenBuffers(3, this->dotCube->vbo);
		glGenBuffers(1, &this->dotCube->ebo);

		glBindVertexArray(this->dotCube->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->dotCube->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->dotCube->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->dotCube->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->dotCube->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(0);
		if (!this->dotCubeTexture)
			this->dotCubeTexture = new Texture2D("Images/dots.png");
	}

	//init heightmap
	if (!isHeightMapLoaded)
	{
		isHeightMapLoaded = true;
		std::string heightMapPath = "";
		for (int i = 0; i < 200; i++)
		{
			heightMapPath = "Images/heightmap/" + std::to_string(i) + ".png";
			heightMaps.push_back(Texture2D(heightMapPath.c_str()));
		}
	}

	//init sinwave
	if (!this->sinWave || !this->heightMap || !this->interactive)
	{
		const int waveMeshDivide = 96;
		float dt = 200.0f / waveMeshDivide; //200 is width
		GLfloat  vertices[waveMeshDivide * waveMeshDivide * 4 * 3];
		int verticesIndex = 0;
		float xt0, xt1 = -100.0f, zt0, zt1 = -100.0f;
		for (int j = 0; j < waveMeshDivide; j++)
		{
			zt0 = zt1;
			zt1 = zt0 + dt;
			for (int i = 0; i < waveMeshDivide; i++)
			{
				xt0 = xt1;
				xt1 = xt0 + dt;

				vertices[verticesIndex++] = xt0;
				vertices[verticesIndex++] = 0.0f;
				vertices[verticesIndex++] = zt0;

				vertices[verticesIndex++] = xt1;
				vertices[verticesIndex++] = 0.0f;
				vertices[verticesIndex++] = zt0;

				vertices[verticesIndex++] = xt1;
				vertices[verticesIndex++] = 0.0f;
				vertices[verticesIndex++] = zt1;

				vertices[verticesIndex++] = xt0;
				vertices[verticesIndex++] = 0.0f;
				vertices[verticesIndex++] = zt1;
			}
			xt1 = -100.0f;
		}

		GLfloat  texture_coordinate[waveMeshDivide * waveMeshDivide * 8];
		verticesIndex = 0;
		xt1 = 0.0f;
		zt1 = 0.0f;
		dt = 1.0f / waveMeshDivide;
		for (int j = 0; j < waveMeshDivide; j++)
		{
			zt0 = zt1;
			zt1 = zt0 + dt;
			for (int i = 0; i < waveMeshDivide; i++)
			{
				xt0 = xt1;
				xt1 = xt0 + dt;

				texture_coordinate[verticesIndex++] = xt0;
				texture_coordinate[verticesIndex++] = zt1;
				texture_coordinate[verticesIndex++] = xt1;
				texture_coordinate[verticesIndex++] = zt1;
				texture_coordinate[verticesIndex++] = xt1;
				texture_coordinate[verticesIndex++] = zt0;
				texture_coordinate[verticesIndex++] = xt0;
				texture_coordinate[verticesIndex++] = zt0;
			}
		}

		GLuint element[waveMeshDivide * waveMeshDivide * 6];
		int elementsIndex = 0;
		verticesIndex = 0;
		for (int j = 0; j < waveMeshDivide; j++)
		{
			for (int i = 0; i < waveMeshDivide; i++)
			{
				element[elementsIndex++] = verticesIndex;
				element[elementsIndex++] = verticesIndex + 1;
				element[elementsIndex++] = verticesIndex + 2;
				element[elementsIndex++] = verticesIndex;
				element[elementsIndex++] = verticesIndex + 2;
				element[elementsIndex++] = verticesIndex + 3;
				verticesIndex += 4;
			}
		}

		// sinewave
		this->sinWave = new VAO;
		this->sinWave->element_amount = sizeof(element) / sizeof(GLuint);

		glGenVertexArrays(1, &this->sinWave->vao);
		glGenBuffers(2, this->sinWave->vbo);
		glGenBuffers(1, &this->sinWave->ebo);

		glBindVertexArray(this->sinWave->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->sinWave->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->sinWave->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->sinWave->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(0);

		//heightmap
		this->heightMap = new VAO;
		this->heightMap->element_amount = sizeof(element) / sizeof(GLuint);

		glGenVertexArrays(1, &this->heightMap->vao);
		glGenBuffers(2, this->heightMap->vbo);
		glGenBuffers(1, &this->heightMap->ebo);

		glBindVertexArray(this->heightMap->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->heightMap->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->heightMap->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->heightMap->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(0);

		// sinewave
		this->interactive = new VAO;
		this->interactive->element_amount = sizeof(element) / sizeof(GLuint);

		glGenVertexArrays(1, &this->interactive->vao);
		glGenBuffers(2, this->interactive->vbo);
		glGenBuffers(1, &this->interactive->ebo);

		glBindVertexArray(this->interactive->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->interactive->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->interactive->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->interactive->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(0);
	}

	if (!this->billboard)
	{
		GLfloat vertices[] = {
			-1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f
		};
		GLfloat normal[] = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};
		GLfloat texture_coordinate[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
		};
		GLuint element[] = {
			0, 1, 2,
			0, 2, 3,
		};

		this->billboard = new VAO;
		this->billboard->element_amount = sizeof(element) / sizeof(GLuint);
		glGenVertexArrays(1, &this->billboard->vao);
		glGenBuffers(3, this->billboard->vbo);
		glGenBuffers(1, &this->billboard->ebo);

		glBindVertexArray(this->billboard->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->billboard->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->billboard->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		// Texture Coordinate attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->billboard->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->billboard->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(5);
	}

	if (!this->quad)
	{
		GLfloat  vertices[] = {
			-1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f
		};

		this->quad = new VAO;
		glGenVertexArrays(1, &this->quad->vao);
		glGenBuffers(1, this->quad->vbo);

		glBindVertexArray(this->quad->vao);

		glBindBuffer(GL_ARRAY_BUFFER, this->quad->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Unbind VAO
		glBindVertexArray(0);
	}

	if (!this->particle)
	{
		GLfloat vertices[] = {
			-0.5,-0.5,-0.5,
			-0.5,-0.5,0.5,
			0.5,-0.5,0.5,
			0.5,-0.5,-0.5,

			-0.5f,0.5f,-0.5f,
			-0.5f,0.5f,0.5f,
			0.5f,0.5f,0.5f,
			0.5f,0.5f,-0.5f,

			-0.5f,-0.5f,-0.5f,
			-0.5f,0.5f,-0.5f,
			0.5f,0.5f,-0.5f,
			0.5f,-0.5f,-0.5f,

			-0.5f,-0.5f,0.5f,
			-0.5f,0.5f,0.5f,
			0.5f,0.5f,0.5f,
			0.5f,-0.5f,0.5f,

			0.5f,-0.5f,-0.5f,
			0.5f,0.5f,-0.5f,
			0.5f,0.5f,0.5f,
			0.5f,-0.5f,0.5f,

			-0.5f,-0.5f,0.5f,
			-0.5f,0.5f,0.5f,
			-0.5f,0.5f,-0.5f,
			-0.5f,-0.5f,-0.5f,
		};
		GLfloat normal[] = {
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,

			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,

			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,

			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,

			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,

			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
		};
		GLfloat texture_coordinate[] = {
			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,

			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,

			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,

			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,

			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,

			1.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,
		};
		GLuint element[] = {
			// down
			0, 1, 2,
			0, 2, 3,
			//// up
			4, 5, 6,
			4, 6, 7,
			// forward
			8, 9, 10,
			8, 10, 11,
			// backward
			12, 13, 14,
			12, 14, 15,
			// right
			16, 17, 18,
			16, 18, 19,
			// left
			20, 21, 22,
			20, 22, 23 };
		for (unsigned int i = 0; i < MAXPARTICLES; ++i)
		{
			sevenCount = i % 7;
			//fountain.push_back(CreateFountain());
			firework.push_back(CreateFirework(sevenCount, false));
		}
		this->particle = new VAO;
		this->particle->element_amount = sizeof(element) / sizeof(GLuint);
		glGenVertexArrays(1, &this->particle->vao);
		glGenBuffers(3, this->particle->vbo);
		glGenBuffers(1, &this->particle->ebo);

		glBindVertexArray(this->particle->vao);

		// Position attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->particle->vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glBindBuffer(GL_ARRAY_BUFFER, this->particle->vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normal), normal, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, this->particle->vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinate), texture_coordinate, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(2);

		//Element attribute
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->particle->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(element), element, GL_STATIC_DRAW);

		// Unbind VAO
		glBindVertexArray(4);
	}

	if (!uv_buffer)
	{
		uv_buffer = new FBO;
	}
	if (!environment)
	{
		environment = new Environment(w(), h(), waveCenter);
	}
	if (tw->waveBrowser->value() == 1)
	{
		//originally water sine
		environment->setCameraPos(waveCenter);
	}
		
	if (tw->waveBrowser->value() == 3)
	{
		environment->setCameraPos(waveCenter);
		if (!pixelation)
		{
			pixelation = new Pixelation(w(), h());
		}
		renderScreen(pixelation);
	}
	if (tw->waveBrowser->value() == 2)
	{
		drawStuff();
	}
	if (tw->waveBrowser->value() == 1)
	{
		drawGame(false);
	}
	
	renderEnvironmentMap(environment);
	
	drawItems(false, false, false, false);
	
}

void TrainView::
drawItems(bool getEnvironment, bool getScreen, bool onlyDrawWave, bool getWaveFrame) 
{
	if (!getEnvironment)
	{
		// setting camera position
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		glm::mat4 modelViewMat4(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7], matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15]);
		modelViewMat4 = glm::inverse(modelViewMat4);
		cameraPos = glm::vec3(modelViewMat4[3][0], modelViewMat4[3][1], modelViewMat4[3][2]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&origin_view_matrix[0][0]);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&origin_projection_matrix[0][0]);
	}

	glm::mat4 view_matrix;

	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// directional light
	glm::vec3 dirLight_direction(1.0f, -1.0f, 1.0f);
	glm::vec3 dirLight_ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 dirLight_diffuse(0.9f, 0.9f, 0.9f);
	glm::vec3 dirLight_specular(1.0f, 1.0f, 1.0f);

	//Draw Tilebox
	if (!onlyDrawWave && tw->waveBrowser->value() == 1)
	{
		tileboxShader->Use();
		glBindVertexArray(this->tilebox->vao);
		glActiveTexture(GL_TEXTURE0 + 1);
		tileboxShader->setInt("tiles", 1);
		tileboxShader->setVec3("viewPos", cameraPos);
		glUniformMatrix4fv(glGetUniformLocation(this->tileboxShader->Program, "u_model"), 1, GL_FALSE, &glm::mat4()[0][0]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tileCubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_TRIANGLES, 30, 6); // ignore top
		glBindVertexArray(0);
	}

	if (!onlyDrawWave && tw->waveBrowser->value() == 3)
	{
		tboxShader->Use();
		glBindVertexArray(this->tbox->vao);
		glActiveTexture(GL_TEXTURE0 + 1);
		tboxShader->setInt("tiles", 1);
		tboxShader->setVec3("viewPos", cameraPos);
		glUniformMatrix4fv(glGetUniformLocation(this->tboxShader->Program, "u_model"), 1, GL_FALSE, &glm::mat4()[0][0]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tCubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 24);
		glDrawArrays(GL_TRIANGLES, 30, 6); // ignore top
		glBindVertexArray(0);
	}

	//Draw Cube
	if (tw->waveBrowser->value() == 3)
	{
		//bind shader
		this->dotCubeShader->Use();
		glm::mat4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, this->worldCenter + glm::vec3(0, 100.0f, 0));
		model_matrix = glm::rotate(model_matrix, 30.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		model_matrix = glm::translate(model_matrix, -this->worldCenter);
		model_matrix = glm::scale(model_matrix, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniformMatrix4fv(glGetUniformLocation(this->dotCubeShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
		glUniform3fv(glGetUniformLocation(this->dotCubeShader->Program, "u_color"), 1, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);
		this->dotCubeTexture->bind(4);
		dotCubeShader->setInt("u_texture", 4); //ID 4
		dotCubeShader->setVec3("viewPos", cameraPos);
		dotCubeShader->setInt("material.diffuse", 4); //ID 4
		dotCubeShader->setInt("material.specular", 4); //ID 4
		dotCubeShader->setFloat("material.shininess", 10320.0f);
		dotCubeShader->setVec3("light.position", glm::vec3(-50.0f, 100.0f, 10.0f));
		dotCubeShader->setVec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		dotCubeShader->setVec3("light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		dotCubeShader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		if (tw->GouraudShading->value()) dotCubeShader->setInt("shading", 0);
		else if (tw->PhongShading->value()) dotCubeShader->setInt("shading", 1);
		else dotCubeShader->setInt("shading", 2);
		// simple cube
		dotCubeShader->setVec3("dirLight.direction", dirLight_direction);
		dotCubeShader->setVec3("dirLight.ambient", dirLight_ambient);
		dotCubeShader->setVec3("dirLight.diffuse", dirLight_diffuse);
		dotCubeShader->setVec3("dirLight.specular", dirLight_specular);
		// point light 1~4
		for (int i = 0; i < 4; i++) //4 of point lights
		{
			
			dotCubeShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
			dotCubeShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.0f);
			dotCubeShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0f);
		}
		// spotlight
		dotCubeShader->setFloat("spotLight.constant", 0.01f);
		dotCubeShader->setFloat("spotLight.linear", 0.0f);
		dotCubeShader->setFloat("spotLight.quadratic", 0.0f);
		dotCubeShader->setFloat("spotLight.cutOff", cos(glm::radians(12.5f)));
		dotCubeShader->setFloat("spotLight.outerCutOff", cos(glm::radians(15.0f)));

		//bind VAO
		glBindVertexArray(this->dotCube->vao);

		glDrawElements(GL_TRIANGLES, this->dotCube->element_amount, GL_UNSIGNED_INT, 0);

		//unbind VAO
		glBindVertexArray(0);

		//unbind shader(switch to fixed pipeline)
		glUseProgram(0);
	}
	
	//draw skybox
	if (!onlyDrawWave)
	{
		glm::mat4 skybox_view_matrix;
		glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
		skybox_view_matrix = glm::mat4(glm::mat3(view_matrix));
		glm::mat4 skybox_projection_matrix;
		glGetFloatv(GL_PROJECTION_MATRIX, &skybox_projection_matrix[0][0]);

		glBindBuffer(GL_UNIFORM_BUFFER, this->skybox_matrices->ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &skybox_projection_matrix[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &skybox_view_matrix[0][0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader->Use();
		skyboxShader->setInt("skybox", 0);
		// skybox cube
		glBindVertexArray(this->skybox->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
	}

	//draw sinWave
	if (tw->waveBrowser->value() == 1 && !getEnvironment)
	{
		this->interactiveShader->Use();
		this->concreteTexture->bind(0);
		interactiveShader->setInt("u_texture", 0); //ID 0
		heightMaps[heightMapIndex].bind(1);
		interactiveShader->setInt("heightMap", 1); //ID heightMapIndex(1~200)
		interactiveShader->setInt("skybox", 0); //ID 0
		interactiveShader->setInt("environmentbox", 2); //ID 2
		interactiveShader->setVec3("environmentCenter", environment->getCameraPos());
		interactiveShader->setVec3("bboxMax", glm::vec3(100.0, 100.0, 100.0));
		interactiveShader->setVec3("bboxMin", glm::vec3(-100.0, -100.0, -100.0));
		glm::mat4 waves_model_matrix = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(this->interactiveShader->Program, "u_model"), 1, GL_FALSE, &waves_model_matrix[0][0]);
		interactiveShader->setVec2("u_delta", glm::vec2(1.0f / heightMaps[heightMapIndex].size.x, 1.0f / heightMaps[heightMapIndex].size.y));
		interactiveShader->setVec3("viewPos", cameraPos);
		interactiveShader->setFloat("amplitude", 1);
		//interactiveShader->setVec2("dropCenter", uv1);
		interactiveShader->setFloat("uv_t", interactTime);
		interactiveShader->setFloat("t", waveTime / 20.0);
		interactiveShader->setInt("material.diffuse", 0); //ID 0
		interactiveShader->setInt("material.specular", 0); //ID 0
		interactiveShader->setFloat("material.shininess", 32.0f);

		// directional light
		interactiveShader->setVec3("dirLight.direction", dirLight_direction);
		interactiveShader->setVec3("dirLight.ambient", dirLight_ambient);
		interactiveShader->setVec3("dirLight.diffuse", dirLight_diffuse);
		interactiveShader->setVec3("dirLight.specular", dirLight_specular);
		// point light 1~4
		for (int i = 0; i < 4; i++) //4 of point lights
		{
			
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.0f);
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0f);
		}
		// spotLight
		
		interactiveShader->setFloat("spotLight.constant", 0.01f);
		interactiveShader->setFloat("spotLight.linear", 0.0f);
		interactiveShader->setFloat("spotLight.quadratic", 0.0f);
		interactiveShader->setFloat("spotLight.cutOff", cos(glm::radians(12.5f)));
		interactiveShader->setFloat("spotLight.outerCutOff", cos(glm::radians(15.0f)));

		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.diffuse"), 1, &glm::vec3(1.0f, 5.0f, 0.31f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.specular"), 1, &glm::vec3(5.0f, 5.0f, 5.0f)[0]);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "material.shininess"), 2.0f);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.diffuse"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.specular"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.direction"), 1, &glm::vec3(0, -1, 0)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.position"), 1, &glm::vec3(0, 200, 0)[0]);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.cutOff"), glm::cos(glm::radians(30.0f)));

		glBindVertexArray(this->interactive->vao);

		glDrawElements(GL_TRIANGLES, this->interactive->element_amount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);


	}
	
	if (tw->waveBrowser->value() == 3 && !getEnvironment)
	{
		glm::mat4 waves_model_matrix = glm::mat4();//sine wave
		this->sinWaveShader->Use();
		this->waterTexture->bind(0);
		sinWaveShader->setInt("u_texture", 0); //ID 0
		environment->bind(2);
		sinWaveShader->setInt("skybox", 0); //ID 0
		sinWaveShader->setInt("environmentbox", 2); //ID 2
		sinWaveShader->setVec3("environmentCenter", environment->getCameraPos());
		sinWaveShader->setVec3("bboxMax", glm::vec3(100.0, 100.0 + environment->getCameraPos().y, 100.0));
		sinWaveShader->setVec3("bboxMin", glm::vec3(-100.0, -100.0 + environment->getCameraPos().y, -100.0));
		sinWaveShader->setFloat("steepness", 6.3 / 10);//1.4
		sinWaveShader->setFloat("wavelength", 35.0f);//35
		sinWaveShader->setFloat("ratio", tw->refRatio->value());
		sinWaveShader->setInt("time", waveTime);
		sinWaveShader->setInt("reflection", 0);
		sinWaveShader->setInt("cartoon", tw->toonShading->value());
		sinWaveShader->setInt("showRleRra", 3);
		sinWaveShader->setVec3("waveDir", glm::vec3(1.0f, 0.0f, 0.0f));
		waves_model_matrix = glm::translate(waves_model_matrix, waveCenter);
		glUniformMatrix4fv(glGetUniformLocation(this->sinWaveShader->Program, "u_model"), 1, GL_FALSE, &waves_model_matrix[0][0]);
		sinWaveShader->setVec3("viewPos", cameraPos);

		sinWaveShader->setInt("material.diffuse", 0); //ID 0
		sinWaveShader->setInt("material.specular", 0); //ID 0
		sinWaveShader->setFloat("material.shininess", 32.0f);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "material.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "material.diffuse"), 1, &glm::vec3(1.0f, 5.0f, 0.31f)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "material.specular"), 1, &glm::vec3(5.0f, 5.0f, 5.0f)[0]);
		glUniform1f(glGetUniformLocation(sinWaveShader->Program, "material.shininess"), 2.0f);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "light.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "light.diffuse"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "light.specular"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "light.direction"), 1, &glm::vec3(0, -1, 0)[0]);
		glUniform3fv(glGetUniformLocation(sinWaveShader->Program, "light.position"), 1, &glm::vec3(0, 200, 0)[0]);
		glUniform1f(glGetUniformLocation(sinWaveShader->Program, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(sinWaveShader->Program, "light.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(sinWaveShader->Program, "light.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(sinWaveShader->Program, "light.cutOff"), glm::cos(glm::radians(30.0f)));
		glBindVertexArray(this->sinWave->vao);
		glDrawElements(GL_TRIANGLES, this->sinWave->element_amount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	if (tw->waveBrowser->value() == 2 && !getEnvironment)
	{
		this->interactiveShader->Use();
		this->uvTexture->bind(0);
		interactiveShader->setInt("u_texture", 0); //ID 0
		heightMaps[heightMapIndex].bind(1);
		interactiveShader->setInt("heightMap", 1); //ID heightMapIndex(1~200)
		interactiveShader->setInt("skybox", 0); //ID 0
		interactiveShader->setInt("environmentbox", 2); //ID 2
		interactiveShader->setVec3("environmentCenter", environment->getCameraPos());
		interactiveShader->setVec3("bboxMax", glm::vec3(100.0, 100.0, 100.0));
		interactiveShader->setVec3("bboxMin", glm::vec3(-100.0, -100.0, -100.0));
		glm::mat4 waves_model_matrix = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(this->interactiveShader->Program, "u_model"), 1, GL_FALSE, &waves_model_matrix[0][0]);
		interactiveShader->setVec2("u_delta", glm::vec2(1.0f / heightMaps[heightMapIndex].size.x, 1.0f / heightMaps[heightMapIndex].size.y));
		interactiveShader->setVec3("viewPos", cameraPos);
		interactiveShader->setFloat("amplitude", 1);
		//interactiveShader->setVec2("dropCenter", uv1);
		interactiveShader->setFloat("uv_t", interactTime);
		interactiveShader->setFloat("t", waveTime / 20.0);
		interactiveShader->setInt("material.diffuse", 0); //ID 0
		interactiveShader->setInt("material.specular", 0); //ID 0
		interactiveShader->setFloat("material.shininess", 32.0f);

		// directional light
		interactiveShader->setVec3("dirLight.direction", dirLight_direction);
		interactiveShader->setVec3("dirLight.ambient", dirLight_ambient);
		interactiveShader->setVec3("dirLight.diffuse", dirLight_diffuse);
		interactiveShader->setVec3("dirLight.specular", dirLight_specular);
		// point light 1~4
		for (int i = 0; i < 4; i++) //4 of point lights
		{
			
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.0f);
			interactiveShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.0f);
		}
		// spotLight
		interactiveShader->setFloat("spotLight.constant", 0.01f);
		interactiveShader->setFloat("spotLight.linear", 0.0f);
		interactiveShader->setFloat("spotLight.quadratic", 0.0f);
		interactiveShader->setFloat("spotLight.cutOff", cos(glm::radians(12.5f)));
		interactiveShader->setFloat("spotLight.outerCutOff", cos(glm::radians(15.0f)));

		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.diffuse"), 1, &glm::vec3(1.0f, 5.0f, 0.31f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "material.specular"), 1, &glm::vec3(5.0f, 5.0f, 5.0f)[0]);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "material.shininess"), 2.0f);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.ambient"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.diffuse"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.specular"), 1, &glm::vec3(1.0f, 1.0f, 1.0f)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.direction"), 1, &glm::vec3(0, -1, 0)[0]);
		glUniform3fv(glGetUniformLocation(interactiveShader->Program, "light.position"), 1, &glm::vec3(0, 200, 0)[0]);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(interactiveShader->Program, "light.cutOff"), glm::cos(glm::radians(30.0f)));

		glBindVertexArray(this->interactive->vao);

		glDrawElements(GL_TRIANGLES, this->interactive->element_amount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	if (!getScreen && !getEnvironment && !onlyDrawWave && tw->waveBrowser->value() == 3)
	{
		this->pixelationShader->Use();
		pixelation->bind(5);
		pixelationShader->setInt("screen", 5); //ID 0
		pixelationShader->setFloat("time", waveTime);
		pixelationShader->setFloat("param", tw->shaderParam->value());
		pixelationShader->setVec2("resolution", glm::vec2(1,1));
		if (tw->noShading->value() == 1)
		{
			pixelationShader->setFloat("offset", 0);
		}
		else
		{
			pixelationShader->setFloat("offset", tw->offset->value());
		}
		pixelationShader->setInt("isOffset", tw->shBrowser->value());
		pixelationShader->setVec2("size", glm::vec2(w(), h()));

		glBindVertexArray(this->quad->vao);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
	}

	if (tw->waveBrowser->value() == 1 && !getEnvironment)
	{
		this->billboardShader->Use();

		glm::mat4 model_matrix = glm::mat4();
		//model_matrix = glm::rotate(model_matrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		
		model_matrix = glm::scale(model_matrix, glm::vec3(50.0f, 50.0f, 50.0f));
		//model_matrix = glm::rotate(model_matrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(this->billboardShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
		this->billboardTexture->bind(5);
		glUniform1i(glGetUniformLocation(this->billboardShader->Program, "u_texture"), 5);
		glUniform1f(glGetUniformLocation(this->billboardShader->Program, "u_trainU"), m_pTrack->trainU);
		//bind VAO
		glBindVertexArray(this->billboard->vao);

		glDrawElements(GL_TRIANGLES, this->billboard->element_amount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(5);
		glUseProgram(0);

		updateFirework();
		for (int i = 0; i < firework.size(); i++)
		{
			this->particleShader->Use();
			model_matrix = glm::mat4();
			glm::vec3 tran = glm::vec3(firework[i].position.x, 0 + firework[i].position.y, 0 + firework[i].position.z);
			model_matrix = glm::translate(model_matrix, tran);
			glm::vec3 scale;
			if (firework[i].life <= 30) {
				scale = glm::vec3(3 * firework[i].life / LIFE, 3 * firework[i].life / LIFE, 3 * firework[i].life / LIFE);
			}
			else {
				scale = glm::vec3(1.0, 1.0, 1.0);
			}
			model_matrix = glm::scale(model_matrix, scale);
			glUniformMatrix4fv(glGetUniformLocation(this->particleShader->Program, "u_model"), 1, GL_FALSE, &model_matrix[0][0]);
			if (color == 0)
			{
				glUniform4fv(glGetUniformLocation(this->particleShader->Program, "u_color"), 1, &glm::vec4(1, (rand() % 70) / 100.0f, (rand() % 70) / 100.0f, (float)firework[i].life / 50.0f)[0]);
			}
			else if (color == 1)
			{
				glUniform4fv(glGetUniformLocation(this->particleShader->Program, "u_color"), 1, &glm::vec4((rand() % 70) / 100.0f, 1, (rand() % 70) / 100.0f, (float)firework[i].life / 50.0f)[0]);
			}
			else if (color == 2)
			{
				glUniform4fv(glGetUniformLocation(this->particleShader->Program, "u_color"), 1, &glm::vec4((rand() % 70) / 100.0f, (rand() % 70) / 100.0f, 1, (float)firework[i].life / 50.0f)[0]);
			}
			glUniform1i(glGetUniformLocation(this->particleShader->Program, "u_type"), 1);
			//bind VAO
			glBindVertexArray(this->particle->vao);
			glDrawElements(GL_TRIANGLES, this->particle->element_amount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(4);
			glUseProgram(0);
		}
	}
	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);
}

GLuint TrainView::
loadCubemap(std::vector<std::string> skybox_textures)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height;
	unsigned char* image;
	for (GLuint i = 0; i < skybox_textures.size(); i++)
	{
		image = stbi_load(skybox_textures[i].c_str(), &width, &height, 0, 0);
		if (image)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			stbi_image_free(image);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << skybox_textures[i] << std::endl;
			stbi_image_free(image);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void TrainView::
renderEnvironmentMap(Environment* environment)
{
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, environment->getSize().x, environment->getSize().y); //GL_DEPTH_COMPONENT24
	// attach it
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glViewport(0, 0, environment->getSize().x, environment->getSize().y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, .1, 1000);
	glm::vec3 cameraPosT = cameraPos;
	cameraPos = environment->getCameraPos();
	
	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment->getID(), 0); //+i

		// clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		environment->switchToFace(i);

		drawItems(true, false);
	}

	cameraPos = cameraPosT;

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, w(), h());

	arcball.setProjection(false);

	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteFramebuffers(1, &FBO);
}

void TrainView::
renderScreen(Pixelation* pixelation)
{
	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w(), h()); //GL_DEPTH_COMPONENT24
	// attach it
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pixelation->getID(), 0);

	// clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawItems(false, true); //, false

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteFramebuffers(1, &FBO);

}

void TrainView::
pick(int x, int y)
{
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, uv_buffer->fbo);
	
	//glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &uv[0]);

	//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	
	glReadPixels(Fl::event_x(), h() - Fl::event_y(), 1, 1, GL_RGB, GL_FLOAT, &uv[0]);

	//std::cout << uv[0] << ' ' << uv[1] << ' ' << uv[2] << " " << (uv[0] / 4 * 10 - uv[1] / 7 * 10) << " " << (uv[0] / 4 * 10 - uv[2]) << ' ' << (uv[2] - uv[1] / 7 * 10) << std::endl;

	if (tw->waveBrowser->value() == 2)
	{
		if (((uv[0] - uv[1])<0.00001f && (uv[0] - uv[1]) > -0.00001f)&& ((uv[0] - uv[2]) < 0.00001f && (uv[0] - uv[2]) > -0.00001f)&& ((uv[2] - uv[1]) < 0.00001f && (uv[2] - uv[1]) > -0.00001f))
		{
			tw->waveBrowser->select(1);
		}
		else if(uv[2] > uv[1] && uv[2] > uv[0])
		{
			tw->waveBrowser->select(3);
		}
	}

	if (tw->waveBrowser->value() == 1)
	{
		if (uv[2] > uv[1] && uv[2] > uv[0] && color == 2)
		{
			correct = true;
		}
		if (uv[2] > uv[1] && uv[2] > uv[0] && color != 2 && correct == false)
		{
			firework.clear();
			drawGame(true);
		}
		
		if (uv[1] > uv[0] && uv[1] > uv[2] && color == 1)
		{
			correct = true;
		}
		if (uv[1] > uv[0] && uv[1] > uv[2] && color != 1 && correct == false)
		{
			firework.clear();
			drawGame(true);
		}
		if (uv[0] > uv[1] && uv[0] > uv[2] && color == 0)
		{
			correct = true;
		}
		if (uv[0] > uv[1] && uv[0] > uv[2] && color != 0 && correct == false)
		{
			firework.clear();
			drawGame(true);
		}
		
	}
	
	glDeleteBuffers(1, &uv_buffer->fbo);
}


//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	else if (tw->trainCam->value())
	{
		glm::vec3 currpos = pointPos[pos] + pointOrientation[pos] * 5.0f;
		glm::vec3 forward = pointPos[(pos + 1) % pointPos.size()] + pointOrientation[pos] * 5.0f;
		glm::vec3 up = pointOrientation[pos];
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, aspect, 0.01, 200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(currpos.x, currpos.y, currpos.z
			, forward.x, forward.y, forward.z
			, up.x, up.y, up.z);
		//std::cout << currpos.x << " " << currpos.y << " " << currpos.z << " " << forward.x << " " << forward.y << " " << forward.z << " " << up.x << " " << up.y << " " << up.z << std::endl;
	}
	if (tw->waveBrowser->value() == 1)
	{
		/*arcball.now.x = -0.01;
		arcball.now.y = -0.005;
		arcball.now.z = -0.005;
		arcball.now.w = 1.0f;
		arcball.fieldOfView = 40.0f;*/

	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)

	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			if (!tw->detail->value())
			{
				m_pTrack->points[i].draw(false, tw->subdivision->value());
			}
			else
			{
				m_pTrack->points[i].draw(false, arcball.zoom - 10);
			}
			
			if (!doingShadows) {
				glColor3ub(0, 0, 0);
			}
			if (!tw->detail->value())
			{
				m_pTrack->points[i].draw(true, tw->subdivision->value());
			}
			else
			{
				m_pTrack->points[i].draw(true, arcball.zoom - 10);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				glColor3ub(0, 0, 0);
			}
			if (!tw->detail->value())
			{
				m_pTrack->points[i].draw(true, tw->subdivision->value());
			}
			else
			{
				m_pTrack->points[i].draw(true, arcball.zoom - 10);
			}
			
		}
	}



	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	drawCardinal(doingShadows);
	drawFerris(doingShadows);
	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################

	if (!tw->trainCam->value())
	{
		calcdrawTrain(this, doingShadows, true);
		calcdrawCarriage(this, doingShadows, true);
	}
	else
	{
		calcdrawTrain(this, doingShadows, false);
		calcdrawCarriage(this, doingShadows, true);
	}

	drawTree(Pnt3f(40, 80, 0), 1, Pnt3f(107.0 / 255, 142.0 / 255, 35.0 / 255));
	drawTree(Pnt3f(60, 10, 0), 0.5, Pnt3f(50.0 / 255, 205.0 / 255, 50.0 / 255));
	drawTree(Pnt3f(-50, -80, 0), 0.75, Pnt3f(154.0 / 255, 205.0 / 255, 50.0 / 255));
	drawTree(Pnt3f(-40, 35, 0), 1, Pnt3f(50.0 / 255, 205.0 / 255, 50.0 / 255));
	drawTree(Pnt3f(20, 60, 0), 0.5, Pnt3f(154.0 / 255, 205.0 / 255, 50.0 / 255));
	drawTree(Pnt3f(-50, -55, 0), 0.75, Pnt3f(85.0 / 255, 107.0 / 255, 47.0 / 255));
	drawTree(Pnt3f(30, -60, 0), 0.5, Pnt3f(50.0 / 255, 205.0 / 255, 50.0 / 255));
	drawTree(Pnt3f(-75, 20, 0), 0.75, Pnt3f(107.0 / 255, 142.0 / 255, 35.0 / 255));
	drawFence(tw->fenceParam->value());
}

void TrainView::drawCardinal(bool doingShadows)
{
	pointPos.clear();
	pointOrientation.clear();
	pointLength.clear();
	float culDis = 0;
	bool dw = false;
	//set up cardinal cubic matrix
	float cardinalMatrix[16] =
	{
		-0.5f, 1.5f, -1.5f, 0.5f,
		1.0f, -2.5f, 2.0f, -0.5f,
		-0.5f, .0f, 0.5f, .0f,
		.0f, 1.0f, .0f, .0f
	};
	for (int i = 0; i < 4; ++i)
	{
		//Ctrl point pos
		Pnt3f cp_pos_p1 = m_pTrack->points[i].pos;
		Pnt3f cp_pos_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		Pnt3f cp_pos_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].pos;
		Pnt3f cp_pos_p4 = m_pTrack->points[(i + 3) % m_pTrack->points.size()].pos;
		//ctrl point orientation
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p4 = m_pTrack->points[(i + 3) % m_pTrack->points.size()].orient;

		//dummy ctrl point orientation for tunnel
		Pnt3f cp_orient_p1_t = Pnt3f(.0f, 1.0f, .0f);
		Pnt3f cp_orient_p2_t = Pnt3f(.0f, 1.0f, .0f);
		Pnt3f cp_orient_p3_t = Pnt3f(.0f, 1.0f, .0f);
		Pnt3f cp_orient_p4_t = Pnt3f(.0f, 1.0f, .0f);

		Pnt3f posVec[4];
		Pnt3f oriVec[4];
		Pnt3f oriVec_t[4];

		//calculate position
		for (int i = 0; i < 4; i++)
		{
			posVec[i] = cp_pos_p1 * cardinalMatrix[i * 4] + cp_pos_p2 * cardinalMatrix[i * 4 + 1] + cp_pos_p3 * cardinalMatrix[i * 4 + 2] + cp_pos_p4 * cardinalMatrix[i * 4 + 3];
		}
		//calculate orient
		for (int i = 0; i < 4; i++)
		{
			oriVec[i] = cp_orient_p1 * cardinalMatrix[i * 4] + cp_orient_p2 * cardinalMatrix[i * 4 + 1] + cp_orient_p3 * cardinalMatrix[i * 4 + 2] + cp_orient_p4 * cardinalMatrix[i * 4 + 3];
		}
		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt = posVec[0] * t * t * t + posVec[1] * t * t + posVec[2] * t + posVec[3];
		for (size_t j = 0; j < DIVIDE_LINE; j++)
		{
			dw = false;
			if (j == 0 && i == 0)
			{
				dw = true;
			}
			Pnt3f qt0 = qt;
			t += percent;
			//calculate curPos
			qt = posVec[0] * t * t * t + posVec[1] * t * t + posVec[2] * t + posVec[3];
			Pnt3f qt1 = qt;
			//calculate nxtPos
			Pnt3f qt2 = posVec[0] * (t + percent / 2) * (t + percent / 2) * (t + percent / 2) + posVec[1] * (t + percent / 2) * (t + percent / 2) + posVec[2] * (t + percent / 2) + posVec[3];

			//calculate current orientation
			Pnt3f orient_t = oriVec[0] * t * t * t + oriVec[1] * t * t + oriVec[2] * t + oriVec[3];
			orient_t.normalize();
			Pnt3f cross_t = (qt2 - qt0) * orient_t;

			//calculate distortion of track
			cross_t.normalize();
			cross_t = cross_t * 2.5f;
			pointPos.push_back(glm::vec3(qt0.x, qt0.y, qt0.z));
			pointOrientation.push_back(glm::vec3(orient_t.x, orient_t.y, orient_t.z));
			pointLength.push_back(sqrt((qt2 - qt0).x * (qt2 - qt0).x + (qt2 - qt0).y * (qt2 - qt0).y + (qt2 - qt0).z * (qt2 - qt0).z));
			culDis += sqrt((qt2 - qt0).x * (qt2 - qt0).x + (qt2 - qt0).y * (qt2 - qt0).y + (qt2 - qt0).z * (qt2 - qt0).z);
			if (culDis >= 2.5)
			{
				dw = true;
				culDis = 0;
			}

			if (!doingShadows)
			{
				glColor3ub(32, 32, 64);
			}
			glBegin(GL_LINES);
			glVertex3f(qt0.x + cross_t.x, qt0.y + cross_t.y, qt0.z + cross_t.z);
			glVertex3f(qt2.x + cross_t.x, qt2.y + cross_t.y, qt2.z + cross_t.z);
			glVertex3f(qt0.x - cross_t.x, qt0.y - cross_t.y, qt0.z - cross_t.z);
			glVertex3f(qt2.x - cross_t.x, qt2.y - cross_t.y, qt2.z - cross_t.z);
			glEnd();
			glLineWidth(5);
			if (j % 2 == 1)
			{
				glBegin(GL_QUADS);
				if (!doingShadows)
					glColor3f(0.45, 0.22, 0.0f);
				glVertex3f(qt0.x + 1.5 * cross_t.x, qt0.y + 1.5 * cross_t.y, qt0.z + 1.5 * cross_t.z);
				glVertex3f(qt2.x + 1.5 * cross_t.x, qt2.y + 1.5 * cross_t.y, qt2.z + 1.5 * cross_t.z);
				glVertex3f(qt2.x - 1.5 * cross_t.x, qt2.y - 1.5 * cross_t.y, qt2.z - 1.5 * cross_t.z);
				glVertex3f(qt0.x - 1.5 * cross_t.x, qt0.y - 1.5 * cross_t.y, qt0.z - 1.5 * cross_t.z);
				glEnd();
			}
		}
	}
}

void TrainView::calcdrawTrain(TrainView* tar, bool doingshadows, bool drawTrain)
{
	vector<float> track;
	for (int i = 0; i < floor(tw->trn->value()); i++) {
		//determinePosition

		pos = (int)(waveTime + i * 17) % pointPos.size();
		//draw car
		if (drawTrain)
		{
			bool raise = false;
			if (pos != 0)
			{
				if (pointPos[pos - 1].y - pointPos[pos].y > 0)
				{
					raise = true;
				}
			}
			else
			{
				if (pointPos[pointPos.size() - 1].y - pointPos[pos].y > 0)
				{
					raise = true;
				}
			}
			Pnt3f qt = Pnt3f(pointPos[pos].x, pointPos[pos].y, pointPos[pos].z);
			
			Pnt3f orient_t = Pnt3f(pointOrientation[pos].x, pointOrientation[pos].y, pointOrientation[pos].z);
			glm::vec3 f = pointPos[(pos + 1) % pointPos.size()] - pointPos[pos];
			Pnt3f face = Pnt3f(f.x, f.y, f.z);
			face.normalize();
			Pnt3f w = face * orient_t;
			w.normalize();
			Pnt3f v = w * face;
			v.normalize();

			float rotation[16] = {
				face.x,face.y,face.z,0.0,
				v.x,v.y,v.z,0.0,
				w.x,w.y,w.z,0.0,
				0.0,0.0,0.0,1.0
			};


			glPushMatrix();
			glTranslatef(qt.x, qt.y, qt.z);
			glMultMatrixf(rotation);
			glScalef(7.0f, 7.0f, 7.0f);
			glTranslatef(-0.5f, 0.0f, -0.5f);

			int faces[6][4] =
			{
				{0,1,2,3},
				{7,6,5,4},
				{0,4,5,1},
				{1,5,6,2},
				{3,2,6,7},
				{0,3,7,4}
			};

			//draw the car
			float color[] = { .0f, 0.5f, 1.0f, 1.0f };
			float frontColor[] = { 1.0f, 0.0f, .0f, 1.0f };
			float points[8][3] =
			{
				{-0.2,0.0,0.}, {1.2,0.0,0.0},
				{1.2,0.0,1.0}, {-0.2,0.0,1.0},
				{-0.2,1.0,0.0}, {1.2,1.0,0.0},
				{1.2,1.0,1.0}, {-0.2,1.0,1.0},
			};
			for (int j = 0; j < 6; j++) {
				switch (j) {
				case 0:
					glNormal3f(0.0, -1.0, 0.0);
					break;
				case 1:
					glNormal3f(0.0, 1.0, 0.0);
					break;
				case 2:
					glNormal3f(1.0, 0.0, 0.0);
					break;
				case 3:
					glNormal3f(0.0, 0.0, 1.0);
					break;
				case 4:
					glNormal3f(-1.0, 0.0, 0.0);
					break;
				case 5:
					glNormal3f(0.0, 0.0, -1.0);
					break;

				}
				if (!doingshadows)
				{
					if (j == 3)
					{
						glColor3fv(frontColor);
					}
					else
					{
						glColor3fv(color);
					}
				}
				glBegin(GL_QUADS);
				glVertex3fv(points[faces[j][0]]);
				glVertex3fv(points[faces[j][1]]);
				glVertex3fv(points[faces[j][2]]);
				glVertex3fv(points[faces[j][3]]);
				glEnd();
			}

			//draw people
			float headColor[] = { .0f, .0f, .0f, .0f };
			float handColor[] = { .0f, 1.0f, .0f, 1.0f };
			float pointsRHand[8][3] =
			{
				{0.45,1.25,0.75}, {0.45,1.25,0.85},
				{0.55,1.25,0.85}, {0.55,1.25,0.75},
				{0.45,1.75,0.75}, {0.45,1.75,0.85},
				{0.55,1.75,0.85}, {0.55,1.75,0.75},
			};
			float pointsLHand[8][3] =
			{
				{0.45,1.25,0.25}, {0.45,1.25,0.25},
				{0.55,1.25,0.15}, {0.55,1.25,0.15},
				{0.45,1.75,0.25}, {0.45,1.75,0.25},
				{0.55,1.75,0.15}, {0.55,1.75,0.15},
			};
			float pointsHead[8][3] =
			{
				{0.25,1.0,0.25}, {0.75,1.0,0.25},
				{0.75,1.0,0.75}, {0.25,1.0,0.75},
				{0.25,1.5,0.25}, {0.75,1.5,0.25},
				{0.75,1.5,0.75}, {0.25,1.5,0.75}
			};
			float pointsHands[8][3] =
			{
				{0.45,1.20,-0.25}, {0.55,1.20,-0.25},
				{0.55,1.20,1.25}, {0.45,1.20,1.25},
				{0.45,1.30,-0.25}, {0.55,1.30,-0.25},
				{0.55,1.30,1.25}, {0.45,1.30,1.25}
			};
			for (int j = 0; j < 6; j++) {
				switch (j) {
				case 0:
					glNormal3f(0.0, -1.0, 0.0);
					break;
				case 1:
					glNormal3f(0.0, 1.0, 0.0);
					break;
				case 2:
					glNormal3f(1.0, 0.0, 0.0);
					break;
				case 3:
					glNormal3f(0.0, 0.0, 1.0);
					break;
				case 4:
					glNormal3f(-1.0, 0.0, 0.0);
					break;
				case 5:
					glNormal3f(0.0, 0.0, -1.0);
					break;

				}
				if (!doingshadows)
				{
					glColor3fv(headColor);
				}
				glBegin(GL_QUADS);
				glVertex3fv(pointsHead[faces[j][0]]);
				glVertex3fv(pointsHead[faces[j][1]]);
				glVertex3fv(pointsHead[faces[j][2]]);
				glVertex3fv(pointsHead[faces[j][3]]);
				glEnd();

				if (!doingshadows)
				{
					glColor3fv(handColor);
				}
				if (raise == true)
				{
					glBegin(GL_QUADS);
					glVertex3fv(pointsRHand[faces[j][0]]);
					glVertex3fv(pointsRHand[faces[j][1]]);
					glVertex3fv(pointsRHand[faces[j][2]]);
					glVertex3fv(pointsRHand[faces[j][3]]);

					glVertex3fv(pointsLHand[faces[j][0]]);
					glVertex3fv(pointsLHand[faces[j][1]]);
					glVertex3fv(pointsLHand[faces[j][2]]);
					glVertex3fv(pointsLHand[faces[j][3]]);
					glEnd();
				}
				else
				{
					glBegin(GL_QUADS);
					glVertex3fv(pointsHands[faces[j][0]]);
					glVertex3fv(pointsHands[faces[j][1]]);
					glVertex3fv(pointsHands[faces[j][2]]);
					glVertex3fv(pointsHands[faces[j][3]]);
					glEnd();
				}

			}


			//draw wheels
			float r = 0.2;

			glNormal3f(1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);
			for (int j = 0; j < 15; j++) {
				if (!doingshadows)
					glColor3ub(0, 0, 0);
				float theta = 2.0f * 3.1415926f * float(j) / 15.0f;
				float x = r * cosf(theta);
				float y = r * sinf(theta);
				glVertex3f(0.0 + x, 0.0 + y, -0.02);
			}
			glEnd();
			glBegin(GL_POLYGON);
			for (int j = 0; j < 15; j++) {
				if (!doingshadows)
					glColor3ub(0, 0, 0);
				float theta = 2.0f * 3.1415926f * float(j) / 15.0f;
				float x = r * cosf(theta);
				float y = r * sinf(theta);
				glVertex3f(1.0 + x, 0.0 + y, -0.02);
			}
			glEnd();

			glNormal3f(-1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);
			for (int j = 0; j < 15; j++) {
				if (!doingshadows)
					glColor3ub(0, 0, 0);
				float theta = 2.0f * 3.1415926f * float(j) / 15.0f;
				float x = r * cosf(theta);
				float y = r * sinf(theta);
				glVertex3f(0.0 + x, 0.0 + y, 1.02);
			}
			glEnd();

			glBegin(GL_POLYGON);
			for (int j = 0; j < 15; j++) {
				if (!doingshadows)
					glColor3ub(0, 0, 0);
				float theta = 2.0f * 3.1415926f * float(j) / 15.0f;
				float x = r * cosf(theta);
				float y = r * sinf(theta);
				glVertex3f(1.0 + x, 0.0 + y, 1.02);
			}
			glEnd();
			glPopMatrix();
		}
	}
}

void TrainView::drawFerris(bool doingShadows)
{
	pointPosF.clear();
	pointOrientationF.clear();
	pointLengthF.clear();
	float culDis = 0;
	bool dw = false;
	//set up cardinal cubic matrix
	float BSplineMatrix[16] =
	{
		-0.166f, 0.5f, -0.5f, 0.166666f,
		0.5f, -1.0f, 0.5f, .0f,
		-0.5f, .0f, 0.5f, .0f,
		0.166666f, 0.666666f, 0.166666f, .0f
	};
	for (int i = 0; i < 4; ++i)
	{
		//Ctrl point pos
		Pnt3f cp_pos_p1, cp_pos_p2 , cp_pos_p3 , cp_pos_p4;
		if (i == 0)
		{
			cp_pos_p1 = Pnt3f(0, 0, 0);
			cp_pos_p2 = Pnt3f(60, 60, 0);
			cp_pos_p3 = Pnt3f(0, 120, 0);
			cp_pos_p4 = Pnt3f(-60, 60, 0);
		}
		if (i == 1)
		{
			cp_pos_p4 = Pnt3f(0, 0, 0);
			cp_pos_p1 = Pnt3f(60, 60, 0);
			cp_pos_p2 = Pnt3f(0, 120, 0);
			cp_pos_p3 = Pnt3f(-60, 60, 0);
		}
		if (i == 2)
		{
			cp_pos_p3 = Pnt3f(0, 0, 0);
			cp_pos_p4 = Pnt3f(60, 60, 0);
			cp_pos_p1 = Pnt3f(0, 120, 0);
			cp_pos_p2 = Pnt3f(-60, 60, 0);
		}
		if (i == 3)
		{
			cp_pos_p2 = Pnt3f(0, 0, 0);
			cp_pos_p3 = Pnt3f(60, 60, 0);
			cp_pos_p4 = Pnt3f(0, 120, 0);
			cp_pos_p1 = Pnt3f(-60, 60, 0);
		}
		//ctrl point orientation
		Pnt3f cp_orient_p1 = m_pTrack->points[i].orient;
		Pnt3f cp_orient_p2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p3 = m_pTrack->points[(i + 2) % m_pTrack->points.size()].orient;
		Pnt3f cp_orient_p4 = m_pTrack->points[(i + 3) % m_pTrack->points.size()].orient;
		if (i == 0)
		{
			cp_orient_p1 = Pnt3f(0, -1, 0);
			cp_orient_p2 = Pnt3f(0, -1, 0);
			cp_orient_p3 = Pnt3f(0, -1, 0);
			cp_orient_p4 = Pnt3f(0, -1, 0);
		}
		if (i == 1)
		{
			cp_orient_p1 = Pnt3f(0, -1, 0);
			cp_orient_p2 = Pnt3f(0, -1, 0);
			cp_orient_p3 = Pnt3f(0, -1, 0);
			cp_orient_p4 = Pnt3f(0, -1, 0);
		}
		if (i == 2)
		{
			cp_orient_p1 = Pnt3f(0, 1, 0);
			cp_orient_p2 = Pnt3f(0, 1, 0);
			cp_orient_p3 = Pnt3f(0, 1, 0);
			cp_orient_p4 = Pnt3f(0, 1, 0);
		}
		if (i == 3)
		{
			cp_orient_p1 = Pnt3f(0, 1, 0);
			cp_orient_p2 = Pnt3f(0, 1, 0);
			cp_orient_p3 = Pnt3f(0, 1, 0);
			cp_orient_p4 = Pnt3f(0, 1, 0);
		}

		Pnt3f posVec[4];
		Pnt3f oriVec[4];
		Pnt3f oriVec_t[4];

		//calculate position G * M
		for (int i = 0; i < 4; i++)
		{
			posVec[i] = cp_pos_p1 * BSplineMatrix[i * 4] + cp_pos_p2 * BSplineMatrix[i * 4 + 1] + cp_pos_p3 * BSplineMatrix[i * 4 + 2] + cp_pos_p4 * BSplineMatrix[i * 4 + 3];
		}
		//calculate orient G * M
		for (int i = 0; i < 4; i++)
		{
			oriVec[i] = cp_orient_p1 * BSplineMatrix[i * 4] + cp_orient_p2 * BSplineMatrix[i * 4 + 1] + cp_orient_p3 * BSplineMatrix[i * 4 + 2] + cp_orient_p4 * BSplineMatrix[i * 4 + 3];
		}
		
		float percent = 1.0f / DIVIDE_LINE;
		float t = 0;
		Pnt3f qt = posVec[0] * t * t * t + posVec[1] * t * t + posVec[2] * t + posVec[3];

		//calculate curPos
		for (size_t j = 0; j < DIVIDE_LINE; j++)
		{
			dw = false;
			if (j == 0 && i == 0)
			{
				dw = true;
			}
			Pnt3f qt0 = qt;
			t += percent;
			//calculate curPos
			qt = posVec[0] * t * t * t + posVec[1] * t * t + posVec[2] * t + posVec[3];
			Pnt3f qt1 = qt;
			//calculate nxtPos
			Pnt3f qt2 = posVec[0] * (t + percent / 2) * (t + percent / 2) * (t + percent / 2) + posVec[1] * (t + percent / 2) * (t + percent / 2) + posVec[2] * (t + percent / 2) + posVec[3];

			//calculate current orientation
			Pnt3f orient_t = oriVec[0] * t * t * t + oriVec[1] * t * t + oriVec[2] * t + oriVec[3];

			//calculate current orientation
			Pnt3f orient_temp = oriVec_t[0] * t * t * t + oriVec_t[1] * t * t + oriVec_t[2] * t + oriVec_t[3];

			orient_t.normalize();
			Pnt3f cross_t = (qt2 - qt0) * orient_t;

			orient_temp.normalize();
			Pnt3f cross_temp = (qt2 - qt0) * orient_temp;

			//calculate distortion of track
			cross_t.normalize();
			cross_t = cross_t * 2.5f;
			//calculate distortion of track
			cross_temp.normalize();
			cross_temp = cross_temp * 2.5f;
			pointPosF.push_back(glm::vec3(qt0.x, qt0.y, qt0.z));
			pointOrientationF.push_back(glm::vec3(orient_t.x, orient_t.y, orient_t.z));
			pointLengthF.push_back(sqrt((qt2 - qt0).x * (qt2 - qt0).x + (qt2 - qt0).y * (qt2 - qt0).y + (qt2 - qt0).z * (qt2 - qt0).z));
			culDis += sqrt((qt2 - qt0).x * (qt2 - qt0).x + (qt2 - qt0).y * (qt2 - qt0).y + (qt2 - qt0).z * (qt2 - qt0).z);
			if (culDis >= 2.5)
			{
				dw = true;
				culDis = 0;
			}
		}
	}

	
	GLUquadricObj* qobj = 0;
	qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	
	//glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(0, 60, -2.5);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(qobj, GL_TRUE);
	if (!tw->detail->value())
	{
		gluSphere(qobj, 15, tw->subdivision->value(), tw->subdivision->value());
	}
	else
	{
		gluSphere(qobj, 15, arcball.zoom - 10, arcball.zoom - 10);
	}
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	GLUquadricObj* cobj = 0;
	cobj = gluNewQuadric();
	glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(0, 60, -28);
	gluCylinder(cobj, 10, 10, 50, 8, 8);
	gluDeleteQuadric(cobj);
	glPopMatrix();

	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, 20);
	glVertex3f(12, 70, 20);
	glVertex3f(12, 0, 20);
	glVertex3f(-12, 0, 20);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, 27);
	glVertex3f(12, 70, 27);
	glVertex3f(12, 0, 27);
	glVertex3f(-12, 0, 27);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, 20);
	glVertex3f(12, 70, 20);
	glVertex3f(12, 70, 27);
	glVertex3f(-12, 70, 27);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(12, 70, 20);
	glVertex3f(12, 0, 20);
	glVertex3f(12, 0, 27);
	glVertex3f(12, 70, 27);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, 20);
	glVertex3f(-12, 0, 20);
	glVertex3f(-12, 0, 27);
	glVertex3f(-12, 70, 27);
	
	glEnd();


	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, -25);
	glVertex3f(12, 70, -25);
	glVertex3f(12, 0, -25);
	glVertex3f(-12, 0, -25);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, -32);
	glVertex3f(12, 70, -32);
	glVertex3f(12, 0, -32);
	glVertex3f(-12, 0, -32);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, -25);
	glVertex3f(12, 70, -25);
	glVertex3f(12, 70, -32);
	glVertex3f(-12, 70, -32);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(12, 70, -25);
	glVertex3f(12, 0, -25);
	glVertex3f(12, 0, -32);
	glVertex3f(12, 70, -32);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(-12, 70, -25);
	glVertex3f(-12, 0, -25);
	glVertex3f(-12, 0, -32);
	glVertex3f(-12, 70, -32);
	glEnd();
}

void TrainView::calcdrawCarriage(TrainView* tar, bool doingshadows, bool drawTrain)
{
	vector<float> track;
	for (int i = 0; i < floor(tw->cge->value()); i++) {
		//determinePosition

		posF = (int)(waveTime/2 + i * pointPosF.size() / floor(tw->cge->value())) % pointPosF.size();
		//draw car
		if (drawTrain)
		{
			Pnt3f qt = Pnt3f(pointPosF[posF].x, pointPosF[posF].y, pointPosF[posF].z);
			Pnt3f orient_t = Pnt3f(pointOrientationF[posF].x, pointOrientationF[posF].y, pointOrientationF[posF].z);
			glm::vec3 f = pointPosF[(posF + 1) % pointPosF.size()] - pointPosF[posF];
			Pnt3f face = Pnt3f(f.x, f.y, f.z);
			face.normalize();
			Pnt3f w = face * orient_t;
			w.normalize();
			Pnt3f v = w * face;
			v.normalize();


			int faces[6][4] =
			{
				{0,1,2,3},
				{7,6,5,4},
				{0,4,5,1},
				{1,5,6,2},
				{3,2,6,7},
				{0,3,7,4}
			};
			float rotation[16] = {
				face.x,face.y,face.z,0.0,
				v.x,v.y,v.z,0.0,
				w.x,w.y,w.z,0.0,
				0.0,0.0,0.0,1.0
			};
			glPushMatrix();
			glTranslatef(qt.x, qt.y, qt.z);
			glMultMatrixf(rotation);
			glScalef(7.0f, 7.0f, 7.0f);
			glTranslatef(-0.5f, 0.0f, -0.5f);
			//draw people
			float headColor[] = { .0f, .0f, .0f, .0f };
			float pointsHead[8][3] =
			{
				{-0.25,-0.25,-0.25}, {0.25,-0.25,-0.25},
				{0.25,-0.25,0.25}, {-0.25,-0.25,0.25},
				{-0.25,5.625,-0.25}, {0.25,5.625,-0.25},
				{0.25,5.625,0.25}, {-0.25,5.625,0.25}
			};
			for (int j = 0; j < 6; j++) {
				switch (j) {
				case 0:
					glNormal3f(0.0, -1.0, 0.0);
					break;
				case 1:
					glNormal3f(0.0, 1.0, 0.0);
					break;
				case 2:
					glNormal3f(1.0, 0.0, 0.0);
					break;
				case 3:
					glNormal3f(0.0, 0.0, 1.0);
					break;
				case 4:
					glNormal3f(-1.0, 0.0, 0.0);
					break;
				case 5:
					glNormal3f(0.0, 0.0, -1.0);
					break;

				}
				if (!doingshadows)
				{
					glColor3fv(headColor);
				}
				glBegin(GL_QUADS);
				glVertex3fv(pointsHead[faces[j][0]]);
				glVertex3fv(pointsHead[faces[j][1]]);
				glVertex3fv(pointsHead[faces[j][2]]);
				glVertex3fv(pointsHead[faces[j][3]]);
				glEnd();
			}
			glPopMatrix();

			float rotation1[16] = {
				1.0,0.0,0.0,0.0,
				0.0,1.0,0.0,0.0,
				0.0,0.0,1.0,0.0,
				0.0,0.0,0.0,1.0
			};


			glPushMatrix();
			glTranslatef(qt.x, qt.y, qt.z);
			glMultMatrixf(rotation1);
			glScalef(7.0f, 7.0f, 7.0f);
			//glTranslatef(-0.5f, 0.0f, -0.5f);

			//draw the car
			float color[] = { .0f, 0.5f, 1.0f, 1.0f };
			float points[8][3] =
			{
				{-1.2	,-1.2	,-1.2},
				{1.2	,-1.2	,-1.2},
				{1.2	,-1.2	,1.2},
				{-1.2	,-1.2	,1.2},
				{-1.2	,1.2	,-1.2},
				{1.2	,1.2	,-1.2},
				{1.2	,1.2	,1.2},
				{-1.2	,1.2	,1.2},
			};
			for (int j = 0; j < 6; j++) {
				switch (j) {
				case 0:
					glNormal3f(0.0, -1.0, 0.0);
					break;
				case 1:
					glNormal3f(0.0, 1.0, 0.0);
					break;
				case 2:
					glNormal3f(1.0, 0.0, 0.0);
					break;
				case 3:
					glNormal3f(0.0, 0.0, 1.0);
					break;
				case 4:
					glNormal3f(-1.0, 0.0, 0.0);
					break;
				case 5:
					glNormal3f(0.0, 0.0, -1.0);
					break;

				}
				if (!doingshadows)
				{
					glColor3fv(color);
				}
				glBegin(GL_QUADS);
				glVertex3fv(points[faces[j][0]]);
				glVertex3fv(points[faces[j][1]]);
				glVertex3fv(points[faces[j][2]]);
				glVertex3fv(points[faces[j][3]]);
				glEnd();
			}
			glPopMatrix();
		}
	}
}

void TrainView::drawTree(Pnt3f pos, float scale, Pnt3f leafColor)
{
	glRotatef(90,1,0,0);

	GLUquadricObj* coobj = 0;
	coobj = gluNewQuadric();
	glColor3f(leafColor.x, leafColor.y, leafColor.z);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, -40 * scale);
	gluCylinder(coobj, 0 * scale, 10 * scale, 30 * scale, 15, 15);
	gluDeleteQuadric(coobj);
	glPopMatrix();

	GLUquadricObj* cyobj = 0;
	cyobj = gluNewQuadric();
	glColor3f(0.54, 0.27, 0.07);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, -10 * scale);
	gluCylinder(cyobj, 5 * scale, 5 * scale, 10 * scale, 15, 15);
	gluDeleteQuadric(cyobj);
	glPopMatrix();

	glRotatef(-90, 1, 0, 0);
}

void TrainView::drawFence(int width)
{
	for (int i = 5; -98 + i * 2 * width + width <= 100; i++)
	{
		glBegin(GL_QUADS);
		glColor3d(0.80, 0.33, 0.80);
		glVertex3f(98, 13, -98 + i * 2 * width);
		glVertex3f(98, 0, -98 + i* 2 * width);
		glVertex3f(98, 0, -98 + i* 2 * width + width);
		glVertex3f(98, 13, -98 + i * 2 * width + width);
		glEnd();
	}
	for (int i = 0; -80 + i * 2 * width + width <= 100; i++)
	{
		glBegin(GL_QUADS);
		glColor3d(0.80, 0.33, 0.80);
		glVertex3f(-98, 13, -98 + i * 2 * width);
		glVertex3f(-98, 0, -98 + i * 2 * width);
		glVertex3f(-98, 0, -98 + i * 2 * width + width);
		glVertex3f(-98, 13, -98 + i * 2 * width + width);
		glEnd();
	}
	for (int i = 0; -98 + i * 2 * width + width <= 80; i++)
	{
		glBegin(GL_QUADS);
		glColor3d(0.80, 0.33, 0.80);
		glVertex3f(-98 + i * 2 * width, 13, -98);
		glVertex3f(-98 + i * 2 * width, 0, -98);
		glVertex3f(-98 + i * 2 * width + width, 0, -98);
		glVertex3f(-98 + i * 2 * width + width, 13, -98);
		glEnd();
	}

	for (int i = 0; -98 + i * 2 * width + width <= 74; i++)
	{
		glBegin(GL_QUADS);
		glColor3d(0.80, 0.33, 0.80);
		glVertex3f(-98 + i * 2 * width, 13, 98);
		glVertex3f(-98 + i * 2 * width, 0, 98);
		glVertex3f(-98 + i * 2 * width + width, 0, 98);
		glVertex3f(-98 + i * 2 * width + width, 13, 98);
		glEnd();
	}

	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(-98, 8, 98);
	glVertex3f(-98, 8, -98);
	glVertex3f(-98, 10, -98);
	glVertex3f(-98, 10, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98, 8, 98);
	glVertex3f(98, 8, -80);
	glVertex3f(98, 10, -80);
	glVertex3f(98, 10, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(80, 8, -98);
	glVertex3f(-98, 8, -98);
	glVertex3f(-98, 10, -98);
	glVertex3f(80, 10, -98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(75, 8, 98);
	glVertex3f(-98, 8, 98);
	glVertex3f(-98, 10, 98);
	glVertex3f(75, 10, 98);
	glEnd();


	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98, 0, 98);
	glVertex3f(90, 0, 98);
	glVertex3f(90, 18, 98);
	glVertex3f(98, 18, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98, 0, 98);
	glVertex3f(98, 0, 90);
	glVertex3f(98, 18, 90);
	glVertex3f(98, 18, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98, 18, 98);
	glVertex3f(90, 18, 98);
	glVertex3f(90, 18, 90);
	glVertex3f(98, 18, 90);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(90, 0, 90);
	glVertex3f(90, 0, 98);
	glVertex3f(90, 18, 98);
	glVertex3f(90, 18, 90);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(90, 0, 90);
	glVertex3f(98, 0, 90);
	glVertex3f(98, 18, 90);
	glVertex3f(90, 18, 90);
	glEnd();


	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98 - 20, 0, 98);
	glVertex3f(90 - 20, 0, 98);
	glVertex3f(90 - 20, 18, 98);
	glVertex3f(98 - 20, 18, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98 - 20, 0, 98);
	glVertex3f(98 - 20, 0, 90);
	glVertex3f(98 - 20, 18, 90);
	glVertex3f(98 - 20, 18, 98);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(98 - 20, 18, 98);
	glVertex3f(90 - 20, 18, 98);
	glVertex3f(90 - 20, 18, 90);
	glVertex3f(98 - 20, 18, 90);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(90 - 20, 0, 90);
	glVertex3f(90 - 20, 0, 98);
	glVertex3f(90 - 20, 18, 98);
	glVertex3f(90 - 20, 18, 90);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.80, 0.33, 0.80);
	glVertex3f(90 - 20, 0, 90);
	glVertex3f(98 - 20, 0, 90);
	glVertex3f(98 - 20, 18, 90);
	glVertex3f(90 - 20, 18, 90);
	glEnd();
	
	glBegin(GL_QUADS);
	glColor3d(0.55,0.5,0.5);
	glVertex3f(-100, 0, 100);
	glVertex3f(-80, 0, 100);
	glVertex3f(-80, 18, 100);
	glVertex3f(-100, 18, 100);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.55, 0.5, 0.5);
	glVertex3f(-100, 0, 100);
	glVertex3f(-100, 0, 80);
	glVertex3f(-100, 18, 80);
	glVertex3f(-100, 18, 100);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.4, 0.7, 1.0);
	glVertex3f(-100, 14, 100);
	glVertex3f(-80, 14, 100);
	glVertex3f(-80, 14, 80);
	glVertex3f(-100, 14, 80);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.55, 0.5, 0.5);
	glVertex3f(-80, 0, 80);
	glVertex3f(-80, 0, 100);
	glVertex3f(-80, 18, 100);
	glVertex3f(-80, 18, 80);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.55, 0.5, 0.5);
	glVertex3f(-80, 0, 80);
	glVertex3f(-100, 0, 80);
	glVertex3f(-100, 18, 80);
	glVertex3f(-80, 18, 80);
	glEnd();


	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.3);
	glVertex3f(100, 0, -100);
	glVertex3f(80, 0, -100);
	glVertex3f(80, 10, -100);
	glVertex3f(100, 10, -100);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.3);
	glVertex3f(100, 0, -100);
	glVertex3f(100, 0, -80);
	glVertex3f(100, 10, -80);
	glVertex3f(100, 10, -100);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glVertex3f(100, 0.2, -100);
	glVertex3f(80, 0.2, -100);
	glVertex3f(80, 0.2, -80);
	glVertex3f(100, 0.2, -80);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.3);
	glVertex3f(80, 0, -80);
	glVertex3f(80, 0, -100);
	glVertex3f(80, 4, -100);
	glVertex3f(80, 4, -80);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(1, 0.3, 0.3);
	glVertex3f(80, 0, -80);
	glVertex3f(100, 0, -80);
	glVertex3f(100, 4, -80);
	glVertex3f(80, 4, -80);
	glEnd();


	glBegin(GL_QUADS);
	glColor3d(0.98, 1.0, 1.0);
	glVertex3f(-92, 22, 92);
	glVertex3f(-88, 22, 92);
	glVertex3f(-88, 26, 92);
	glVertex3f(-92, 26, 92);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.98, 1.0, 1.0);
	glVertex3f(-92, 22, 92);
	glVertex3f(-92, 22, 88);
	glVertex3f(-92, 26, 88);
	glVertex3f(-92, 26, 92);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.98, 1.0, 1.0);
	glVertex3f(-92, 26, 92);
	glVertex3f(-88, 26, 92);
	glVertex3f(-88, 26, 88);
	glVertex3f(-92, 26, 88);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.98, 1.0, 1.0);
	glVertex3f(-88, 22, 88);
	glVertex3f(-88, 22, 92);
	glVertex3f(-88, 26, 92);
	glVertex3f(-88, 26, 88);
	glEnd();
	glBegin(GL_QUADS);
	glColor3d(0.98, 1.0, 1.0);
	glVertex3f(-88, 22, 88);
	glVertex3f(-92, 22, 88);
	glVertex3f(-92, 26, 88);
	glVertex3f(-88, 26, 88);
	glEnd();
}

void TrainView::drawGame(bool t)
{
	Pnt3f center = Pnt3f(0, 0, 0);
	if (t == true)
	{
		correct = false;
		loc1 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
		loc2 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
		loc3 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
		while ((loc1 - loc3).length() < 30 || (loc2 - loc3).length() < 30 || (loc1 - loc2).length() < 30 || (loc1 - center).length() < 20 || (loc2 - center).length() < 20 || (loc3 - center).length() < 20)
		{
			loc1 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
			loc2 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
			loc3 = Pnt3f(rand() % 160 - 80, rand() % 160 - 80, 0);
		}
		color = rand() % 3;
		updateTime = waveTime;
		correct = false;
	}
	GLUquadricObj* robj = 0;
	robj = gluNewQuadric();
	gluQuadricTexture(robj, GL_TRUE);
	glPushMatrix();
	glTranslatef(loc1.x, 0, loc1.y);
	glColor3f(1, 0, 0);
	gluQuadricTexture(robj, GL_TRUE);
	gluSphere(robj, 15, 15, 15);
	gluDeleteQuadric(robj);
	glPopMatrix();
	GLUquadricObj* gobj = 0;
	gobj = gluNewQuadric();
	gluQuadricTexture(gobj, GL_TRUE);
	glPushMatrix();
	glTranslatef(loc2.x, 0, loc2.y);
	glColor3f(0, 1, 0);
	gluQuadricTexture(gobj, GL_TRUE);
	gluSphere(gobj, 15, 15, 15);
	gluDeleteQuadric(gobj);
	glPopMatrix();
	GLUquadricObj* bobj = 0;
	bobj = gluNewQuadric();
	gluQuadricTexture(bobj, GL_TRUE);
	glPushMatrix();
	glTranslatef(loc3.x, 0, loc3.y);
	glColor3f(0, 0, 1);
	gluQuadricTexture(bobj, GL_TRUE);
	gluSphere(bobj, 15, 15, 15);
	gluDeleteQuadric(bobj);
	glPopMatrix();
	if (waveTime - updateTime > 45 && correct != true)
	{
		
		firework.clear();
		drawGame(true);
	}
}
// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();

	// where is the mouse?
	int mx = Fl::event_x();
	int my = Fl::event_y();
	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	//drawItems(false, false);


	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((double)mx, (double)(viewport[3] - my),
		5, 5, viewport);


	// now set up the projection
	setProjection();
	pick(mx, h() - my - 1); //030

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
		glLoadName((GLuint)(i + 1));
		if (!tw->detail->value())
		{
			m_pTrack->points[i].draw(true, tw->subdivision->value());
		}
		else
		{
			m_pTrack->points[i].draw(true, arcball.zoom - 10);
		}
		
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3] - 1;
	}
	else // nothing hit, nothing selected
		selectedCube = -1;

	//printf("Selected Cube %d\n", selectedCube);
}

void TrainView::setUBO()
{
	float wdt = this->pixel_w();
	float hgt = this->pixel_h();

	glm::mat4 view_matrix;
	glGetFloatv(GL_MODELVIEW_MATRIX, &view_matrix[0][0]);
	//HMatrix view_matrix; 
	//this->arcball.getMatrix(view_matrix);

	glm::mat4 projection_matrix;
	glGetFloatv(GL_PROJECTION_MATRIX, &projection_matrix[0][0]);
	//projection_matrix = glm::perspective(glm::radians(this->arcball.getFoV()), (GLfloat)wdt / (GLfloat)hgt, 0.01f, 1000.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, this->commom_matrices->ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection_matrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view_matrix[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
