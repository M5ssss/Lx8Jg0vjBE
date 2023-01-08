#include "imgui.h"
#include "lab_work_2.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		// Delete program
		glDeleteProgram( idProgram );

		// Delete VAO
		glDisableVertexArrayAttrib( idVAO, 0 );
		glDeleteVertexArrays( 3, &idVBO );

		// Delete VBO
		glDeleteBuffers( 1, &idVBO );

		// Delete EBO
		glDeleteBuffers( 1, &idEBO );
	}

	void LabWork2::init_circle( Vec2f C, unsigned int N, float R ) {
		// populate the vertices vector, the face vector and the color vector
		for ( float i = 0; i < 2 * M_PI; i += ( 2 * M_PI ) / N )
		{
			Vertices.push_back( Vec2f( C.x + (cos(i) * R), C.y + (sin(i) * R))); // stopped here
			VertexColor.push_back( Vec4f( getRandomFloat(), getRandomFloat(), getRandomFloat(), getRandomFloat() ) );
		}
		Vertices.push_back( Vec2f( C.x, C.y ) ); // center = i_div + 1
		VertexColor.push_back( Vec4f( getRandomFloat(), getRandomFloat(), getRandomFloat(), getRandomFloat() ) );
		std::cout << Vertices.size() << std::endl;


		for ( int i = 0; i < N; i++ )
		{
			Faces.push_back( N + 1 );
			Faces.push_back(i);
			Faces.push_back( ( i + 1 ) % N );
		}

	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;
		// Read the shader files
		const std::string vertexShaderStr = readFile( _shaderFolder + "lw1.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "lw1.frag" );

		// Convert the string into a char readable by GL
		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragShaderStr.c_str();

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

		// populate the vertices vector, the face vector and the color vector

		/* regular exercise
		Vertices.push_back( Vec2f( -0.5f, 0.5f ) );
		Vertices.push_back( Vec2f( 0.5f, 0.5f ) );
		Vertices.push_back( Vec2f( 0.5f, -0.5f ) );
		Vertices.push_back( Vec2f( -0.5f, -0.5f ) );

		Faces = { 0, 1, 2, 0, 2, 3 };

		VertexColor.push_back( Vec4f( 1.0, 0.0, 0.0, 1.0 ) );
		VertexColor.push_back( Vec4f( 0.0, 1.0, 0.0, 1.0 ) );
		VertexColor.push_back( Vec4f( 0.0, 0.0, 1.0, 1.0 ) );
		VertexColor.push_back( Vec4f( 1.0, 0.0, 1.0, 1.0 ) );
		*/

		// Optional exercise
		LabWork2::init_circle( Vec2f( 0, 0 ), 128, 0.5 );

		// Create the VBO
		glCreateBuffers( 1, &idVBO );

		// Populate the VBO
		glNamedBufferData( idVBO, sizeof( Vec2f ) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW );

		// Create the VAO
		glCreateVertexArrays( 1, &idVAO );

		// Activate and set a0
		glEnableVertexArrayAttrib( idVAO, 0 );
		glVertexArrayAttribFormat( idVAO, 0, 2, GL_FLOAT, false, 0 );

		// Link VBO and VAO
		glVertexArrayVertexBuffer( idVAO, 0, idVBO, 0, sizeof( Vec2f ) );

		// Connect VAO and vertex shader
		glVertexArrayAttribBinding( idVAO, 0, 0 );

		// Create EBO
		glCreateBuffers( 1, &idEBO );

		// Populate EBO
		glNamedBufferData( idEBO, sizeof( unsigned int ) * Faces.size(), Faces.data(), GL_STATIC_DRAW );

		// Link EBO and VAO
		glVertexArrayElementBuffer( idVAO, idEBO );

		// Create, populate and link Color VBO
		glCreateBuffers( 1, &idColorVBO );
		glEnableVertexArrayAttrib( idVAO, 1 );
		glVertexArrayAttribFormat( idVAO, 1, 4, GL_FLOAT, false, 0 );
		glNamedBufferData( idColorVBO, sizeof( Vec4f ) * VertexColor.size(), VertexColor.data(), GL_STATIC_DRAW );
		glVertexArrayVertexBuffer( idVAO, 1, idColorVBO, 0, sizeof( Vec4f ) );
		glVertexArrayAttribBinding( idVAO, 1, 1 );


		// get uniform variable
		idTranslationX = glGetUniformLocation( idProgram, "uTranslationX" );
		idTranslationY = glGetUniformLocation( idProgram, "uTranslationY" );
		idBrightness   = glGetUniformLocation( idProgram, "uBrightness" );
		glProgramUniform1f( idProgram, idBrightness, _brightess );

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Tell GL to use our program
		glUseProgram( idProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {
		// set translation
		_time += p_deltaTime;
		glProgramUniform1f( idProgram, idTranslationX, glm::sin( _time * _anim_frequency ) * _anim_amplitude );
		glProgramUniform1f( idProgram, idTranslationY, glm::cos( _time * _anim_frequency ) * _anim_amplitude );
	}

	void LabWork2::render()
	{
		// Clear the previous buffer (go white now)
		glClear( GL_COLOR_BUFFER_BIT );

		// VAO link
		glBindVertexArray( idVAO );

		// start pipeline
		glDrawElements( GL_TRIANGLES, Faces.size(), GL_UNSIGNED_INT, 0 );

		// Unlink VAO
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );
		ImGui::SliderFloat( "Animation Frequency", &_anim_frequency, 0.1f, 100.0f );
		ImGui::SliderFloat( "Animation Amplitude", &_anim_amplitude, 0.01f, 2.0f );
		if (ImGui::SliderFloat("Brightness", &_brightess, 0.0f, 1.0f)) {
			glProgramUniform1f( idProgram, idBrightness, _brightess );
		}
		if (ImGui::ColorEdit3("Background Color", glm::value_ptr(_bgColor))) {
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}
		ImGui::End();
	}

} // namespace M3D_ISICG
