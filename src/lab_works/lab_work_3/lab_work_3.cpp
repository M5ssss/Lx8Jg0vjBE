#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_3.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "common/camera.hpp"

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3()
	{
		// Delete program
		glDeleteProgram( idProgram );

		// Delete VAO
		glDisableVertexArrayAttrib( _cube.idVAO, 0 );
		glDisableVertexArrayAttrib( _cube.idVAO, 1);
		glDeleteVertexArrays( 1, &_cube.idVBO );

		// Delete VBO
		glDeleteBuffers( 1, &_cube.idVBO );

		// Delete EBO
		glDeleteBuffers( 1, &_cube.idEBO );
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
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
		_cube = _createCube();
		_initCamera();

		// Create the VBO
		glCreateBuffers( 1, &_cube.idVBO );

		// Populate the VBO
		glNamedBufferData(_cube.idVBO, sizeof( Vec3f ) * _cube.Vertices.size(), _cube.Vertices.data(), GL_STATIC_DRAW );

		// Create the VAO
		glCreateVertexArrays( 1, &_cube.idVAO );

		// Activate and set a0
		glEnableVertexArrayAttrib( _cube.idVAO, 0 );
		glVertexArrayAttribFormat( _cube.idVAO, 0, 3, GL_FLOAT, false, 0 );

		// Link VBO and VAO
		glVertexArrayVertexBuffer( _cube.idVAO, 0, _cube.idVBO, 0, sizeof( Vec3f ) );

		// Connect VAO and vertex shader
		glVertexArrayAttribBinding( _cube.idVAO, 0, 0 );

		// Create EBO
		glCreateBuffers( 1, &_cube.idEBO );

		// Populate EBO
		glNamedBufferData(_cube.idEBO, sizeof( unsigned int ) * _cube.Faces.size(), _cube.Faces.data(), GL_STATIC_DRAW );

		// Link EBO and VAO
		glVertexArrayElementBuffer( _cube.idVAO, _cube.idEBO );

		// Create, populate and link Color VBO
		glCreateBuffers( 1, &_cube.idColorVBO );
		glEnableVertexArrayAttrib( _cube.idVAO, 1 );
		glVertexArrayAttribFormat( _cube.idVAO, 1, 3, GL_FLOAT, false, 0 );
		glNamedBufferData(_cube.idColorVBO, sizeof( Vec3f ) * _cube.VertexColor.size(), _cube.VertexColor.data(), GL_STATIC_DRAW );
		glVertexArrayVertexBuffer( _cube.idVAO, 1, _cube.idColorVBO, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube.idVAO, 1, 1 );

		// get uniform variable
		idTranslationX = glGetUniformLocation( idProgram, "uTranslationX" );
		idTranslationY = glGetUniformLocation( idProgram, "uTranslationY" );
		idBrightness   = glGetUniformLocation( idProgram, "uBrightness" );
		idMtxTransformation = glGetUniformLocation( idProgram, "uMtxModel" );
		idMtxView			= glGetUniformLocation( idProgram, "uMtxView" );
		idMtxProj			= glGetUniformLocation( idProgram, "uMtxProj" );
		idMtxClip			= glGetUniformLocation( idProgram, "uMtxClip" );
		glProgramUniform1f( idProgram, idBrightness, _brightess );
		_updateClipSpaceMatrix();

		// tell gl to depth test
		glEnable( GL_DEPTH_TEST ); 

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Tell GL to use our program
		glUseProgram( idProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( const float p_deltaTime )
	{
		// set translation
		_time += p_deltaTime;
		_cube.MatTransformation = glm::rotate( _cube.MatTransformation, p_deltaTime, { 0, 1, 1 } );
		glProgramUniform1f( idProgram, idTranslationX, glm::sin( _time * _anim_frequency ) * _anim_amplitude );
		glProgramUniform1f( idProgram, idTranslationY, glm::cos( _time * _anim_frequency ) * _anim_amplitude );
		glProgramUniformMatrix4fv(idProgram, idMtxTransformation, 1, false, glm::value_ptr( _cube.MatTransformation ) );
		_updateClipSpaceMatrix();
	}

	void LabWork3::render()
	{
		// Clear the previous buffer (go white now)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// VAO link
		glBindVertexArray( _cube.idVAO );

		// start pipeline
		glDrawElements( GL_TRIANGLES, _cube.Faces.size(), GL_UNSIGNED_INT, 0 );

		// Unlink VAO
		glBindVertexArray( 0 );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateClipSpaceMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateClipSpaceMatrix();
		}
	}


	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		ImGui::SliderFloat( "Animation Frequency", &_anim_frequency, 0.1f, 100.0f );
		ImGui::SliderFloat( "Animation Amplitude", &_anim_amplitude, 0.01f, 2.0f );
		if ( ImGui::SliderFloat( "Brightness", &_brightess, 0.0f, 1.0f ) )
		{
			glProgramUniform1f( idProgram, idBrightness, _brightess );
		}
		if ( ImGui::ColorEdit3( "Background Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}
		if (ImGui::SliderFloat("FOV", &_fov, 10.f, 120.0f)) {
			_camera.setFovy( _fov );
			_updateClipSpaceMatrix();
		}
		ImGui::End();
	}

	LabWork3::Mesh LabWork3::_createCube() {
		Mesh m;
		m.Vertices.push_back( Vec3f( 0.5f, 0.5f, 0.5f ) );
		m.Vertices.push_back( Vec3f( -0.5f, 0.5f, 0.5f ) );
		m.Vertices.push_back( Vec3f( -0.5f, -0.5f,0.5f ) );
		m.Vertices.push_back( Vec3f( 0.5f, -0.5f, 0.5f ) );
		m.Vertices.push_back( Vec3f( 0.5f, 0.5f, -0.5f ) );
		m.Vertices.push_back( Vec3f( -0.5f, 0.5f, -0.5f ) );
		m.Vertices.push_back( Vec3f( -0.5f, -0.5f, -0.5f ) );
		m.Vertices.push_back( Vec3f( 0.5f, -0.5f, -0.5f ) );
		for (int i = 0; i < 8; i++) {
			m.VertexColor.push_back( getRandomVec3f());
		}
		m.Faces = { 0,1,2,  0,2,3,  0,3,7,  0,4,7,  1,2,6,  1,5,6,  2,3,6,  3,6,7,  4,5,6,  4,6,7,};
		m.MatTransformation = glm::mat4(1.0);
		m.MatTransformation = glm::scale( m.MatTransformation, { 0.8f, 0.8f, 0.8f } );
		return m;
	}

	void LabWork3::_updateViewMatrix() {
		glProgramUniformMatrix4fv( idProgram, idMtxView, 1, false, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjectionMatrix() {
		glProgramUniformMatrix4fv( idProgram, idMtxProj, 1, false, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork3::_updateClipSpaceMatrix() {
		Mat4f MtxClip = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube.MatTransformation;
		glProgramUniformMatrix4fv( idProgram, idMtxClip, 1, false, glm::value_ptr( MtxClip ) );
	}

	void LabWork3::_initCamera() {
		_camera = Camera();
		_camera.setPosition( {0,1,3} );
		_camera.setScreenSize( _windowWidth, _windowHeight );
	}

} // namespace M3D_ISICG
