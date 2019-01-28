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
	
	drawPhongMulti_fs4x.glsl
	Calculate and output Phong shading model for multiple lights using data 
		from prior shader.
*/

#version 410

// ****TO-DO: 
//	1) declare varyings to read from vertex shader
//		-> *test all varyings by outputting them as color
//	2) declare uniforms for textures (diffuse, specular)
//	3) sample textures and store as temporary values
//		-> *test all textures by outputting samples
//	4) declare fixed-sized arrays for lighting values and other related values
//		-> *test lighting values by outputting them as color
//		-> one day this will be made easier... soon, soon...
//	5) implement Phong shading calculations
//		-> *save time where applicable
//		-> diffuse, specular, attenuation
//		-> remember to be modular (e.g. write a function)
//	6) calculate Phong shading model for one light
//		-> *test shading values (diffuse, specular) by outputting them as color
//	7) calculate Phong shading for all lights
//		-> *test shading values
//	8) add all light values appropriately
//	9) calculate final Phong model (with textures) and copy to output
//		-> *test the individual shading totals
//		-> use alpha channel from diffuse sample for final alpha

out vec4 rtFragColor;

// (1) declare varyings from vertex shader
in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexcoord;

// (2) declare uniforms for textures (diffuse, specular)
uniform sampler2D tex_dm[]; // diffuse sample
uniform sampler2D tex_sm[]; // specular sample

// (3) temporary values for textures
vec4 tempTex_dm;
vec4 tempTex_sm;

// (8) moved from the vertex shader
// (4) declare fixed-sized arrays for lighting values and other related values
#define max_Lights 8
uniform int uLightCt;
uniform vec4 uLightPos[max_Lights];
uniform vec4 uLightCol[max_Lights];
uniform float uLightSz[max_Lights];

vec3 phongShading;

//References
//(a) OpenGL SuperBible book pages 574-575
//(b) https://learnopengl.com/Lighting/Basic-Lighting
//(c) https://www.tomdalling.com/blog/modern-opengl/07-more-lighting-ambient-specular-attenuation-gamma/


// (5) implement Phong shading calculation (modular)
vec3 PhongShadingCalculations(int lightNumber)
{
	//(a) all of the values are referenced from the Phong Shading Model in the OpenGL SuperBible
	vec3 diffuse_albedo = uLightCol[lightNumber].rgb;
	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0; //128

	//Normalize vectors
	// surface normal
	vec3 N = normalize(vNormal.xyz);

	// unit vector from the point being shaded to the light
	vec3 lightVector = uLightPos[lightNumber].xyz - vPosition.xyz;
	vec3 L = normalize(lightVector);

	// vector to the viewer
	vec3 viewVector = -vPosition.xyz;
	vec3 V = normalize(viewVector);

	//Calculate R locally
	// reflection of the negative of the light vector L in the plane defined by N
	vec3 R = reflect(-L, N);

	//Compute the diffuse and specular components for each fragment
	vec3 diffuse = max(dot(N,L), 0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(R,V), 0.0), specular_power) * specular_albedo;

	//attenuation (c)
	float lightAttVar = .001; // falloff range (the variable increases the falloff range as the number gets smaller)
	float attenuation = 1.0 / (1.0 + lightAttVar * pow(distance(vPosition, uLightPos[lightNumber]),2));

	// (6) add all of the lighting effects for one light together
	vec3 result = (diffuse + specular) * attenuation;

	return result;
}

void main()
{
	//(3) sample textures and store as temporary values
	tempTex_dm = texture(tex_dm[0], vTexcoord);
	tempTex_sm = texture(tex_sm[0], vTexcoord);

	// (7) calculate Phong shading for all lights
	for(int i = 0; i < max_Lights; i++)
	{
		phongShading += PhongShadingCalculations(i); // (8) add the light values appropriately
	}

	// (9) calculate all the models with textures and output correctly
	// use alpha channel from diffuse sample for final alpha
	rtFragColor = vec4(phongShading * tempTex_dm.rgb, tempTex_dm.a);
}