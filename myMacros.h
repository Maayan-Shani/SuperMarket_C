#pragma once

#define CHECK_RETURN_0(ptr) if(ptr==NULL) return 0;

#define CHECK_MSG_RETURN_0(ptr,msg) { \
						if (ptr == NULL) {\
						printf(#msg); \
						return 0;	\
						} \
						}		

#define FREE_CLOSE_FILE_RETURN_0(ptr,file) {\
						free(ptr);\
						fclose(file);\
						return 0;\
						}
				
#define CLOSE_RETURN_0(file) {   \
					fclose(file);  \
					return 0;	\
					}

