#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
int main(int argc,char**argv){
 if(argc!=3||std::string(argv[1])!="info"){std::cerr<<"Usage: namtool info <file.nam>\n";return 2;}
 std::ifstream in(argv[2],std::ios::binary);if(!in){std::cerr<<"Cannot open nametable file\n";return 1;}
 const auto size=std::filesystem::file_size(argv[2]);
 std::cout<<"Bytes: "<<size<<"\n";
 if(size==1024)std::cout<<"Size matches one 1 KiB NES nametable (960 tile bytes + 64 attribute bytes).\n";
 else std::cout<<"Expected a single nametable is 1024 bytes; this file may be a different container.\n";
 return 0;
}