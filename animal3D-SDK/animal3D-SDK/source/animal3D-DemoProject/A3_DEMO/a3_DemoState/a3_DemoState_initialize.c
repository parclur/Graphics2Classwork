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
	
	a3_DemoState_initialize.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     INITIALIZATION in this file.             ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// INITIALIZE

// initialize non-asset objects
void a3demo_initScene(a3_DemoState *demoState)
{
	a3ui32 i;
	a3_DemoCamera *camera;
	a3_DemoPointLight *pointLight;

	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 20.0f;
	const a3vec3 sceneCameraStartPos = {
		demoState->verticalAxis ? +sceneCameraAxisPos : +sceneCameraAxisPos,
		demoState->verticalAxis ? +sceneCameraAxisPos : -sceneCameraAxisPos,
		demoState->verticalAxis ? +sceneCameraAxisPos : +sceneCameraAxisPos,
	};
	const a3vec3 sceneCameraStartEuler = {
		demoState->verticalAxis ? -35.0f : +55.0f,
		demoState->verticalAxis ? +45.0f : + 0.0f,
		demoState->verticalAxis ? + 0.0f : +45.0f,
	};


	// all objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoState->sceneObject + i);

	// cameras
	for (i = 0; i < demoStateMaxCount_camera; ++i)
	{
		a3demo_setCameraSceneObject(demoState->camera + i, demoState->cameraObject + i);
		a3demo_initCamera(demoState->camera + i);
	}

	// camera params
	demoState->activeCamera = 0;

	// scene cameras
	camera = demoState->sceneCamera + 0;
	camera->perspective = a3true;
	camera->fovy = a3realFortyFive;
	camera->znear = 1.0f;
	camera->zfar = 1000.0f;
	camera->ctrlMoveSpeed = 10.0f;
	camera->ctrlRotateSpeed = 5.0f;
	camera->ctrlZoomSpeed = 5.0f;
	camera->sceneObject->position = sceneCameraStartPos;
	camera->sceneObject->euler = sceneCameraStartEuler;


	// init transforms
	if (demoState->verticalAxis)
	{
		// vertical axis is Y
	}
	else
	{
		// vertical axis is Z
	}


	// demo modes
	demoState->demoModeCount = 3;
	demoState->demoMode = 0;

	// modes/pipelines: 
	// A: Phong shading mode
	//	- draw objects sub-mode
	//		- color buffer
	//		- depth buffer
	//	- stencil test sub-mode (same outputs)
	// B: Phong MRT mode
	//	- draw objects sub-mode
	//		- position attribute
	//		- normal attribute
	//		- texcoord attribute
	//		- diffuse map
	//		- specular map
	//		- diffuse shading
	//		- specular shading
	//		- Phong shading
	//		- depth buffer
	// C: custom MRT
	//	- draw objects sub-mode
	//		- 4x custom outputs
	//		- depth buffer
	demoState->demoSubModeCount[0] = 2;
	demoState->demoOutputCount[0][0] = 2;
	demoState->demoOutputCount[0][1] = 2;
	demoState->demoSubModeCount[1] = 1;
	demoState->demoOutputCount[1][0] = 9;
	demoState->demoSubModeCount[2] = 1;
	demoState->demoOutputCount[2][0] = 5;


	// initialize other objects
	demoState->displayGrid = 1;
	demoState->displayWorldAxes = 1;
	demoState->displayObjectAxes = 1;
	demoState->displaySkybox = 1;
	demoState->updateAnimation = 1;


	// lights
	demoState->lightCount = demoStateMaxCount_light;

	// first light is hard-coded
	pointLight = demoState->pointLight;
	pointLight->worldPos = a3wVec4;
	if (demoState->verticalAxis)
		pointLight->worldPos.y = 10.0f;
	else
		pointLight->worldPos.z = 10.0f;
	pointLight->radius = 100.0f;
	pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	pointLight->color = a3oneVec4;

	// all other lights are random
	a3randomSetSeed(0);
	for (i = 1, pointLight = demoState->pointLight + i;
		i < demoStateMaxCount_light;
		++i, ++pointLight)
	{
		// set to zero vector
		pointLight->worldPos = a3wVec4;

		// random positions
		pointLight->worldPos.x = a3randomRange(-10.0f, +10.0f);
		if (demoState->verticalAxis)
		{
			pointLight->worldPos.z = -a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.y = -a3randomRange(-2.0f, +8.0f);
		}
		else
		{
			pointLight->worldPos.y = a3randomRange(-10.0f, +10.0f);
			pointLight->worldPos.z = a3randomRange(-2.0f, +8.0f);
		}

		// random colors
		pointLight->color.r = a3randomNormalized();
		pointLight->color.g = a3randomNormalized();
		pointLight->color.b = a3randomNormalized();
		pointLight->color.a = a3realOne;

		// random radius
		pointLight->radius = a3randomRange(10.0f, 50.0f);
		pointLight->radiusInvSq = a3recip(pointLight->radius * pointLight->radius);
	}


	// position scene objects
	demoState->sphereObject->scale.x = 2.0f;
	demoState->cylinderObject->scale.x = 4.0f;
	demoState->cylinderObject->scale.y = demoState->cylinderObject->scale.z = 2.0f;
	demoState->torusObject->scale.x = 2.0f;
	demoState->sphereObject->scaleMode = 1;		// uniform
	demoState->cylinderObject->scaleMode = 2;	// non-uniform
	demoState->torusObject->scaleMode = 1;		// uniform

	demoState->sphereObject->position.x = +8.0f;
	demoState->torusObject->position.x = -8.0f;
	if (demoState->verticalAxis)
	{
		demoState->planeObject->position.y = -4.0f;
		demoState->cylinderObject->position.z = -8.0f;
		demoState->teapotObject->position.z = +8.0f;
	}
	else
	{
		demoState->planeObject->position.z = -4.0f;
		demoState->cylinderObject->position.y = +8.0f;
		demoState->teapotObject->position.y = -8.0f;
	}
}


//-----------------------------------------------------------------------------
