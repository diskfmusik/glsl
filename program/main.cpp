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


// ���_�̏�񂩂� ��f�̏��ւ� �ϊ�����
// ���X�^�� ���X�^���C�Y

// �����𕡐��̃X�e�[�W(�����̒i�K)�ɕ����āA
// �e�X�e�[�W���f�[�^�������肷�鏈���̌`�Ԃ̎����A�p�C�v���C�������ƌ����B
// �}�`�`��̂��߂̃p�C�v���C�������������_�����O�p�C�v���C���ƌ����B
// ��
// 1. ���_�̉�ʏ�ł̈ʒu�����肷��
// 2. ���_�̈ʒu�Ɛ}�`�̎�ނ����Ƃɉ�f�̏��𐶐�����(���X�^��)
// 3. ��f�̐F�����肵�ĉ摜�𐶐�����(��f����)

// ���_�o�b�t�@
// �o�[�e�b�N�X�V�F�[�_
// ���X�^���C�U
// �t���O�����g�V�F�[�_
// �t���[���o�b�t�@

// 1. glCreateProgram() �v���O�����I�u�W�F�N�g�̍쐬
// 2. glCreateShader() �o�[�e�b�N�X�V�F�[�_�ƃt���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬
// 3. glShaderSource() �쐬�������ꂼ��̃V�F�[�_�I�u�W�F�N�g�ɑ΂��ă\�[�X�v���O������ǂݍ���
// 4. glCompileShader() �ǂݍ��񂾃\�[�X�v���O�������R���p�C������
// 5. glAtttachShader() �v���O�����I�u�W�F�N�g�ɃV�F�[�_�I�u�W�F�N�g��g�ݍ���
// 6. glLinkProgram() �v���O�����I�u�W�F�N�g�������N����


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
//  args------------------------------------------
//    shader : �V�F�[�_�I�u�W�F�N�g��
//    str    : �R���p�C���G���[�����������ꏊ������������
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	// �R���p�C�����ʂ��擾����
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) printf("Compile Error\n");

	// �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
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
//    �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
//  args------------------------------------------
//    program : �v���O�����I�u�W�F�N�g��
GLboolean printProgramInfoLog(GLuint program)
{
	// �R���p�C�����ʂ��擾����
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) printf("Link Error\n");

	// �V�F�[�_�̃����N���̃��O�̒������擾����
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
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
//    �v���O�����I�u�W�F�N�g���쐬����
//  args------------------------------------------
//    name : �V�F�[�_�̃\�[�X�t�@�C����
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
//    �v���O�����I�u�W�F�N�g���쐬����
//  args------------------------------------------
//    vsrc : �o�[�e�b�N�X�V�F�[�_�̖��O
//    pv   : vsrc �� in �̖��O
//    fsrc : �t���O�����g�V�F�[�_�̖��O
//    fc   : fsrc �� out �̖��O
GLuint createProgram(const char* vsrc, const char* pv, const char* fsrc, const char* fc)
{
	// ��̃v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(glCreateProgram());

	if (vsrc)
	{
		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
		if (printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc)
	{
		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���쐬����
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g���v���O�����I�u�W�F�N�g�ɑg�ݍ���
		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	// �v���O�����I�u�W�F�N�g�������N����
	glBindAttribLocation(program, 0, pv);
	glBindFragDataLocation(program, 0, fc);
	glLinkProgram(program);

	// �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
	if (printProgramInfoLog(program))
		return program;

	// �쐬�o���Ă��Ȃ���΁A0 ��Ԃ�
	glDeleteProgram(program);
	return 0;
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���ŁA�v���O�����I�u�W�F�N�g���쐬����
//  args------------------------------------------
//    vsrc : �o�[�e�b�N�X�V�F�[�_�̖��O
//    pv   : vsrc �� in �̖��O
//    fsrc : �t���O�����g�V�F�[�_�̖��O
//    fc   : fsrc �� out �̖��O
GLuint loadProgram(const char* vert, const char* pv, const char* frag, const char* fc)
{
	// �V�F�[�_�̃\�[�X�t�@�C����ǂݍ���
	const GLchar* vsrc(readShaderSource(vert));
	const GLchar* fsrc(readShaderSource(frag));

	// �v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(createProgram(vsrc, pv, fsrc, fc));

	// �ǂݍ��݂Ɏg�������������J������
	delete vsrc;
	delete fsrc;

	// �쐬�����v���O�����I�u�W�F�N�g��Ԃ�
	return program;
}


// l---------------------------------------------------------------------------
//  work------------------------------------------
//    ���_�z��I�u�W�F�N�g�̍쐬
//  args------------------------------------------
//    vertices : ���_�̐�
//    position : ���_�̈ʒu���i�[�����z��
GLuint createObject(GLuint vertices, const GLfloat(*position)[2])
{
	// ���_�z��I�u�W�F�N�g
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// ���_�o�b�t�@�I�u�W�F�N�g
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// __glewBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * vertices, position, GL_STATIC_DRAW);

	// ��������Ă��钸�_�o�b�t�@�I�u�W�F�N�g�� in �ϐ�����Q�Əo����悤�ɂ���
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// ���_�z��I�u�W�F�N�g�̌�������������
	glBindVertexArray(0);

	// ���_�o�b�t�@�I�u�W�F�N�g�̌�������������
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}


struct Object
{
	GLuint vao_;	// ���_�z��I�u�W�F�N�g
	GLsizei vnum_;	// ���_��
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
	// �E�B���h�E���쐬����
	mso::Window window(640, 480, "Hello!");

	// �w�i�F�̎w��
	glClearColor(1, 1, 1, 0);


	// �v���O�����I�u�W�F�N�g���쐬����
	const GLuint program(loadProgram("point.vert", "pv", "point.frag", "fc"));

	// uniform �ϐ��̏ꏊ���擾����
	const GLuint sizeLocation(glGetUniformLocation(program, "size"));
	const GLuint scaleLocation(glGetUniformLocation(program, "scale"));

	// �`��f�[�^���쐬����
	const Object object(createRectangle());


	while (!window.shouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// �V�F�[�_�v���O�����̎g�p�J�n
		glUseProgram(program);

		// uniform �ϐ��ɒl��ݒ肷��
		glUniform2fv(sizeLocation, 1, window.getSize());
		glUniform1f(scaleLocation, window.getScale());

		// �`��
		glBindVertexArray(object.vao_);
		glDrawArrays(GL_LINE_LOOP, 0, object.vnum_);

		// �J���[�o�b�t�@�����ւ��āA�C�x���g�����o��
		window.swapBuffers();

		fps();
	}

	return 0;
}