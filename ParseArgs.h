#pragma once
#include <stdint.h>

enum DATA_TYPE{
	INT,
	BOOL,
	STRING
};

typedef struct{
	const char* name;
	int type;
		
}struct_def;

void * parse_args(int argc, char* argv[], void * struct_ptr, struct_def defs[], int defs_size, int amount_of_static_args);

#ifdef PARSE_SWITCH_IMPLEMENT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

char** create_struct_bounds(char* ret, struct_def defs[], int defs_size){
	char ** bounds = (char**)malloc(sizeof(char*) * defs_size);

	char * tmp_ptr = ret;
	for(int i =0; i < defs_size; i++){
		bounds[i] = tmp_ptr;
		switch(defs[i].type){
			case INT:
				tmp_ptr += sizeof(int);
			break;
	
			case BOOL:
				tmp_ptr += sizeof(char);
			break;

			case STRING:
				tmp_ptr += sizeof(char*);
			break;		
		}
	}

	
	return bounds;
}

int str_eq(char * str1, char * str2){
	if(strlen(str1) != strlen(str2))
		return 0;

	while(1){
		if(*str1 == 0x00)
			return 1;
		if(*str1 != *str2)
			return 0;
		str1++;
		str2++;
	}
	return 1;
}


void * parse_args(int argc, char* argv[], void * struct_ptr, struct_def defs[], int defs_size, int amount_of_static_args){
	char * ret = (char*)struct_ptr;
	char ** struct_bounds = create_struct_bounds(ret, defs, defs_size);

	
	for(unsigned int i = 1 + amount_of_static_args; i < argc; i++){
		char * tmp_str = NULL;
		uintptr_t tmp_ptr = 0;
		char decoded = 0;
		
		for(unsigned int i1 = 0; i1 < defs_size; i1++){
			if(decoded == 1){
				decoded = 0;
				break;			
			}
			if(str_eq(argv[i], (char*)defs[i1].name)){
				decoded = 1;
				switch(defs[i1].type){
					case INT:
						*((int*)struct_bounds[i1]) = atoi(argv[i + 1]);
						i++;
					break;

					case BOOL:
						*(struct_bounds[i1]) = ((char)1);
					break;

					case STRING:
						tmp_str = (char*)malloc(strlen(argv[i + 1]));
						tmp_str = strcpy(tmp_str, argv[i + 1]);
						tmp_ptr = (uintptr_t)tmp_str;
						for(int i2 = 0; i2 < sizeof(char*); i2++){
							struct_bounds[i1][i2 + 4] = ((char*)&tmp_ptr)[i2];
						}
						i++;
					break;
				}		
			}	
		}
	}


	free(((void*)struct_bounds));

	return ret;
}
#endif
