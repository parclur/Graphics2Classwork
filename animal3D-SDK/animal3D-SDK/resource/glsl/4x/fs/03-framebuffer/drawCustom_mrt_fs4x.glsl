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
	
	drawCustom_fs4x.glsl
	Custom effects for MRT.
*/

#version 410

// ****TO-DO: 
//	0) copy entirety of other fragment shader to start, or start from scratch: 
//		-> declare varyings to read from vertex shader
//		-> *test all varyings by outputting them as color
//	1) declare at least four render targets
//	2) implement four custom effects, outputting each one to a render target

layout(location = 0) out vec4 rtShader1;
layout(location = 1) out vec4 rtShader2;
layout(location = 2) out vec4 rtShader3;
layout(location = 3) out vec4 rtShader4;

out vec4 rtFragColor;

void main()
{
	rtShader1 = vec4(1.0, 0.0, 0.0, 1.0);
	rtShader2 = vec4(0.0, 1.0, 0.0, 1.0);
	rtShader3 = vec4(0.0, 0.0, 1.0, 1.0);
	rtShader4 = vec4(1.0, 0.0, 1.0, 1.0);
}
