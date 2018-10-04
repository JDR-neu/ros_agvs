#ifndef GLOBAL_STRUCT_H
#define GLOBAL_STRUCT_H

typedef union{
    float f;
    unsigned char uc[4];
}float2uchar;

typedef union{
    int d_32;
    unsigned char uc[4];
}int32uchar;

typedef struct{
    int32uchar wheelSpeed[4];
}VData;

#endif // GLOBAL_STRUCT_H
