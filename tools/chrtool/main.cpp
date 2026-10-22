#include "nes/types.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
int main(int argc,char**argv){
 if(argc!=3||std::string(argv[1])!="info"){std::cerr<<"Usage: chrtool info <file.chr>\n";return 2;}
 std::ifstream in(argv[2],std::ios::binary);if(!in){std::cerr<<"Cannot open CHR file\n";return 1;}
 std::vector<nes::u8> bytes((std::istreambuf_iterator<char>(in)),{});
 if(bytes.size()%16){std::cerr<<"CHR size is not a multiple of 16 bytes per 2bpp tile\n";return 1;}
 std::cout<<"CHR bytes: "<<bytes.size()<<"\nTiles: "<<bytes.size()/16<<"\n8 KiB banks: "<<(bytes.size()+8191)/8192<<'\n';
 return 0;
}