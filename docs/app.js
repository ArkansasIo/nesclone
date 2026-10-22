(() => {
  const $ = s => document.querySelector(s);
  const editor = $('#code-editor');
  const terminal = $('#terminal');
  const status = $('#build-status');
  const canvas = $('#game-canvas');
  const ctx = canvas.getContext('2d');
  const defaultSource = `; NEX8 Pong cartridge demo\n; CELL8 Rev-A browser assembler profile\nSTART:\n  LDI A, $01\n  LDI B, $20\n  LDI X, $80\n  LDI Y, $40\n  JMP START\n`;
  const files = {
    'main.cell8': defaultSource,
    'game.logic': '// Lua-like high-level game logic\nplayer.hp = 100\nplayer.score = 0\nif input.fire then player.score = player.score + 1 end\n',
    'README.n8': '# NEX8 PROJECT\n# CELL8 cartridge / browser Studio\n'
  };
  let activeFile = 'main.cell8';
  let rom = new Uint8Array(0);
  let running = false, paused = false, raf = 0, last = 0;
  const keys = {};
  const pong = {x:128,y:120,vx:95,vy:62,p1:120,p2:120,score1:0,score2:0};
  const cpu = {a:0,b:0,c:0,d:0,e:0,f:0,x:0,y:0,pc:0x8000,sp:0xffff,z:0,n:0,c:0,v:0,halted:false,steps:0};
  const memory = new Uint8Array(65536);

  function log(text){ terminal.textContent += `\n> ${text}`; terminal.scrollTop = terminal.scrollHeight; }
  function renderFiles(){
    const tree=$('#file-tree'); if(!tree)return; tree.innerHTML='';
    Object.keys(files).forEach(name=>{const b=document.createElement('button');b.className='file-item'+(name===activeFile?' active':'');b.textContent=name;b.onclick=()=>{files[activeFile]=editor.value;activeFile=name;editor.value=files[name];$('#editor-tab').textContent=name;renderFiles();};tree.appendChild(b);});
  }
  function save(){ files[activeFile]=editor.value; localStorage.setItem('nex8-studio-project',JSON.stringify(files)); log('project saved to browser localStorage'); status.textContent='SAVED'; }
  function load(){ try{const x=JSON.parse(localStorage.getItem('nex8-studio-project'));if(x&&typeof x==='object')Object.assign(files,x);}catch(e){} editor.value=files[activeFile]; const tab=$('#editor-tab');if(tab)tab.textContent=activeFile;renderFiles(); }
  function parseNum(v, labels={}){ const t=String(v||'').trim(); if(Object.prototype.hasOwnProperty.call(labels,t))return labels[t]; if(t.startsWith('$'))return parseInt(t.slice(1),16); if(/^%[01]+$/.test(t))return parseInt(t.slice(1),2); const n=Number(t); if(Number.isNaN(n))throw Error(`invalid operand ${t}`); return n; }
  const regs={A:0,B:1,C:2,D:3,E:4,F:5,X:6,Y:7};
  const sizes={NOP:1,HALT:1,LDI:2,LDA:3,STA:3,LDB:3,STB:3,ADD:2,SUB:2,AND:2,OR:2,XOR:2,CMP:2,INC:1,DEC:1,JMP:3,JZ:3,JNZ:3,JC:3,JNC:3};
  function tokenize(source){const lines=source.split(/\r?\n/), labels={}, clean=[]; let pc=0; lines.forEach((raw,i)=>{let s=raw.replace(/;.*/,'').trim();if(!s)return;while(s.includes(':')){const m=s.match(/^([^\s:]+):\s*(.*)$/);if(!m)break;labels[m[1]]=pc;s=m[2].trim();if(!s)break;}if(!s)return;const op=s.replace(/,/g,' ').split(/\s+/)[0].toUpperCase();if(!sizes[op])throw Error(`line ${i+1}: unsupported opcode ${op}`);clean.push({s,line:i+1});pc+=sizes[op];});return {clean,labels};}
  function assemble(source){
    const {clean,labels}=tokenize(source), out=[];
    const emit=(...b)=>b.forEach(v=>out.push(v&255));
    clean.forEach(({s,line})=>{
      const p=s.replace(/,/g,' ').split(/\s+/).filter(Boolean), op=p[0].toUpperCase(), arg=p[1];
      if(op==='NOP')emit(0x00); else if(op==='HALT')emit(0x01);
      else if(op==='LDI'){if(!(p[1] in regs)||p.length!==3)throw Error(`line ${line}: LDI register, immediate`);emit(0x10+regs[p[1]],parseNum(p[2],labels));}
      else if(op==='LDA'){const n=parseNum(arg,labels);emit(0x20,n,n>>8);} else if(op==='STA'){const n=parseNum(arg,labels);emit(0x21,n,n>>8);}
      else if(op==='LDB'){const n=parseNum(arg,labels);emit(0x22,n,n>>8);} else if(op==='STB'){const n=parseNum(arg,labels);emit(0x23,n,n>>8);}
      else if(op==='ADD')emit(0x30,parseNum(arg,labels)); else if(op==='SUB')emit(0x31,parseNum(arg,labels)); else if(op==='AND')emit(0x32,parseNum(arg,labels)); else if(op==='OR')emit(0x33,parseNum(arg,labels)); else if(op==='XOR')emit(0x34,parseNum(arg,labels)); else if(op==='CMP')emit(0x35,parseNum(arg,labels));
      else if(op==='INC'&&arg==='A')emit(0x38); else if(op==='DEC'&&arg==='A')emit(0x39); else if(op==='INC'&&arg==='X')emit(0x3A); else if(op==='DEC'&&arg==='X')emit(0x3B);
      else if(['JMP','JZ','JNZ','JC','JNC'].includes(op)){const addr=parseNum(arg,labels);const code={JMP:0x40,JZ:0x41,JNZ:0x42,JC:0x43,JNC:0x44}[op];emit(code,addr,addr>>8);}
      else throw Error(`line ${line}: unsupported opcode ${op}`);
    }); return Uint8Array.from(out);
  }
  function resetCpu(){cpu.a=cpu.b=cpu.c=cpu.d=cpu.e=cpu.f=cpu.x=cpu.y=0;cpu.pc=0x8000;cpu.sp=0xffff;cpu.z=cpu.n=cpu.c=cpu.v=0;cpu.halted=false;cpu.steps=0;memory.fill(0);rom.forEach((v,i)=>{if(0x8000+i<65536)memory[0x8000+i]=v;});updateRegs();}
  function setZN(v){v&=255;cpu.z=v===0?1:0;cpu.n=(v&0x80)?1:0;}
  function fetch(){return memory[cpu.pc++];}
  function addr16(){const lo=fetch(),hi=fetch();return lo|(hi<<8);}
  function stepCpu(){if(cpu.halted||!rom.length)return false;const op=fetch();let v,a;switch(op){
    case 0x00:break; case 0x01:cpu.halted=true;break;
    case 0x10:cpu.a=fetch();setZN(cpu.a);break; case 0x11:cpu.b=fetch();setZN(cpu.b);break; case 0x12:cpu.c=fetch();setZN(cpu.c);break; case 0x13:cpu.d=fetch();setZN(cpu.d);break; case 0x14:cpu.e=fetch();setZN(cpu.e);break; case 0x15:cpu.f=fetch();setZN(cpu.f);break; case 0x16:cpu.x=fetch();setZN(cpu.x);break; case 0x17:cpu.y=fetch();setZN(cpu.y);break;
    case 0x20:a=addr16();cpu.a=memory[a];setZN(cpu.a);break; case 0x21:a=addr16();memory[a]=cpu.a;setZN(cpu.a);break; case 0x22:a=addr16();cpu.b=memory[a];setZN(cpu.b);break; case 0x23:a=addr16();memory[a]=cpu.b;setZN(cpu.b);break;
    case 0x30:v=fetch();a=cpu.a+v;cpu.c=a>255?1:0;cpu.a=a&255;setZN(cpu.a);break; case 0x31:v=fetch();a=cpu.a-v;cpu.c=a<0?1:0;cpu.a=a&255;setZN(cpu.a);break;
    case 0x32:cpu.a&=fetch();setZN(cpu.a);break; case 0x33:cpu.a|=fetch();setZN(cpu.a);break; case 0x34:cpu.a^=fetch();setZN(cpu.a);break; case 0x35:v=fetch();setZN((cpu.a-v)&255);cpu.c=cpu.a>=v?1:0;break;
    case 0x38:cpu.a=(cpu.a+1)&255;setZN(cpu.a);break; case 0x39:cpu.a=(cpu.a-1)&255;setZN(cpu.a);break; case 0x3A:cpu.x=(cpu.x+1)&255;setZN(cpu.x);break; case 0x3B:cpu.x=(cpu.x-1)&255;setZN(cpu.x);break;
    case 0x40:cpu.pc=addr16();break; case 0x41:a=addr16();if(cpu.z)cpu.pc=a;break; case 0x42:a=addr16();if(!cpu.z)cpu.pc=a;break; case 0x43:a=addr16();if(cpu.c)cpu.pc=a;break; case 0x44:a=addr16();if(!cpu.c)cpu.pc=a;break;
    default:cpu.halted=true;log(`CPU trap: unsupported opcode ${op.toString(16).padStart(2,'0')}`);break;
  } cpu.steps++;return true;}
  function updateRegs(){const hex=n=>(n>>>0).toString(16).padStart(2,'0').toUpperCase();const r=$('#registers');if(!r)return;r.innerHTML=`PC ${cpu.pc.toString(16).padStart(4,'0').toUpperCase()} · SP ${cpu.sp.toString(16).padStart(4,'0').toUpperCase()}<br>A ${hex(cpu.a)} · B ${hex(cpu.b)} · X ${hex(cpu.x)} · Y ${hex(cpu.y)}<br>FLAGS Z${cpu.z} N${cpu.n} C${cpu.c} V${cpu.v}<br>CPU ${cpu.halted?'HALTED':'READY'} · STEPS ${cpu.steps}<br><br>PRG: ${Array.from(rom.slice(0,12)).map(hex).join(' ')||'--'}`;}
  function runBuild(){files[activeFile]=editor.value;try{rom=assemble(files['main.cell8']||editor.value);resetCpu();status.textContent=`BUILT ${rom.length} BYTES`;log(`assembled ${rom.length} bytes · labels resolved to 16-bit addresses`);log('browser CELL8 execution profile initialized');}catch(e){status.textContent='BUILD ERROR';log(String(e.message));}}
  function exportN8C(){if(!rom.length)runBuild();if(!rom.length)return;const enc=new TextEncoder();const header=new Uint8Array(32);header.set(enc.encode('N8CR'),0);header[4]=1;header[5]=0;new DataView(header.buffer).setUint32(8,rom.length,true);const image=new Uint8Array(32+rom.length);image.set(header);image.set(rom,32);const blob=new Blob([image],{type:'application/octet-stream'});const a=document.createElement('a');a.href=URL.createObjectURL(blob);a.download='nex8-program.n8c';a.click();setTimeout(()=>URL.revokeObjectURL(a.href),500);log('exported N8CR v1 cartridge image: nex8-program.n8c');}
  function draw(){ctx.fillStyle='#03070b';ctx.fillRect(0,0,256,240);ctx.fillStyle='#0d2538';for(let y=0;y<240;y+=16)ctx.fillRect(0,y,256,1);ctx.fillStyle='#55e2ff';ctx.fillRect(8,pong.p1-18,6,36);ctx.fillRect(242,pong.p2-18,6,36);ctx.fillRect(pong.x-3,pong.y-3,6,6);ctx.font='16px monospace';ctx.fillText(String(pong.score1),112,24);ctx.fillText(String(pong.score2),140,24);}
  function reset(){Object.assign(pong,{x:128,y:120,vx:95,vy:62,p1:120,p2:120,score1:0,score2:0});draw();}
  function frame(t){const dt=Math.min(.033,(t-last)/1000||0);last=t;if(running&&!paused){if(keys.ArrowUp||keys.w)pong.p1-=150*dt;if(keys.ArrowDown||keys.s)pong.p1+=150*dt;pong.p2+=(pong.y-pong.p2)*Math.min(1,dt*4);pong.p1=Math.max(20,Math.min(220,pong.p1));pong.p2=Math.max(20,Math.min(220,pong.p2));pong.x+=pong.vx*dt;pong.y+=pong.vy*dt;if(pong.y<4||pong.y>236)pong.vy*=-1;if(pong.x<18&&Math.abs(pong.y-pong.p1)<24){pong.vx=Math.abs(pong.vx)*1.03;pong.x=18}if(pong.x>238&&Math.abs(pong.y-pong.p2)<24){pong.vx=-Math.abs(pong.vx)*1.03;pong.x=238}if(pong.x<0){pong.score2++;pong.x=128;pong.y=120;pong.vx=95}if(pong.x>256){pong.score1++;pong.x=128;pong.y=120;pong.vx=-95}draw();updateRegs();}raf=requestAnimationFrame(frame);}
  window.addEventListener('keydown',e=>{keys[e.key]=true;keys[e.key.toLowerCase()]=true;if(['ArrowUp','ArrowDown',' '].includes(e.key))e.preventDefault();});window.addEventListener('keyup',e=>{delete keys[e.key];delete keys[e.key.toLowerCase()];});
  $('#game-start').onclick=()=>{running=true;paused=false;$('#console-status').textContent='RUNNING · NEX8 PONG CARTRIDGE · PLAYER 1 W/S · PLAYER 2 AI';cancelAnimationFrame(raf);last=performance.now();raf=requestAnimationFrame(frame);};
  document.querySelectorAll('.launch-game').forEach(b=>b.onclick=()=>{$('#console').scrollIntoView({behavior:'smooth'});setTimeout(()=>$('#game-start').click(),250);});
  $('#game-reset').onclick=()=>{reset();running=false;resetCpu();$('#console-status').textContent='RESET · CARTRIDGE SLOT 1';};
  $('#game-pause').onclick=()=>{paused=!paused;$('#console-status').textContent=paused?'PAUSED · NEX8 CPU HALTED':'RUNNING · NEX8 PONG';};
  $('#game-fullscreen').onclick=()=>canvas.requestFullscreen?.();
  $('#assemble').onclick=runBuild;$('#save-project').onclick=save;
  $('#new-project').onclick=()=>{if(confirm('Reset browser project?')){Object.keys(files).forEach(k=>delete files[k]);Object.assign(files,{'main.cell8':defaultSource,'game.logic':'// game logic\n','README.n8':'# NEX8 PROJECT\n'});activeFile='main.cell8';load();log('new project created');}};
  const exportButton=$('#download-rom');if(exportButton){exportButton.onclick=()=>{runBuild();if(!rom.length)return;const blob=new Blob([rom],{type:'application/octet-stream'});const a=document.createElement('a');a.href=URL.createObjectURL(blob);a.download='nex8-program.bin';a.click();setTimeout(()=>URL.revokeObjectURL(a.href),500);log('exported raw CELL8 PRG: nex8-program.bin');};}
  const n8cButton=$('#download-n8c');if(n8cButton)n8cButton.onclick=exportN8C;
  const stepButton=$('#cpu-step');if(stepButton)stepButton.onclick=()=>{if(!rom.length)runBuild();stepCpu();updateRegs();};
  const runCpuButton=$('#cpu-run');if(runCpuButton)runCpuButton.onclick=()=>{if(!rom.length)runBuild();for(let i=0;i<1000&&!cpu.halted;i++)stepCpu();updateRegs();log(`executed ${cpu.steps} CELL8 instructions`);};
  document.querySelectorAll('.open-ide').forEach(b=>b.onclick=()=>$('#ide').scrollIntoView({behavior:'smooth'}));
  editor.addEventListener('input',()=>{files[activeFile]=editor.value;status.textContent='EDITING';});
  load();reset();runBuild();
})();
