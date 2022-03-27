/************************************************************************
     File:        TrainView.H

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

#pragma once

#include "RenderUtilities/BufferObject.h"
#include "RenderUtilities/Shader.h"
#include "RenderUtilities/Texture.h"
#include "RenderUtilities/Environment.h"
#include "RenderUtilities/Pixelation.h"
#include "RenderUtilities/Shadow.h"

// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;


//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#pragma warning(pop)
#include "Utilities/Pnt3f.H"
#include <AL/alut.h>
//#include <AL/al.h>
//#include <AL/alc.h>

// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"

#include <ctime>
#include <cstdlib>
#define MAXPARTICLES 100
#define LIFE 50

struct Particle {
	glm::vec3 position, velocity, color;
	float     life;
	bool first = false;
	Particle(glm::vec3 p, glm::vec3 v, glm::vec3 c, float l, bool t)
		: position(p), velocity(v), color(c), life(l), first(t) { }

};

class TrainView : public Fl_Gl_Window
{
	public:
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();

		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();

		//set ubo
		void setUBO();

		GLuint loadCubemap(std::vector<std::string> skybox_textures);

		void renderEnvironmentMap(Environment* environment);

		void renderScreen(Pixelation* pixelation);

		void drawItems(bool getEnvironment, bool getScreen, bool onlyDrawWave = false, bool getWaveFrame = false); //, bool renderShadow

		void pick(int x, int y);

		void calcdrawTrain(TrainView* tar, bool shadow, bool drawTrain);

		void drawCardinal(bool doingShadows);

		void calcdrawCarriage(TrainView* tar, bool shadow, bool drawTrain);

		void drawFerris(bool doingShadows);

		void drawTree(Pnt3f pos, float scale, Pnt3f color);

		void drawFence(int width);

		void updateFirework();

		void drawGame(bool t);

		Particle CreateFirework(int count, bool t);


		glm::vec3 cameraPos;
	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube;  // simple - just remember which cube is selected
		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene

		Shader* dotCubeShader = nullptr;
		Shader* skyboxShader = nullptr;
		Shader* tileboxShader = nullptr;
		Shader* tboxShader = nullptr;
		Shader* sinWaveShader = nullptr;
		Shader* billboardShader = nullptr;
		Shader* heightMapShader = nullptr;
		Shader* interactiveShader = nullptr;
		Shader* dropShader = nullptr;
		Shader* updateShader = nullptr;
		Shader* pixelationShader = nullptr;
		Shader* particleShader = nullptr;
		Texture2D* dotCubeTexture = nullptr;
		Texture2D* waterTexture = nullptr;
		Texture2D* uvTexture = nullptr;
		Texture2D* concreteTexture = nullptr;
		Texture2D* billboardTexture = nullptr;
		GLint skyboxTexture;
		GLint tileCubeTexture;
		GLint tCubeTexture;
		VAO* particle = nullptr;
		VAO* dotCube	= nullptr;
		VAO* skybox = nullptr;
		VAO* tilebox = nullptr;
		VAO* tbox = nullptr;
		VAO* sinWave = nullptr;
		VAO* heightMap = nullptr;
		VAO* interactive = nullptr;
		VAO* quad = nullptr;
		VAO* billboard = nullptr;

		UBO* commom_matrices = nullptr;
		UBO* skybox_matrices= nullptr;
		FBO* uv_buffer = nullptr;
		float waveTime = 0;
		float interactTime = -1000;
		int heightMapIndex = 0;
		bool isHeightMapLoaded = false;
		bool isSkyboxTextureLoaded = false;
		bool isTileCubeTextureLoad = false;
		bool isTCubeTextureLoad = false;

		std::vector<std::string> skybox_textures;
		std::vector<std::string> tilebox_textures;
		std::vector<std::string> tbox_textures;
		std::vector<Texture2D> heightMaps;

		Environment* environment = nullptr;
		Pixelation* pixelation = nullptr;
		glm::vec3 waveCenter = glm::vec3(0.0f, 25.0f, 0.0f);
		glm::mat4 origin_view_matrix;
		glm::mat4 origin_projection_matrix;

		glm::vec3 uv;
		glm::vec3 uv1;
		unsigned int framebuffer;
		unsigned int textureColorbuffer;

		//OpenAL
		glm::vec3 worldCenter = { 0.0f,0.0f,0.0f };

		float DIVIDE_LINE = 100.0f; //Divide Line for calculations
		float t_time = 0;
		std::vector <glm::vec3> pointPos;
		std::vector <glm::vec3> pointOrientation;
		std::vector <float> pointLength;
		std::vector <glm::vec3> pointPosF;
		std::vector <glm::vec3> pointOrientationF;
		std::vector <float> pointLengthF;
		int pos = 0;
		int posF = 0;

		int	sevenCount;
		std::vector<Particle> firework;

		Pnt3f loc1 = Pnt3f(40, 40, 0);
		Pnt3f loc2 = Pnt3f(-15, -15, 0);
		Pnt3f loc3 = Pnt3f(15,15,0);
		bool correct = false;
		int color = time(NULL)%3;
		float updateTime = 0;
};