//Edited by Zach Phillips and Claire Yeash with permission from the author
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

	drawTangentBasis_gs4x.glsl
	Draw tangent basis.
*/

#version 410

// ****TO-DO: 
//	1) declare input and output varying data
//	2) draw tangent basis in place of each vertex

#define max_verts 18
layout(triangles) in;
layout(line_strip, max_vertices = max_verts) out;

uniform mat4 uP;

//(1)
in mat4 vPassTangentBasis[];

out vec4 vPassColor;

mat4 tangentBasis_Clip;

//colors for the for loop
vec4 colorVectors[3] = vec4[](
	vec4(1.0, 0.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0),
	vec4(0.0, 0.0, 1.0, 1.0));

float axisSz = 0.1;

vec4 color;

void main()
{
	//For each vertex, draw a series of lines from the vertex position
	//offset by tangent, bitangent and normal vectors	

	//loops through the different triangles
	for (int i = 0; i < 3; i++)
	{
		//normalize the values, except for the 4th (vPosition)
		tangentBasis_Clip = uP * mat4(
			normalize(vPassTangentBasis[i][0]),
			normalize(vPassTangentBasis[i][1]),
			normalize(vPassTangentBasis[i][2]),
			vPassTangentBasis[i][3]);

		//Loops though the tangent, bitangent, and normal
		for (int j = 0; j < 3; j++)
		{
			vPassColor = colorVectors[j];

			//Orgin of the vertex
			gl_Position = tangentBasis_Clip[3];
			EmitVertex();

			//second point 
			gl_Position = (tangentBasis_Clip[3] + tangentBasis_Clip[j] * axisSz);
			EmitVertex();

			EndPrimitive();
		}

		//*** OLD CODE, BUT STIL WORKS IF NEEDED ***
		////TANGENT ******
		//color = vec4(1.0, 0.0, 0.0, 1.0);
		//vPassColor = color;
		//
		////Orgin of the vertex
		//gl_Position = tangentBasis_Clip[3];
		//EmitVertex();
		//
		////second point 
		//gl_Position = (tangentBasis_Clip[3] + tangentBasis_Clip[0] * axisSz);
		//EmitVertex();
		//
		//EndPrimitive();
		//
		////BITANGENT ******
		//color = vec4(0.0, 1.0, 0.0, 1.0);
		//vPassColor = color;
		//
		////Orgin of the vertex
		//gl_Position = tangentBasis_Clip[3];
		//EmitVertex();
		//
		////second point 
		//gl_Position = (tangentBasis_Clip[3] + tangentBasis_Clip[1] * axisSz);
		//EmitVertex();
		//
		//EndPrimitive();
		//
		////NORMAL ******
		//color = vec4(0.0, 0.0, 1.0, 1.0);
		//vPassColor = color;
		//
		////Orgin of the vertex
		//gl_Position = tangentBasis_Clip[3];
		//EmitVertex();
		//
		////second point 
		//gl_Position = (tangentBasis_Clip[3] + tangentBasis_Clip[2] * axisSz);
		//EmitVertex();
		//
		//EndPrimitive();
	}
}
