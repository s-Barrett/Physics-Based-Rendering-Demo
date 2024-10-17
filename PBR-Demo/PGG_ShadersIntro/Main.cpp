
// This is the main SDL include file
#include <SDL/SDL.h>

// I have added glew.h, wglew.h and glew.c to my VS project
// I find this the easiest way to include it
// To compile and link GLEW like this ('statically') you must add  GLEW_STATIC  into Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions
#include "glew.h"

#include "Scene.h"

// GUI system: https://github.com/ocornut/imgui
// - prevent compile error by building with: WINDOWS_IGNORE_PACKING_MISMATCH
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

// iostream is so we can output error messages to console
#include <iostream>
#include <string>

// An initialisation function, mainly for GLEW
// This will also print to console the version of OpenGL we are using
bool InitGL()
{
	// GLEW has a problem with loading core OpenGL
	// See here: https://www.opengl.org/wiki/OpenGL_Loading_Library
	// The temporary workaround is to enable its 'experimental' features
	glewExperimental = GL_TRUE;

	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr<<"Error: GLEW failed to initialise with message: "<< glewGetErrorString(err) <<std::endl;
		return false;
	}
	std::cout<<"INFO: Using GLEW "<< glewGetString(GLEW_VERSION)<<std::endl;

	std::cout<<"INFO: OpenGL Vendor: "<< glGetString( GL_VENDOR ) << std::endl;
	std::cout<<"INFO: OpenGL Renderer: "<< glGetString( GL_RENDERER ) << std::endl;
	std::cout<<"INFO: OpenGL Version: "<< glGetString( GL_VERSION ) << std::endl;
	std::cout<<"INFO: OpenGL Shading Language Version: "<< glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
	
	return true;
}


