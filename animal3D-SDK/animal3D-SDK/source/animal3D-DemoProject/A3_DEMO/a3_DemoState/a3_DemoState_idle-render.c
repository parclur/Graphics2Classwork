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
	
	a3_DemoState_idle-render.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     RENDERING THE STATS in this file.        ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


// OpenGL
#ifdef _WIN32
#include <Windows.h>
#include <GL/GL.h>
#else	// !_WIN32
#include <OpenGL/gl3.h>
#endif	// _WIN32


//-----------------------------------------------------------------------------
// SETUP UTILITIES

// set default GL state
void a3demo_setDefaultGraphicsState()
{
	const a3f32 lineWidth = 2.0f;
	const a3f32 pointSize = 4.0f;

	// lines and points
	glLineWidth(lineWidth);
	glPointSize(pointSize);

	// backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// alpha blending
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
}


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline a3real4x4r a3demo_quickInverseTranspose_internal(a3real4x4p m_out, const a3real4x4p m_in)
{
	// the basis for this calculation is "inverse transpose" which 
	//	will result in the scale component inverting while rotation 
	//	stays the same
	// expanding the formula using fundamendal matrix identities 
	//	yields this optimized version
	// translation column is untouched
	a3real4ProductS(m_out[0], m_in[0], a3real3LengthSquaredInverse(m_in[0]));
	a3real4ProductS(m_out[1], m_in[1], a3real3LengthSquaredInverse(m_in[1]));
	a3real4ProductS(m_out[2], m_in[2], a3real3LengthSquaredInverse(m_in[2]));
	a3real4SetReal4(m_out[3], m_in[3]);
	return m_out;
}

inline a3real4x4r a3demo_quickInvertTranspose_internal(a3real4x4p m_inout)
{
	a3real4MulS(m_inout[0], a3real3LengthSquaredInverse(m_inout[0]));
	a3real4MulS(m_inout[1], a3real3LengthSquaredInverse(m_inout[1]));
	a3real4MulS(m_inout[2], a3real3LengthSquaredInverse(m_inout[2]));
	return m_inout;
}

inline a3real4x4r a3demo_quickTransposedZeroBottomRow(a3real4x4p m_out, const a3real4x4p m_in)
{
	a3real4x4GetTransposed(m_out, m_in);
	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3realZero;
	return m_out;
}


// forward declare text render functions
void a3demo_render_controls(const a3_DemoState *demoState);
void a3demo_render_data(const a3_DemoState *demoState);


//-----------------------------------------------------------------------------
// RENDER

