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
	
	a3_HierarchyState.c
	Implementation of transform hierarchy state.
*/

#include "a3_HierarchyState.h"

#include <stdlib.h>


//-----------------------------------------------------------------------------

// Euler rotation concatenation order
#define a3concatEuler a3real4x4SetRotateXYZ


//-----------------------------------------------------------------------------
// internal blending operations

// reset pose
inline void a3hierarchyNodePoseReset_internal(a3_HierarchyNodePose *nodePose)
{
	// set defaults for all channels
	nodePose->orientation = a3wVec4;
	nodePose->translation = a3zeroVec4;
	nodePose->scale = a3oneVec4;
}

inline void a3hierarchyPoseReset_internal(const a3_HierarchyPose *pose, const a3ui32 nodeCount)
{
	// iterate through list and reset each one
	a3_HierarchyNodePose *nodePose = pose->nodePose, *const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseReset_internal(nodePose++);
}


// copy pose
inline void a3hierarchyNodePoseCopy_internal(a3_HierarchyNodePose *nodePose_out, const a3_HierarchyNodePose *copyNodePose)
{
	*nodePose_out = *copyNodePose;
}

inline void a3hierarchyPoseCopy_internal(const a3_HierarchyPose *pose_out, const a3_HierarchyPose *copyPose, const a3ui32 nodeCount)
{
	a3_HierarchyNodePose *nodePose_out = pose_out->nodePose, *const end = nodePose_out + nodeCount;
	const a3_HierarchyNodePose *copyNodePose = copyPose->nodePose;
	while (nodePose_out < end)
		a3hierarchyNodePoseCopy_internal(nodePose_out++, copyNodePose++);
}


// conversion to matrix
inline void a3hierarchyNodePoseConvert_euler_scale_translate_internal(a3mat4 *mat_out, const a3_HierarchyNodePose *nodePose)
{
	// convert euler to matrix, with translation
	a3concatEuler(mat_out->m, nodePose->orientation.x, nodePose->orientation.y, nodePose->orientation.z);
	mat_out->v3.xyz = nodePose->translation.xyz;

	// adjust scale
	a3real3MulS(mat_out->v0.v, nodePose->scale.x);
	a3real3MulS(mat_out->v1.v, nodePose->scale.y);
	a3real3MulS(mat_out->v2.v, nodePose->scale.z);
}

inline void a3hierarchyPoseConvert_euler_scale_translate_internal(const a3_HierarchyTransform *transform_out, const a3_HierarchyPose *pose, const a3ui32 nodeCount)
{
	a3mat4 *mat_out = transform_out->transform;
	const a3_HierarchyNodePose *nodePose = pose->nodePose, *const end = nodePose + nodeCount;
	while (nodePose < end)
		a3hierarchyNodePoseConvert_euler_scale_translate_internal(mat_out++, nodePose++);
}


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
extern inline a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount)
{
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		// allocate contiguous list of size: nodes * poses
		const a3ui32 nodeCount = hierarchy->numNodes;
		const a3ui32 totalPoses = nodeCount * poseCount;
		a3ui32 i;

		// pointer to pose list for current node
		a3_HierarchyNodePose *nodePosePtr;
		a3_HierarchyPose *posePtr;

		// set hierarchy and count
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->poseCount = poseCount;

		// allocate contiguous data and pointers
		poseGroup_out->nodePoseContiguous = (a3_HierarchyNodePose *)malloc(totalPoses * sizeof(a3_HierarchyNodePose) + poseCount * sizeof(a3_HierarchyPose *));
		poseGroup_out->pose = (a3_HierarchyPose *)(poseGroup_out->nodePoseContiguous + totalPoses);

		// set all pointers and reset all poses
		for (i = 0, nodePosePtr = poseGroup_out->nodePoseContiguous, posePtr = poseGroup_out->pose; 
			i < poseCount; 
			++i, nodePosePtr += nodeCount, ++posePtr)
		{
			posePtr->nodePose = nodePosePtr;
			a3hierarchyPoseReset_internal(posePtr, nodeCount);
		}

		// return pose count
		return poseCount;
	}
	return -1;
}

// release pose set
extern inline a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup)
{
	if (poseGroup && poseGroup->hierarchy)
	{
		free(poseGroup->nodePoseContiguous);
		poseGroup->hierarchy = 0;
		poseGroup->nodePoseContiguous = 0;
		poseGroup->pose = 0;
		poseGroup->poseCount = 0;

		// done
		return 1;
	}
	return -1;
}

// get offset to hierarchy pose in contiguous set
extern inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes);
	return -1;
}

// get offset to single node pose in contiguous set
extern inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes + nodeIndex);
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
extern inline a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_HierarchyPoseGroup *poseGroup)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && poseGroup && !state_out->poseGroup && poseGroup->hierarchy && poseGroup->hierarchy->nodes)
	{
		const a3_Hierarchy *hierarchy = poseGroup->hierarchy;

		const a3ui32 count = hierarchy->numNodes;
		const a3ui32 count4 = count * 4;
		a3ui32 i;

		// set pose set pointer
		state_out->poseGroup = poseGroup;

		// allocate set of matrices in state
		//	('local' points to contiguous array of all matrices and floats)
		state_out->localPose->nodePose = (a3_HierarchyNodePose *)malloc(count * sizeof(a3_HierarchyNodePose) + count4 * sizeof(a3mat4));
		state_out->localSpace->transform = (a3mat4 *)(state_out->localPose->nodePose + count);
		state_out->objectSpace->transform = (a3mat4 *)(state_out->localSpace->transform + count);
		state_out->objectSpaceInverse->transform = (a3mat4 *)(state_out->objectSpace->transform + count);
		state_out->objectSpaceBindToCurrent->transform = (a3mat4 *)(state_out->objectSpaceInverse->transform + count);

		// set all matrices to identity
		for (i = 0; i < count4; ++i)
			a3real4x4SetIdentity(state_out->localSpace->transform[i].m);

		// set all poses to default values
		a3hierarchyPoseReset_internal(state_out->localPose, hierarchy->numNodes);

		// return number of nodes
		return count;
	}
	return -1;
}


