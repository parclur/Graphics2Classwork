//This file was modified by Claire Yeash and Zach Phillips with permission of the author.

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
	
	passPhongAttribs_transform_vs4x.glsl
	Transform position attribute and pass attributes related to Phong shading 
		model down the pipeline.
*/

#version 410

// ****TO-DO: 
//	1) declare separate uniforms for MV and P matrices
//	2) transform object-space position to eye-space
//	3) transform eye-space position to clip space, store as output
//	4) declare normal attribute
//		-> look in 'a3_VertexDescriptors.h' for the correct location index
//	5) declare normal MV matrix
//	6) transform object-space normal to eye-space using the correct matrix
//	7) declare texcoord attribute
//	8) declare varyings for lighting data
//	9) copy lighting data to varyings

layout (location = 0) in vec4 aPosition;
layout (location = 2) in vec4 aNormal; // (4)
layout (location = 8) in vec2 aTexcoord; // (7)

// (1)
uniform mat4 uMV; //model view matrix
uniform mat4 uP; //projection matrix
uniform mat4 uMV_nrm; // (5) normal model view matrix

// (4,7) varyings to be sent to fragment shader 
out vec4 vPosition;
out vec4 vNormal;
out vec2 vTexcoord;

void main()
{
	vPosition = uMV * aPosition; // (3) view-space coordinate
	vNormal = uMV_nrm * aNormal; // (6) transform object-space normal to eye-space using the correct matrix; normal in view space
	vTexcoord = aTexcoord; // (7)

	/*
	// (9) light data moved to fragment shader, intitialized there; otherwise would be repetitive
	vLightCt = uLightCt;
	vLightPos = uLightPos;
	vLightCol = uLightCol;
	vLightSz = uLightSz;
	*/

	//condensing (2) and (3) into the below line. Previous steps shown above
	gl_Position = uP * uMV * aPosition;
}
