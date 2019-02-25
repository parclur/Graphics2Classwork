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
	
	manipTriangle_gs4x.glsl
	Manipulate a single input triangle.
*/

#version 410

// ****TO-DO: 
//	1) declare input and output varying data
//	2) either copy input directly to output for each vertex, or 
//		do something with the vertices first (e.g. explode, invert)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 uP;

//received from "passPhongAttributes_transform_vs"
in vbPassDataBlock
{
	vec4 vPosition;
	vec4 vNormal;
	vec2 vTexcoord;
} vPassData_in[];

//sending to "drawPhongMulti_fs"
out vbPassDataBlock
{
	vec4 vPosition;
	vec4 vNormal;
	vec2 vTexcoord;
} vPassData_out;

int maxVertices = 3;

void main()
{
	const float explodeSz = 0.5;

	//try inverting, try bloating, try duplicating, try rotating, try wireframe
	for (int i = 0; i < maxVertices; i++)
	{
		//vPassData_out.vPosition = vPassData_in[i].vPosition;
		vPassData_out.vPosition = vPassData_in[i].vPosition + normalize(vPassData_in[i].vNormal) * explodeSz;
		vPassData_out.vNormal = vPassData_in[i].vNormal;
		vPassData_out.vTexcoord = vPassData_in[i].vTexcoord;
		//gl_Position = gl_in[i].gl_Position;
		gl_Position = uP * vPassData_out.vPosition;
		EmitVertex();
	}

	EndPrimitive();
}
