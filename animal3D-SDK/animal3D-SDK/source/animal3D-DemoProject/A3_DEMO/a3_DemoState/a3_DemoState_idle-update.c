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
	
	a3_DemoState_idle-update.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     UPDATING THE STATE in this file.         ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline void a3demo_applyScale_internal(a3_DemoSceneObject *sceneObject, a3real4x4p s)
{
	if (sceneObject->scaleMode)
	{
		if (sceneObject->scaleMode == 1)
		{
			s[0][0] = s[1][1] = s[2][2] = sceneObject->scale.x;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverseUniformScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
		else
		{
			s[0][0] = sceneObject->scale.x;
			s[1][1] = sceneObject->scale.y;
			s[2][2] = sceneObject->scale.z;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverse(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
	}
	else
		a3real4x4TransformInverseIgnoreScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
}


//-----------------------------------------------------------------------------
// UPDATE SUB-ROUTINES

// update for main render pipeline
void a3demo_update_main(a3_DemoState *demoState, a3f64 dt)
{
	a3ui32 i;

	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;

	const a3i32 useVerticalY = demoState->verticalAxis;

	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2Y = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2X = {
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};


	// tmp matrix for scale
	a3mat4 scaleMat = a3identityMat4;

	// active camera
	a3_DemoCamera *camera = demoState->camera + demoState->activeCamera;
	a3_DemoSceneObject *cameraObject = camera->sceneObject;
	a3_DemoSceneObject *currentSceneObject;

	// light pointers
	a3_DemoPointLight *pointLight;
//	a3mat4 *lightMVPptr, *lightMVPBptr;
//	a3ui32 tmpLightCount, tmpBlockLightCount;


	// do simple animation
	if (useVerticalY)
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.y += dr;
			currentSceneObject->euler.y = a3trigValid_sind(currentSceneObject->euler.y);
		}
	}
	else
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.z += dr;
			currentSceneObject->euler.z = a3trigValid_sind(currentSceneObject->euler.z);
		}
	}


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update cameras/projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateCameraViewProjection(demoState->camera + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3identityMat4;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = camera->sceneObject->modelMat.v3;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3wVec4;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;


	// update lights
	for (i = 0, pointLight = demoState->forwardPointLight + i;
		i < demoState->forwardLightCount;
		++i, ++pointLight)
	{
		// convert to view space and retrieve view position
		a3real4Real4x4Product(pointLight->viewPos.v, cameraObject->modelMatInv.m, pointLight->worldPos.v);
	}

	// send point light data
	pointLight = demoState->forwardPointLight;
	a3bufferFill(demoState->ubo_pointLight, 0, demoState->forwardLightCount * sizeof(a3_DemoPointLight), pointLight, 0);
	demoState->ubo_pointLight->used[0] = 0;

/*
	for (i = 0, pointLight = demoState->deferredPointLight + i,
		lightMVPptr = demoState->deferredLightMVP + i,
		lightMVPBptr = demoState->deferredLightMVPB + i;
		i < demoState->deferredLightCount;
		++i, ++pointLight, ++lightMVPptr, ++lightMVPBptr)
	{
		// set light scale and world position
		a3real4x4SetScale(lightMVPptr->m, pointLight->radius);
		lightMVPptr->v3 = pointLight->worldPos;

		// convert to view space and retrieve view position
		a3real4x4ConcatR(cameraObject->modelMatInv.m, lightMVPptr->m);
		pointLight->viewPos = lightMVPptr->v3;

		// complete by converting to clip space
		a3real4x4ConcatR(camera->projectionMat.m, lightMVPptr->m);

		// calculate biased clip as well
		a3real4x4Product(lightMVPBptr->m, bias.m, lightMVPptr->m);
	}

	// upload buffer data
	tmpLightCount = demoState->deferredLightCount;
	if (tmpLightCount && demoState->lightingPipelineMode == demoStatePipelineMode_deferredLighting)
	{
		demoState->deferredLightBlockCount = (tmpLightCount - 1) / demoStateMaxCount_lightVolumePerBlock + 1;
		for (i = 0, pointLight = demoState->deferredPointLight,
			lightMVPptr = demoState->deferredLightMVP, lightMVPBptr = demoState->deferredLightMVPB; i < demoState->deferredLightBlockCount;
			++i, tmpLightCount -= tmpBlockLightCount,
			pointLight += demoStateMaxCount_lightVolumePerBlock,
			lightMVPptr += demoStateMaxCount_lightVolumePerBlock, lightMVPBptr += demoStateMaxCount_lightVolumePerBlock)
		{
			// upload data for current light set
			tmpBlockLightCount = a3minimum(tmpLightCount, demoStateMaxCount_lightVolumePerBlock);
			demoState->deferredLightCountPerBlock[i] = tmpBlockLightCount;
			a3bufferFill(demoState->ubo_transformMVP_light + i, 0, tmpBlockLightCount * sizeof(a3mat4), lightMVPptr, 0);
			a3bufferFill(demoState->ubo_transformMVPB_light + i, 0, tmpBlockLightCount * sizeof(a3mat4), lightMVPBptr, 0);
			a3bufferFill(demoState->ubo_pointLight + i, 0, tmpBlockLightCount * sizeof(a3_DemoPointLight), pointLight, 0);

			// reset used flag so we can always fill buffers
			demoState->ubo_transformMVP_light[i].used[0] = 0;
			demoState->ubo_transformMVPB_light[i].used[0] = 0;
			demoState->ubo_pointLight[i].used[0] = 0;
		}
	}
	else
	{
		demoState->deferredLightBlockCount = 0;
	}
*/

	// correct rotations as needed
	if (useVerticalY)
	{
		// plane's axis is Z
		a3real4x4ConcatL(demoState->planeObject->modelMat.m, convertZ2Y.m);

		// sphere's axis is Z
		a3real4x4ConcatL(demoState->sphereObject->modelMat.m, convertZ2Y.m);
	}
	else
	{
		// need to rotate skybox if Z-up
		a3real4x4ConcatL(demoState->skyboxObject->modelMat.m, convertY2Z.m);

		// teapot's axis is Y
		a3real4x4ConcatL(demoState->teapotObject->modelMat.m, convertY2Z.m);
	}


	// apply scales
	a3demo_applyScale_internal(demoState->sphereObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->cylinderObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->torusObject, scaleMat.m);
	a3demo_applyScale_internal(demoState->teapotObject, scaleMat.m);

/*
	// simple animation controller
	if (demoState->targetCount && demoState->updateAnimation)
	{
		// time step for controller
		demoState->targetTime += (a3real)dt;

		// if we surpass the time for one target
		if (demoState->targetTime >= demoState->targetDuration)
		{
			demoState->targetTime -= demoState->targetDuration;
			demoState->targetIndexPrev = demoState->targetIndex;
			demoState->targetIndex = demoState->targetIndexNext;
			demoState->targetIndexNext = demoState->targetIndexNextNext;
			demoState->targetIndexNextNext = (demoState->targetIndexNextNext + 1) % demoState->targetCount;
		}

		// in any case calculate interpolation param
		demoState->targetParam = demoState->targetTime * demoState->targetDurationInv;
	}
*/
}

