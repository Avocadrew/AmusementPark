/************************************************************************
     File:        TrainWindow.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						this class defines the window in which the project 
						runs - its the outer windows that contain all of 
						the widgets, including the "TrainView" which has the 
						actual OpenGL window in which the train is drawn

						You might want to modify this class to add new widgets
						for controlling	your train

						This takes care of lots of things - including installing 
						itself into the FlTk "idle" loop so that we get periodic 
						updates (if we're running the train).


     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <FL/fl.h>
#include <FL/Fl_Box.h>

// for using the real time clock
#include <time.h>

#include "TrainWindow.H"
#include "TrainView.H"
#include "CallBacks.H"



//************************************************************************
//
// * Constructor
//========================================================================
TrainWindow::
TrainWindow(const int x, const int y) 
	: Fl_Double_Window(x,y,1000,600,"Train and Roller Coaster") //030
//========================================================================
{
	// make all of the widgets
	begin();	// add to this widget
	{
		int pty=5;			// where the last widgets were drawn

		trainView = new TrainView(5,5,590,590);
		trainView->tw = this;
		trainView->m_pTrack = &m_Track;
		this->resizable(trainView);

		// to make resizing work better, put all the widgets in a group
		widgets = new Fl_Group(600,5,990,590); //030
		widgets->begin();

		runButton = new Fl_Button(605,pty,60,20,"Run");
		togglify(runButton);

		Fl_Button* fb = new Fl_Button(700,pty,25,20,"@>>");
		fb->callback((Fl_Callback*)forwCB,this);
		Fl_Button* rb = new Fl_Button(670,pty,25,20,"@<<");
		rb->callback((Fl_Callback*)backCB,this);
		
		detail = new Fl_Button(730,pty,65,20,"Detail");
		togglify(detail,0);
  
		pty+=25;
		speed = new Fl_Value_Slider(655,pty,140,20,"speed");
		speed->range(0,10);
		speed->value(2);
		speed->align(FL_ALIGN_LEFT);
		speed->type(FL_HORIZONTAL);

		pty += 30;

		// camera buttons - in a radio button group
		
		worldCam = new Fl_Button(735, pty, 60, 20, "World");
        worldCam->type(FL_RADIO_BUTTON);		// radio button
        worldCam->value(1);			// turned on
        worldCam->selection_color((Fl_Color)3); // yellow when pressed
		worldCam->callback((Fl_Callback*)damageCB,this);
		trainCam = new Fl_Button(605, pty, 60, 20, "Train");
        trainCam->type(FL_RADIO_BUTTON);
        trainCam->value(0);
        trainCam->selection_color((Fl_Color)3);
		trainCam->callback((Fl_Callback*)damageCB,this);
		topCam = new Fl_Button(670, pty, 60, 20, "Top");
        topCam->type(FL_RADIO_BUTTON);
        topCam->value(0);
        topCam->selection_color((Fl_Color)3);
		topCam->callback((Fl_Callback*)damageCB,this);
		

		pty += 30;

		// add and delete points
		Fl_Button* ap = new Fl_Button(605, pty, 90, 20, "Add Point");
		ap->callback((Fl_Callback*)addPointCB, this);
		Fl_Button* dp = new Fl_Button(705, pty, 90, 20, "Delete Point");
		dp->callback((Fl_Callback*)deletePointCB, this);

		pty += 25;
		// reset the points
		resetButton = new Fl_Button(735, pty, 60, 20, "Reset");
		resetButton->callback((Fl_Callback*)resetCB, this);
		Fl_Button* loadb = new Fl_Button(605, pty, 60, 20, "Load");
		loadb->callback((Fl_Callback*)loadCB, this);
		Fl_Button* saveb = new Fl_Button(670, pty, 60, 20, "Save");
		saveb->callback((Fl_Callback*)saveCB, this);

		pty += 25;
		// roll the points
		Fl_Button* rx = new Fl_Button(605, pty, 43, 20, "R+X");
		rx->callback((Fl_Callback*)rpxCB, this);
		Fl_Button* rxp = new Fl_Button(654, pty, 43, 20, "R-X");
		rxp->callback((Fl_Callback*)rmxCB, this);
		Fl_Button* rz = new Fl_Button(703, pty, 43, 20, "R+Z");
		rz->callback((Fl_Callback*)rpzCB, this);
		Fl_Button* rzp = new Fl_Button(752, pty, 43, 20, "R-Z");
		rzp->callback((Fl_Callback*)rmzCB, this);

		pty += 35;
		waveBrowser = new Fl_Browser(605, pty, 190, 55, "Scene Type");
		waveBrowser->type(2);		// select
		waveBrowser->callback((Fl_Callback*)damageCB, this);
		waveBrowser->add("Game");
		waveBrowser->add("Amusement Park");
		waveBrowser->add("Water + Rendering");
		waveBrowser->select(2);

		pty += 75;
		subdivision = new Fl_Value_Slider(675, pty, 120, 20, "Subdivision");
		subdivision->range(5, 20);
		subdivision->value(8.0f);
		subdivision->align(FL_ALIGN_LEFT);
		subdivision->type(FL_HORIZONTAL);
		pty += 30;
		fenceParam = new Fl_Value_Slider(675, pty, 120, 20, "Fence");
		fenceParam->range(1, 6);
		fenceParam->value(2.0f);
		fenceParam->align(FL_ALIGN_LEFT);
		fenceParam->type(FL_HORIZONTAL);
		pty += 30;
		cge = new Fl_Value_Slider(675, pty, 120, 20, "Carriage");
		cge->range(3, 8);
		cge->value(4);
		cge->align(FL_ALIGN_LEFT);
		cge->type(FL_HORIZONTAL);
		pty += 30;
		trn = new Fl_Value_Slider(675, pty, 120, 20, "Train");
		trn->range(3, 8);
		trn->value(4);
		trn->align(FL_ALIGN_LEFT);
		trn->type(FL_HORIZONTAL);

		pty = 5;


		Fl_Group* lightGroup = new Fl_Group(800, pty, 195, 20);
		lightGroup->begin();
		GouraudShading = new Fl_Button(805, pty, 60, 20, "Gouraud");
		GouraudShading->type(FL_RADIO_BUTTON);		// radio button
		GouraudShading->value(0);			// turned on
		GouraudShading->selection_color((Fl_Color)3); // yellow when pressed
		GouraudShading->callback((Fl_Callback*)damageCB, this);
		PhongShading = new Fl_Button(870, pty, 60, 20, "Phong");
		PhongShading->type(FL_RADIO_BUTTON);
		PhongShading->value(0);
		PhongShading->selection_color((Fl_Color)3); // yellow when pressed
		PhongShading->callback((Fl_Callback*)damageCB, this);
		MultLightShading = new Fl_Button(935, pty, 60, 20, "Multiple");
		MultLightShading->type(FL_RADIO_BUTTON);
		MultLightShading->value(1);
		MultLightShading->selection_color((Fl_Color)3); // yellow when pressed
		MultLightShading->callback((Fl_Callback*)damageCB, this);
		lightGroup->end();
		pty += 30;
		Fl_Group* shadeGroup = new Fl_Group(800, pty, 195, 20);
		shadeGroup->begin();
		noShading = new Fl_Button(805, pty, 60, 20, "None");
		noShading->type(FL_RADIO_BUTTON);		// radio button
		noShading->value(1);			// turned on
		noShading->selection_color((Fl_Color)3); // yellow when pressed
		noShading->callback((Fl_Callback*)damageCB, this);
		screenShading = new Fl_Button(870, pty, 60, 20, "Screen");
		screenShading->type(FL_RADIO_BUTTON);
		screenShading->value(0);
		screenShading->selection_color((Fl_Color)3); // yellow when pressed
		screenShading->callback((Fl_Callback*)damageCB, this);
		shadeGroup->end();
		toonShading = new Fl_Button(935, pty, 60, 20, "Toon");
		togglify(toonShading);
		toonShading->value(0);
		
		pty += 30;
		refRatio = new Fl_Value_Slider(875, pty, 120, 20, "Ratio");
		refRatio->range(0, 1);
		refRatio->value(0.5f);
		refRatio->align(FL_ALIGN_LEFT);
		refRatio->type(FL_HORIZONTAL); 
		pty += 30;
		shBrowser = new Fl_Browser(810, pty, 185, 135, "Visual Type");
		shBrowser->type(2);		// select
		shBrowser->callback((Fl_Callback*)damageCB, this);
		shBrowser->add("Pixelation");
		shBrowser->add("Panic");
		shBrowser->add("Offset");
		shBrowser->add("Brightness");
		shBrowser->add("Color Mask");
		shBrowser->add("NPR");
		shBrowser->add("Grayscale");
		shBrowser->add("Sobel");
		shBrowser->select(1);
		pty += 155;
		offset = new Fl_Value_Slider(875, pty, 120, 20, "Offset");
		offset->range(0, 1);
		offset->value(0.5f);
		offset->align(FL_ALIGN_LEFT);
		offset->type(FL_HORIZONTAL);
		offset->callback((Fl_Callback*)damageCB, this);
		pty += 30;
		shaderParam = new Fl_Value_Slider(875, pty, 120, 20, "Shading");
		shaderParam->range(-0.5, 0.5);
		shaderParam->value(0.0f);
		shaderParam->align(FL_ALIGN_LEFT);
		shaderParam->type(FL_HORIZONTAL);
		shaderParam->callback((Fl_Callback*)damageCB, this);

		// we need to make a little phantom widget to have things resize correctly
		Fl_Box* resizebox = new Fl_Box(600, 595, 200, 5);
		widgets->resizable(resizebox);

		widgets->end();

	}
	end();	// done adding to this widget

	// set up callback on idle
	Fl::add_idle((void (*)(void*))runButtonCB,this);
}

//************************************************************************
//
// * handy utility to make a button into a toggle
//========================================================================
void TrainWindow::
togglify(Fl_Button* b, int val)
//========================================================================
{
	b->type(FL_TOGGLE_BUTTON);		// toggle
	b->value(val);		// turned off
	b->selection_color((Fl_Color)3); // yellow when pressed	
	b->callback((Fl_Callback*)damageCB,this);
}

//************************************************************************
//
// *
//========================================================================
void TrainWindow::
damageMe()
//========================================================================
{
	if (trainView->selectedCube >= ((int)m_Track.points.size()))
		trainView->selectedCube = 0;
	trainView->damage(1);
}

//************************************************************************
//
// * This will get called (approximately) 30 times per second
//   if the run button is pressed
//========================================================================
void TrainWindow::
advanceTrain(float dir)
//========================================================================
{
	//#####################################################################
	// TODO: make this work for your train
	//#####################################################################
#ifdef EXAMPLE_SOLUTION
	// note - we give a little bit more example code here than normal,
	// so you can see how this works

	if (arcLength->value()) {
		float vel = ew.physics->value() ? physicsSpeed(this) : dir * (float)speed->value();
		world.trainU += arclenVtoV(world.trainU, vel, this);
	} else {
		world.trainU +=  dir * ((float)speed->value() * .1f);
	}

	float nct = static_cast<float>(world.points.size());
	if (world.trainU > nct) world.trainU -= nct;
	if (world.trainU < 0) world.trainU += nct;
#endif
}