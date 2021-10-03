#pragma once
#include <vector>
#include <string>
#include <os.h>
#include <typedefs.h>
#include <escape_codes.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "Settings.h"

enum FILE_BLOCK_TYPE{
    SAME,
    SUBTRACT,
    ADD
};


unsigned long long int getHash(binfile& f){
	unsigned long long int fh = 0;

	for(unsigned long long int i = 0; i < f.size; i++){
		srand48(f.dat[i]);
		fh = ((fh >> 1) & rand()) + f.dat[i] - f.size + fh;
	}
	return fh;
}

class FileBlock{
    public:
        FileBlock(){
            f1 = 0;
            f2 = 0;
            type = 0;
            fail = false;
        }
    public:
        std::vector<std::string> lines;



        uint64 f1;
        uint64 f2;

        struct{
            uint8 type : 2;
            bool fail : 1;
        };

        const void print(void){
            const char* types[3] = {
                "Normal",
                "Sub",
                "Add"
            };
            #ifdef DEBUG
                printf("BLOCK: F1 = %i, F2 = %i, T = %s\n", f1, f2, types[type]);
                for(uint64 i = 0; i < lines.size(); i++){
                    printf("\t%i: \"%s\"\n", i, lines[i].c_str());
                }
            #endif
        }
};

class FileDiff{
    public:
        FileDiff(std::string fp1, std::string fp2){
            //Load Documents
            std::vector<std::string> f1_lines = load_text(fp1);
            std::vector<std::string> f2_lines = load_text(fp2);

            //Used for printing. Gets the max size of the two files
            max_doc_size = f1_lines.size();
            if(f2_lines.size() > max_doc_size){
                max_doc_size = f2_lines.size();
            }

            uint64 f1 = 0;
            uint64 f2 = 0;

            while(f1 < f1_lines.size() && f2 < f2_lines.size()){
                FileBlock tmp = get_next_block(f1_lines, f2_lines, f1, f2);
                if(!tmp.fail && tmp.lines.size() != 0){
                    this->blocks.push_back(tmp);
                    
                    f1 = tmp.f1;
                    f2 = tmp.f2;
                }

                tmp = get_removal(f1_lines, f2_lines, f1, f2); //Re-purpose 'tmp'
                FileBlock tmp1 = get_added(f1_lines, f2_lines, f1, f2);

                if(!tmp.fail && !tmp1.fail){ //Tests if it found the same block
                    if(tmp.lines.size() == tmp1.lines.size())
                        if(tmp.f1 == tmp1.f1){
                            continue;
                        }
                }

                if(!test_block(tmp, f1, f2))
                    if(!test_block(tmp1, f2, f1)){
                        //Line has been edited
                        tmp = FileBlock();
                        tmp.f1 = f1;
                        tmp.f2 = f2;
                        tmp.type = SUBTRACT;
                        tmp.lines.push_back(f1_lines[f1]);
                        blocks.push_back(tmp);
                        
                        tmp = FileBlock();
                        tmp.f1 = f1 + 1;
                        tmp.f2 = f2 + 1;
                        tmp.type = ADD;
                        tmp.lines.push_back(f2_lines[f2]);
                        blocks.push_back(tmp);
                        
                        f1++;
                        f2++;
                    }
            }
            
        }
    private:
        std::vector<FileBlock> blocks;
        uint64 max_doc_size;

    private:
        FileBlock get_next_block(std::vector<std::string>& f1l, std::vector<std::string>& f2l, uint64 f1, uint64 f2){
            FileBlock ret;
            ret.f1 = f1;
            ret.f2 = f2;

            while(ret.f1 < f1l.size() && ret.f2 < f2l.size()){
                if(f1l[ret.f1] != f2l[ret.f2]){
                    return ret;
                }
                
                ret.lines.push_back(f1l[ret.f1]);
                ret.f1++;
                ret.f2++;
            }
            if(ret.lines.size() != 0){
                return ret;
            }

            ret.fail = true;
            return ret;
        }

