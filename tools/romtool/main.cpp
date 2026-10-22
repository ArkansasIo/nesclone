#include "nes/cartridge/ines.hpp"
#include <iostream>
#include <string>
int main(int argc,char**argv){
 if(argc!=3||std::string(argv[1])!="info"){
  std::cerr<<"Usage: nesrom info <file.nes>\nOnly the info command is implemented.\n";return 2;
 }
 const auto r=nes::loadInes(argv[2]);if(!r){std::cerr<<"Invalid ROM: "<<r.error<<'\n';return 1;}
 const auto&i=r.image;
 std::cout<<"Format: "<<(i.nes2?"NES 2.0":"iNES 1.0")<<"\nMapper: "<<i.mapperId<<':'<<static_cast<unsigned>(i.submapper)<<"\nPRG: "<<i.prgRom.size()<<" bytes\nCHR-ROM: "<<i.chrRom.size()<<" bytes\nCHR-RAM: "<<i.chrRam.size()<<" bytes\nTrainer: "<<(i.trainerPresent?"yes":"no")<<"\nBattery: "<<(i.battery?"yes":"no")<<"\nMirroring: "<<(i.fourScreen?"four-screen":(i.verticalMirroring?"vertical":"horizontal"))<<'\n';return 0;
}