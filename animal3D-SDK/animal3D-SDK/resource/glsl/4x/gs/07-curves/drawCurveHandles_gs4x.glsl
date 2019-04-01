//Edited by Zach Phillips and Claire Yeash with permission from the author
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

	drawCurveHandles_gs4x.glsl
	Draw curve control values.
*/

#version 410

// ****TO-DO: 
//	1) declare input varying data
//	2) declare uniform blocks
//	3) draw line between waypoint and handle
//	4) draw shape for handle (e.g. diamond)

#define max_verts 8
#define max_waypoints 32

layout(points) in;
layout(line_strip, max_vertices = max_verts) out;

uniform mat4 uMVP;

// (1) declare input varying data
//flat in int vPassTangentBasis[]; // varying data from passInstanceID_vs4x
flat in int vPassInstanceID[];

// (2) declare uniform blocks
// referenced from Claire's Lab 6 drawPhong_Volume_fs4x
//struct sGeneralSceneObjects
//{
//	mat4 modelMat; // model matrix: transform relative to scene
//	mat4 modelMatInv; // inverse model matrix: scene relative to this
//	vec3 euler; // euler angles for direct rotation control
//	vec3 position; // scene position for direct control
//	vec3 scale; // scale (not accounted for in update)
//	int scaleMode; // 0 = off; 1 = uniform; other = non-uniform (nightmare)
//};
//uniform ubGeneralSceneObjects
//{
//	sGeneralSceneObjects uGeneralSceneObjects[max_waypoints];
//};
uniform ubCurveWaypoint{
	vec4 vCurveWaypoint[max_waypoints];
}ubWaypoint;

uniform ubCurveHandle{
	vec4 vCurveHandle[max_verts];
}ubHandles;

void main()
{
	// (3) draw line between waypoint and handle
	//for (int i = 0; i < vPassTangentBasis.length; i++)
	//{
	//	// Position of the handle
	//	//gl_Position.xyz = uGeneralSceneObjects[vPassInstanceID].position;
	//	gl_Position = vec4(normalize(ubCurveHandle[vPassTangentBasis[i]].x), normalize(ubCurveHandle[vPassTangentBasis[i]].y), -1.0, 1.0);
	//	//gl_Position = normalize(ubCurveHandle[vPassTangentBasis[i]]);
	//	EmitVertex();
	//
	//	// Postion of next waypoint
	//	//gl_Position.xyz = uGeneralSceneObjects[vPassInstanceID].position;
	//	//gl_Position = vec4(ubCurveWaypoint[vPassTangentBasis[i]].xy, -1.0, 1.0);
	//	gl_Position = vec4(1.0, 1.0, -1.0, 1.0);
	//	EmitVertex();
	//
	//	EndPrimitive();
	//}

	//// Position of the handle
	////gl_Position.xyz = uGeneralSceneObjects[vPassInstanceID].position;
	//gl_Position = ubCurveWaypoint[vPassTangentBasis[0]];
	//EmitVertex();
	//
	//// Postion of next waypoint
	////gl_Position.xyz = uGeneralSceneObjects[vPassInstanceID].position;
	//gl_Position = vec4(-1.0, 0.0, -1.0, 1.0);
	//EmitVertex();

	//Get the point from the vertex shader
	int index = vPassInstanceID[0];

	vec4 offsetVectorArray[4] = vec4[](
		vec4(0.25, 0.0, 0.0, 0.0),
		vec4(0.0, 0.25, 0.0, 0.0),
		vec4(-0.25, 0.0, 0.0, 0.0),
		vec4(0.0, -0.25, 0.0, 0.0));

	//Waypoint to Handle
	gl_Position = uMVP * ubWaypoint.vCurveWaypoint[index];
	EmitVertex();

	gl_Position = uMVP * ubHandles.vCurveHandle[index];
	EmitVertex();

	EndPrimitive();

	// (4) draw shape for handle (e.g. diamond)
	gl_Position = uMVP * ((ubHandles.vCurveHandle[index]) + offsetVectorArray[0]);
	EmitVertex();

	gl_Position = uMVP * ((ubHandles.vCurveHandle[index]) + offsetVectorArray[1]);
	EmitVertex();

	gl_Position = uMVP * ((ubHandles.vCurveHandle[index]) + offsetVectorArray[2]);
	EmitVertex();

	gl_Position = uMVP * ((ubHandles.vCurveHandle[index]) + offsetVectorArray[3]);
	EmitVertex();

	gl_Position = uMVP * ((ubHandles.vCurveHandle[index]) + offsetVectorArray[0]);
	EmitVertex();

	EndPrimitive();
}