#include "nes/cartridge/ines.hpp"
#include <iostream>
#include <string>
int main(int argc,char**argv){
 if(argc!=3||std::string(argv[1])!="validate"){std::cerr<<"Usage: inesutil validate <file.nes>\n";return 2;}
 auto r=nes::loadInes(argv[2]);if(!r){std::cerr<<"FAIL: "<<r.error<<'\n';return 1;}
 std::cout<<"PASS: header and declared payload loaded\nMapper "<<r.image.mapperId<<", PRG "<<r.image.prgRom.size()<<" bytes, CHR "<<r.image.chrRom.size()<<" bytes\n";
 return 0;
}