#pragma once
#include "common/camera.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "lab_work_6.hpp"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

	const GLenum drawBuffers[5] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4
	};

	LabWork6::~LabWork6()
	{
		// Delete programs
		glDeleteProgram( _geometryPassProgram );
		glDeleteProgram( _shadingPassProgram );
		glDeleteProgram( _postProcessingProgram);

		// Delete VAO
		glDisableVertexArrayAttrib(_frameVAO, 0);
		glDeleteVertexArrays(3, &_frameVBO);

		// Delete VBO
		glDeleteBuffers(1, &_frameVBO);

		// Delete EBO
		glDeleteBuffers(1, &_frameEBO);

		// delete object buffers
		_sponzaModel.cleanGL();
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;
		
		// G_buffer creation
		prepareGBuffer();

		if (initGPassProg() == false) {
			return false;
		}

		//S_pass creation
		if (initSPassProg() == false) {
			return false;
		}

		// Custom_pass creation
		if (initPostProcessingProg() == false) {
			return false;
		}

		// init quad, VBO and VAO
		glCreateBuffers(1, &_frameVBO);
		glNamedBufferData(_frameVBO, sizeof(Vec2f) * _frameCoord.size(), _frameCoord.data(), GL_STATIC_DRAW);

		glCreateVertexArrays(1, &_frameVAO);
		glEnableVertexArrayAttrib(_frameVAO, 0);
		glVertexArrayAttribFormat(_frameVAO, 0, 2, GL_FLOAT, false, 0);
		glVertexArrayVertexBuffer(_frameVAO, 0, _frameVBO, 0, sizeof(Vec2f));
		glVertexArrayAttribBinding(_frameVAO, 0, 0);

		glCreateBuffers(1, &_frameEBO);
		glNamedBufferData(_frameEBO, sizeof(unsigned int) * _vertexIndc.size(), _vertexIndc.data(), GL_STATIC_DRAW);
		glVertexArrayElementBuffer(_frameVAO, _frameEBO);


		// init model and buffers
		_sponzaModel.load( "sponza", ".\\data\\models\\sponza-20221206\\sponza.obj" );
		_sponzaModel._transformation = glm::scale( _sponzaModel._transformation, { 0.003f, 0.003f, 0.003f } );

		// init camera
		_initCamera();
		_camera.setPosition( {0,0,0.2} );

		// init light
		_lightpos  = Vec3f( 0, 300, 0 );
		idVLightPos = glGetUniformLocation(_geometryPassProgram, "uVLightPos" );
		glProgramUniform3fv(_geometryPassProgram, idVLightPos, 1, &_lightpos.x );
		idFLightPos = glGetUniformLocation(_geometryPassProgram, "uFLightPos" );
		glProgramUniform3fv(_geometryPassProgram, idFLightPos, 1, &_lightpos.x );

		// get uniform variable
		idMMatrix = glGetUniformLocation(_geometryPassProgram, "uMMatrix");
		idVMatrix = glGetUniformLocation(_geometryPassProgram, "uVMatrix");
		idMVMatrix	 = glGetUniformLocation(_geometryPassProgram, "uMVMatrix" );
		idMVPMatrix	 = glGetUniformLocation(_geometryPassProgram, "uMVPMatrix" );
		idNormMatrix = glGetUniformLocation(_geometryPassProgram, "uNormalMatrix" );
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

	bool LabWork6::initMainProg() {
		// Read the shader files
		const std::string vertexShaderStr = readFile(_shaderFolder + "mesh_texture.vert");
		const std::string fragShaderStr = readFile(_shaderFolder + "mesh_texture.frag");

		// Convert the string into a char readable by GL
		const GLchar* vSrc = vertexShaderStr.c_str();
		const GLchar* fSrc = fragShaderStr.c_str();

		// Create hollow shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the str source for each shader
		glShaderSource(vertexShader, 1, &vSrc, NULL);
		glShaderSource(fragShader, 1, &fSrc, NULL);

		// Compile the shaders
		glCompileShader(vertexShader);
		glCompileShader(fragShader);

		// Check if compilation is ok .
		GLint compiled;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLchar log[1024];
			glGetShaderInfoLog(vertexShader, sizeof(log), NULL, log);
			glDeleteShader(vertexShader);
			glDeleteShader(fragShader);
			std::cerr << " Error compiling vertex shader : " << log << std::endl;
			return false;
		}

		// Create hollow programs
		idProgram = glCreateProgram();

		// Attach the shaders to the newly created program
		glAttachShader(idProgram, vertexShader);
		glAttachShader(idProgram, fragShader);

		// Links the program to GL
		glLinkProgram(idProgram);

		// Check if link is ok .
		GLint linked;
		glGetProgramiv(idProgram, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLchar log[1024];
			glGetProgramInfoLog(idProgram, sizeof(log), NULL, log);
			std::cerr << " Error linking program : " << log << std::endl;
			return false;
		}

		// Shaders arent needed anymore, we can destroy them
		glDeleteShader(vertexShader);
		glDeleteShader(fragShader);

		return true;
	}

	void LabWork6::prepareGBuffer() {
		// create and bind FBO
		glCreateFramebuffers(1, &idFBO);

		// create textures
		glCreateTextures(GL_TEXTURE_2D, 6, _gBufferTextures);
		glTextureStorage2D(_gBufferTextures[0], 1, GL_RGBA32F, _windowWidth, _windowHeight);
		glTextureStorage2D(_gBufferTextures[1], 1, GL_RGBA32F, _windowWidth, _windowHeight);
		glTextureStorage2D(_gBufferTextures[2], 1, GL_RGBA32F, _windowWidth, _windowHeight);
		glTextureStorage2D(_gBufferTextures[3], 1, GL_RGBA32F, _windowWidth, _windowHeight);
		glTextureStorage2D(_gBufferTextures[4], 1, GL_RGBA32F, _windowWidth, _windowHeight);
		glTextureStorage2D(_gBufferTextures[5], 1, GL_DEPTH_COMPONENT32F, _windowWidth, _windowHeight);

		// link textures and FBO
		glNamedFramebufferTexture(idFBO, GL_COLOR_ATTACHMENT0, _gBufferTextures[0], 0);
		glNamedFramebufferTexture(idFBO, GL_COLOR_ATTACHMENT1, _gBufferTextures[1], 0);
		glNamedFramebufferTexture(idFBO, GL_COLOR_ATTACHMENT2, _gBufferTextures[2], 0);
		glNamedFramebufferTexture(idFBO, GL_COLOR_ATTACHMENT3, _gBufferTextures[3], 0);
		glNamedFramebufferTexture(idFBO, GL_COLOR_ATTACHMENT4, _gBufferTextures[4], 0);
		glNamedFramebufferTexture(idFBO, GL_DEPTH_ATTACHMENT, _gBufferTextures[5], 0);

		// assoc textures at the end of the FShader
		glNamedFramebufferDrawBuffers(idFBO, 5, drawBuffers);

		if (glCheckNamedFramebufferStatus(idFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FBO issue!" << std::endl;
		}
	}

	bool LabWork6::initGPassProg() {
		// Read the shader files
		const std::string vertexShaderStr = readFile(_shaderFolder + "geometry_pass.vert");
		const std::string fragShaderStr = readFile(_shaderFolder + "geometry_pass.frag");

		// Convert the string into a char readable by GL
		const GLchar* vSrc = vertexShaderStr.c_str();
		const GLchar* fSrc = fragShaderStr.c_str();

		// Create hollow shaders
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the str source for each shader
		glShaderSource(vertexShader, 1, &vSrc, NULL);
		glShaderSource(fragShader, 1, &fSrc, NULL);

		// Compile the shaders
		glCompileShader(vertexShader);
		glCompileShader(fragShader);

		// Check if compilation is ok .
		GLint compiled;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLchar log[1024];
			glGetShaderInfoLog(vertexShader, sizeof(log), NULL, log);
			glDeleteShader(vertexShader);
			glDeleteShader(fragShader);
			std::cerr << " Error compiling vertex shader : " << log << std::endl;
			return false;
		}

		// Create hollow programs
		_geometryPassProgram = glCreateProgram();

		// Attach the shaders to the newly created program
		glAttachShader(_geometryPassProgram, vertexShader);
		glAttachShader(_geometryPassProgram, fragShader);

		// Links the program to GL
		glLinkProgram(_geometryPassProgram);

		// Check if link is ok .
		GLint linked;
		glGetProgramiv(_geometryPassProgram, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLchar log[1024];
			glGetProgramInfoLog(_geometryPassProgram, sizeof(log), NULL, log);
			std::cerr << " Error linking program : " << log << std::endl;
			return false;
		}

		// Shaders arent needed anymore, we can destroy them
		glDeleteShader(vertexShader);
		glDeleteShader(fragShader);

		return true;
	}

	bool LabWork6::initSPassProg() {
		// Read the shader files
		const std::string fragShaderStr = readFile(_shaderFolder + "shading_pass.frag");

		// Convert the string into a char readable by GL
		const GLchar* fSrc = fragShaderStr.c_str();

		// Create hollow shaders
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the str source for each shader
		glShaderSource(fragShader, 1, &fSrc, NULL);

		// Compile the shaders
		glCompileShader(fragShader);

		// Check if compilation is ok .
		GLint compiled;
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLchar log[1024];
			glGetShaderInfoLog(fragShader, sizeof(log), NULL, log);
			glDeleteShader(fragShader);
			std::cerr << " Error compiling shading_pass shader : " << log << std::endl;
			return false;
		}

		// Create hollow programs
		_shadingPassProgram = glCreateProgram();

		// Attach the shader to the newly created program
		glAttachShader(_shadingPassProgram, fragShader);

		// Links the program to GL
		glLinkProgram(_shadingPassProgram);

		// Check if link is ok .
		GLint linked;
		glGetProgramiv(_shadingPassProgram, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLchar log[1024];
			glGetProgramInfoLog(_shadingPassProgram, sizeof(log), NULL, log);
			std::cerr << " Error linking program : " << log << std::endl;
			return false;
		}

		// Shaders arent needed anymore, we can destroy them
		glDeleteShader(fragShader);

		return true;
	}

	bool LabWork6::initPostProcessingProg() {
		// Read the shader files
		const std::string fragShaderStr = readFile(_shaderFolder + "post_processing.frag");

		// Convert the string into a char readable by GL
		const GLchar* fSrc = fragShaderStr.c_str();

		// Create hollow shaders
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Set the str source for each shader
		glShaderSource(fragShader, 1, &fSrc, NULL);

		// Compile the shaders
		glCompileShader(fragShader);

		// Check if compilation is ok .
		GLint compiled;
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLchar log[1024];
			glGetShaderInfoLog(fragShader, sizeof(log), NULL, log);
			glDeleteShader(fragShader);
			std::cerr << " Error compiling shading_pass shader : " << log << std::endl;
			return false;
		}

		// Create hollow programs
		_postProcessingProgram = glCreateProgram();

		// Attach the shader to the newly created program
		glAttachShader(_postProcessingProgram, fragShader);

		// Links the program to GL
		glLinkProgram(_postProcessingProgram);

		// Check if link is ok .
		GLint linked;
		glGetProgramiv(_postProcessingProgram, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			GLchar log[1024];
			glGetProgramInfoLog(_postProcessingProgram, sizeof(log), NULL, log);
			std::cerr << " Error linking program : " << log << std::endl;
			return false;
		}

		// Shaders arent needed anymore, we can destroy them
		glDeleteShader(fragShader);

		return true;
	}

	void LabWork6::render() {
		// Clear the previous buffer (go white now)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Normal matrix calculation
		Mat4f NormMatrix = glm::transpose(glm::inverse(_camera.getViewMatrix() * _sponzaModel._transformation));
		glProgramUniformMatrix4fv(_geometryPassProgram, idNormMatrix, 1, false, glm::value_ptr(NormMatrix));

		// Light pos calculation
		Vec3f CurrentLightPos = _camera.getViewMatrix() * _sponzaModel._transformation * Vec4f(_lightpos, 1.0);
		glProgramUniform3fv(_geometryPassProgram, idVLightPos, 1, &CurrentLightPos.x);
		glProgramUniform3fv(_geometryPassProgram, idFLightPos, 1, &CurrentLightPos.x);

		//FBO and program call
		glEnable(GL_DEPTH_TEST);
		glUseProgram(_geometryPassProgram);
		glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_sponzaModel.render(_geometryPassProgram);

		// select the buffer to read
		//glNamedFramebufferReadBuffer(idFBO, drawBuffers[_selectedBuffer]);

		// copy 0->_windowWidth x 0->_windowHeight from the selected buffer to the visible buffer
		//glBlitNamedFramebuffer(idFBO, 0, 0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);



		// Quad draw
		glUseProgram(_shadingPassProgram);
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(_frameVAO);

		//textures
		glBindTextureUnit(0, _gBufferTextures[0]);
		glBindTextureUnit(1, _gBufferTextures[1]);
		glBindTextureUnit(2, _gBufferTextures[2]);
		glBindTextureUnit(3, _gBufferTextures[3]);
		glBindTextureUnit(4, _gBufferTextures[4]);

		glDrawElements(GL_TRIANGLES, _vertexIndc.size(), GL_UNSIGNED_INT, 0);


		glBindTextureUnit(0, 0);
		glBindTextureUnit(1, 0);
		glBindTextureUnit(2, 0);
		glBindTextureUnit(3, 0);
		glBindTextureUnit(4, 0);

		// post processing
		
		//copy current frame buffer into another texture (_gBufferTextures[0] bcuz im lazy to allocate a new one)
		glBlitNamedFramebuffer(0, _gBufferTextures[0], 0, 0, _windowWidth, _windowHeight, 0, 0, _windowWidth, _windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glUseProgram( _postProcessingProgram );
		glClear(GL_COLOR_BUFFER_BIT);


		glBindTextureUnit(0, _gBufferTextures[0]);

		glDrawElements(GL_TRIANGLES, _vertexIndc.size(), GL_UNSIGNED_INT, 0);

		glBindTextureUnit(0, 0);



		glBindVertexArray(0);
	}

	void LabWork6::animate( const float p_deltaTime )
	{
		_time += p_deltaTime;
		_updateClipSpaceMatrix();
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
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

	void LabWork6::displayUI()
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
		//if (ImGui::BeginListBox("Buffer to draw"))
		//{
		//	if (ImGui::Selectable("fragPos",	_selectedBuffer == 0)) {
		//		_selectedBuffer = 0;
		//	}
		//	if (ImGui::Selectable("fragNorm",	_selectedBuffer == 1)) {
		//		_selectedBuffer = 1;
		//	}
		//	if (ImGui::Selectable("fragAmb",	_selectedBuffer == 2)) {
		//		_selectedBuffer = 2;
		//	}
		//	if (ImGui::Selectable("fragDiff",	_selectedBuffer == 3)) {
		//		_selectedBuffer = 3;
		//	}
		//	if (ImGui::Selectable("fragSpec",	_selectedBuffer == 4)) {
		//		_selectedBuffer = 4;
		//	}
		//	if (ImGui::Selectable("DepthMap",	_selectedBuffer == 5)) {
		//		_selectedBuffer = 5;
		//	}
		//	ImGui::EndListBox();
		//}
		//ImGui::End();
	}

	void LabWork6::_updateClipSpaceMatrix()
	{
		Mat4f MtxClip = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _sponzaModel._transformation;
		glProgramUniformMatrix4fv(_geometryPassProgram, idMMatrix, 1, false, glm::value_ptr(_sponzaModel._transformation));
		glProgramUniformMatrix4fv(_geometryPassProgram, idVMatrix, 1, false, glm::value_ptr(_camera.getViewMatrix()));
		glProgramUniformMatrix4fv(_geometryPassProgram,
								   idMVMatrix,
								   1, false, glm::value_ptr( _camera.getViewMatrix() * _sponzaModel._transformation ) );
		glProgramUniformMatrix4fv(_geometryPassProgram, idMVPMatrix, 1, false, glm::value_ptr( MtxClip ) );
	}

	void LabWork6::_initCamera()
	{
		_camera = Camera();
		_camera.setPosition( { 0, 0, 3 } );
		_camera.setScreenSize( _windowWidth, _windowHeight );
	}

} // namespace M3D_ISICG
