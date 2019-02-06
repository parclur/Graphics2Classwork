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

// blending state for composition
inline void a3demo_enableCompositeBlending()
{
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

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

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);

	// alpha blending
	a3demo_enableCompositeBlending();
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
// RENDER SUB-ROUTINES

void a3demo_stencilTest(const a3_DemoState *demoState)
{
	const a3_DemoStateShaderProgram *currentDemoProgram;
	const a3_VertexDrawable *currentDrawable;
	const a3f32 testColor[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
	const a3mat4 testMmat = {
		4.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 4.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 4.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	a3mat4 testMVPmat = testMmat;
	
	// inverted small sphere in solid transparent color
	// used as our "lens" for the depth and stencil tests
	currentDemoProgram = demoState->prog_drawColorUnif;
	a3shaderProgramActivate(demoState->prog_drawColorUnif->program);

	currentDrawable = demoState->draw_sphere;
	a3real4x4Product(testMVPmat.m, demoState->camera->viewProjectionMat.m, testMmat.m);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, testMVPmat.mm);

/*
	// 1. depth buffer hax: 
	//	- see what happens with depth test disabled for all shapes
	//	- draw sphere, see where it ends up (still no depth test)
	//	- change color of sphere to fully transparent (using depth test)
	//	- switch to using color mask to the same effect (using depth test)

	// 2. draw to stencil buffer: 
	//	- render first sphere to the stencil buffer to set drawable area
	//		- don't want values for the shape to actually be drawn to 
	//			color or depth buffers, so apply a MASK for this object
	//	- enable stencil test for everything else

*/
}


//-----------------------------------------------------------------------------
// RENDER

void a3demo_render(const a3_DemoState *demoState)
{
	const a3_VertexDrawable *currentDrawable;
	const a3_DemoStateShaderProgram *currentDemoProgram;
	const a3_Framebuffer *writeFBO, *readFBO;

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
	a3mat4 modelViewProjectionBiasMat[demoStateMaxCount_object];

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


	// ****TO-DO: declare as needed
	// other tmp data


	//-------------------------------------------------------------------------
	// 0) PRE-PROCESSING PASS: render scene for pre-processing 
	//	(e.g. acquire shadow map)
	//	- activate pre-processing framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture whatever data is needed

	writeFBO = demoState->fbo_shadowmap;
	a3framebufferActivate(writeFBO);

	// clear buffers
	glClear(GL_DEPTH_BUFFER_BIT);

	// shadow mapping: render shadow-casters (inverted helps)
	currentDemoProgram = demoState->prog_transform;
	a3shaderProgramActivate(currentDemoProgram->program);

	glCullFace(GL_FRONT);
	for (k = 0, currentDrawable = demoState->draw_plane,
		currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject;
		currentSceneObject <= endSceneObject;
		++k, ++currentDrawable, ++currentSceneObject)
	{
		// calculate and use projector MVP, save for later (no bias yet)
		a3real4x4Product(modelViewProjectionBiasMat[k].m, demoState->projectorLight->viewProjectionMat.m, currentSceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionBiasMat[k].mm);
		a3vertexDrawableActivateAndRender(currentDrawable);
	}
	glCullFace(GL_BACK);


	//-------------------------------------------------------------------------
	// 1) SCENE PASS: render scene with desired shader
	//	- activate scene framebuffer
	//	- draw scene
	//		- clear buffers
	//		- render shapes using appropriate shaders
	//		- capture color and depth

	// activate framebuffer (notebook)
	writeFBO = demoState->fbo_scene;
	a3framebufferActivate(writeFBO);
	
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// draw grid aligned to world
	if (demoState->displayGrid)
	{
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
			switch (demoMode)
			{
			case 0:
				currentDemoProgram = demoState->prog_drawPhongMulti_projtex;
				break;
			case 1:
				currentDemoProgram = demoState->prog_drawPhongMulti_shadowmap;
				break;
			case 2:
				currentDemoProgram = demoState->prog_drawPhongMulti_shadowmap_projtex;
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
			a3textureActivate(demoState->tex_earth_dm, a3tex_unit06);
			a3framebufferBindDepthTexture(demoState->fbo_shadowmap, a3tex_unit07);

			// individual object requirements: 
			//	- modelviewprojection
			//	- modelview
			//	- modelview for normals
			for (k = 0, currentDrawable = demoState->draw_plane,
				currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject;
				currentSceneObject <= endSceneObject;
				++k, ++currentDrawable, ++currentSceneObject)
			{
				a3real4x4Product(modelViewMat.m, cameraObject->modelMatInv.m, currentSceneObject->modelMat.m);
				a3real4x4Product(modelViewProjectionMat.m, camera->projectionMat.m, modelViewMat.m);
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV, 1, modelViewMat.mm);
				a3demo_quickInvertTranspose_internal(modelViewMat.m);
				modelViewMat.v3 = a3zeroVec4;
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMV_nrm, 1, modelViewMat.mm);
				a3real4x4ConcatR(bias.m, modelViewProjectionBiasMat[k].m);
				a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVPB_proj, 1, modelViewProjectionBiasMat[k].mm);
				a3textureActivate(tex_dm[k], a3tex_unit00);
				a3textureActivate(tex_sm[k], a3tex_unit01);
				a3vertexDrawableActivateAndRender(currentDrawable);
			}
			break;

			// additional geometry passes
		case 1:
			break;
		}
	}


	//-------------------------------------------------------------------------
	// 2) COMPOSITE PASS: display framebuffer on full-screen quad (FSQ); 
	//		opportunity to add foreground and background objects
	//	- deactivate framebuffer
	//	- draw common background
	//		- if your clear color has zero alpha, scene will overlay correctly
	//	- activate render texture(s)
	//	- draw FSQ with appropriate program

	// draw to back buffer with depth disabled
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, 
		-demoState->frameBorder, -demoState->frameBorder, demoState->frameWidth, demoState->frameHeight);

	// ****TO-DO: for post-processing, uncomment above line 
	//	and prepare to continue drawing off-screen


	// display skybox or clear
	// - disable blending whether or not skybox is drawn
	//	-> don't need to test for skybox, and if it's not drawn, no clearing
	glDisable(GL_BLEND);
	if (demoState->displaySkybox)
	{
		// draw solid color box, inverted
		currentDrawable = demoState->draw_skybox;
		currentSceneObject = demoState->skyboxObject;
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);

		currentDemoProgram = demoState->prog_drawTexture;
		a3shaderProgramActivate(currentDemoProgram->program);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3textureActivate(demoState->tex_skybox_clouds, a3tex_unit00);

		// draw inverted box
		glCullFace(GL_FRONT);
		a3vertexDrawableActivateAndRender(currentDrawable);
		glCullFace(GL_BACK);

		// enable blending to ensure proper composition
		a3demo_enableCompositeBlending();
	}
	else
	{
		// clearing is expensive!
		// nothing do do here because FSQ will draw over everything; disabled 
		//	blending means the transparency from prior clear doesn't matter
	}

	// use unit quad as FSQ drawable
	currentDrawable = demoState->draw_unitquad;
	a3vertexDrawableActivate(currentDrawable);

	// select FBO (notebook) to use for display
	if (demoSubMode == 0)
		readFBO = demoState->fbo_scene;
	else
		readFBO = demoState->fbo_shadowmap;

	// select texture(s) (pages) to use for display
	// select color if FBO has color and either no depth or less than max output
	if (readFBO->color && (!readFBO->depthStencil || demoOutput < demoOutputCount - 1))
		a3framebufferBindColorTexture(readFBO, a3tex_unit00, demoOutput);
	else
		a3framebufferBindDepthTexture(readFBO, a3tex_unit00);

	// draw FSQ with texturing
	currentDemoProgram = demoState->prog_drawTexture;
	a3shaderProgramActivate(currentDemoProgram->program);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, a3identityMat4.mm);
	a3vertexDrawableRenderActive();


	//-------------------------------------------------------------------------
	// 3) POST-PROCESSING: process final image further; you can render FSQ 
	//		straight to back buffer with post-processing program activated, or 
	//		continue the pipeline by adding multiple FSQ passes... try one!
	//	- in previous step (compositing), instead of deactivating FBO, 
	//		activate FBO set up for compositing (composition is off-screen)
	//	- here, deactivate FBO (or activate another one for more passes)
	//	- activate post-processing program
	//	- send post-processing-related uniforms
	//	- render FSQ again

	// ****TO-DO: this
	// draw to back buffer with depth disabled


	// ****TO-DO: this
	// draw FSQ (still active) with post-processing or texturing
	glDisable(GL_BLEND);
	if (demoState->enablePostProcessing && demoSubMode == 0)
	{
	}
	else
	{
	}
