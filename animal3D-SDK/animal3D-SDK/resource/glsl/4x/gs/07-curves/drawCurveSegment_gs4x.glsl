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

	drawCurveSegment_gs4x.glsl
	Draw curve segment using interpolation algorithm.
*/

#version 410

// ****TO-DO: 
//	1) declare input varying data
//	2) declare uniform blocks
//	3) implement interpolation sampling function(s)
//	4) implement curve drawing function(s)
//	5) call choice curve drawing function in main

#define max_verts 32
#define max_waypoints 32

// (1) declare input varying data
flat in int vPassInstanceID[];

layout(points) in;
layout(line_strip, max_vertices = max_verts) out;

// (2) declare uniform blocks
uniform ubCurveWaypoint{
	vec4 vCurveWaypoint[max_waypoints];
}ubWaypoint;

uniform ubCurveHandle{
	vec4 vCurveHandle[max_verts];
}ubHandles;

uniform mat4 uMVP;

//(3, 4)
vec4 BezierCurves(float t, vec4 point0, vec4 point1, vec4 point2, vec4 point3)
{
	vec4 returnVector;

	//Taken from the lecture 7-8 interpolation pdf in Dan's notes
	returnVector = (pow((1 - t), 3) * point0) +
		(3 * pow((1 - t), 2) * t * point1) +
		(3 * (1 - t) * pow(t, 2) * point2) +
		(pow(t, 3) * point3);

	return returnVector;
}

void main()
{
	//Get the point from the vertex shader
	int index = vPassInstanceID[0];

	//Get the curve weights set properly
	vec4 point0 = uMVP * ubWaypoint.vCurveWaypoint[index];
	vec4 point1 = uMVP * ((2 * (ubWaypoint.vCurveWaypoint[index] - ubHandles.vCurveHandle[index])) + ubHandles.vCurveHandle[index]);
	vec4 point2 = uMVP * ((2 * (ubWaypoint.vCurveWaypoint[index + 1] - ubHandles.vCurveHandle[index + 1])) + ubHandles.vCurveHandle[index + 1]);
	vec4 point3 = uMVP * ubWaypoint.vCurveWaypoint[index + 1];

	//vec4 point0 = uMVP * ubWaypoint.vCurveWaypoint[index];
	//vec4 point1 = uMVP * ubHandles.vCurveHandle[index];
	//vec4 point2 = uMVP * ubHandles.vCurveHandle[index + 1];
	//vec4 point3 = uMVP * ubWaypoint.vCurveWaypoint[index + 1];

	//Create the first point on the curve
	gl_Position = uMVP * ubWaypoint.vCurveWaypoint[index];
	EmitVertex();

	//(5) fill in the points on the curve
	for (float t = 0; t <= 1.0; t += .1)
	{
		gl_Position = BezierCurves(t, point0, point1, point2, point3);
		EmitVertex();
	}

	//set the last point on the curve
	gl_Position = uMVP * ubWaypoint.vCurveWaypoint[index + 1];
	EmitVertex();

	EndPrimitive();
}