/*
// update for curve drawing
void a3demo_update_curve(a3_DemoState *demoState, a3f64 dt)
{
	a3ui32 i;


	// active camera
	a3_DemoCamera *camera = demoState->camera + demoState->activeCamera;
	a3_DemoSceneObject *cameraObject = camera->sceneObject;
	a3_DemoSceneObject *currentSceneObject;


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update cameras/projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateCameraViewProjection(demoState->camera + i);


	// simple animation controller
	if (demoState->curveWaypointCount > 1 && demoState->updateAnimation)
	{
		// time step for controller
		demoState->curveSegmentTime += (a3real)dt;

		// if we surpass the time for one segment
		if (demoState->curveSegmentTime >= demoState->curveSegmentDuration)
		{
			demoState->curveSegmentTime -= demoState->curveSegmentDuration;
			demoState->curveSegmentIndex = (demoState->curveSegmentIndex + 1) % (demoState->curveWaypointCount - 1);
		}

		// in any case calculate interpolation param
		demoState->curveSegmentParam = demoState->curveSegmentTime * demoState->curveSegmentDurationInv;

		// interpolate object position
		// ****TO-DO: choose interpolation algorithm from below
		currentSceneObject = demoState->curveFollowObject;
	
		// lerp
	//	a3real3Lerp(currentSceneObject->position.v,
	//		demoState->curveWaypoint[demoState->curveSegmentIndex + 0].v,
	//		demoState->curveWaypoint[demoState->curveSegmentIndex + 1].v,
	//		demoState->curveSegmentParam);
		
		// Catmull-Rom
	//	a3real3CatmullRom(currentSceneObject->position.v,
	//		demoState->curveWaypoint[a3maximum(0, (a3i32)demoState->curveSegmentIndex - 1)].v,
	//		demoState->curveWaypoint[demoState->curveSegmentIndex + 0].v,
	//		demoState->curveWaypoint[demoState->curveSegmentIndex + 1].v,
	//		demoState->curveWaypoint[a3minimum(demoState->curveSegmentIndex + 2, demoState->curveWaypointCount - 1)].v,
	//		demoState->curveSegmentParam);
		
		// cubic Hermite
		a3real3HermiteControl(currentSceneObject->position.v,
			demoState->curveWaypoint[demoState->curveSegmentIndex + 0].v,
			demoState->curveWaypoint[demoState->curveSegmentIndex + 1].v,
			demoState->curveHandle[demoState->curveSegmentIndex + 0].v,
			demoState->curveHandle[demoState->curveSegmentIndex + 1].v,
			demoState->curveSegmentParam);
		
	}

	// fill curve uniform buffers
	a3bufferFill(demoState->ubo_curveWaypoint, 0, demoState->curveWaypointCount * sizeof(a3vec4), demoState->curveWaypoint->v, 0);
	a3bufferFill(demoState->ubo_curveHandle, 0, demoState->curveWaypointCount * sizeof(a3vec4), demoState->curveHandle->v, 0);
	demoState->ubo_curveWaypoint->used[0] = 0;
	demoState->ubo_curveHandle->used[0] = 0;
}
*/
/*
// update for skeletal animation
void a3demo_update_skeletal(a3_DemoState *demoState, a3f64 dt)
{
	a3ui32 i, j;

	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;

	const a3i32 useVerticalY = demoState->verticalAxis;

	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2Y = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2X = {
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};


	// tmp matrix for scale
//	a3mat4 scaleMat = a3identityMat4;

	// active camera
	a3_DemoCamera *camera = demoState->camera + demoState->activeCamera;
	a3_DemoSceneObject *cameraObject = camera->sceneObject;
//	a3_DemoSceneObject *currentSceneObject;


	// other objects
	a3_HierarchyState *currentHierarchyState;
	const a3_HierarchyPoseGroup *currentHierarchyPoseGroup;
	const a3_Hierarchy *currentHierarchy;


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update cameras/projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateCameraViewProjection(demoState->camera + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3identityMat4;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = camera->sceneObject->modelMat.v3;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3wVec4;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;


	// update animation: 
	//	-> copy pose from set to state (pro tip: seems pointless but it is not)
	//	-> convert the current pose to transforms
	//	-> forward kinematics
	//	-> skinning matrices
	currentHierarchyState = demoState->hierarchyState_skel + demoState->editSkeletonIndex;
	currentHierarchyPoseGroup = currentHierarchyState->poseGroup;
	currentHierarchy = currentHierarchyPoseGroup->hierarchy;

	a3hierarchyPoseCopy(currentHierarchyState->localPose,
		currentHierarchyPoseGroup->pose + 0, currentHierarchy->numNodes);
	a3hierarchyPoseConvert(currentHierarchyState->localSpace,
		currentHierarchyState->localPose, currentHierarchy->numNodes, 0);
	a3kinematicsSolveForward(demoState->hierarchyState_skel);
//	a3hierarchyStateUpdateObjectBindToCurrent(currentHierarchyState, ???);


	// update buffers: 
	//	-> calculate and store bone transforms
	//	-> calculate and store joint transforms
	//	-> calculate and store skinning transforms
	{
		a3mat4 modelViewProjectionMat, localModelViewProjectionMat[128];
		
		// update common MVP
		a3real4x4Product(modelViewProjectionMat.m, camera->viewProjectionMat.m, demoState->skeletonObject->modelMat.m);

		// calculate and send bone transforms
		// need to make the bone point from parent to current
		a3real4x4SetScale(localModelViewProjectionMat[0].m, a3realZero);
		a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[0].m);
		for (i = 1; i < currentHierarchy->numNodes; ++i)
		{
			j = currentHierarchy->nodes[i].parentIndex;
			localModelViewProjectionMat[i] = currentHierarchyState->objectSpace->transform[j];
			a3real3Diff(localModelViewProjectionMat[i].v2.v,
				currentHierarchyState->objectSpace->transform[i].v3.v,
				currentHierarchyState->objectSpace->transform[j].v3.v);
			a3real3CrossUnit(localModelViewProjectionMat[i].v0.v,
				(a3isNotNearZero(localModelViewProjectionMat[i].m20) || a3isNotNearZero(localModelViewProjectionMat[i].m21)) ? a3zVec3.v : a3yVec3.v,
				localModelViewProjectionMat[i].v2.v);
			a3real3CrossUnit(localModelViewProjectionMat[i].v1.v,
				localModelViewProjectionMat[i].v2.v,
				localModelViewProjectionMat[i].v0.v);
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferFill(demoState->ubo_transformLMVP_bone, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat, 0);

		// calculate and send joint transforms
		for (i = 0; i < currentHierarchy->numNodes; ++i)
		{
			a3real4x4SetScale(localModelViewProjectionMat[i].m, 0.2f);
			a3real4x4ConcatR(currentHierarchyState->objectSpace->transform[i].m, localModelViewProjectionMat[i].m);
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferFill(demoState->ubo_transformLMVP_joint, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat, 0);
		
		// calculate and send skinning matrices
		for (i = 0; i < currentHierarchy->numNodes; ++i)
		{
			localModelViewProjectionMat[i] = currentHierarchyState->objectSpaceBindToCurrent->transform[i];
			a3real4x4ConcatR(modelViewProjectionMat.m, localModelViewProjectionMat[i].m);
		}
		a3bufferFill(demoState->ubo_transformBindPoseToCurrentPose_joint, 0, currentHierarchy->numNodes * sizeof(a3mat4), localModelViewProjectionMat, 0);

		// reset used flags
		demoState->ubo_transformLMVP_bone->used[0] = 0;
		demoState->ubo_transformLMVP_joint->used[0] = 0;
		demoState->ubo_transformBindPoseToCurrentPose_joint->used[0] = 0;
	}
}
*/

