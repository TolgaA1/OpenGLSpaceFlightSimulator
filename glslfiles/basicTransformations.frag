#version 400

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightPos;  //light direction arriving from the vertex
in vec3 ex_LightDir;

in vec3 ex_PositionEye;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;

uniform sampler2D DiffuseMap;
uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;
uniform float constantAttenuation2;
uniform float linearAttenuation2;
uniform float quadraticAttenuation2;
void main(void)
{
	float dist,att;
	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture values

	//out_Color = vec4(ex_Normal,1.0); //Show normals

	//out_Color = vec4(ex_TexCoord,0.0,1.0); //show texture coords

	//Calculate lighting
	vec3 n, L;
	vec4 color, color1,color2;
	float NdotL;
	color = light_ambient * material_ambient;

	dist = length(ex_LightPos - ex_PositionEye);
	n = normalize(ex_Normal);
	L = normalize(ex_LightPos-ex_PositionEye);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);

	
	
	if(NdotL > 0.0) 
	{
			att = min(1.0 / (constantAttenuation +
                linearAttenuation * dist +
                quadraticAttenuation * dist * dist),1);

		color1 = (light_ambient * material_diffuse * NdotL);
	}

	color1 += material_specular * light_specular * pow(RdotV, material_shininess);

	color1 *=att;
	color+=color1;
	//light source 2
	


	

	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);

	NdotL = max(dot(n, L),0.0);

	if(NdotL > 0.0)
	{
		color += (light_diffuse * material_diffuse * NdotL);
	}
	//out_Color = color;  //show just lighting

	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture only
    out_Color = color * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}