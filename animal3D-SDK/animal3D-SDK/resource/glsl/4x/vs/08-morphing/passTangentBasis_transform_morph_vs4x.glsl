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
	
	passTangentBasis_transform_morph_vs4x.glsl
	Transform and pass complete tangent basis after performing morph animation.
*/

#version 410

// ****TO-DO: 
//	1) declare all tangent basis attributes configured for morphing
//	2) declare transformation matrix uniforms
//	3) declare tangent basis to pass along
//	4) define morphing algorithm and related uniforms
//	5) define local tangent basis and store morphed attributes
//	6) transform and pass tangent basis
//	7) set final clip-space position
//	8) declare additional attributes, uniforms and varyings for shading
//	9) transform and pass additional values for shading

#define max_morphTarget 5

// (1)
layout (location = 0)	in vec4 aPosition[max_morphTarget];
layout (location = 5)	in vec4 aNormal[max_morphTarget];
layout (location = 10)	in vec4 aTangent[max_morphTarget];
// (8)
layout (location = 15)	in vec4 aTexcoord;

uniform mat4 uMV, uP; // (2)

// (4)
uniform ivec4 uIndex;
uniform float uTime; 

#define i0 uIndex[0]
#define i1 uIndex[1]
#define iNext uIndex[2]
#define iPrev uIndex[3]

// Catmull-Rom Interpolation
float t;
//mat4 controlParameterT = mat4(1, t, pow(t, 2), pow(t, 3));
//mat4 MCR = mat4(1 / 2(0, 2, 0, 0,
//	-1, 0, 1, 0,
//	2, -5, 4, -1,
//	-1, 3, -3, 1));

void main()
{
	//vec4 position = aPosition[uIndex[0]];
	//vec4 position = mix(aPosition[uIndex[0]], aPosition[uIndex[1]], uTime); // pro-tip it is ugly, challenge: mix function is LERP - give catmull rom and curbic hermite, challenge 2: complete the shader (normal, tangent, bitangent, tangent basis), challenge 3: make all of the shapes morph

	vec4 catmullRom = vec4(0.5f * ((((-1 * t) + (2 * pow(t, 2)) - pow(t, 3)) * iPrev) +
								((2 - (5 * pow(t, 2)) + (3 * pow(t, 3))) * i0) +
								((t + (4 * pow(t,2)) - (3 * pow(t, 3))) * i1) +
								(((-1 * pow(t,2)) + pow(t, 3)) * iNext)));

	// DUMMY OUTPUT: directly assign input position to output position
	//gl_Position = uP * uMV * position;
	gl_Position = uP * uMV * catmullRom;
}
