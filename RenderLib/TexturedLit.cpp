#include "stdafx.h"
#include "texturedLit.h"
#include "GeometryHelper.h"
#include <cmath>

void TexturedLit::CreateGeometry(const GeometryHelper &gHelper )
{
	// load the shader program
	if ( normalMap != nullptr )
		shader = new SOF::ShaderProgram("shaders/vshader_tex_bump.glsl", "shaders/fshader_tex_bump.glsl");
	else
		shader = new SOF::ShaderProgram("shaders/vshader_tex.glsl", "shaders/fshader_tex.glsl");
	// make the SOF::Geometry object
	geom = new SOF::Geometry(gHelper.GetNumVertices());
	// add the position attribute
	geom->AddAttribute(gHelper.GetPositionVector(), "vertexPos");
	geom->AddAttribute(gHelper.GetNormalVector(), "vertexNormal");
	geom->AddAttribute(gHelper.GetUVVector(), "vertexUV");
	if ( normalMap != nullptr )
		geom->AddAttribute(gHelper.GetTangentVector(), "vertexTangent");
	// finish up
	geom->Finalize(shader);
	// clean up
}

void TexturedLit::Draw(const Transforms &trans, const PointLight &light)
{

	// calculate the mvp matrix;
	glm::mat4 vp = trans.GetProj() * trans.GetView();
	// enable the shader, set uniforms
	shader->Use();
	shader->SetUniformMat4("viewProj", vp);
	shader->SetUniformMat4("model", trans.GetModel());
	// get the camera position
	glm::vec3 camPos = - glm::vec3(trans.GetView()[3]) * glm::mat3(trans.GetView());
	// set the lighting uniforms
	// set the lighting uniforms
	shader->SetUniformVec3("lightK", glm::vec3(light.GetAmbientLevel(), light.GetDiffuseLevel(), light.GetSpecularLevel()));
	shader->SetUniformFloat("alpha", light.GetSpecularPower());
	shader->SetUniformMat4("lightPos", light.GetPosition());
	shader->SetUniformVec3("cameraPos", camPos);
	shader->SetUniformVec4("ambientColor", light.GetColor()[0]);
	shader->SetUniformMat4("lightColor", light.GetColor());
	glm::vec4 attConstant = glm::vec4(1, 1, 1, 1) / (light.GetAttenuationRadius() *
		light.GetAttenuationRadius());
	shader->SetUniformVec4("attenuation", attConstant);
	// hook up the textures
	shader->AssignTextureSampler("textureSampler", texture);
	if ( normalMap != nullptr )	
		shader->AssignTextureSampler("normalSampler", normalMap);
	// draw the triangles
	geom->DrawPrimitives();
}
