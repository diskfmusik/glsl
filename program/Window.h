#ifndef _MSO_WINDOW_
#define _MSO_WINDOW_

namespace mso
{

	class Window
	{
	public:

		Window(int width = 640, int height = 480, const char* title = "Hello!");
		virtual ~Window();

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    �E�B���h�E�����ׂ����𔻒肷��
		//    �E�B���h�E�����K�v�����鎞�ɁA�߂�l�� �� 0 �ɂȂ�
		int shouldClose() const;

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    �J���[�o�b�t�@�����ւ��ăC�x���g�����o��
		void swapBuffers();

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    �E�B���h�E�̃T�C�Y�����o��
		const GLfloat* getSize()const { return size_; }

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    ���[���h���W�n�ɑ΂���f�o�C�X���W�n�̊g�嗦�����o��
		GLfloat getScale() const { return scale_; }

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    �E�B���h�E�̃T�C�Y�ύX���̏���
		//  args----------------------------------------
		//    window : window�C���X�^���X
		//    width  : ����
		//    height : ����
		static void resize(GLFWwindow* const window, int width, int height);

		// l---------------------------------------------------------------------------
		//  work----------------------------------------
		//    �}�E�X�z�C�[�����̏���
		//  args----------------------------------------
		//    window : window�C���X�^���X
		//    y : �z�C�[���̃X�s�[�h
		static void wheel(GLFWwindow* window, double x, double y);

	private:
		GLFWwindow* window_;

		GLfloat size_[2];

		// ���[���h���W�n�ɑ΂���f�o�C�X���W�n�̊g�嗦
		GLfloat scale_;
	};

} // namespace mso

#endif