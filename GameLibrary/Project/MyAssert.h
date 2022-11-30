#pragma once

#include <assert.h>

// 自作のアサート
// 引数1 bool : 調べたい値
// 引数2 const wchar_t* : falseだったの時に表示してほしいメッセージ
#ifdef NDEBUG
	#define Myassert(expresion, sMessage) (0, 0)
#else
	#define Myassert(expresion, sMessage) if(!expresion) _wassert(sMessage, _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#endif