void a3demo_render(const a3_DemoState *demoState)
{
	const a3_VertexDrawable *currentDrawable;
	const a3_DemoStateShaderProgram *currentDemoProgram;

	a3ui32 i, j, k;

	// RGB
	const a3vec4 rgba4[] = {
		{ 1.0f, 0.0f, 0.0f, 1.0f },	// red
		{ 0.0f, 1.0f, 0.0f, 1.0f },	// green
		{ 0.0f, 0.0f, 1.0f, 1.0f },	// blue
		{ 0.0f, 1.0f, 1.0f, 1.0f },	// cyan
		{ 1.0f, 0.0f, 1.0f, 1.0f },	// magenta
		{ 1.0f, 1.0f, 0.0f, 1.0f },	// yellow
		{ 1.0f, 0.5f, 0.0f, 1.0f },	// orange
		{ 0.0f, 0.5f, 1.0f, 1.0f },	// sky blue
		{ 0.5f, 0.5f, 0.5f, 1.0f },	// solid grey
		{ 0.5f, 0.5f, 0.5f, 0.5f },	// translucent grey
	};
	const a3real 
		*const red = rgba4[0].v, *const green = rgba4[1].v, *const blue = rgba4[2].v,
		*const cyan = rgba4[3].v, *const magenta = rgba4[4].v, *const yellow = rgba4[5].v,
		*const orange = rgba4[6].v, *const skyblue = rgba4[7].v,
		*const grey = rgba4[8].v, *const grey_t = rgba4[9].v;


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};

	// final model matrix and full matrix stack
	a3mat4 modelViewProjectionMat = a3identityMat4, modelViewMat = a3identityMat4;

	// camera used for drawing
	const a3_DemoCamera *camera = demoState->camera + demoState->activeCamera;
	const a3_DemoSceneObject *cameraObject = camera->sceneObject;

	// current scene object being rendered, for convenience
	const a3_DemoSceneObject *currentSceneObject, *endSceneObject;

	// display mode for current pipeline
	// ensures we don't go through the whole pipeline if not needed
	const a3ui32 demoMode = demoState->demoMode, demoPipelineCount = demoState->demoModeCount;
	const a3ui32 demoSubMode = demoState->demoSubMode[demoMode], demoPassCount = demoState->demoSubModeCount[demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoMode][demoSubMode], demoOutputCount = demoState->demoOutputCount[demoMode][demoSubMode];


	// temp light data
	a3vec4 lightPos_eye[demoStateMaxCount_light];
	a3vec4 lightCol[demoStateMaxCount_light];
	a3f32 lightSz[demoStateMaxCount_light];

	// temp texture pointers for scene objects
	const a3_Texture *tex_dm[] = {
		demoState->tex_stone_dm,
		demoState->tex_earth_dm, 
		demoState->tex_stone_dm, 
		demoState->tex_mars_dm,
		demoState->tex_checker,
	};
	const a3_Texture *tex_sm[] = {
		demoState->tex_stone_dm,
		demoState->tex_earth_sm,
		demoState->tex_stone_dm,
		demoState->tex_mars_sm,
		demoState->tex_checker,
	};


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- render shapes using appropriate shaders
	//	- capture color and depth

	// display skybox or clear
	if (demoState->displaySkybox)
	{
		// draw solid color box, inverted
		currentDrawable = demoState->draw_skybox;
		currentSceneObject = demoState->skyboxObject;
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);

		// dummy draw code: render with solid color
		// ****TO-DO: activate texturing program
		//	-> remove dummy draw code (the next 4 lines of code)
		//	-> select and activate program (2 lines)
		//	-> send correct MVP matrix as uniform (1 line)
		//	-> activate skybox texture (1 line)
		//		(pro tip: sampler uniform already sent, only activate texture)
		currentDemoProgram = demoState->prog_drawTexture; //select program
		a3shaderProgramActivate(currentDemoProgram->program); //activate program
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, a3identityMat4.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, grey);

		glDepthFunc(GL_ALWAYS);
		glCullFace(GL_FRONT);
		a3vertexDrawableActivateAndRender(currentDrawable);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
	}
	else
	{
		// clearing is expensive!
		// instead, draw skybox and force depth to farthest possible value in scene
		// we could call this a "skybox clear" because it serves both purposes
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	if (demoState->displayGrid)
	{
		// draw grid aligned to world
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);
		currentDrawable = demoState->draw_grid;
		modelViewProjectionMat = camera->viewProjectionMat;
		a3real4x4ConcatL(modelViewProjectionMat.m, demoState->gridTransform.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, demoState->gridColor.v);
		a3vertexDrawableActivateAndRender(currentDrawable);
	}


	// copy temp light data
	for (k = 0; k < demoState->lightCount; ++k)
	{
		lightPos_eye[k] = demoState->pointLight[k].viewPos;
		lightCol[k] = demoState->pointLight[k].color;
		lightSz[k] = demoState->pointLight[k].radiusInvSq;
	}


	// draw objects: 
	//	- correct "up" axis if needed
	//	- calculate proper transformation matrices
	//	- move lighting objects' positions into object space as needed
	//	- send uniforms
	//	- draw

	// support multiple geometry passes
	for (i = 0, j = 1; i < j; ++i)
	{
		// select forward algorithm
		switch (i)
		{
			// forward pass
		case 0:
			// select depending on mode and/or sub-mode
			// ****TO-DO: activate program for current mode
			//	-> complete the switch cases
			//		(all draw code below is set up correctly for all cases)
			switch (demoMode)
			{
			case 0:
				currentDemoProgram = demoState->prog_drawColorUnif;
				break;
			}
			a3shaderProgramActivate(currentDemoProgram->program);

			// send shared data: 
			//	- projection matrix
			//	- light data
			//	- activate texture atlases
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uP, 1, camera->projectionMat.mm);
			a3shaderUniformSendInt(a3unif_single, currentDemoProgram->uLightCt, 1, &demoState->lightCount);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uLightPos, demoState->lightCount, lightPos_eye->v);
			a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uLightCol, demoState->lightCount, lightCol->v);
			a3shaderUniformSendFloat(a3unif_single, currentDemoProgram->uLightSz, demoState->lightCount, lightSz);
			a3textureActivate(demoState->tex_ramp_dm, a3tex_unit04);
			a3textureActivate(demoState->tex_ramp_sm, a3tex_unit05);

			// individual object requirements: 
			//	- modelviewprojection
			//	- modelview
			//	- modelview for normals
			for (k = 0, currentDrawable = demoState->draw_plane,
				currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject;
				currentSceneObject <= endSceneObject;
				++k, ++currentDrawable, ++currentSceneObject)
			{
				a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, rgba4[(k % 5) + 3].v);
				a3textureActivate(tex_dm[k], a3tex_unit00);
				a3textureActivate(tex_sm[k], a3tex_unit01);
				a3real4x4Product(modelViewMat.m, cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
				a3real4x4Product(modelViewProjectionMat.m, camera->projectionMat.m, modelViewMat.m);
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
				a3demo_quickInvertTranspose_internal(modelViewMat.m);
				modelViewMat.v3 = a3zeroVec4;
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV_nrm, 1, modelViewMat.mm);
				a3vertexDrawableActivateAndRender(currentDrawable);
			}
			break;

			// additional geometry passes
		case 1:
			break;
		}
	}


	//-------------------------------------------------------------------------
	// overlays

	// superimpose axes
	// draw coordinate axes in front of everything
	glDisable(GL_DEPTH_TEST);

	currentDemoProgram = demoState->prog_drawColorAttrib;
	a3shaderProgramActivate(currentDemoProgram->program);
	currentDrawable = demoState->draw_axes;
	a3vertexDrawableActivate(currentDrawable);

	// center of world from current viewer
	// also draw other viewer/viewer-like object in scene
	if (demoState->displayWorldAxes)
	{
		modelViewProjectionMat = camera->viewProjectionMat;
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3vertexDrawableRenderActive();
	}

	// individual objects
	if (demoState->displayObjectAxes)
	{
		for (k = 0, 
			currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject; 
			currentSceneObject <= endSceneObject; 
			++k, ++currentSceneObject)
		{
			a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3vertexDrawableRenderActive();
		}
	}

	glEnable(GL_DEPTH_TEST);
	


	// deactivate things
	a3vertexDrawableDeactivate();
	a3shaderProgramDeactivate();
	a3textureDeactivate(a3tex_unit00);


	// text
	if (demoState->textInit)
	{
		// set viewport to window size and disable depth
		glViewport(0, 0, demoState->windowWidth, demoState->windowHeight);
		glDisable(GL_DEPTH_TEST);

		// choose text render mode
		switch (demoState->textMode)
		{
		case 1:
			a3demo_render_controls(demoState);
			break;
		case 2: 
			a3demo_render_data(demoState);
			break;
		}

		// re-enable depth
		glEnable(GL_DEPTH_TEST);
	}
}


