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
	
	drawTexture_fs4x.glsl
	Draw texture sample using texture coordinate from prior shader.
*/

#version 410

// ****TO-DO: 
//	1) declare image handles (see blend pass in render)
//	2) sample and *test images
//	3) implement some multi-image blending algorithms
//	4) output result as blend between multiple images

in vec2 vPassTexcoord;

uniform sampler2D uImage0;

layout (location = 0) out vec4 rtFragColor;


void main()
{
	// DUMMY OUTPUT: all fragments are PURPLE
//	rtFragColor = vec4(0.5, 0.0, 1.0, 1.0);

	// DEBUGGING
	vec4 sample0 = texture(uImage0, vPassTexcoord);
//	rtFragColor = vec4(vPassTexcoord, 0.0, 1.0);
	rtFragColor = sample0;
}
