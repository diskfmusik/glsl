#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include "Window.h"


// 頂点の情報から 画素の情報への 変換処理
// ラスタ化 ラスタライズ

// 処理を複数のステージ(処理の段階)に分けて、
// 各ステージがデータを順送りする処理の形態の事を、パイプライン処理と言う。
// 図形描画のためのパイプライン処理をレンダリングパイプラインと言う。
// ↓
// 1. 頂点の画面上での位置を決定する
// 2. 頂点の位置と図形の種類をもとに画素の情報を生成する(ラスタ化)
// 3. 画素の色を決定して画像を生成する(画素処理)

// 頂点バッファ
// バーテックスシェーダ
// ラスタライザ
// フラグメントシェーダ
// フレームバッファ

// 1. glCreateProgram() プログラムオブジェクトの作成
// 2. glCreateShader() バーテックスシェーダとフラグメントシェーダのシェーダオブジェクトを作成
// 3. glShaderSource() 作成したそれぞれのシェーダオブジェクトに対してソースプログラムを読み込む
// 4. glCompileShader() 読み込んだソースプログラムをコンパイルする
// 5. glAtttachShader() プログラムオブジェクトにシェーダオブジェクトを組み込む
// 6. glLinkProgram() プログラムオブジェクトをリンクする


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    シェーダオブジェクトのコンパイル結果を表示する
//  args------------------------------------------
//    shader : シェーダオブジェクト名
//    str    : コンパイルエラーが発生した場所を示す文字列
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	// コンパイル結果を取得する
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) printf("Compile Error\n");

	// シェーダのコンパイル時のログの長さを取得する
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// シェーダのコンパイル時のログの内容を取得する
		std::vector<GLchar> infolog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infolog[0]);
		std::cerr << &infolog[0] << std::endl;
		//printf("%s\n", &infolog[0]);
	}

	return static_cast<GLboolean>(status);
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    プログラムオブジェクトのリンク結果を表示する
//  args------------------------------------------
//    program : プログラムオブジェクト名
GLboolean printProgramInfoLog(GLuint program)
{
	// コンパイル結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) printf("Link Error\n");

	// シェーダのリンク時のログの長さを取得する
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// シェーダのコンパイル時のログの内容を取得する
		std::vector<GLchar> infolog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infolog[0]);
		std::cerr << &infolog[0] << std::endl;
		//printf("%s\n", &infolog[0]);
	}

	return static_cast<GLboolean>(status);
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    プログラムオブジェクトを作成する
//  args------------------------------------------
//    name : シェーダのソースファイル名
GLchar* readShaderSource(const char* name)
{
	if (!name) return NULL;

	std::ifstream file(name, std::ios::binary);
	if (file.fail())
	{
		printf("Can't open source file\n");
		return NULL;
	}

	file.seekg(0L, std::ios::end);
	const GLsizei length(static_cast<GLsizei>(file.tellg()));
	GLchar* buf(new(std::nothrow) GLchar[length + 1]);
	if (!buf)
	{
		printf("Too large file\n");
		file.close();
		return NULL;
	}

	file.seekg(0L, std::ios::beg);
	file.read(buf, length);
	buf[length] = '\0';

	if (file.bad())
	{
		printf("Could not read source file\n");
		delete[] buf;
		buf = NULL;
	}

	file.close();

	return buf;
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    プログラムオブジェクトを作成する
//  args------------------------------------------
//    vsrc : バーテックスシェーダの名前
//    pv   : vsrc の in の名前
//    fsrc : フラグメントシェーダの名前
//    fc   : fsrc の out の名前
GLuint createProgram(const char* vsrc, const char* pv, const char* fsrc, const char* fc)
{
	// 空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());

	if (vsrc)
	{
		// バーテックスシェーダのシェーダオブジェクトを作成する
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc)
	{
		// フラグメントシェーダのシェーダオブジェクトを作成する
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	// プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, pv);
	glBindFragDataLocation(program, 0, fc);
	glLinkProgram(program);

	// 作成したプログラムオブジェクトを返す
	if (printProgramInfoLog(program))
		return program;

	// 作成出来ていなければ、0 を返す
	glDeleteProgram(program);
	return 0;
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    シェーダのソースファイルを読み込んで、プログラムオブジェクトを作成する
//  args------------------------------------------
//    vsrc : バーテックスシェーダの名前
//    pv   : vsrc の in の名前
//    fsrc : フラグメントシェーダの名前
//    fc   : fsrc の out の名前
GLuint loadProgram(const char* vert, const char* pv, const char* frag, const char* fc)
{
	// シェーダのソースファイルを読み込む
	const GLchar* vsrc(readShaderSource(vert));
	const GLchar* fsrc(readShaderSource(frag));

	// プログラムオブジェクトを作成する
	const GLuint program(createProgram(vsrc, pv, fsrc, fc));

	// 読み込みに使ったメモリを開放する
	delete vsrc;
	delete fsrc;

	// 作成したプログラムオブジェクトを返す
	return program;
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    頂点配列オブジェクトの作成
//  args------------------------------------------
//    vertices : 頂点の数
//    position : 頂点の位置を格納した配列
GLuint createObject(GLuint vertices, const GLfloat(*position)[2])
{
	// 頂点配列オブジェクト
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 頂点バッファオブジェクト
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// __glewBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * vertices, position, GL_STATIC_DRAW);

	// 結合されている頂点バッファオブジェクトを in 変数から参照出来るようにする
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 頂点配列オブジェクトの結合を解除する
	glBindVertexArray(0);

	// 頂点バッファオブジェクトの結合を解除する
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}


struct Object
{
	GLuint vao_;	// 頂点配列オブジェクト
	GLsizei vnum_;	// 頂点数
};


Object createRectangle()
{
	static const GLfloat position[][2] = {
		{ -0.5f, -0.5f },
		{ 0.5f, -0.5f },
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
	};

	static const int vertices(sizeof position / sizeof position[0]);

	Object obj;

	obj.vao_ = createObject(vertices, position);
	obj.vnum_ = vertices;

	return obj;
}


void fps()
{
	static int fpsCounter = 0;
	static int prevTime = (int)time(NULL);
	const int nowTime = (int)time(NULL);

	++fpsCounter;

	if (nowTime != prevTime)
	{
		printf("fps:%d\n", fpsCounter);
		fpsCounter = 0;
	}
	prevTime = (int)time(NULL);
}


int main(void)
{
	// ウィンドウを作成する
	mso::Window window(640, 480, "Hello!");

	// 背景色の指定
	glClearColor(1, 1, 1, 0);


	// プログラムオブジェクトを作成する
	const GLuint program(loadProgram("point.vert", "pv", "point.frag", "fc"));

	// uniform 変数の場所を取得する
	const GLuint sizeLocation(glGetUniformLocation(program, "size"));
	const GLuint scaleLocation(glGetUniformLocation(program, "scale"));

	// 形状データを作成する
	const Object object(createRectangle());


	while (!window.shouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダプログラムの使用開始
		glUseProgram(program);

		// uniform 変数に値を設定する
		glUniform2fv(sizeLocation, 1, window.getSize());
		glUniform1f(scaleLocation, window.getScale());

		// 描画
		glBindVertexArray(object.vao_);
		glDrawArrays(GL_LINE_LOOP, 0, object.vnum_);

		// カラーバッファを入れ替えて、イベントを取り出す
		window.swapBuffers();

		fps();
	}

	return 0;
}