//	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, a3identityMat4.mm);
//	a3vertexDrawableRenderActive();


	//-------------------------------------------------------------------------
	// OVERLAYS: done after FSQ so they appear over everything else
	//	- disable depth testing
	//	- draw overlays appropriately

	// camera to use for overlay
	if (demoSubMode == 0)
		camera = demoState->sceneCamera;
	else
		camera = demoState->projectorLight;

	// hidden volumes
	if (demoState->displayHiddenVolumes)
	{
		currentDemoProgram = demoState->prog_drawColorUnif;
		a3shaderProgramActivate(currentDemoProgram->program);

		// projector point light
		currentDrawable = demoState->draw_pointlight;
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->projectorLight->sceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3shaderUniformSendFloat(a3unif_vec4, currentDemoProgram->uColor, 1, lightCol[0].v);
		a3vertexDrawableActivateAndRender(currentDrawable);
	}

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
		// scene objects
		for (k = 0, 
			currentSceneObject = demoState->planeObject, endSceneObject = demoState->teapotObject; 
			currentSceneObject <= endSceneObject; 
			++k, ++currentSceneObject)
		{
			a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, currentSceneObject->modelMat.m);
			a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
			a3vertexDrawableRenderActive();
		}

		// other objects
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->projectorLight->sceneObject->modelMat.m);
		a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewProjectionMat.mm);
		a3vertexDrawableRenderActive();
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
		"Projective texturing",
		"Shadow mapping",
		"Shadow mapping and projective texturing",
	};
	const a3byte *subModeText[demoStateMaxModes][demoStateMaxSubModes] = {
		{
			"Draw scene",
			"Shadow map",
		},
		{
			"Draw scene",
			"Shadow map",
		},
		{
			"Draw scene",
			"Shadow map",
		},
	};
	const a3byte *outputText[demoStateMaxModes][demoStateMaxSubModes][demoStateMaxOutputModes] = {
		{
			{
				"Color buffer",
				"Depth buffer",
			},
			{
				"Depth buffer (shadow map)",
			},
		},
		{
			{
				"Color buffer",
				"Depth buffer",
			},
			{
				"Depth buffer (shadow map)",
			},
		},
		{
			{
				"Color buffer",
				"Depth buffer",
			},
			{
				"Depth buffer (shadow map)",
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
		"HIDDEN VOLUMES (toggle 'h') %d ", demoState->displayHiddenVolumes);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"ANIMATION updates (toggle 'm') %d", demoState->updateAnimation);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"POST-PROCESSING (toggle 'n') %d", demoState->enablePostProcessing);
	a3textDraw(demoState->text, textAlign, textOffset += textOffsetDelta, textDepth, col.r, col.g, col.b, col.a,
		"SINGLE LIGHT ONLY (toggle 'l') %d", demoState->singleLight);

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
