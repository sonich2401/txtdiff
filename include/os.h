#pragma once
#include <vector>
#include <string>
#include <fstream>

static std::vector<std::string> load_text(std::string fp){
    std::vector<std::string> ret;
    ret.reserve(60);
    std::string tmp;

    std::fstream file;
    file.open(fp, std::ios::in);
    while(!file.eof()){
        std::getline(file, tmp);
        ret.push_back(tmp);
    }

    return ret;
}

struct binfile{
    unsigned char* dat;
    unsigned long int size;

    ~binfile(){
        free(dat);
    }
};

//https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array
binfile load_bin(char* file_path){
    binfile ret;
    FILE *fileptr;
    char *buffer;
    size_t filelen;

    fileptr = fopen(file_path, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (char *)malloc(filelen * sizeof(char)); // Enough memory for the file
    ret.size = (unsigned long long int )filelen;
    //Reuse filelen to make compiler happy
    filelen = fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file

    ret.dat = (unsigned char*)buffer;
    return ret;
}
//