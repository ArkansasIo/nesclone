# Development status

| Area | State |
|---|---|
| C++23 build | scaffolded |
| iNES/NES 2.0 parser | initial implementation; more format tests needed |
| Mapper 0 NROM | basic PRG/CHR mapping |
| 6502 CPU | starter subset only |
| PPU | register/latch prototype; no completed renderer |
| APU | register prototype; no audio synthesis |
| Controller | basic serial shift input |
| XMB model | category/menu navigation model; no GUI renderer |
| ROM library scanner | local metadata scanner source |
| Settings store | local key/value persistence source |
| NESLogic | lexer/runtime data model and parser stub; language execution not implemented |
| SDK | ca65/ld65 examples and hardware constants |
| ROM tool | metadata info command only |
| Other mappers | documented roadmap |
| Save states and self-update | planned |

Do not market this as a fully playable or accuracy-complete NES emulator until CPU/PPU/APU, timing, cartridge and frontend acceptance tests pass.