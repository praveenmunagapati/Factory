Factory
=======

C�ŏ������c�[���ƃ��C�u�����̊񂹏W�߂ł��B
�K�v�ɉ����ĕK�v�ȋ@�\���������������Ă������̂Ȃ̂ŎG�R�Ƃ��Ă��܂��B
���s�`�����܂ރA�[�J�C�u�͈ȉ��̏ꏊ���痎�Ƃ��܂��B

	http://stackprobe.dip.jp/_kit/Factory


�J����
--------

Windows 7 Home Premium
Visual C++ 2010 Express Edition (���Ԃ� Visual C++ 2008 Express Edition �ł�OK)

�V�X�e���h���C�u = C


�ꎞ�t�@�C���Ȃ�
----------------

�ꕔ�̃v���O�������ȉ��̃p�X�����ꎞ�t�@�C���Ƃ��Ďg�p���܂��B

	C:\1, C:\2, C:\3, ... C:\999

	���r���h�菇���� fcrlf �����̃p�X�𐶐����Azz �������̃p�X���폜���܂��B


�K���I�Ɉȉ��̃f�B���N�g�����g�p���܂��B

	C:\app       = �t���[�\�t�g���A�O���A�v���u����
	C:\app\Kit   = https://github.com/stackprobe/Kit/tree/master/
	C:\appdata   = APP_DATA
	C:\temp      = �ꎞ�t�@�C���p�A���O�I�����ɋ�ɂ���B
	C:\tmp       = �ꎞ�t�@�C���p�A�N���A���Ȃ��B


�r���h�菇
----------

1. �S�Ẵt�@�C�������[�J���ɓW�J����B

	���̃t�@�C���� C:\Factory\Readme.txt �ƂȂ�悤�ɔz�u����B

2. �R���\�[�����J���B

	OpenConsole.bat �����s����B

3. �S�ăR���p�C���E�����N�i�ȉ��̃R�}���h�����s����j

	> cd build\_cx
	> rebuild
	> ff
	> cx **

4. ���s�R�[�h��CR-LF�ɂ���i�ȉ��̃R�}���h�����s����j

	> fcrlf

5. �ꎞ�t�@�C�����폜����i�ȉ��̃R�}���h�����s����j

	> zz


�⑫
----

�{���|�W�g�����ł� /J �R���p�C���I�v�V�����ɂ�� char �� unsigned char �ɂȂ�܂��B
