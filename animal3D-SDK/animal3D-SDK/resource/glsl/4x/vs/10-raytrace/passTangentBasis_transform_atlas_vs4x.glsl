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
	
	passTangentBasis_transform_atlas_vs4x.glsl
	Transform and pass complete tangent basis after performing skinning.
*/

#version 410

// ****TO-DO: 
//	1) 

layout (location = 10)	in vec3 aTangent;
layout (location = 11)	in vec3 aBitangent;
layout (location = 2)	in vec3 aNormal;
layout (location = 0)	in vec4 aPosition;
layout (location = 8)	in vec4 aTexcoord;

uniform mat4 uMV, uP;
uniform mat4 uAtlas;

out mat4 vPassTangentBasis;
out vec4 vPassTexcoord;

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
//	gl_Position = aPosition;

	mat4 tangentBasis = mat4(
		aTangent,	0.0,
		aBitangent,	0.0,
		aNormal,	0.0,
		aPosition);

	vPassTangentBasis = uMV * tangentBasis;
	vPassTexcoord = uAtlas * aTexcoord;	
	gl_Position = uP * vPassTangentBasis[3];
}
