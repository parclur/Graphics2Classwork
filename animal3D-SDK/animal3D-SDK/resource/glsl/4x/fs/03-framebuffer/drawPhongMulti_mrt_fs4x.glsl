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
	
	drawPhongMulti_mrt_fs4x.glsl
	Phong shading model with splitting to multiple render targets (MRT).
*/

#version 410

// ****TO-DO: 
//	0) copy entirety of Phong fragment shader
//	1) declare eight render targets
//	2) output appropriate data to render targets

//out vec4 rtFragColor;

// testing
//layout qualifier

layout (location = 0) out vec4 rtFragColor;
layout (location = 1) out vec4 rtFragColor1;
layout (location = 2) out vec4 rtFragColor2;
layout (location = 3) out vec4 rtFragColor3;

//layout(location = 0) out vec4 rtPoosition;
//layout(location = 1) out vec4 rtNormal;
//layout(location = 2) out vec4 rtTexcoord;
//layout(location = 3) out vec4 rtDiffuseMap;

void main()
{
	// DUMMY OUTPUT: all fragments are FADED CYAN
	rtFragColor = vec4(0.5, 1.0, 1.0, 1.0);

	// testing
	rtFragColor1 = vec4(1.0, 0.5, 0.0, 1.0);
	rtFragColor2 = vec4(1.1, 0.0, 1.0, 1.0);
	rtFragColor3 = vec4(1.0, 0.0, 0.5, 1.0);
}
