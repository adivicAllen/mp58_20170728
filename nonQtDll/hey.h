#ifndef HEY_H
#define HEY_H

#if defined (__GNUC__)
  #define BOGO_PRINT_API __attribute__ ((__visibility__("default")))
#elif defined (WIN32)
  #ifdef BUILD_BOGO_DLL
    #define BOGO_PRINT_API __declspec(dllexport)
  #else
    #define BOGO_PRINT_API __declspec(dllimport)
  #endif
#endif
extern "C" BOGO_PRINT_API void printHey();


#endif // HEY_H

