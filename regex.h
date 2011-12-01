// -*- mode: cpp -*-
// regex.h �� c++ ��
// -lregex
#ifndef __NOMLIB_REGEX_H__
#define __NOMLIB_REGEX_H__

#include <regex.h>
#include <string>
#include <vector>

namespace nl{
  class RegEx{
  public:
	// ���K�\����������R���p�C������
	// @param rxstr ���K�\�� ������
	explicit RegEx(const std::string &rxstr);
	~RegEx();
	// �R���X�g���N�^�ɓn���ꂽ�������Ԃ�
	std::string rx_str() const{ return rx_str_; };
	// ���K�\���𐳏�ɃR���p�C���ł������H
	bool isValid() const { return valid; }
	
	/// ���K�\���Ƀ}�b�`����?
	// @return  �}�b�`������ true
	bool match(const std::string &str_);
	/// �O��̎��s�ɂ��}�b�`��������������� idx �Ԗڂ̂��̂�Ԃ�
	std::string get(int idx) const; // ����������
	int getStartPos(int idx) const; // �J�n�C���f�N�X
	int getEndPos(int idx) const;   // �I���C���f�N�X
	// ����������̌�
	int length() const{ return idx_list.size(); }
	
  private:
	bool valid;
	regex_t reg;
	std::string rx_str_; // �R���X�g���N�^�ɓn���ꂽ������
	// �O�� match()���s�����Ƃ��̕�����
	std::string str;
	// �O��� match()���s�ɂ��}�b�`���������������(�C���f�N�X,����) �������ő�100��
	std::vector<std::pair<int,int> > idx_list;
	
  };
}; // namespace nl

#endif

