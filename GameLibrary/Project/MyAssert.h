#pragma once

#include <assert.h>

// ����̃A�T�[�g
// ����1 bool : ���ׂ����l
// ����2 const wchar_t* : false�������̎��ɕ\�����Ăق������b�Z�[�W
#ifdef NDEBUG
	#define Myassert(expresion, sMessage) (0, 0)
#else
	#define Myassert(expresion, sMessage) if(!expresion) _wassert(sMessage, _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#endif
