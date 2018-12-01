/*
	Copyright 2011-2019 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_DemoState.h
	Demo state interface and programmer function declarations.

	********************************************
	*** THIS IS YOUR DEMO'S MAIN HEADER FILE ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOSTATE_H
#define __ANIMAL3D_DEMOSTATE_H


//-----------------------------------------------------------------------------
// animal3D framework includes

#include "animal3D/animal3D.h"
#include "animal3D-A3DG/animal3D-A3DG.h"


//-----------------------------------------------------------------------------
// other demo includes

#include "_utilities/a3_DemoSceneObject.h"
#include "_utilities/a3_DemoShaderProgram.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_DemoState					a3_DemoState;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

	// object maximum counts for easy array storage
	// good idea to make these numbers greater than what you actually need 
	//	and if you end up needing more just increase the count... there's 
	//	more than enough memory to hold extra objects
	enum a3_DemoStateObjectMaxCounts
	{
		demoStateMaxCount_object = 1,
		demoStateMaxCount_camera = 1,
		demoStateMaxCount_light = 1,
		demoStateMaxCount_sceneObject = demoStateMaxCount_object + demoStateMaxCount_camera + demoStateMaxCount_light,

		demoStateMaxCount_timer = 1,
		demoStateMaxCount_drawDataBuffer = 1,
		demoStateMaxCount_vertexArray = 4,
		demoStateMaxCount_drawable = 4,
		demoStateMaxCount_shaderProgram = 4,
	};

	// additional counters for demo modes
	enum a3_DemoStateModeCounts
	{
		demoStateMaxModes = 1,
		demoStateMaxSubModes = 1,
		demoStateMaxOutputModes = 1,
	};


//-----------------------------------------------------------------------------

	// persistent demo state data structure
	struct a3_DemoState
	{
		//---------------------------------------------------------------------
		// general variables pertinent to the state

		// terminate key pressed
		a3i32 exitFlag;

		// global vertical axis: Z = 0, Y = 1
		a3i32 verticalAxis;

		// asset streaming between loads enabled (careful!)
		a3i32 streaming;

		// window and full-frame dimensions
		a3ui32 windowWidth, windowHeight;
		a3ui32 frameWidth, frameHeight;
		a3i32 frameBorder;


		//---------------------------------------------------------------------
		// objects that have known or fixed instance count in the whole demo

		// text renderer
		a3i32 textInit, textMode, textModeCount;
		a3_TextRenderer text[1];

		// input
		a3_MouseInput mouse[1];
		a3_KeyboardInput keyboard[1];
		a3_XboxControllerInput xcontrol[4];

		// pointer to fast trig table
		a3f32 trigTable[4096 * 4];


		//---------------------------------------------------------------------
		// scene variables and objects

		// demo mode array: 
		//	- mode (1): which mode/pipeline is being viewed
		//	- sub-mode (1): which sub-mode/pass in the pipeline is being viewed
		//	- output (1): which output from the sub-mode/pass is being viewed
		a3ui32 demoMode, demoSubMode[demoStateMaxModes], demoOutputMode[demoStateMaxModes][demoStateMaxSubModes];
		a3ui32 demoModeCount, demoSubModeCount[demoStateMaxModes], demoOutputCount[demoStateMaxModes][demoStateMaxSubModes];

		// toggle grid in scene and axes superimposed
		a3i32 displayGrid, displaySkybox, displayWorldAxes, displayObjectAxes;
		a3i32 updateAnimation;

		// grid properties
		a3mat4 gridTransform;
		a3vec4 gridColor;

		// cameras
		a3ui32 activeCamera;

		// lights
		a3_DemoPointLight pointLight[demoStateMaxCount_light];
		a3mat4 pointLightMVP[demoStateMaxCount_light];
		a3ui32 lightCount;


		//---------------------------------------------------------------------
		// object arrays: organized as anonymous unions for two reasons: 
		//	1. easy to manage entire sets of the same type of object using the 
		//		array component
		//	2. at the same time, variables are named pointers

		// scene objects
		union {
			a3_DemoSceneObject sceneObject[demoStateMaxCount_sceneObject];
			struct {
				a3_DemoSceneObject
					cameraObject[demoStateMaxCount_camera],	// transform for cameras
					lightObject[demoStateMaxCount_light],	// transform for lights
					object[demoStateMaxCount_object];		// transform for general objects
			};
			struct {
				a3_DemoSceneObject
					mainCamera[1],

					mainLight[1],

					skyboxObject[1];
			};
		};

		// cameras
		//	- any object can have a camera "component"
		union {
			a3_DemoCamera camera[demoStateMaxCount_camera];
			struct {
				a3_DemoCamera
					sceneCamera[1];						// scene viewing cameras
			};
		};

		// timers
		union {
			a3_Timer timer[demoStateMaxCount_timer];
			struct {
				a3_Timer
					renderTimer[1];						// render FPS timer
			};
		};


		// draw data buffers
		union {
			a3_VertexBuffer drawDataBuffer[demoStateMaxCount_drawDataBuffer];
			struct {
				a3_VertexBuffer
					vbo_staticSceneObjectDrawBuffer[1];			// buffer to hold all data for static scene objects (e.g. grid)
			};
		};

		// vertex array objects
		union {
			a3_VertexArrayDescriptor vertexArray[demoStateMaxCount_vertexArray];
			struct {
				a3_VertexArrayDescriptor
					vao_position[1],							// VAO for vertex format with only position
					vao_position_color[1],						// VAO for vertex format with position and color
					vao_position_texcoord[1];					// VAO for vertex format with position and UVs
			};
		};

		// drawables
		union {
			a3_VertexDrawable drawable[demoStateMaxCount_drawable];
			struct {
				a3_VertexDrawable
					draw_grid[1],								// wireframe ground plane to emphasize scaling
					draw_axes[1],								// coordinate axes at the center of the world
					draw_skybox[1];								// skybox cube mesh
			};
		};


		// shader programs and uniforms
		union {
			a3_DemoStateShaderProgram shaderProgram[demoStateMaxCount_shaderProgram];
			struct {
				a3_DemoStateShaderProgram
					prog_drawColorUnif[1],						// draw uniform color
					prog_drawColorAttrib[1];					// draw color attribute
			};
		};


		//---------------------------------------------------------------------
	};

	
//-----------------------------------------------------------------------------

	// demo-related functions

	// idle loop
	void a3demo_input(a3_DemoState *demoState, a3f64 dt);
	void a3demo_update(a3_DemoState *demoState, a3f64 dt);
	void a3demo_render(const a3_DemoState *demoState);

	// loading
	void a3demo_loadGeometry(a3_DemoState *demoState);
	void a3demo_loadShaders(a3_DemoState *demoState);
	void a3demo_refresh(a3_DemoState *demoState);

	// unloading
	void a3demo_unloadGeometry(a3_DemoState *demoState);
	void a3demo_unloadShaders(a3_DemoState *demoState);
	void a3demo_validateUnload(const a3_DemoState *demoState);

	// other utils & setup
	void a3demo_setDefaultGraphicsState();
	void a3demo_initScene(a3_DemoState *demoState);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSTATE_H