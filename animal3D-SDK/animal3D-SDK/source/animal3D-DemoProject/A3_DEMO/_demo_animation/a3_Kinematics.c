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
	
	a3_Kinematics.c
	Implementation of kinematics solvers.
*/

#include "a3_Kinematics.h"


//-----------------------------------------------------------------------------

// FK solver
extern inline a3i32 a3kinematicsSolveForward(const a3_HierarchyState *hierarchyState)
{
	return a3kinematicsSolveForwardPartial(hierarchyState, 0, hierarchyState->poseGroup->hierarchy->numNodes);
}

// partial FK solver
extern inline a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->poseGroup && 
		firstIndex < hierarchyState->poseGroup->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement forward kinematics algorithm

		/*
		// From Header file
		// general forward kinematics: 
		// given local transforms for hierarchy nodes, calculate object-space: 
		//		if not root, 
		//			object-space node = object-space parent * local-space node
		//		else
		//			object-space node = local-space node

		// forward kinematics solver given an initialized hierarchy state
		inline a3i32 a3kinematicsSolveForward(const a3_HierarchyState *hierarchyState);

		// forward kinematics solver starting at a specified joint
		inline a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount);
		*/

		a3ui32 i, end = firstIndex + nodeCount;
		for (i = firstIndex; i < end; i++)
		{
			if (i - 1 == -1)
			{
		
				hierarchyState->objectSpace->transform[i] = hierarchyState->localSpace->transform[i];
			}
			else
			{
				//Get the parent of the node with: hierarchyState->poseGroup->hierarchy->nodes[i].parentIndex
				a3real4x4Product(hierarchyState->objectSpace->transform[i].m, hierarchyState->objectSpace->transform[hierarchyState->poseGroup->hierarchy->nodes[i].parentIndex].m, hierarchyState->localSpace->transform[i].m);
			}
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------

// IK solver
extern inline a3i32 a3kinematicsSolveInverse(const a3_HierarchyState *hierarchyState)
{
	return a3kinematicsSolveInversePartial(hierarchyState, 0, hierarchyState->poseGroup->hierarchy->numNodes);
}

// partial IK solver
extern inline a3i32 a3kinematicsSolveInversePartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->poseGroup &&
		firstIndex < hierarchyState->poseGroup->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement inverse kinematics algorithm

	}
	return -1;
}


//-----------------------------------------------------------------------------
