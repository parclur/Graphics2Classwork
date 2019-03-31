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
	
	a3_HierarchyState.h
	Hierarchy transformation state.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATE_H
#define __ANIMAL3D_HIERARCHYSTATE_H


// A3 hierarchy
#include "a3_Hierarchy.h"

// math library
#include "animal3D-A3DM/animal3D-A3DM.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
	typedef struct a3_HierarchyNodePose		a3_HierarchyNodePose;
	typedef struct a3_HierarchyPose			a3_HierarchyPose;
	typedef struct a3_HierarchyTransform	a3_HierarchyTransform;
	typedef struct a3_HierarchyPoseGroup	a3_HierarchyPoseGroup;
	typedef struct a3_HierarchyState		a3_HierarchyState;
	typedef enum a3_HierarchyPoseFlag		a3_HierarchyPoseFlag;
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------

	// single pose for a single node
	struct a3_HierarchyNodePose
	{
		// 4D vector to hold up to 4 values (quat) or 3 Euler angles
		// default value is identity quaternion (0, 0, 0, 1), which 
		//	also works for default Euler angles (0, 0, 0)
		a3vec4 orientation;

		// 3D vector for translation
		// default value is zero vector (0, 0, 0)
		// can use w to store bone length (distance from parent)
		a3vec4 translation;

		// 3D vector for scale (uniform means all values are the same)
		// scale is not typically used, but it's here in case it's useful
		// default is unit scale (1, 1, 1)
		// can use w to store uniform flag
		a3vec4 scale;
	};


	// single pose for a collection of nodes
	// makes algorithms easier to keep this as a separate data type
	struct a3_HierarchyPose
	{
		a3_HierarchyNodePose *nodePose;
	};


	// collection of matrices for transformation set
	struct a3_HierarchyTransform
	{
		a3mat4 *transform;
	};


	// pose group
	struct a3_HierarchyPoseGroup
	{
		// pointer to hierarchy
		const a3_Hierarchy *hierarchy;

		// contiguous array of all node poses (for efficiency)
		a3_HierarchyNodePose *nodePoseContiguous;

		// list of poses for full hierarchy
		a3_HierarchyPose *pose;

		// number of hierarchy poses in set
		a3ui32 poseCount;
	};


	// hierarchy state structure, with a pointer to the source pose group 
	//	and transformations for kinematics
	struct a3_HierarchyState
	{
		// pointer to pose set that the poses come from
		const a3_HierarchyPoseGroup *poseGroup;

		// local poses
		a3_HierarchyPose localPose[1];

		// local transformations (relative to parent's space)
		a3_HierarchyTransform localSpace[1];

		// object transformations (relative to root's parent's space)
		a3_HierarchyTransform objectSpace[1];

		// inverse object transformations (root relative to this)
		a3_HierarchyTransform objectSpaceInverse[1];

		// skinning transformations (bind pose to current object space)
		a3_HierarchyTransform objectSpaceBindToCurrent[1];
	};
	

//-----------------------------------------------------------------------------

	// flags to describe transformation components in use
	enum a3_HierarchyPoseFlag
	{
		a3poseFlag_identity,		// not using transform components
		a3poseFlag_rotate,			// using Euler angles for rotation
		a3poseFlag_scale = 0x4,		// using scale
		a3poseFlag_translate = 0x8,	// using translation
	};
	

//-----------------------------------------------------------------------------

	// initialize pose set given an initialized hierarchy and key pose count
	inline a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount);

	// release pose set
	inline a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup);

	// get offset to hierarchy pose in contiguous set
	inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex);

	// get offset to single node pose in contiguous set
	inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex);


//-----------------------------------------------------------------------------

	// initialize hierarchy state given an initialized hierarchy
	inline a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_HierarchyPoseGroup *poseGroup);

	// release hierarchy state
	inline a3i32 a3hierarchyStateRelease(a3_HierarchyState *state);

	// update inverse object-space matrices
	inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state, const a3i32 useScale);

	// update bind-to-current given bind-pose object-space transforms
	inline a3i32 a3hierarchyStateUpdateObjectBindToCurrent(const a3_HierarchyState *state, const a3_HierarchyTransform *objectSpaceBindInverse);


//-----------------------------------------------------------------------------

	// set values for a single node pose
	inline a3i32 a3hierarchyNodePoseSetRotation(a3_HierarchyNodePose *nodePose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees, const a3i32 orderXYZ);

	// scale
	inline a3i32 a3hierarchyNodePoseSetScale(a3_HierarchyNodePose *nodePose, const a3f32 sx, const a3f32 sy, const a3f32 sz);

	// translation
	inline a3i32 a3hierarchyNodePoseSetTranslation(a3_HierarchyNodePose *nodePose, const a3f32 tx, const a3f32 ty, const a3f32 tz);


//-----------------------------------------------------------------------------

	// reset single node pose
	inline a3i32 a3hierarchyNodePoseReset(a3_HierarchyNodePose *nodePose_inout);

	// copy single node pose
	inline a3i32 a3hierarchyNodePoseCopy(a3_HierarchyNodePose *nodePose_out, const a3_HierarchyNodePose *copyNodePose);

	// convert single node pose to matrix
	inline a3i32 a3hierarchyNodePoseConvert(a3mat4 *mat_out, const a3_HierarchyNodePose *nodePose, const a3_HierarchyPoseFlag flag);


	// reset full hierarchy pose
	inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose *pose_inout, const a3ui32 nodeCount);

	// copy full hierarchy pose
	inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose *pose_out, const a3_HierarchyPose *copyPose, const a3ui32 nodeCount);

	// convert full hierarchy pose to hierarchy transforms
	inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyTransform *transform_out, const a3_HierarchyPose *pose, const a3ui32 nodeCount, const a3_HierarchyPoseFlag flag);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_HIERARCHYSTATE_H