//This file was modified by Claire Yeash with permission of the author.

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

#define max_Lights 8
uniform int uLightCt;
uniform vec4 uLightPos[max_Lights];
uniform vec4 uLightCol[max_Lights];
uniform float uLightSz[max_Lights];

in vec4 stupidPosition;
in vec4 stupidNormal;
in vec2 stupidTexcoord;

// (2)
uniform sampler2D tex_dm[];
uniform sampler2D tex_sm[];

// (3)
vec4 tempTex_dm;
vec4 tempTex_sm;

//https://learnopengl.com/Lighting/Basic-Lighting
void main()
{
	//(3)
	tempTex_dm = texture(tex_dm[0], stupidTexcoord);
	tempTex_sm = texture(tex_sm[0], stupidTexcoord);

	//ambience
	float ambientStrength = 1.0;
	vec3 ambient = ambientStrength * uLightCol[0].rgb;

	//diffuse
	vec4 norm = normalize(stupidNormal);
	vec4 lightDir = normalize(uLightPos[0] - stupidPosition);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * uLightCol[0].rgb;

	//specular
	float specularStrength = 0.1;
	vec4 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(stupidPosition, reflectDir), 0.0), 16);
	//float spec = pow(max(dot(stupidPosition, lightDir), 0.0), 16);
	vec3 specular = specularStrength * spec * uLightCol[0].rgb;

	//attenuation

	// DUMMY OUTPUT: all fragments are FADED CYAN
	//rtFragColor = vec4(0.5, 1.0, 1.0, 1.0);
	vec3 result = (ambient + diffuse + specular) * tempTex_dm.rgb;

	rtFragColor = vec4(result, 1.0);
}


	//float fDotProduct = max(0.0, dot(stupidNormal, uLightPos[0] - stupidPosition));
	//vec3 vDiffuseColor = tempTex_dm * diffuseLight * fDotProduct;
