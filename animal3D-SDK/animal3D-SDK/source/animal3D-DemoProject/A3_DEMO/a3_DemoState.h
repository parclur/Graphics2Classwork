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

#include "_demo_animation/a3_Kinematics.h"


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
		demoStateMaxCount_sceneObject = 8,
		demoStateMaxCount_cameraObject = 2,
		demoStateMaxCount_lightObject = 4,
		demoStateMaxCount_projector = 3,

		demoStateMaxCount_lightVolumeBlock = 4,
		demoStateMaxCount_lightVolumePerBlock = a3index_countMaxShort / sizeof(a3_DemoPointLight),
		demoStateMaxCount_lightVolume = demoStateMaxCount_lightVolumeBlock * demoStateMaxCount_lightVolumePerBlock,

		demoStateMaxCount_timer = 1,
		demoStateMaxCount_drawDataBuffer = 1,
		demoStateMaxCount_vertexArray = 8,
		demoStateMaxCount_drawable = 16,
		demoStateMaxCount_shaderProgram = 32,

		demoStateMaxCount_texture = 16,
		demoStateMaxCount_framebuffer = 4,
		demoStateMaxCount_framebufferDouble = 8,

		demoStateMaxCount_uniformBuffer = 8 + 3 * demoStateMaxCount_lightVolumeBlock,

		// misc
		demoStateMaxCount_curveWaypoint = 32,
		demoStateMaxCount_morphTargetPerModel = 5,
	};

	// additional counters for demo modes
	enum a3_DemoStateModeCounts
	{
		demoStateMaxModes = 3,
		demoStateMaxSubModes = 14,
		demoStateMaxOutputModes = 5,
	};

	// demo mode names
	enum a3_DemoStateModeNames
	{
		demoStateMode_main,
		demoStateMode_curves,
		demoStateMode_skeletal,
	};

	
