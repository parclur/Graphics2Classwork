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
	
	drawPhongMulti_shadowmap_fs4x.glsl
	Phong shading with shadow mapping.
*/

#version 410

// ****TO-DO: 
//	0) copy entirety of Phong fragment shader
//	1) declare shadow map texture
//	2) declare varying to receive shadow coordinate
//	3) perform perspective divide to acquire projector's screen-space coordinate
//		-> *try outputting this as color
//	4) sample shadow map using screen-space coordinate
//		-> *try outputting this on its own
//	5) perform shadow test
//		-> *try outputting result (boolean)
//	6) apply shadow to shading model

layout (location = 0) out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are LIME
	rtFragColor = vec4(0.0, 1.0, 0.5, 1.0);
}
