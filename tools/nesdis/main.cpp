#include "nes/debugger/disassembler.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
int main(int argc,char**argv){
 if(argc!=3||std::string(argv[1])!="raw"){std::cerr<<"Usage: nesdis raw <binary-file>\n";return 2;}
 std::ifstream in(argv[2],std::ios::binary);if(!in){std::cerr<<"Cannot open input\n";return 1;}
 std::vector<nes::u8> bytes((std::istreambuf_iterator<char>(in)),{});
 for(std::size_t i=0;i<bytes.size();){
  std::vector<nes::u8> tail(bytes.begin()+static_cast<std::ptrdiff_t>(i),bytes.end());
  const auto line=nes::disassemble6502(static_cast<nes::u16>(i),tail);
  std::cout<<std::hex<<std::uppercase<<std::setw(4)<<std::setfill('0')<<line.address<<"  ";
  for(auto b:line.bytes)std::cout<<std::setw(2)<<static_cast<unsigned>(b)<<' ';
  std::cout<<" "<<line.text<<'\n';i+=1;
 }
 return 0;
}