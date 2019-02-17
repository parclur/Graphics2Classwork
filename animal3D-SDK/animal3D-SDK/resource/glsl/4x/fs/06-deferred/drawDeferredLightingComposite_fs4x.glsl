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
	
	drawDeferredLightingComposite_fs4x.glsl
	Composite deferred lighting.
*/

#version 410

// ****TO-DO: 
//	1) declare images with results of lighting pass
//	2) declare remaining geometry inputs (atlas coordinates) and atlas textures
//	3) declare any other shading variables not involved in lighting pass
//	4) sample inputs to get components of Phong shading model
//		-> surface colors, lighting results
//		-> *test by outputting as color
//	5) compute final Phong shading model (i.e. the final sum)

in vec2 vPassTexcoord;

layout (location = 0) out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are FADED CYAN
	rtFragColor = vec4(0.5, 1.0, 1.0, 1.0);
}
