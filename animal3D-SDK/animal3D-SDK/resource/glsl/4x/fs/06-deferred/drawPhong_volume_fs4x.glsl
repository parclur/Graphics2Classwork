// This file was modified by Claire Yeash and Zach Phillips with permission of the author

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
	
	drawPhong_volume_fs4x.glsl
	Perform Phong shading for a single light within a volume; output 
		components to MRT.
*/

#version 410

// ****TO-DO: 
//	0) copy entirety of Phong multi-light shader
//	1) geometric inputs from scene objects are not received from VS!
//		-> we are drawing a textured FSQ so where does the geometric 
//			input data come from? declare appropriately
//	2) declare varyings for light volume geometry
//		-> biased clip-space position, index
//	3) declare uniform blocks
//		-> implement data structure matching data in renderer
//		-> replace old lighting uniforms with new block
//	4) declare multiple render targets
//		-> diffuse lighting, specular lighting
//	5) compute screen-space coordinate for current light
//	6) retrieve new geometric inputs (no longer from varyings)
//		-> *test by outputting as color
//	7) use new inputs where appropriate in lighting
//		-> remove anything that can be deferred further

in vec2 vPassTexcoord;

//	(4) declare multiple render targets
//layout (location = 0) out vec4 rtFragColor;
//layout (location = 0) out vec4 rtPosition;
layout (location = 0) out vec4 rtDiffuseShading;
layout (location = 1) out vec4 rtSpecularShading;

// (2) declare varyings for light volume geometry
in vec4 vPassBiasClipCoord;
flat in int vPassInstanceID;

// (3) declare uniform blocks
#define max_lights 1024
struct sPointLight
{
	vec4 worldPos;						// position in world space
	vec4 viewPos;						// position in viewer space
	vec4 color;							// RGB color with padding
	float radius;						// radius (distance of effect from center)
	float radiusInvSq;					// radius inverse squared (attenuation factor)
	float pad[2];						// padding
};
uniform ubPointLight{ //scene objects header for point light
	sPointLight uPointLight[max_lights];
};

// (1) geometric inputs from scene objects are not received from VS!
uniform sampler2D uImage4;
uniform sampler2D uImage5;
uniform sampler2D uImage6;
uniform sampler2D uImage7;

// (1) geometric inputs from scene objects are not received from VS!
vec4 gPosition;
vec4 gNormal;
vec2 gTexcoord;
float gDepth;

//out vec4 rtFragColor;

// declare varyings from vertex shader
//in vec4 vPosition;
//in vec4 vNormal;
//in vec2 vTexcoord;

// declare uniforms for textures (diffuse, specular)
uniform sampler2D tex_dm; // diffuse sample
uniform sampler2D tex_sm; // specular sample

// temporary values for textures
vec4 tempTex_dm;
vec4 tempTex_sm;

// (3) replace old lighting uniforms with new block
//#define max_Lights 8
//uniform int uLightCt;
//uniform vec4 uLightPos[max_Lights];
//uniform vec4 uLightCol[max_Lights];
//uniform float uLightSz[max_Lights];
vec3 phongShading;

//References
//(a) OpenGL SuperBible book pages 574-575
//(b) https://learnopengl.com/Lighting/Basic-Lighting
//(c) https://www.tomdalling.com/blog/modern-opengl/07-more-lighting-ambient-specular-attenuation-gamma/
// implement Phong shading calculation (modular)
vec3 PhongShadingCalculations(int lightNumber)
{
	//(a) all of the values are referenced from the Phong Shading Model in the OpenGL SuperBible
	vec3 diffuse_albedo = uPointLight[lightNumber].color.rgb;
	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0; //128

	//Normalize vectors
	// surface normal
	vec3 N = normalize(gNormal.xyz);
	// unit vector from the point being shaded to the light
	vec3 lightVector = uPointLight[lightNumber].worldPos.xyz - gPosition.xyz;
	vec3 L = normalize(lightVector);
	// vector to the viewer
	vec3 viewVector = -gPosition.xyz;
	vec3 V = normalize(viewVector);
	//Calculate R locally
	// reflection of the negative of the light vector L in the plane defined by N
	vec3 R = reflect(-L, N);

	//Compute the diffuse and specular components for each fragment
	vec3 diffuse = max(dot(N,L), 0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(R,V), 0.0), specular_power) * specular_albedo;
	//attenuation (c)
	float lightAttVar = .001; // falloff range (the variable increases the falloff range as the number gets smaller)
	float attenuation = 1.0 / (1.0 + lightAttVar * pow(distance(gPosition, uPointLight[lightNumber].viewPos),2));

	// add all of the lighting effects for one light together
	vec3 result = (diffuse + specular) * attenuation;
	//vec3 result = (diffuse) * attenuation;

	return result;
}

