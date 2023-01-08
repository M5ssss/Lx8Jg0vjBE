#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1() {
		// Delete program
		glDeleteProgram( idProgram );

		// Delete VAO
		glDisableVertexArrayAttrib( idVAO, 0 );
		glDeleteVertexArrays( 3, &idVBO );

		// Delete VBO
		glDeleteBuffers( 3, &idVBO );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Read the shader files
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lw1.frag" );
		
		// Convert the string into a char readable by GL
		const GLchar *	  vSrc			  = vertexShaderStr.c_str();
		const GLchar *	  fSrc			  = fragShaderStr.c_str();

		// Create hollow shaders 
		GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
		GLuint fragShader	= glCreateShader( GL_FRAGMENT_SHADER );

		// Set the str source for each shader
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragShader, 1, &fSrc, NULL );

		// Compile the shaders
		glCompileShader( vertexShader );
		glCompileShader( fragShader );

		// Check if compilation is ok .
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Create hollow program
		idProgram = glCreateProgram();

		// Attach the shaders to the newly created program
		glAttachShader( idProgram, vertexShader );
		glAttachShader( idProgram, fragShader );

		// Links the program to GL
		glLinkProgram( idProgram );

		// Check if link is ok .
		GLint linked;
		glGetProgramiv( idProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( idProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		// Shaders arent needed anymore, we can destroy them
		glDeleteShader( vertexShader );
		glDeleteShader( fragShader );

		// populate the triangle vertices
		triangleVertices.push_back( Vec2f(-0.5f, 0.5f) );
		triangleVertices.push_back( Vec2f( 0.5f, 0.5f ) );
		triangleVertices.push_back( Vec2f( 0.5f, -0.5f ) );

		// Create the VBO
		glCreateBuffers( 1, &idVBO );

		// Populate the VBO
		glNamedBufferData( idVBO, sizeof( Vec2f ) * triangleVertices.size(), triangleVertices.data(), GL_STATIC_DRAW );

		// Create the VAO
		glCreateVertexArrays( 1, &idVAO );

		// Activate and set a0
		glEnableVertexArrayAttrib( idVAO, 0 );
		glVertexArrayAttribFormat( idVAO, 0, 2, GL_FLOAT, false, 0 );

		// Link VBO and VAO
		glVertexArrayVertexBuffer( idVAO, 0, idVBO, 0, sizeof(Vec2f));

		// Connect VAO and vertex shader
		glVertexArrayAttribBinding( idVAO, 0, 0 );


		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Tell GL to use our program
		glUseProgram( idProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() { 
		// Clear the previous buffer (go white now)
		glClear( GL_COLOR_BUFFER_BIT );

		// VAO link
		glBindVertexArray( idVAO );

		// start pipeline
		glDrawArrays( GL_TRIANGLES, 0, triangleVertices.size() );

		// Unlink VAO
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
