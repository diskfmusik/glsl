#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"


namespace mso
{


	Window::Window(int width, int height, const char* title) :
		scale_(100.0f)
	{
		// GLFW �̏�����
		if (!glfwInit())
		{
			printf("Failed to initialize GLFW\n");
			exit(1);
		}

		// �v���O�����I�����̏�����o�^����
		atexit(glfwTerminate);

		// OpenGL Version 3.2 Core Profile ��I������
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		// glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
		window_ = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!window_)
		{
			printf("Failed to create GLFW window\n");
			exit(1);
		}

		// ���݂̃E�B���h�E�������Ώۂɂ���
		//// ���� window �Ɏw�肵���n���h���̃E�B���h�E�̃����_�����O�R���e�L�X�g���J�����g(�����Ώ�)�ɂ���
		//// �����_�����O�R���e�L�X�g�͕`��ɗp��������ŁA�E�B���h�E���ɕێ������B
		//// �}�`�̕`��͂�����J�����g�ɐݒ肵���E�B���h�E�ɑ΂��čs����B
		glfwMakeContextCurrent(window_);


		// GLEW ������������
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			printf("Failed to initialize GLEW\n");
			exit(1);
		}

		// ���������̃^�C�~���O��҂�
		glfwSwapInterval(1);

		// �E�B���h�E�̃T�C�Y�ύX���ɌĂяo�������̓o�^
		glfwSetWindowSizeCallback(window_, resize);

		// �}�E�X�z�C�[�����쎞�ɌĂяo�������̓o�^
		glfwSetScrollCallback(window_, wheel);

		// ���̃C���X�^���X�� this �|�C���^���L�^���Ă���
		glfwSetWindowUserPointer(window_, this);

		// �J�����E�B���h�E�̏����ݒ�
		resize(window_, width, height);
	}


	Window::~Window()
	{
		glfwDestroyWindow(window_);
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    �E�B���h�E�����ׂ����𔻒肷��
	//    �E�B���h�E�����K�v�����鎞�ɁA�߂�l�� �� 0 �ɂȂ�
	int Window::shouldClose() const
	{
		return glfwWindowShouldClose(window_) || glfwGetKey(window_, GLFW_KEY_ESCAPE);
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
	void Window::swapBuffers()
	{
		// �J���[�o�b�t�@�����ւ���
		glfwSwapBuffers(window_);

		// �C�x���g�����o��
		glfwPollEvents();
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    �E�B���h�E�̃T�C�Y�ύX���̏���
	//  args----------------------------------------
	//    window : window�C���X�^���X
	//    width  : ����
	//    height : ����
	void Window::resize(GLFWwindow* const window, int width, int height)
	{
		glViewport(0, 0, width, height);

		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));
		if (instance)
		{
			instance->size_[0] = static_cast<GLfloat>(width);
			instance->size_[1] = static_cast<GLfloat>(height);
		}
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    �}�E�X�z�C�[�����̏���
	//  args----------------------------------------
	//    window : window�C���X�^���X
	//    y : �z�C�[���̃X�s�[�h
	void Window::wheel(GLFWwindow* window, double x, double y)
	{
		// ���� y >= 1.0
		// ���� y <= -1.0
		//printf("%f %f\n", x, y);

		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));
		if (instance)
		{

		}

	}


} // namespace mso