#ifndef DEFS_H
#define DEFS_H 1

// the compiler should optimise this to a real ROR, if it's right...
#define ROR(X,R) (((X)>>(R)) | ((X)<<(32-(R))))

// these may need changing for different compilers/platforms
typedef unsigned int uint5;
typedef signed int sint5;
typedef unsigned short uint4;
typedef signed short sint4;
typedef unsigned char uint3;
typedef signed char sint3;

#endif