//-----------------------------------------------------------------------------
// RENDER TEXT

// controls
void a3demo_render_controls(const a3_DemoState *demoState)
{
	// display mode info
	const a3byte *modeText[demoStateMaxModes] = {
		"Shader playground scene",
		"Scene with Phong shading",
		"Scene with non-photorealistic shading",
	};
	const a3byte *subModeText[demoStateMaxModes][demoStateMaxSubModes] = {
		{
			"Draw scene objects",
		},
		{
			"Draw scene objects",
		},
		{
			"Draw scene objects",
		},
	};
	const a3byte *outputText[demoStateMaxModes][demoStateMaxSubModes][demoStateMaxOutputModes] = {
		{
			{
				"Back buffer",
			},
		},
		{
			{
				"Back buffer",
			},
		},
		{
			{
				"Back buffer",
			},
		},
	};

	// current modes
	const a3ui32 demoMode = demoState->demoMode, demoPipelineCount = demoState->demoModeCount;
	const a3ui32 demoSubMode = demoState->demoSubMode[demoMode], demoPassCount = demoState->demoSubModeCount[demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoMode][demoSubMode], demoOutputCount = demoState->demoOutputCount[demoMode][demoSubMode];

	// text color
	const a3vec4 col = {
		demoState->displaySkybox ? a3realZero : a3realOne,
		demoState->displaySkybox ? a3realQuarter : a3realOne,
		demoState->displaySkybox ? a3realHalf : a3realOne,
		a3realOne
	};

	// amount to offset text as each line is rendered
	a3f32 textAlign = -0.98f;
	a3f32 textOffset = 1.00f;
	a3f32 textDepth = -1.00f;
	const a3f32 textOffsetDelta = -0.10f;

	// modes
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Demo mode (%u / %u) (',' prev | next '.'): %s", demoMode + 1, demoPipelineCount, modeText[demoMode]);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"    Sub-mode (%u / %u) ('<' | '>'): %s", demoSubMode + 1, demoPassCount, subModeText[demoMode][demoSubMode]);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"        Output (%u / %u) ('{' | '}'): %s", demoOutput + 1, demoOutputCount, outputText[demoMode][demoSubMode][demoOutput]);

	// toggles
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"ACTIVE CAMERA ('c' | 'v'): %d / %d", demoState->activeCamera + 1, demoStateMaxCount_camera);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"GRID in scene (toggle 'g') %d | SKYBOX backdrop ('b') %d", demoState->displayGrid, demoState->displaySkybox);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"WORLD AXES (toggle 'x') %d | OBJECT AXES ('z') %d", demoState->displayWorldAxes, demoState->displayObjectAxes);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"ANIMATION updates (toggle 'm') %d", demoState->updateAnimation);

	//  move down
	textOffset = -0.5f;

	// display controls
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"Xbox controller camera control: ");
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"    Left joystick = rotate | Right joystick, triggers = move");
	}
	else
	{
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"Keyboard/mouse camera control: ");
		a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
			"    Left click & drag = rotate | WASDEQ = move | wheel = zoom");
	}

	// global controls
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Toggle text display:        't' (toggle) | 'T' (alloc/dealloc) ");
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"Reload all shader programs: 'P' ****CHECK CONSOLE FOR ERRORS!**** ");
}

// scene data (HUD)
void a3demo_render_data(const a3_DemoState *demoState)
{
	// text color
	const a3vec4 col = {
		demoState->displaySkybox ? a3realZero : a3realOne,
		demoState->displaySkybox ? a3realQuarter : a3realOne,
		demoState->displaySkybox ? a3realHalf : a3realOne,
		a3realOne
	};

	// amount to offset text as each line is rendered
	a3f32 textAlign = -0.98f;
	a3f32 textOffset = 1.00f;
	a3f32 textDepth = -1.00f;
	const a3f32 textOffsetDelta = -0.10f;

	// move down
	textOffset = +0.9f;

	// display some data
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"t = %+.3lf ", demoState->renderTimer->totalTime);
}


//-----------------------------------------------------------------------------
