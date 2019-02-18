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
	
	passBiasClipCoord_transform_instanced_vs4x.glsl
	Transform position attribute for rasterization and pass biased clip; use 
		instancing to draw multiple objects.
*/

#version 410

// ****TO-DO: 
//	1) declare uniform blocks
//		-> transformations
//	2) declare varyings for info to be passed to FS
//		-> biased clip-space position, index
//	3) transform input position appropriately
//	4) write to outputs

layout (location = 0) in vec4 aPosition;

// (1) found in load shaders line 368
#define max_lights 1024
uniform ubTransformMVP{
	mat4 uMVP[max_lights];
};
uniform ubTransformMVPB{
	mat4 uMVPB[max_lights];
};

// (2)
out vec4 vPassBiasClipCoord;
flat out int vPassInstanceID; // (2)

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
	//gl_Position = aPosition;

	int i = gl_InstanceID;
	vPassInstanceID = i; // (4)
	vPassBiasClipCoord = uMVPB[i] * aPosition; // (3,4) // perspective divide to get screen space
	gl_Position = uMVP[i] * aPosition; // (3,4)
}
