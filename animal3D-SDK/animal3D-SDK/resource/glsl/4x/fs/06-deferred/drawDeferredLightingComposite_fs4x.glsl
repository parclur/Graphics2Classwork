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
in vec4 vPassBiasClipCoord;

layout (location = 0) out vec4 rtFragColor;

uniform sampler2D uImage4;
uniform sampler2D uImage5;
uniform sampler2D uImage6;
uniform sampler2D uImage7;

// (2) declare uniforms for textures (diffuse, specular)
uniform sampler2D tex_dm[]; // diffuse sample
uniform sampler2D tex_sm[]; // specular sample

uniform sampler2D tex_atlas_dm[]; // diffuse sample
uniform sampler2D tex_atlas_sm[]; // specular sample

// (3) temporary values for textures
vec4 tempTex_dm;
vec4 tempTex_sm;

vec4 gPosition; // (2)
vec4 gNormal; // (2)
vec2 gTexcoord; // (2)
float gDepth; // (2)

void main()
{
	gPosition = texture(uImage4, vPassTexcoord); // (2)
	gNormal = texture(uImage5, vPassTexcoord); // (2)
	gTexcoord = texture(uImage6, vPassTexcoord).xy; // (2)

	tempTex_dm = texture(tex_atlas_dm[0], gTexcoord.xy);
	tempTex_sm = texture(tex_atlas_sm[0], gTexcoord.xy);

	// DUMMY OUTPUT: all fragments are FADED CYAN
	//rtFragColor = tempTex_dm * tempTex_sm * gPosition;

	//Pretty sure this is wrong
	vec4 diffuse_DM_atlas_total = (tempTex_dm * vPassBiasClipCoord);
	vec4 diffuse_SM_atlas_total = (tempTex_sm * vPassBiasClipCoord);

	vec4 total = (diffuse_DM_atlas_total + diffuse_SM_atlas_total) * vec4(.5, .5, .5, 1.0);
	//total.a = 1;

	rtFragColor = total;

	//diffuse of atlas * diffuse of lighting 
	//same for specular
	//add and multiply by ambiant
	//make sure alpha is the gTexcoord.w
}
