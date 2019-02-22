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

//layout (location = 0) out vec4 rtFragColor;
layout (location = 0) out vec4 rtPosition;
layout (location = 1) out vec4 rtNormal;
layout (location = 2) out vec4 rtTexcoord;
layout (location = 3) out vec4 rtDiffuseMap;
layout (location = 4) out vec4 rtSpecularMap;
layout (location = 5) out vec4 rtDiffuseShading;
layout (location = 6) out vec4 rtSpecularShading;
layout (location = 7) out vec4 rtPhongSum;

out vec4 rtFragColor;

// (1)
uniform sampler2D uImage4;
uniform sampler2D uImage5;
uniform sampler2D uImage6;
uniform sampler2D uImage7;

vec4 gPosition; // (2)
vec4 gNormal; // (2)
vec2 gTexcoord; // (2)
float gDepth; // (2)

//(4) declare fixed-sized arrays for lighting values and other related values
#define max_lights 8
uniform int uLightCt;
uniform vec4 uLightPos[max_lights];
uniform vec4 uLightCol[max_lights];
uniform float uLightSz[max_lights];

//(2) declare uniforms for textures (diffuse, specular)
uniform sampler2D tex_dm[8];
uniform sampler2D tex_sm[8];

// (1) declare varyings to read from vertex shader
//in vec4 vPosition;
//in vec4 vNormal;
//in vec2 vTexcoord;

//(3) sample textures and store as temporary values
vec4 tempDM;
vec4 tempSM;

float ambiantStrength = 0.5;

vec3 PhongCalculations(int lightNumber)
{
	//Setting up the albedo values
	vec3 diffuse_albedo = uLightCol[lightNumber].rgb;
	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0;

	//Normalize the vectors
	vec3 N = normalize(gNormal.xyz);
	vec3 lightVector = uLightPos[lightNumber].xyz - gPosition.xyz;
	vec3 L = normalize(lightVector);
	vec3 viewVector = -gPosition.xyz;
	vec3 V = normalize(viewVector);

	//Calculate R locally
	vec3 R = reflect(-L, N);

	//create the final varable
	vec3 returnValue;

	//calculate ambiant light (Not needed)
	//vec3 ambiant = ambiantStrength * uLightCol[0].rgb;

	//(5) implement Phong shading calculations
	//calculate the diffuse and specular light
	vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;

	//calculate the attenuation
	float lightAttVar = .001; //This varable increases the falloff range as the number gets smaller
	float attenuation = 1.0 / (1.0 + lightAttVar * pow(distance(gPosition, uLightPos[lightNumber]), 2));
	
	//bring them all together
	//returnValue = (ambiant + diffuse + specular) * tempDM.rgb;
	returnValue = (diffuse + specular) * tempDM.rgb * attenuation;

	//(6) calculate Phong shading model for one light
	return returnValue;
}

vec3 DiffuseShadingTotalCalculation()
{
	vec3 total;

	for (int i; i < max_lights; i++)
	{
		vec3 diffuse_albedo = uLightCol[i].rgb;

		vec3 N = normalize(gNormal.xyz);
		vec3 lightVector = uLightPos[i].xyz - gPosition.xyz;
		vec3 L = normalize(lightVector);

		total += max(dot(N, L), 0.0) * diffuse_albedo;
	}	

	return total;
}

vec3 SpecularShadingTotalCalculation()
{
	vec3 total;

	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0;

	for (int i; i < max_lights; i++)
	{
		vec3 diffuse_albedo = uLightCol[i].rgb;

		vec3 viewVector = -gPosition.xyz;
		vec3 V = normalize(viewVector);
		vec3 N = normalize(gNormal.xyz);
		vec3 lightVector = uLightPos[i].xyz - gPosition.xyz;
		vec3 L = normalize(lightVector);

		//Calculate R locally
		vec3 R = reflect(-L, N);

		total += pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;
	}	

	return total;
}

void main()
{
	// DUMMY OUTPUT: all fragments are PURPLE
	//rtFragColor = vec4(0.5, 0.0, 1.0, 1.0);

	gPosition = texture(uImage4, vPassTexcoord); // (2)
	gNormal = texture(uImage5, vPassTexcoord); // (2)
	gTexcoord = texture(uImage6, vPassTexcoord).xy; // (2)
	gDepth = texture(uImage7, vPassTexcoord).x; // (2)
	
	// Testing
	//rtFragColor = gPosition;
	//rtFragColor = vec4(gNormal.xyz * 0.5 + 0.5, 1.0);
	//rtFragColor = vec4(gTexcoord, 0.0, 1.0);
	//rtFragColor = vec4(gDepth, gDepth, gDepth, 1.0);

	//(3) sample textures and store as temporary values
	tempDM = texture2D(tex_dm[0], gTexcoord);
	tempSM = texture2D(tex_sm[0], gTexcoord);
	
	vec3 totalLighting;

	//(7) calculate Phong shading for all lights
	for (int i; i < max_lights; i++)
	{
		//(8) add all light values appropriately
		totalLighting += PhongCalculations(i);
	}

	rtPosition = gPosition; 
	rtNormal = vec4(normalize(gNormal.xyz) * .5 + .5, 1.0); 
	rtTexcoord = vec4(gTexcoord, 0.0, 1.0);
	rtDiffuseMap = tempDM;
	rtSpecularMap = tempSM; //not working properly
	rtDiffuseShading = vec4(DiffuseShadingTotalCalculation(), 1);
	rtSpecularShading = vec4(SpecularShadingTotalCalculation(), 1);
	rtPhongSum = vec4(totalLighting, tempDM.a);
	//rtFragColor = rtPhongSum;
}
