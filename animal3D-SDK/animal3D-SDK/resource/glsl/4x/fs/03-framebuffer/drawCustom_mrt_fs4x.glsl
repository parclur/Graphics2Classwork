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

//(0)
//(1)
layout(location = 0) out vec4 rtShader1;
layout(location = 1) out vec4 rtShader2;
layout(location = 2) out vec4 rtShader3;
layout(location = 3) out vec4 rtShader4;

//declare fixed-sized arrays for lighting values and other related values
#define max_lights 8
uniform int uLightCt;
uniform vec4 uLightPos[max_lights];
uniform vec4 uLightCol[max_lights];
uniform float uLightSz[max_lights];

//declare varyings to read from vertex shader
in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexcoord;

//sample textures and store as temporary values
vec4 tempDM;
vec4 tempSM;

uniform sampler2D tex_dm[];
uniform sampler2D tex_sm[];

//Common Varables
vec3 result;

//RGB to HLS variables
float r, g, b, tMin, tMax;
float H, S, L;

//RGB to HSL Color Conversion
//Refrences from: http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
vec3 RGB2HSL(vec4 textureDM)
{
	r = textureDM.x / 255;
	g = textureDM.y / 255;
	b = textureDM.z / 255;

	tMax = max(r, g);
	tMax = max(tMax, b);

	tMin = min(r, g);
	tMin = min(tMin, b);

	//Lightness calculation
	L = (tMax + tMin) / 2;

	//Saturation calculation	
	if (L < .5)
	{
		S = (tMax - tMin) / (tMin - tMax);
	}
	else
	{
		S = (tMax - tMin) / (2.0 - tMin - tMax);
	}

	//Hue calculation
	if (tMax == r)
	{
		H = g - b;
	}
	else if (tMax == g)
	{
		H = 2 + b - r;
	}
	else
	{
		H = 4 + r - g;
	}
	H /= (tMax - tMin);

	result = vec3(H, S, L);

	return result;
}

void main()
{
	//sample textures and store as temporary values
	tempDM = texture2D(tex_dm[0], vTexcoord);
	tempSM = texture2D(tex_sm[0], vTexcoord);

	//(2)
	//RGB to HSL Color Conversion
	rtShader1 = vec4(RGB2HSL(tempDM), 1.0);
	rtShader2 = vec4(1.0, 0.0, 0.0, 1.0);
	rtShader3 = vec4(0.0, 1.0, 0.0, 1.0);
	rtShader4 = vec4(0.0, 0.0, 1.0, 1.0);
}
}
