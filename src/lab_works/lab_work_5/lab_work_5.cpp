#pragma once
#include "common/camera.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_5.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_5/shaders/";

	LabWork5::~LabWork5()
	{
		// Delete program
		glDeleteProgram( idProgram );

		// delete object buffers
		_sponzaModel.cleanGL();
	}

	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;
		// Read the shader files
		const std::string vertexShaderStr = readFile( _shaderFolder + "mesh_texture.vert" );
		const std::string fragShaderStr	  = readFile( _shaderFolder + "mesh_texture.frag" );

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

		// init model and buffers
		_sponzaModel.load( "sponza", ".\\data\\models\\sponza-20221206\\sponza.obj" );
		_sponzaModel._transformation = glm::scale( _sponzaModel._transformation, { 0.003f, 0.003f, 0.003f } );

		// init camera
		_initCamera();
		_camera.setPosition( {0,0,0.2} );

		// init light
		_lightpos  = Vec3f( 0, 300, 0 );
		idVLightPos = glGetUniformLocation( idProgram, "uVLightPos" );
		glProgramUniform3fv( idProgram, idVLightPos, 1, &_lightpos.x );
		idFLightPos = glGetUniformLocation( idProgram, "uFLightPos" );
		glProgramUniform3fv( idProgram, idFLightPos, 1, &_lightpos.x );

		// get uniform variable
		idMMatrix = glGetUniformLocation(idProgram, "uMMatrix");
		idVMatrix = glGetUniformLocation(idProgram, "uVMatrix");
		idMVMatrix = glGetUniformLocation(idProgram, "uMVMatrix");
		idMVPMatrix = glGetUniformLocation(idProgram, "uMVPMatrix");
		idNormMatrix = glGetUniformLocation(idProgram, "uNormalMatrix");
		_updateClipSpaceMatrix();

		// enable depth test and transparency
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_BLEND );
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// Tell GL to use our program
		glUseProgram( idProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork5::animate( const float p_deltaTime )
	{
		_time += p_deltaTime;
		_updateClipSpaceMatrix();
	}

	void LabWork5::render()
	{
		// Clear the previous buffer (go white now)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Normal matrix calculation
		Mat4f NormMatrix = glm::transpose( glm::inverse( _camera.getViewMatrix() * _sponzaModel._transformation ) );
		glProgramUniformMatrix4fv( idProgram, idNormMatrix, 1, false, glm::value_ptr( NormMatrix ) );

		// Light pos calculation
		Vec3f CurrentLightPos = _camera.getViewMatrix() * _sponzaModel._transformation * Vec4f( _lightpos, 1.0 );
		glProgramUniform3fv( idProgram, idVLightPos, 1, &CurrentLightPos.x );
		glProgramUniform3fv( idProgram, idFLightPos, 1, &CurrentLightPos.x );

		// Render the bunny
		_sponzaModel.render( idProgram );
	}

	void LabWork5::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_camera.getViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_camera.getViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_camera.getViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_camera.getViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_camera.getViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_camera.getViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_camera.getViewMatrix();
		}
	}

	void LabWork5::displayUI()
	{
		ImGui::Begin( "Settings lab work 5" );
		if ( ImGui::ColorEdit3( "Background Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}
		if ( ImGui::SliderFloat( "FOV", &_fov, 10.f, 120.0f ) )
		{
			_camera.setFovy( _fov );
			_updateClipSpaceMatrix();
		}
		ImGui::End();
	}

	void LabWork5::_updateClipSpaceMatrix()
	{
		Mat4f MtxClip = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _sponzaModel._transformation;
		glProgramUniformMatrix4fv(idProgram, idMMatrix, 1, false, glm::value_ptr(_sponzaModel._transformation));
		glProgramUniformMatrix4fv(idProgram, idVMatrix, 1, false, glm::value_ptr(_camera.getViewMatrix()));
		glProgramUniformMatrix4fv(idProgram,
			idMVMatrix,
			1, false, glm::value_ptr(_camera.getViewMatrix() * _sponzaModel._transformation));
		glProgramUniformMatrix4fv(idProgram, idMVPMatrix, 1, false, glm::value_ptr(MtxClip));
	}

	void LabWork5::_initCamera()
	{
		_camera = Camera();
		_camera.setPosition( { 0, 0, 3 } );
		_camera.setScreenSize( _windowWidth, _windowHeight );
	}

} // namespace M3D_ISICG