int main(int argc, char *argv[])
{
	// This is our initialisation phase

	// SDL_Init is the main initialisation function for SDL
	// It takes a 'flag' parameter which we use to tell SDL what systems we're going to use
	// Here, we want to use SDL's video system, so we give it the flag for this
	// Incidentally, this also initialises the input event system
	// This function also returns an error value if something goes wrong
	// So we can put this straight in an 'if' statement to check and exit if need be
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		// Something went very wrong in initialisation, all we can do is exit
		std::cout<<"Whoops! Something went very wrong, cannot initialise SDL :("<<std::endl;
		return -1;
	}



	// This is how we set the context profile
	// We need to do this through SDL, so that it can set up the OpenGL drawing context that matches this
	// (of course in reality we have no guarantee this will be available and should provide fallback methods if it's not!)
	// Anyway, we basically first say which version of OpenGL we want to use
	// So let's say 4.3:
	// Major version number (4):
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	// Minor version number (3):
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	// Then we say whether we want the core profile or the compatibility profile
	// Flag options are either: SDL_GL_CONTEXT_PROFILE_CORE   or   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
	// We'll go for the core profile
	// This means we are using the latest version and cannot use the deprecated functions
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );



	// Now we have got SDL initialised, we are ready to create a window!
	// These are some variables to help show you what the parameters are for this function
	// You can experiment with the numbers to see what they do
	int winPosX = 100;
	int winPosY = 100;
	int winWidth = 640;
	int winHeight = 640;
	SDL_Window *window = SDL_CreateWindow("My Window!!!",  // The first parameter is the window title
		winPosX, winPosY,
		winWidth, winHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	// The last parameter lets us specify a number of options
	// Here, we tell SDL that we want the window to be shown and that it can be resized
	// You can learn more about SDL_CreateWindow here: https://wiki.libsdl.org/SDL_CreateWindow?highlight=%28\bCategoryVideo\b%29|%28CategoryEnum%29|%28CategoryStruct%29
	// The flags you can pass in for the last parameter are listed here: https://wiki.libsdl.org/SDL_WindowFlags

	// The SDL_CreateWindow function returns an SDL_Window
	// This is a structure which contains all the data about our window (size, position, etc)
	// We will also need this when we want to draw things to the window
	// This is therefore quite important we don't lose it!






	// The SDL_Renderer is a structure that handles rendering
	// It will store all of SDL's internal rendering related settings
	// When we create it we tell it which SDL_Window we want it to render to
	// That renderer can only be used for this window
	// (yes, we can have multiple windows - feel free to have a play sometime)
	SDL_Renderer * renderer = SDL_CreateRenderer( window, -1, 0 );


	// Now that the SDL renderer is created for the window, we can create an OpenGL context for it!
	// This will allow us to actually use OpenGL to draw to the window
	SDL_GLContext glcontext = SDL_GL_CreateContext( window );

	// Call our initialisation function to set up GLEW and print out some GL info to console
	if( !InitGL() )
	{
		return -1;
	}



	// Setting up the GUI system
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Set a dark theme
	ImGui::StyleColorsDark();

	// The GUI system is built with an OpenGL back-end, so need to connect it up
	// Also tell it which shader language version to target
	// #version 130 corresponds to OpenGL 3.0, so should be fine for us
	const char* glslVersion = "#version 130";
	ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// State to keep track of whether our window is expanded open or not
	bool showLightingWindow = true;



	
	// We are going to work out how much time passes from frame to frame
	// We will use this variable to store the time at our previous frame
	// This function returns the number of milliseconds since SDL was initialised
	unsigned int lastTime = SDL_GetTicks();

	
	// Enable the depth test to make sure triangles in front are always in front no matter the order they are drawn
	// When you do this, don't forget to clear the depth buffer at the start of each frame - otherwise you just get an empty screen!
	glEnable(GL_DEPTH_TEST);


	Scene myScene;


	// Ok, hopefully finished with initialisation now
	// Let's go and draw something!


	// We are now preparing for our main loop (also known as the 'game loop')
	// This loop will keep going round until we exit from our program by changing the bool 'go' to the value false
	// This loop is an important concept and forms the basis of most games you'll be writing
	// Within this loop we generally do the following things:
	//   * Check for input from the user (and do something about it!)
	//   * Update our world
	//   * Draw our world
	// We will come back to this in later lectures
	bool go = true;
	while( go )
	{

		// Here we are going to check for any input events
		// Basically when you press the keyboard or move the mouse, the parameters are stored as something called an 'event'
		// SDL has a queue of events
		// We need to check for each event and then do something about it (called 'event handling')
		// the SDL_Event is the datatype for the event
		SDL_Event incomingEvent;
		// SDL_PollEvent will check if there is an event in the queue
		// If there's nothing in the queue it won't sit and wait around for an event to come along (there are functions which do this, and that can be useful too!)
		// For an empty queue it will simply return 'false'
		// If there is an event, the function will return 'true' and it will fill the 'incomingEvent' we have given it as a parameter with the event data
		while( SDL_PollEvent( &incomingEvent ) )
		{
			// Let our GUI system handle some events
			ImGui_ImplSDL2_ProcessEvent(&incomingEvent);

			// If we get in here, we have an event and need to figure out what to do with it
			// For now, we will just use a switch based on the event's type
			switch( incomingEvent.type )
			{
			case SDL_QUIT:
				// The event type is SDL_QUIT
				// This means we have been asked to quit - probably the user clicked on the 'x' at the top right corner of the window
				// To quit we need to set our 'go' bool to false so that we can escape out of the game loop
				go = false;
				break;

				// If you want to learn more about event handling and different SDL event types, see:
				// https://wiki.libsdl.org/SDL_Event
				// and also: https://wiki.libsdl.org/SDL_EventType
				// but don't worry, we'll be looking at handling user keyboard and mouse input soon

			case SDL_KEYDOWN:
				// The event type is SDL_KEYDOWN
				// This means that the user has pressed a key
				// Let's figure out which key they pressed
				switch( incomingEvent.key.keysym.sym )
				{
				case SDLK_DOWN:
					// You can put code in here that you want to run when the down arrow key is pressed
					break;
				case SDLK_UP:
					break;
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					break;
				case SDLK_a:
					break;
				case SDLK_d:
					break;
				case SDLK_w:
					break;
				case SDLK_s:
					break;
				}
				break;
			
			case SDL_KEYUP:
				// The event type is SDL_KEYDOWN
				// This means that the user has pressed a key
				// Let's figure out which key they pressed
				switch( incomingEvent.key.keysym.sym )
				{
				case SDLK_DOWN:
					// You can put code in here that you want to run when the down arrow key is pressed
					break;
				case SDLK_UP:
					break;
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					break;
				case SDLK_a:
					break;
				case SDLK_d:
					break;
				case SDLK_w:
					break;
				case SDLK_s:
					break;
				}
				break;
			}
		}


		// Update our world
		// --------------------------------------------

		// We are going to work out the time between each frame now
		// First, find the current time
		// again, SDL_GetTicks() returns the time in milliseconds since SDL was initialised
		// We can use this as the current time
		unsigned int current = SDL_GetTicks();
		// Next, we want to work out the change in time between the previous frame and the current one
		// This is a 'delta' (used in physics to denote a change in something)
		// So we call it our 'deltaT' and I like to use an 's' to remind me that it's in seconds!
		// (To get it in seconds we need to divide by 1000 to convert from milliseconds)
		float deltaTs = (float) (current - lastTime) / 1000.0f;
		// Now that we've done this we can use the current time as the next frame's previous time
		lastTime = current;
		
		myScene.Update( deltaTs );


		// Draw our world
		// --------------------------------------------

		// Specify the colour to clear the framebuffer to
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		// This writes the above colour to the colour part of the framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		myScene.Draw();


		// Draw GUI
		// --------------------------------------------

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();


		// Our showLightingWindow variable is a state that is kept from frame-to-frame
		// It's the visibility of the GUI window
		// So we only draw the contents of the window if its actually visible
		if (showLightingWindow)
		{
			// Create a window, give it a name
			// All ImGui commands after this to create widgets will be added to the window
			ImGui::Begin("Lighting Controls");

			// This is how you add a bit of text to the window
			ImGui::Text("Hello World!");

			// Here, we do the same sort of thing several times for different properties:
			//  1. Get a state from the scene
			//  2. Use the GUI to present an editor for it
			//  3. Send it back to the scene in case it's changed

			float currentMetallic = myScene.GetCubemetallic();
			ImGui::SliderFloat("Cube Metallic", &currentMetallic, 0.0f, 1.0f);
			myScene.SetCubemetallic(currentMetallic);

			float currentRoughness = myScene.GetcubeRoughness();
			ImGui::SliderFloat("Cube Roughness", &currentRoughness, 0.0f, 1.0f);
			myScene.SetcubeRoughness(currentRoughness);

			// This is a colour editor for the diffuse colour
			glm::vec3 currentCubeDiffuseCol = myScene.GetCubeDiffuseColour();
			ImGui::ColorEdit3("Cube Colour", &(currentCubeDiffuseCol[0]));
			myScene.SetCubeDiffuseColour(currentCubeDiffuseCol);

			// This is a checkbox for whether or not the light is rotating
			bool currentAnimLight = myScene.GetAnimateLight();
			ImGui::Checkbox("Animate Light", &currentAnimLight);
			myScene.SetAnimateLight(currentAnimLight);

			// This is a slider for the current angle of the light
			// Note that when it's animating it will slide back and forth automatically
			// The user can then grab and adjust
			float lightAngle = myScene.GetLightAngle();
			ImGui::SliderFloat("Light Angle", &lightAngle, 0.0f, 2.0f*3.141592653589793238462643383 );
			myScene.SetLightAngle(lightAngle);

			// Checkbox for the centre cube's animation
			bool currentAnimCube = myScene.GetAnimateCube();
			ImGui::Checkbox("Animate Cube", &currentAnimCube);
			myScene.SetAnimateCube(currentAnimCube);

			// Slider for the cube's rotation angle
			float cubeAngle = myScene.GetCubeAngle();
			ImGui::SliderFloat("Cube Angle", &cubeAngle, 0.0f, 2.0f * 3.141592653589793238462643383);
			myScene.SetCubeAngle(cubeAngle);

			// Showing how to insert text into a string and also get FPS!!
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			// We've finished adding stuff to the window
			ImGui::End();
		}

		// Render GUI to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// This tells the renderer to actually show its contents to the screen
		// We'll get into this sort of thing at a later date - or just look up 'double buffering' if you're impatient :P
		SDL_GL_SwapWindow( window );

		
		// Limiter in case we're running really quick
		if( deltaTs < (1.0f/50.0f) )	// not sure how accurate the SDL_Delay function is..
		{
			SDL_Delay((unsigned int) (((1.0f/50.0f) - deltaTs)*1000.0f) );
		}
	}

	// If we get outside the main game loop, it means our user has requested we exit

	// Shut down GUI system
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// Our cleanup phase, hopefully fairly self-explanatory ;)
	SDL_GL_DeleteContext( glcontext );
	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
