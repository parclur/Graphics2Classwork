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
	
	drawPhongMulti_tangentBasis_nm_pom_shadow_fs4x.glsl
	Calculate and output Phong shading model for multiple lights using full 
		tangent basis passed from previous shader; perform normal mapping, 
		parallax occlusion mapping and self-shadowing.
*/

#version 410

// ****TO-DO: 
//	1) 

in mat4 vPassTangentBasis;
in vec4 vPassTexcoord;

layout (location = 0) out vec4 rtFragColor;
layout (location = 1) out vec4 rtNormal;

void main()
{
	// DUMMY OUTPUT: all fragments are FADED CYAN
	rtFragColor = vec4(0.5, 1.0, 1.0, 1.0);

	// DUMMY OUTPUT: standard normal blue
	rtNormal = vec4(0.5, 0.5, 1.0, 1.0);
}
