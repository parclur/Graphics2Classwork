//This file was modified by Claire Yeash and Zach Phillips with permission of the author.

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
	
	drawBrightPass_fs4x.glsl
	Perform bright pass filter: bright stays bright, dark gets darker.
*/

#version 410

// ****TO-DO: 
//	1) implement a function to calculate the relative luminance of a color
//	2) implement a general tone-mapping or bright-pass filter
//	3) sample the input texture
//	4) filter bright areas, output result

in vec2 vPassTexcoord;

uniform sampler2D uImage0;

layout (location = 0) out vec4 rtFragColor;

// reference: (a)https://learnopengl.com/Advanced-Lighting/HDR; (b)https://imdoingitwrong.wordpress.com/2010/08/19/why-reinhard-desaturates-my-blacks-3/

void main()
{
	vec4 sample0 = texture(uImage0, vPassTexcoord);

	// Calculates the luminance of each pixel (should preserve both the hue and saturation of the original image)
	float lum = 0.21126 * sample0.r + 0.7152 * sample0.g + 0.0722 * sample0.b; // (1) make sure it always adds up to one
	
	// (2) tone mapping is the process of transforming floating point color values to the expected 0,0 to 1,0 range
	// simplest tone mapping algotithm is Reinhard tone mapping; sperads out all bightness values onto LDR
	
	// tone map try 1
	//float gamma = 2.2; // (a)
	//vec3 hdrColor = sample0.rgb; // (a)
	////reinhard tone mapping
	//vec3 mapped = hdrColor / (hdrColor + vec3(1.0)); // (a); (b) simplfied verson of Reinhar's operation f(x) = x/(x+1); 
	////gamma correction
	//mapped = pow(mapped, vec3(1.0 / gamma)); // take the (1.0/gamma) ~= 1/2 root of mapped
	//rtFragColor = vec4(mapped, 1.0);
	// Results: puts a grey tone to the entire screen

	// tone map try 2
	float gamma = 2.2; // (a)
	vec3 hdrColor = sample0.rgb; // (a)
	//reinhard tone mapping
	vec3 mapped = lum / (lum + vec3(1.0)); // (a); (b) simplfied verson of Reinhar's operation f(x) = x/(x+1); 
	//gamma correction
	mapped = pow(mapped, vec3(1.0 / gamma)); // take the (1.0/gamma) ~= 1/2 root of mapped
	rtFragColor = vec4(mapped * hdrColor, 1.0);
	// Results: decent bloom, looks like Dan's example image

	// tone map try 3 and 4 (b)
	//float tonedLum = lum / (lum + 1); // (b) calculate the luminance directly from the RGB values, perform tone mapping on this values and the scale the original rgb values appropriately
	//vec3 mapped = lum / (lum + vec3(1.0)); // simplfied verson of Reinhar's operation f(x) = x/(x+1)
	//gamma correction
	//mapped = pow(mapped, vec3(1.0 / gamma)); // take the (1.0/gamma) ~= 1/2 root of mapped
	//float scale = tonedLum / lum;
	//hdrColor.r *= scale;
	//hdrColor.g *= scale;
	//hdrColor.b *= scale;
	//rtFragColor = vec4(hdrColor.r, hdrColor.g, hdrColor.b, 1.0);

	//rtFragColor = vec4(smoothstep(.3, 1.0, mapped) , 1.0);
	//tFragColor = vec4(mapped * hdrColor, 1.0);

	//float bright = deserialize(deserialize(lum));

	//rtFragColor = sample0 * lum;
}
