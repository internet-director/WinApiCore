#ifndef _NON_CRT_MATH_HDR
#define _NON_CRT_MATH_HDR

#if defined(NDEBUG)
#ifndef _WIN64
void _alldiv();
void _alldvrm();
void _allmul();
void _allrem();
void _allshl();
void _allshr();
void _aulldiv();
void _aulldvrm();
void _aullrem();
void _aullshr();
#endif
#endif
#endif