// release hierarchy state
extern inline a3i32 a3hierarchyStateRelease(a3_HierarchyState *state)
{
	// validate param exists and is initialized
	if (state && state->poseGroup)
	{
		// release matrices
		//	(local points to contiguous array of all matrices)
		free(state->localPose->nodePose);

		// reset pointers
		state->localPose->nodePose = 0;
		state->localSpace->transform = 0;
		state->objectSpace->transform = 0;
		state->objectSpaceInverse->transform = 0;
		state->objectSpaceBindToCurrent->transform = 0;

		// done
		return 1;
	}
	return -1;
}

// update inverse object-space matrices
extern inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state, const a3i32 useScale)
{
	if (state && state->poseGroup)
	{
		a3mat4 *objectInverse = state->objectSpaceInverse->transform;
		const a3mat4 *const start = objectInverse, *const end = start + state->poseGroup->hierarchy->numNodes, 
			*object;
		for (object = state->objectSpace->transform;
			objectInverse < end;
			++objectInverse, ++object)
		{
			if (useScale)
				a3real4x4TransformInverse(objectInverse->m, object->m);
			else
				a3real4x4TransformInverseIgnoreScale(objectInverse->m, object->m);
		}

		// done
		return (a3i32)(objectInverse - start);
	}
	return -1;
}

// update bind-to-current given bind-pose object-space transforms
extern inline a3i32 a3hierarchyStateUpdateObjectBindToCurrent(const a3_HierarchyState *state, const a3_HierarchyTransform *objectSpaceBindInverse)
{
	if (state && state->poseGroup)
	{
		a3mat4 *objectBindToCurrent = state->objectSpaceBindToCurrent->transform;
		const a3mat4 *const start = objectBindToCurrent, *const end = start + state->poseGroup->hierarchy->numNodes, 
			*object, *objectBindInverse;
		for (object = state->objectSpace->transform,
			objectBindInverse = objectSpaceBindInverse->transform;
			objectBindToCurrent < end;
			++objectBindToCurrent, ++object, ++objectBindInverse)
		{
			a3real4x4Product(objectBindToCurrent->m, object->m, objectBindInverse->m);
		}

		// done
		return (a3i32)(objectBindToCurrent - start);
	}
	return -1;
}


//-----------------------------------------------------------------------------

// set values for a single node pose
extern inline a3i32 a3hierarchyNodePoseSetRotation(a3_HierarchyNodePose *nodePose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees, const a3i32 orderXYZ)
{
	if (nodePose)
	{
		a3real4Set(nodePose->orientation.v, rx_degrees, ry_degrees, rz_degrees, a3realOne);
		return 1;
	}
	return -1;
}

// scale
extern inline a3i32 a3hierarchyNodePoseSetScale(a3_HierarchyNodePose *nodePose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (nodePose)
	{
		a3real4Set(nodePose->scale.v, sx, sy, sz, a3realOne);
		return 1;
	}
	return -1;
}

// translation
extern inline a3i32 a3hierarchyNodePoseSetTranslation(a3_HierarchyNodePose *nodePose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (nodePose)
	{
		a3real4Set(nodePose->translation.v, tx, ty, tz, a3realZero);
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
extern inline a3i32 a3hierarchyNodePoseReset(a3_HierarchyNodePose *nodePose_inout)
{
	if (nodePose_inout)
	{
		a3hierarchyNodePoseReset_internal(nodePose_inout);
		return 1;
	}
	return -1;
}

// copy single node pose
extern inline a3i32 a3hierarchyNodePoseCopy(a3_HierarchyNodePose *nodePose_out, const a3_HierarchyNodePose *copyNodePose)
{
	if (nodePose_out && copyNodePose)
	{
		a3hierarchyNodePoseCopy_internal(nodePose_out, copyNodePose);
		return 1;
	}
	return -1;
}

// convert single node pose
extern inline a3i32 a3hierarchyNodePoseConvert(a3mat4 *mat_out, const a3_HierarchyNodePose *nodePose, const a3_HierarchyPoseFlag flag)
{
	if (mat_out && nodePose)
	{
		a3hierarchyNodePoseConvert_euler_scale_translate_internal(mat_out, nodePose);
		return 1;
	}
	return -1;
}


// reset full hierarchy pose
extern inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose *pose_inout, const a3ui32 nodeCount)
{
	if (pose_inout && pose_inout->nodePose)
	{
		a3hierarchyPoseReset_internal(pose_inout, nodeCount);
		return nodeCount;
	}
	return -1;
}

// copy full hierarchy pose
extern inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose *pose_out, const a3_HierarchyPose *copyPose, const a3ui32 nodeCount)
{
	if (pose_out && copyPose && pose_out->nodePose && copyPose->nodePose)
	{
		a3hierarchyPoseCopy_internal(pose_out, copyPose, nodeCount);
		return nodeCount;
	}
	return -1;
}

// convert full hierarchy pose
extern inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyTransform *transform_out, const a3_HierarchyPose *pose, const a3ui32 nodeCount, const a3_HierarchyPoseFlag flag)
{
	if (transform_out && pose && transform_out->transform && pose->nodePose)
	{
		a3hierarchyPoseConvert_euler_scale_translate_internal(transform_out, pose, nodeCount);
		return nodeCount;
	}
	return -1;
}


//-----------------------------------------------------------------------------