        FileBlock get_removal(std::vector<std::string>& f1l, std::vector<std::string>& f2l, uint64 f1, const uint64 f2, int8 type = SUBTRACT){
            FileBlock ret;
            ret.type = type;
            ret.f1 = f1;
            ret.f2 = f2;

            for(uint64 i = f1; i < f1l.size(); i++){
                if(f1l[i] == f2l[f2])
                    return ret;
                ret.f1 = i + 1;
                ret.lines.push_back(f1l[i]);
            }
            ret.fail = true;
            ret.lines.clear();
            return ret;
        }

        FileBlock get_added(std::vector<std::string>& f1l, std::vector<std::string>& f2l, uint64 f1, uint64 f2){
            //Switch variable arrangement to effectively use the transitive property of addition
            //Basicly, if you switch the files around, it does the same thing as get_removal()
            //it sees what parts of the this file are not present in the other file
            //Instead of using this to check if a part in f1 is missing in f2, 
            //we check if f2 is missing a part that f1 doesn't have
            //This block from f2 that is "missing" from f1 is actually a new part rather than a part that
            //was deleted
            return get_removal(f2l, f1l, f2, f1, ADD); 
        }

        bool test_block(FileBlock& block, uint64& _f1, uint64& _f2){
            if(!block.fail &&  block.lines.size() != 0){
                _f1 = block.f1;
                _f2 = block.f2;
                blocks.push_back(block);
                return true;
            }
            return false;
        }

    public:
        FileBlock& operator [](uint64 index){
            if(index >= this->blocks.size()){
                perror("FileDiff::operator[uint64 index]: Index OOB! Index was clamped\n");
                index = this->blocks.size() - 1;
            }
            return this->blocks[index];
        }

        uint64 size(){
            return this->blocks.size();
        }

        void print(settings& Settings, struct winsize * window){
            char* colors[3] = {
                T_RESET,
                T_RED,
                T_GREEN
            };
            char start[3] = {
                ' ',
                '-',
                '+'
            };

            if(Settings._stdout){
                colors[0] = "\0";
                colors[1] = "\0";
                colors[2] = "\0";
            }



            uint8 space_count = std::to_string(max_doc_size).size();
            uint64 line_num = 1;
            for(auto& block : blocks){
                printf(colors[block.type]);
                for(std::string& str : block.lines){
                    long int chars_printed = 0;
                    if(!Settings._line_num){
                        if(block.type != SUBTRACT){
                            printf("%li", line_num++);
                            chars_printed += std::to_string(line_num).size();
                        }else{
                            for(uint8 i = 0; i < std::to_string(line_num).size(); i++){
                                printf(" ");
                                chars_printed++;
                            }
                        }
                        for(uint8 i = 0; i < space_count - std::to_string(line_num).size(); i++){
                            printf(" ");
                            chars_printed++;
                        }
                        printf(": ");
                        chars_printed += 2;
                    }
                    printf("%c%s\n", start[block.type], str.c_str());
                    chars_printed += str.size();
                }
            }
            
        }
};



/*
void print(settings& Settings){
            const char* colors[3] = {
                T_RESET,
                T_RED,
                T_GREEN
            };
            const char start[3] = {
                ' ',
                '-',
                '+'
            };

            uint8 space_count = std::to_string(max_doc_size).size();
            uint64 line_num = 1;
            for(auto& block : blocks){
                for(std::string& str : block.lines){
                    if(block.type != SUBTRACT){
                        colstr(printf("%li", line_num++), colors[block.type]);
                    }else{
                        for(uint8 i = 0; i < std::to_string(line_num).size(); i++){
                            printf(" ");
                        }
                    }
                    for(uint8 i = 0; i < space_count - std::to_string(line_num).size(); i++)
                        printf(" ");
                    colstr(printf(": %c%s\n", start[block.type], str.c_str()), colors[block.type]);
                }
            }
            
        }
*/