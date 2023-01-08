#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const {
		// Link Program
		glUseProgram( p_glProgram );

		// VAO link
		glBindVertexArray( _vao );

		// Set materials properties
		//// Ambiant
		glProgramUniform1i(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasAmbiantMap" ), _material._hasAmbientMap );
		glProgramUniform3fv(
			p_glProgram, glGetUniformLocation( p_glProgram, "uColorAmbiant" ), 1, &_material._ambient[ 0 ] );
		if ( _material._hasAmbientMap )
		{
			glBindTextureUnit( 0, _material._ambientMap._id );
		}
		else
		{
			glProgramUniform3fv(
				p_glProgram, glGetUniformLocation( p_glProgram, "uColorAmbiant" ), 1, &_material._ambient[ 0 ] );
		}
		//// Diffuse
		glProgramUniform1i(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasDiffuseMap" ), _material._hasDiffuseMap );
		if (_material._hasDiffuseMap) {
			glBindTextureUnit( 1, _material._diffuseMap._id );
		}
		else {
			glProgramUniform3fv(
				p_glProgram, glGetUniformLocation( p_glProgram, "uColorDiffuse" ), 1, &_material._diffuse[ 0 ] );
		}
		//// Specular
		glProgramUniform1i(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasSpecularMap" ), _material._hasSpecularMap );
		if ( _material._hasSpecularMap )
		{
			glBindTextureUnit( 2, _material._specularMap._id );
		}
		else
		{
			glProgramUniform3fv(
				p_glProgram, glGetUniformLocation( p_glProgram, "uColorSpecular" ), 1, &_material._specular[ 0 ] );
		}
		//// Shininess
		glProgramUniform1i(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasShineMap" ), _material._hasShininessMap );
		if ( _material._hasShininessMap )
		{
			glBindTextureUnit( 3, _material._shininessMap._id );
		}
		else
		{
			glProgramUniform1f(
				p_glProgram, glGetUniformLocation( p_glProgram, "uFvShininess" ), _material._shininess );
		}
		//// Normal
		glProgramUniform1i(
			p_glProgram, glGetUniformLocation( p_glProgram, "uHasNormalMap" ), _material._hasNormalMap );
		if ( _material._hasNormalMap )
		{
			glBindTextureUnit( 4, _material._normalMap._id );
		}



		// Start pipeline
		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );

		if ( _material._hasDiffuseMap )
		{
			glBindTextureUnit( 0, 0 );
			glBindTextureUnit( 1, 0 );
			glBindTextureUnit( 2, 0 );
		}

		// Unlink VAO
		glBindVertexArray( 0 );

	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL() {
		//_vao
		glCreateVertexArrays( 1, &_vao );
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, false, offsetof( Vertex, _position ) );
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, false, offsetof( Vertex, _normal ) );
		glEnableVertexArrayAttrib( _vao, 2 );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, false, offsetof( Vertex, _texCoords ) );
		glEnableVertexArrayAttrib( _vao, 3 );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, false, offsetof( Vertex, _tangent ) );
		glEnableVertexArrayAttrib( _vao, 4 );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, false, offsetof( Vertex, _bitangent ) );
		////vtx shader
		glVertexArrayAttribBinding( _vao, 0, 0 );
		glVertexArrayAttribBinding( _vao, 1, 0 );
		glVertexArrayAttribBinding( _vao, 2, 0 );
		glVertexArrayAttribBinding( _vao, 3, 0 );
		glVertexArrayAttribBinding( _vao, 4, 0 );

		//_vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, sizeof( Vertex ) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW );
		////link
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

		//_ebo
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, sizeof( unsigned int ) * _indices.size(), _indices.data(), GL_STATIC_DRAW );
		////link
		glVertexArrayElementBuffer( _vao, _ebo );
	}
} // namespace M3D_ISICG