vec3 diffuseCalculations(int lightNumber)
{
	//(a) all of the values are referenced from the Phong Shading Model in the OpenGL SuperBible
	vec3 diffuse_albedo = uPointLight[lightNumber].color.rgb;

	//Normalize vectors
	// surface normal
	vec3 N = normalize(gNormal).xyz;
	// unit vector from the point being shaded to the light
	vec3 lightVector = uPointLight[lightNumber].viewPos.xyz - gPosition.xyz;
	vec3 L = normalize(lightVector);
	// vector to the viewer
	//vec3 viewVector = -gPosition.xyz;
	//vec3 V = normalize(viewVector);
	//Calculate R locally
	// reflection of the negative of the light vector L in the plane defined by N
	//vec3 R = reflect(-L, N);

	//Compute the diffuse and specular components for each fragment
	vec3 diffuse = max(dot(N,L), 0.0) * diffuse_albedo;
	//attenuation (c)
	float lightAttVar = .001; // falloff range (the variable increases the falloff range as the number gets smaller)
	float lightDistance = length(uPointLight[lightNumber].viewPos.xyz - gPosition.xyz);
	float attenuation = smoothstep(uPointLight[lightNumber].radius, 0, lightDistance); //1.0 / (1.0 + lightAttVar * pow(distance(gPosition, uPointLight[lightNumber].viewPos),2));

	// add all of the lighting effects for one light together
	vec3 result = diffuse * attenuation;

	return result;
}

vec3 specularCalculations(int lightNumber)
{
	//(a) all of the values are referenced from the Phong Shading Model in the OpenGL SuperBible
	vec3 specular_albedo = vec3(0.7);
	float specular_power = 128.0; //128

	// Normalize vectors
	// surface normal
	vec3 N = normalize(gNormal.xyz);
	// unit vector from the point being shaded to the light
	vec3 lightVector = uPointLight[lightNumber].viewPos.xyz - gPosition.xyz;
	vec3 L = normalize(lightVector);
	// vector to the viewer
	vec3 viewVector = -gPosition.xyz;
	vec3 V = normalize(viewVector);
	//Calculate R locally
	// reflection of the negative of the light vector L in the plane defined by N
	vec3 R = reflect(-L, N);

	// Compute the diffuse and specular components for each fragment
	vec3 specular = pow(max(dot(R,V), 0.0), specular_power) * specular_albedo;
	// attenuation (c)
	float lightAttVar = .001; // falloff range (the variable increases the falloff range as the number gets smaller)
	float lightDistance = length(uPointLight[lightNumber].viewPos.xyz - gPosition.xyz);
	float attenuation = smoothstep(uPointLight[lightNumber].radius, 0, lightDistance); //1.0 / (1.0 + lightAttVar * pow(distance(gPosition, uPointLight[lightNumber].viewPos),2));

	// add all of the lighting effects for one light together
	vec3 result = (specular) * attenuation * uPointLight[lightNumber].color.rgb;;

	return result;
}

void main()
{
	// (5) compute screen-space coordinate for current light
	vec4 clipSpace = vPassBiasClipCoord / vPassBiasClipCoord.w;

	// (2) declare varyings for light volume geometry
	gPosition = texture(uImage4, clipSpace.xy);
	gNormal = texture(uImage5, clipSpace.xy);
	gTexcoord = texture(uImage6, clipSpace.xy).xy;
	gDepth = texture(uImage7, clipSpace.xy).x;

	// (3) sample textures and store as temporary values
	tempTex_dm = texture(tex_dm, gTexcoord);
	tempTex_sm = texture(tex_sm, gTexcoord);

	// calculate Phong shading for all lights
	//for(int i = 0; i < max_lights; i++)
	//{
		//phongShading += PhongShadingCalculations(vPassInstanceID); // add the light values appropriately
	//}
	// calculate all the models with textures and output correctly
	// use alpha channel from diffuse sample for final alpha
	//rtFragColor = vec4(phongShading * tempTex_dm.rgb * tempTex_sm.rgb, tempTex_dm.a * tempTex_sm.a);

	// use the correct targets
	rtDiffuseShading = vec4(diffuseCalculations(vPassInstanceID), 1.0);
	rtSpecularShading = vec4(specularCalculations(vPassInstanceID), 1.0);

	// DUMMY OUTPUT: all fragments are FADED MAGENTA
	//rtFragColor = vec4(1.0, 0.5, 1.0, 1.0);
	//rtFragColor = vec4(diffuseCalculations(vPassInstanceID), 1.0);//uPointLight[vPassInstanceID].color;
}