// update for ray-tracing
void a3demo_update_raytrace(a3_DemoState *demoState, a3f64 dt)
{
	a3ui32 i;

	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;

	const a3i32 useVerticalY = demoState->verticalAxis;

	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2Y = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};
	const a3mat4 convertZ2X = {
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, +1.0f, 0.0f, 0.0f,
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};


	// bias matrix
	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f,
	};


	// tmp matrix for scale
	a3mat4 scaleMat = a3identityMat4;

	// active camera
	a3_DemoCamera *camera = demoState->camera + demoState->activeCamera;
	a3_DemoSceneObject *cameraObject = camera->sceneObject;
	a3_DemoSceneObject *currentSceneObject;

	// light pointers
	a3_DemoPointLight *pointLight;
//	a3mat4 *lightMVPptr, *lightMVPBptr;
//	a3ui32 tmpLightCount, tmpBlockLightCount;


	// do simple animation
	if (useVerticalY)
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.y += dr;
			currentSceneObject->euler.y = a3trigValid_sind(currentSceneObject->euler.y);
		}
	}
	else
	{
		for (i = 0, currentSceneObject = demoState->sphereObject;
			i < 4; ++i, ++currentSceneObject)
		{
			currentSceneObject->euler.z += dr;
			currentSceneObject->euler.z = a3trigValid_sind(currentSceneObject->euler.z);
		}
	}


	// update scene objects
	for (i = 0; i < demoStateMaxCount_sceneObject; ++i)
		a3demo_updateSceneObject(demoState->sceneObject + i, 0);
	for (i = 0; i < demoStateMaxCount_cameraObject; ++i)
		a3demo_updateSceneObject(demoState->cameraObject + i, 1);
	for (i = 0; i < demoStateMaxCount_lightObject; ++i)
		a3demo_updateSceneObject(demoState->lightObject + i, 1);

	// update cameras/projectors
	for (i = 0; i < demoStateMaxCount_projector; ++i)
		a3demo_updateCameraViewProjection(demoState->camera + i);


	// apply corrections if required
	// grid
	demoState->gridTransform = useVerticalY ? convertZ2Y : a3identityMat4;

	// skybox position
	demoState->skyboxObject->modelMat.v3 = camera->sceneObject->modelMat.v3;


	// grid lines highlight
	// if Y axis is up, give it a greenish hue
	// if Z axis is up, a bit of blue
	demoState->gridColor = a3wVec4;
	if (useVerticalY)
		demoState->gridColor.g = 0.25f;
	else
		demoState->gridColor.b = 0.25f;


	// update lights
	for (i = 0, pointLight = demoState->forwardPointLight + i;
		i < demoState->forwardLightCount;
		++i, ++pointLight)
	{
		// convert to view space and retrieve view position
		a3real4Real4x4Product(pointLight->viewPos.v, cameraObject->modelMatInv.m, pointLight->worldPos.v);
	}

	// send point light data
	pointLight = demoState->forwardPointLight;
	a3bufferFill(demoState->ubo_pointLight, 0, demoState->forwardLightCount * sizeof(a3_DemoPointLight), pointLight, 0);
	demoState->ubo_pointLight->used[0] = 0;


	// correct rotations as needed
	if (useVerticalY)
	{
		// plane's axis is Z
		a3real4x4ConcatL(demoState->planeObject->modelMat.m, convertZ2Y.m);

		// sphere's axis is Z
		a3real4x4ConcatL(demoState->sphereObject->modelMat.m, convertZ2Y.m);
	}
	else
	{
		// need to rotate skybox if Z-up
		a3real4x4ConcatL(demoState->skyboxObject->modelMat.m, convertY2Z.m);

		// teapot's axis is Y
		a3real4x4ConcatL(demoState->teapotObject->modelMat.m, convertY2Z.m);
	}


	// apply scales
	a3demo_applyScale_internal(demoState->raytraceBoxObject, scaleMat.m);


	// ****TO-DO: 
	// upload ray-trace shape data to UBO

}

//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update(a3_DemoState *demoState, a3f64 dt)
{
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demo_update_main(demoState, dt);
		break;

		// curve drawing
//	case demoStateMode_curves:
//		a3demo_update_curve(demoState, dt);
//		break;

		// skeletal
//	case demoStateMode_skeletal:
//		a3demo_update_skeletal(demoState, dt);
//		break;

		// planets
	case demoStateMode_planet:
		break;

		// raytrace
	case demoStateMode_raytrace:
		a3demo_update_raytrace(demoState, dt);
		break;
	}
}


//-----------------------------------------------------------------------------
