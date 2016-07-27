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
		// GLFW の初期化
		if (!glfwInit())
		{
			printf("Failed to initialize GLFW\n");
			exit(1);
		}

		// プログラム終了時の処理を登録する
		atexit(glfwTerminate);

		// OpenGL Version 3.2 Core Profile を選択する
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

		// 現在のウィンドウを処理対象にする
		//// 引数 window に指定したハンドルのウィンドウのレンダリングコンテキストをカレント(処理対象)にする
		//// レンダリングコンテキストは描画に用いられる情報で、ウィンドウ毎に保持される。
		//// 図形の描画はこれをカレントに設定したウィンドウに対して行われる。
		glfwMakeContextCurrent(window_);


		// GLEW を初期化する
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			printf("Failed to initialize GLEW\n");
			exit(1);
		}

		// 垂直同期のタイミングを待つ
		glfwSwapInterval(1);

		// ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(window_, resize);

		// マウスホイール操作時に呼び出す処理の登録
		glfwSetScrollCallback(window_, wheel);

		// このインスタンスの this ポインタを記録しておく
		glfwSetWindowUserPointer(window_, this);

		// 開いたウィンドウの初期設定
		resize(window_, width, height);
	}


	Window::~Window()
	{
		glfwDestroyWindow(window_);
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    ウィンドウを閉じるべきかを判定する
	//    ウィンドウを閉じる必要がある時に、戻り値は 非 0 になる
	int Window::shouldClose() const
	{
		return glfwWindowShouldClose(window_) || glfwGetKey(window_, GLFW_KEY_ESCAPE);
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    カラーバッファを入れ替えてイベントを取り出す
	void Window::swapBuffers()
	{
		// カラーバッファを入れ替える
		glfwSwapBuffers(window_);

		// イベントを取り出す
		glfwPollEvents();
	}


	// l---------------------------------------------------------------------------
	//  work----------------------------------------
	//    ウィンドウのサイズ変更時の処理
	//  args----------------------------------------
	//    window : windowインスタンス
	//    width  : 横幅
	//    height : 高さ
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
	//    マウスホイール時の処理
	//  args----------------------------------------
	//    window : windowインスタンス
	//    y : ホイールのスピード
	void Window::wheel(GLFWwindow* window, double x, double y)
	{
		// ↑回し y >= 1.0
		// ↓回し y <= -1.0
		//printf("%f %f\n", x, y);

		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));
		if (instance)
		{

		}

	}


} // namespace mso