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
	
	drawPhongMulti_deferred_fs4x.glsl
	Perform Phong shading on multiple lights, deferred.
*/

#version 410

// ****TO-DO: 
//	0) copy entirety of Phong multi-light shader
//	1) geometric inputs from scene objects are not received from VS!
//		-> we are drawing a textured FSQ so where does the geometric 
//			input data come from? declare appropriately
//	2) retrieve new geometric inputs (no longer from varyings)
//		-> *test by outputting as color
//	3) use new inputs where appropriate in lighting

in vec2 vPassTexcoord;

layout (location = 0) out vec4 rtFragColor;

// (1)
uniform sampler2D uImage4;
uniform sampler2D uImage5;
uniform sampler2D uImage6;
uniform sampler2D uImage7;

void main()
{
	// DUMMY OUTPUT: all fragments are PURPLE
	rtFragColor = vec4(0.5, 0.0, 1.0, 1.0);

	vec4 gPosition = texture(uImage4, vPassTexcoord); // (2)
	vec4 gNormal = texture(uImage5, vPassTexcoord); // (2)
	vec2 gTexcoord = texture(uImage6, vPassTexcoord).xy; // (2)
	float gDepth = texture(uImage7, vPassTexcoord).x; // (2)

	// Testing
	rtFragColor = gPosition;
	rtFragColor = vec4(gNormal.xyz * 0.5 + 0.5, 1.0);
	rtFragColor = vec4(gTexcoord, 0.0, 1.0);
	rtFragColor = vec4(gDepth, gDepth, gDepth, 1.0);
}
