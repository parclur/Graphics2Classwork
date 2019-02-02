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

layout(location = 0) out vec4 rtPosition;
layout(location = 1) out vec4 rtNormal;
layout(location = 2) out vec2 rtTexcoord;
layout(location = 3) out vec4 rtDiffuseMap;
layout(location = 4) out vec4 rtSpecularMap;
layout(location = 5) out vec4 rtDiffuseShading;
layout(location = 6) out vec4 rtSpecularShading;
layout(location = 7) out vec4 rtPhongSum;

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
	vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

	//attenuation (c)
	float lightAttVar = .001; // falloff range (the variable increases the falloff range as the number gets smaller)
	float attenuation = 1.0 / (1.0 + lightAttVar * pow(distance(vPosition, uLightPos[lightNumber]), 2));

	// (6) add all of the lighting effects for one light together
	vec3 result = (diffuse + specular) * attenuation;

	return result;
}

vec3 DiffuseShadingTotalCalculation()
{
	vec3 total;

	for (int i; i < max_Lights; i++)
	{
		vec3 diffuse_albedo = uLightCol[i].rgb;

		vec3 N = normalize(vNormal.xyz);

		// unit vector from the point being shaded to the light
		vec3 lightVector = uLightPos[i].xyz - vPosition.xyz;
		vec3 L = normalize(lightVector);

		total += max(dot(N, L), 0.0) * diffuse_albedo;
	}

	return total;
}

vec3 SpecularShadingTotalCalculation()
{
	vec3 total;

	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0; //128

	for (int i; i < max_Lights; i++)
	{
		vec3 diffuse_albedo = uLightCol[i].rgb;

		// vector to the viewer
		vec3 viewVector = -vPosition.xyz;
		vec3 V = normalize(viewVector);
		vec3 N = normalize(vNormal.xyz);

		// unit vector from the point being shaded to the light
		vec3 lightVector = uLightPos[i].xyz - vPosition.xyz;
		vec3 L = normalize(lightVector);

		//Calculate R locally
		// reflection of the negative of the light vector L in the plane defined by N
		vec3 R = reflect(-L, N);

		total += pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;
	}

	return total;
}

void main()
{
	//(3) sample textures and store as temporary values
	tempTex_dm = texture(tex_dm[0], vTexcoord);
	tempTex_sm = texture(tex_sm[0], vTexcoord);

	// (7) calculate Phong shading for all lights
	for (int i = 0; i < max_Lights; i++)
	{
		phongShading += PhongShadingCalculations(i); // (8) add the light values appropriately
	}

	rtPosition = vPosition;
	rtNormal = normalize(vNormal) * .5 + .5;
	rtTexcoord = vTexcoord;
	rtDiffuseMap = tempTex_dm;
	rtSpecularMap = tempTex_sm;
	rtDiffuseShading = vec4(DiffuseShadingTotalCalculation(), 1);
	rtSpecularShading = vec4(SpecularShadingTotalCalculation(), 1);
	rtPhongSum = vec4(phongShading, tempTex_dm.a);
}