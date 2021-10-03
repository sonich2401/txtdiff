#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "FileDiff.h"
#include "Settings.h"
#define  PARSE_SWITCH_IMPLEMENT
#include "ParseArgs.h"
#include <escape_codes.h>

struct_def settings_struct_def[6] = {
	{"--stdout", BOOL},
	{"--compare", BOOL},
	{"--verify", BOOL},
	{"--lines", BOOL},
	{"--help", BOOL},
};

void print_help(){
	printf("%s v%s: Build Date = %s\n\n", PROG_NAME, VER, DATE);
	printf("Usage: \n\t%s [path to file 1] [path to file 2] < --args >\n", PROG_NAME);
	printf("\nOptions:\n");
	puts("\t--stdout: Do not use colors");
	puts("\t--compare: Side by side comparison of both files (NOT READY YET)");
	puts("\t--verify: Check if the files differ in anyway but do not do a deep comparison");
	puts("\t--lines: Do not show line numbers");
	puts("\t--help: Show this message");
}

int main(int argc, char* argv[]){
	settings Settings;
	if(argc < 3 ){
		parse_args(argc, argv, &Settings, settings_struct_def, 5, 0); //Check for --help
		print_help();
		if(Settings._help){
			return 0;
		}
		printf("Not Enough Args!\n");
		return -2;
	}
	//Get Window size
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	//Parse args
	parse_args(argc, argv, &Settings, settings_struct_def, 5, 2);

	if(Settings._help){
		print_help();
	}else if(Settings._verify){
		binfile f1 = load_bin(argv[1]);
        binfile f2 = load_bin(argv[2]);

		unsigned long long int f1h = getHash(f1);
		unsigned long long int f2h = getHash(f2);

		printf("\"%s\": %llX\n", argv[1], f1h);
		printf("\"%s\": %llX\n", argv[2], f2h);
		if(f1h == f2h){
			puts("Files are the same");
			return 1;
		}else{
			puts("Files are not the same");
			return -1;
		}
	}else if(Settings._compare){
		//TODO
	}else{
		FileDiff diff(argv[1], argv[2]);
		diff.print(Settings, &w);
	}

	
	return 0;
}