//-----------------------------------------------------------------------------

	// render pass names for this demo's render pipelines
	enum a3_DemoStateRenderPassNames
	{
		// general
		demoStateRenderPass_scene,

		// deferred
		demoStateRenderPass_deferred_volumes,

		// compositing
		demoStateRenderPass_composite,

		// bloom
		demoStateRenderPass_bloom_bright_2,
		demoStateRenderPass_bloom_blurH_2,
		demoStateRenderPass_bloom_blurV_2,
		demoStateRenderPass_bloom_bright_4,
		demoStateRenderPass_bloom_blurH_4,
		demoStateRenderPass_bloom_blurV_4,
		demoStateRenderPass_bloom_bright_8,
		demoStateRenderPass_bloom_blurH_8,
		demoStateRenderPass_bloom_blurV_8,
		demoStateRenderPass_bloom_blend,

		// supplementary
		demoStateRenderPass_shadow = demoStateMaxSubModes - 1,
	};

	// pipeline modes
	enum a3_DemoStatePipelineModeNames
	{
		demoStatePipelineMode_forward,
		demoStatePipelineMode_deferredShading,
		demoStatePipelineMode_deferredLighting,
	};

	// forward pipeline modes
	enum a3_DemoStateForwardPipelineModeNames
	{
		demoStateForwardPipelineMode_Phong,
		demoStateForwardPipelineMode_Phong_projective,
		demoStateForwardPipelineMode_Phong_shadowmap,
		demoStateForwardPipelineMode_Phong_shadowmap_projective,
		demoStateForwardPipelineMode_Phong_manip,
		demoStateForwardPipelineMode_Phong_morph,
		demoStateForwardPipelineMode_nonphoto,
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
		a3real windowWidthInv, windowHeightInv, windowAspect;
		a3ui32 frameWidth, frameHeight;
		a3real frameWidthInv, frameHeightInv, frameAspect;
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
		//	- mode: which mode/pipeline is being viewed
		//	- sub-mode: which sub-mode/pass in the pipeline is being viewed
		//	- output: which output from the sub-mode/pass is being viewed
		a3ui32 demoMode, demoSubMode[demoStateMaxModes], demoOutputMode[demoStateMaxModes][demoStateMaxSubModes];
		a3ui32 demoModeCount, demoSubModeCount[demoStateMaxModes], demoOutputCount[demoStateMaxModes][demoStateMaxSubModes];

		// toggle grid in scene and axes superimposed, as well as other mods
		a3boolean displayGrid, displayWorldAxes, displayObjectAxes, displayTangentBases;
		a3boolean displaySkybox, displayHiddenVolumes, displayPipeline;
		a3boolean additionalPostProcessing, previewIntermediatePostProcessing;
		a3boolean stencilTest, singleForwardLight;
		a3boolean lightingPipelineMode;
		a3boolean updateAnimation;

		// lighting modes
		a3ui32 forwardShadingMode, forwardShadingModeCount;

		// grid properties
		a3mat4 gridTransform;
		a3vec4 gridColor;

		// cameras
		a3ui32 activeCamera;

		// lights
		a3_DemoPointLight forwardPointLight[demoStateMaxCount_lightObject];
		a3_DemoPointLight deferredPointLight[demoStateMaxCount_lightVolume];
		a3mat4 deferredLightMVP[demoStateMaxCount_lightVolume];
		a3mat4 deferredLightMVPB[demoStateMaxCount_lightVolume];
		a3ui32 forwardLightCount;
		a3ui32 deferredLightCount, deferredLightBlockCount, deferredLightCountPerBlock[demoStateMaxCount_lightVolumeBlock];

		// texture atlas transforms
		union {
			a3mat4 atlasTransform[4];
			struct {
				a3mat4 atlas_stone[1], atlas_earth[1], atlas_mars[1], atlas_checker[1];
			};
		};


		// animation stuff
		a3vec4 curveWaypoint[demoStateMaxCount_curveWaypoint], curveHandle[demoStateMaxCount_curveWaypoint];
		a3ui32 curveWaypointCount;
		a3ui32 curveSegmentIndex;
		a3real curveSegmentDuration, curveSegmentDurationInv;
		a3real curveSegmentTime, curveSegmentParam;

		// morph targets
		a3ui32 targetCount;
		union {
			a3ui32 targetIndexList[4];
			struct {
				a3ui32 targetIndex, targetIndexNext, targetIndexNextNext, targetIndexPrev;
			};
		};
		a3real targetDuration, targetDurationInv;
		a3real targetTime, targetParam;


		// skeletal objects
		a3_Hierarchy hierarchy_skel[1];
		a3_HierarchyState hierarchyState_skel[1];
		a3_HierarchyPoseGroup hierarchyPoseGroup_skel[1];
		a3_HierarchyPoseFlag hierarchyPoseFlag_skel[1][128];

		// skeletal controls
		a3ui32 editSkeletonIndex;
		a3ui32 editJointIndex;
		a3boolean editingJoint;

		// 2-4 key poses for each joint
		// root // orientation.xyz // translation.xyz
		// spine_lower // orientation.xyz
		// spine_mid // orientation.xyz
		// spine_upper // orientation.xyz
		// neck // orientation.xyz
		// head // orientation.xyz
		// clavicle // orientation.xyz
		// shoulderblade_r // orientation.xyz
		// shoulder_r // orientation.xyz
		// elbow_r // orientation.xyz
		// forearm_r // orientation.xyz
		// wrist_r // orientation.xyz
		// hand_r // orientation.xyz
		// shoulderblade_l // orientation.xyz
		// shoulder_l // orientation.xyz
		// elbow_l // orientation.xyz
		// forearm_l // orientation.xyz
		// wrist_l // orientation.xyz
		// hand_l // orientation.xyz
		// pelvis // orientation.xyz
		// hip_r // orientation.xyz
		// knee_r // orientation.xyz
		// shin_r // orientation.xyz
		// ankle_r // orientation.xyz
		// foot_r // orientation.xyz
		// toe_r // orientation.xyz
		// hip_l // orientation.xyz
		// knee_l // orientation.xyz
		// shin_l // orientation.xyz
		// ankle_l // orientation.xyz
		// foot_l // orientation.xyz
		// toe_l // orientation.xyz

		//Create a struct for all the joint values
		union
		{
			//a3vec3 JointPoses[3];
			struct 
			{
				//must have values return a const a3f32
				a3vec3 rootFirstPoseVec3;
				a3vec3 rootSecondPoseVec3;
				a3vec3 rootThirdPoseVec3;

				a3vec3 spine_lowerFirstPoseVec3;
				a3vec3 spine_lowerSecondPoseVec3;
				a3vec3 spine_lowerThirdPoseVec3;

				a3vec3 spine_midFirstPoseVec3;
				a3vec3 spine_midSecondPoseVec3;
				a3vec3 spine_midThirdPoseVec3;

				a3vec3 spine_upperFirstPoseVec3;
				a3vec3 spine_upperSecondPoseVec3;
				a3vec3 spine_upperThirdPoseVec3;
			};
		};

		/*
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, 0.0, +3.6f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate | a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -90.0f, -5.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, -0.1f, +0.1f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -5.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +90.0f, 0.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.1f, +0.1f, 0.0f);
//	j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
//	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
//	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, 0.0f, +0.1f, -0.1f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, -0.1f, -0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +30.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, -0.1f, -0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_translate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -30.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, +90.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, -0.1f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +90.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, +0.5f, 0.0f, 0.0f);

	j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, -90.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, -0.1f, +0.5f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, +20.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -2.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -90.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -1.0f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetRotation(hierarchyNodePose, 0.0f, 0.0f, -10.0f, a3true);
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);
	hierarchyPoseFlag[j] = a3poseFlag_rotate;
	j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
	hierarchyNodePose = hierarchyPoseGroup->pose[p].nodePose + j;
	a3hierarchyNodePoseSetTranslation(hierarchyNodePose, -0.5f, 0.0f, 0.0f);
		*/


		//---------------------------------------------------------------------
		// object arrays: organized as anonymous unions for two reasons: 
		//	1. easy to manage entire sets of the same type of object using the 
		//		array component
		//	2. at the same time, variables are named pointers

		// scene objects
		union {
			a3_DemoSceneObject sceneObject[demoStateMaxCount_sceneObject];
			struct {
				// main scene objects
				a3_DemoSceneObject
					skyboxObject[1],
					planeObject[1],
					sphereObject[1],
					cylinderObject[1],
					torusObject[1],
					teapotObject[1];

				// curve-drawing objects
				a3_DemoSceneObject
					curveFollowObject[1];

				// skeletal objects
				a3_DemoSceneObject
					skeletonObject[1];
			};
		};
		union {
			a3_DemoSceneObject cameraObject[demoStateMaxCount_cameraObject];
			struct {
				a3_DemoSceneObject
					mainCameraObject[1];
				a3_DemoSceneObject
					topDownCameraObject[1];
			};
		};
		union {
			a3_DemoSceneObject lightObject[demoStateMaxCount_lightObject];
			struct {
				a3_DemoSceneObject
					mainLightObject[1];
			};
		};

		// cameras
		//	- any object can have a camera "component"
		union {
			a3_DemoCamera camera[demoStateMaxCount_projector];
			struct {
				a3_DemoCamera
					sceneCamera[1];						// scene viewing cameras
				a3_DemoCamera
					curveCamera[1];						// top-down drawing camera
				a3_DemoCamera
					projectorLight[1];					// other projectors
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
					vao_position_texcoord[1],					// VAO for vertex format with position and UVs
					vao_position_texcoord_normal[1],			// VAO for vertex format with position, UVs and normal
					vao_tangentBasis[1],						// VAO for vertex format with full tangent basis
					vao_tangentBasis_morph5[1];					// VAO for vertex format with multiple partial tangent bases for morphing
			};
		};

		// drawables
		union {
			a3_VertexDrawable drawable[demoStateMaxCount_drawable];
			struct {
				a3_VertexDrawable
					draw_axes[1];								// coordinate axes at the center of the world
				a3_VertexDrawable
					draw_grid[1],								// wireframe ground plane to emphasize scaling
					draw_bone[1],								// spike shape for skeletal bone
					draw_joint[1],								// ball shape for skeletal joint
					draw_skybox[1],								// skybox cube mesh
					draw_unitquad[1];							// unit quad (used for fsq)
				a3_VertexDrawable
					draw_pointlight[1];							// volume for point light (low-res sphere)
				a3_VertexDrawable
					draw_plane[1],								// procedural plane
					draw_sphere[1],								// procedural sphere
					draw_cylinder[1],							// procedural cylinder
					draw_torus[1],								// procedural torus
					draw_teapot[1];								// can't not have a Utah teapot
				a3_VertexDrawable
					draw_teapot_morph[1];						// morphing teapot model
			};
		};


		// shader programs and uniforms
		union {
			a3_DemoStateShaderProgram shaderProgram[demoStateMaxCount_shaderProgram];
			struct {
				a3_DemoStateShaderProgram
					prog_drawColorUnif[1],						// draw uniform color
					prog_drawColorAttrib[1],					// draw color attribute
					prog_drawColorUnif_instanced[1],			// draw uniform color with instancing
					prog_drawColorAttrib_instanced[1];			// draw color attribute with instancing
				a3_DemoStateShaderProgram
					prog_drawTexture[1],						// draw texture
					prog_drawPhongMulti[1],						// draw Phong shading model (multiple lights)
					prog_drawNonPhotoMulti[1];					// draw non-photorealistic shading model
				a3_DemoStateShaderProgram
					prog_drawPhongMulti_mrt[1],					// draw Phong to MRT
					prog_drawCustom_mrt[1];						// draw custom effects to MRT
				a3_DemoStateShaderProgram
					prog_transform[1],							// transform vertex only; no fragment shader
					prog_drawPhongMulti_projtex[1],				// projective texturing
					prog_drawPhongMulti_shadowmap[1],			// shadow mapping
					prog_drawPhongMulti_shadowmap_projtex[1],	// shadow mapping and projective texturing
					prog_drawCustom_post[1];					// custom post-processing
				a3_DemoStateShaderProgram
					prog_drawBrightPass[1],						// post-process bright pass
					prog_drawBlurGaussian[1],					// post-process blurring (Gaussian)
					prog_drawBlendComposite[1];					// post-process blending & composite
				a3_DemoStateShaderProgram
					prog_drawGBuffers[1],						// output geometry buffers ("g-buffers")
					prog_drawPhongMulti_deferred[1],			// perform deferred Phong shading
					prog_drawPhong_volume[1],					// render one light volume with Phong
					prog_drawDeferredLightingComposite[1];		// composite deferred lighting
				a3_DemoStateShaderProgram
					prog_drawPhongMulti_manip[1],				// manipulate triangle before shading
					prog_drawTangentBasis[1],					// render complete tangent basis as lines
					prog_drawCurveSegment[1],					// draw a curve segment between waypoints
					prog_drawCurveHandles[1];					// draw curve waypoints/handles
				a3_DemoStateShaderProgram
					prog_drawPhongMulti_morph[1],				// Phong shading on a morphing object
					prog_drawTangentBasis_morph[1];				// tangent basis for morphing object
				a3_DemoStateShaderProgram
					prog_drawPhongMulti_skin[1],				// Phong shading on a skinned object
					prog_drawTangentBasis_skin[1];				// tangent basis for skinned object
			};
		};


		// textures
		union {
			a3_Texture texture[demoStateMaxCount_texture];
			struct {
				a3_Texture
					tex_skybox_clouds[1],
					tex_skybox_water[1],
					tex_atlas_dm[1],
					tex_atlas_sm[1],
					tex_stone_dm[1],
					tex_earth_dm[1],
					tex_earth_sm[1],
					tex_mars_dm[1],
					tex_mars_sm[1],
					tex_ramp_dm[1],
					tex_ramp_sm[1],
					tex_checker[1];
			};
		};


		// framebuffers
		union {
			a3_Framebuffer framebuffer[demoStateMaxCount_framebuffer];
			struct {
				a3_Framebuffer
					fbo_scene[1];					// fbo with color and depth
				a3_Framebuffer
					fbo_shadowmap[1];				// fbo for shadow map with depth only
			};
		};
		union {
			a3_FramebufferDouble framebuffer_double[demoStateMaxCount_framebufferDouble];
			struct {
				a3_FramebufferDouble
					fbo_dbl_nodepth[2],				// full frame dfbo (w or w/o border), no depth
					fbo_dbl_nodepth_2[2],			// half-frame, ditto
					fbo_dbl_nodepth_4[2],			// quarter-frame, ditto
					fbo_dbl_nodepth_8[2];			// eighth-frame, ditto
			};
		};


		// uniform buffer objects
		union {
			a3_UniformBuffer uniformBuffer[demoStateMaxCount_uniformBuffer];
			struct {
				// lighting uniform buffers
				a3_UniformBuffer
					ubo_transformMVP_light[demoStateMaxCount_lightVolumeBlock],
					ubo_transformMVPB_light[demoStateMaxCount_lightVolumeBlock],
					ubo_pointLight[demoStateMaxCount_lightVolumeBlock];

				// curve uniform buffers
				a3_UniformBuffer
					ubo_curveWaypoint[1],
					ubo_curveHandle[1];

				// skeletal uniform buffers
				a3_UniformBuffer
					ubo_transformLMVP_bone[1],
					ubo_transformLMVP_joint[1],
					ubo_transformBindPoseToCurrentPose_joint[1];
			};
		};


		// managed objects, no touchie
		a3_VertexDrawable dummyDrawable[1];


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
	void a3demo_loadTextures(a3_DemoState *demoState);
	void a3demo_loadFramebuffers(a3_DemoState *demoState);
	void a3demo_loadAnimation(a3_DemoState *demoState);
	void a3demo_refresh(a3_DemoState *demoState);

	// unloading
	void a3demo_unloadGeometry(a3_DemoState *demoState);
	void a3demo_unloadShaders(a3_DemoState *demoState);
	void a3demo_unloadTextures(a3_DemoState *demoState);
	void a3demo_unloadFramebuffers(a3_DemoState *demoState);
	void a3demo_unloadAnimation(a3_DemoState *demoState);
	void a3demo_validateUnload(const a3_DemoState *demoState);

	// other utils & setup
	void a3demo_setDefaultGraphicsState();
	void a3demo_initScene(a3_DemoState *demoState);
	void a3demo_initSceneRefresh(a3_DemoState *demoState);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSTATE_H