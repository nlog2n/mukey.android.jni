#ifndef __MU_POPEN_H__
#define __MU_POPEN_H__



#ifdef __cplusplus
extern "C" {
#endif


   FILE * _popen(const char *program, const char *type);
   int _pclose(FILE *iop);


#ifdef __cplusplus
}
#endif


#endif





