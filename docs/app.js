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
  const pong = {x:128,y:120,vx:95,vy:62,p1:120,p2:120,score1:0,score2:0};

  function log(text){ terminal.textContent += `\n> ${text}`; terminal.scrollTop = terminal.scrollHeight; }
  function renderFiles(){
    const tree=$('#file-tree'); tree.innerHTML='';
    Object.keys(files).forEach(name=>{const b=document.createElement('button');b.className='file-item'+(name===activeFile?' active':'');b.textContent=name;b.onclick=()=>{files[activeFile]=editor.value;activeFile=name;editor.value=files[name];$('#editor-tab').textContent=name;renderFiles();};tree.appendChild(b);});
  }
  function save(){ files[activeFile]=editor.value; localStorage.setItem('nex8-studio-project',JSON.stringify(files)); log('project saved to browser localStorage'); status.textContent='SAVED'; }
  function load(){ try{const x=JSON.parse(localStorage.getItem('nex8-studio-project'));if(x&&typeof x==='object')Object.assign(files,x);}catch(e){} editor.value=files[activeFile];renderFiles(); }

  function parseNum(v){return v.trim().startsWith('$')?parseInt(v.trim().slice(1),16):parseInt(v.trim(),10);}
  function assemble(source){
    const lines=source.split(/\r?\n/), labels={}, clean=[];
    lines.forEach((raw,i)=>{let s=raw.replace(/;.*/,'').trim();if(!s)return;if(s.endsWith(':')){labels[s.slice(0,-1).trim()]=clean.length;return;}clean.push({s,line:i+1});});
    const out=[];
    const emit=(...b)=>b.forEach(v=>out.push(v&255));
    clean.forEach(({s,line})=>{
      const p=s.replace(/,/g,' ').split(/\s+/).filter(Boolean), op=p[0].toUpperCase(), arg=p[1];
      const regs={A:0,B:1,C:2,D:3,E:4,F:5,X:6,Y:7};
      if(op==='NOP')emit(0x00); else if(op==='HALT')emit(0x01);
      else if(/^LDI$/.test(op)){if(!(p[1] in regs)||p.length!==3)throw Error(`line ${line}: LDI register, immediate`);emit(0x10+regs[p[1]],parseNum(p[2]));}
      else if(op==='LDA'){let n=parseNum(arg);emit(0x20,n,n>>8);} else if(op==='STA'){let n=parseNum(arg);emit(0x21,n,n>>8);}
      else if(op==='LDB'){let n=parseNum(arg);emit(0x22,n,n>>8);} else if(op==='STB'){let n=parseNum(arg);emit(0x23,n,n>>8);}
      else if(op==='ADD')emit(0x30,parseNum(arg)); else if(op==='SUB')emit(0x31,parseNum(arg)); else if(op==='AND')emit(0x32,parseNum(arg)); else if(op==='OR')emit(0x33,parseNum(arg)); else if(op==='XOR')emit(0x34,parseNum(arg)); else if(op==='CMP')emit(0x35,parseNum(arg)); else if(op==='INC'&&arg==='A')emit(0x38); else if(op==='DEC'&&arg==='A')emit(0x39); else if(op==='INC'&&arg==='X')emit(0x3A); else if(op==='DEC'&&arg==='X')emit(0x3B);
      else if(['JMP','JZ','JNZ','JC','JNC'].includes(op)){const target=labels[arg];if(target===undefined)throw Error(`line ${line}: unknown label ${arg}`);const addr=target;const code={JMP:0x40,JZ:0x41,JNZ:0x42,JC:0x43,JNC:0x44}[op];emit(code,addr,addr>>8);}
      else throw Error(`line ${line}: unsupported opcode ${op}`);
    }); return Uint8Array.from(out);
  }
  function runBuild(){
    files[activeFile]=editor.value; try{rom=assemble(files['main.cell8']||editor.value);status.textContent=`BUILT ${rom.length} BYTES`;log(`assembled ${rom.length} bytes · N8CR PRG ready`);log('browser CELL8 execution profile initialized');updateRegs();}catch(e){status.textContent='BUILD ERROR';log(String(e.message));}
  }
  function updateRegs(){
    const bytes=Array.from(rom.slice(0,8)).map(x=>x.toString(16).padStart(2,'0').toUpperCase()).join(' ');$('#registers').innerHTML=`PC 8000 · SP FFFF<br>A 00 · B 00 · X 00 · Y 00<br>FLAGS Z0 N0 C0 V0<br><br>PRG: ${bytes||'--'}`;
  }
  function draw(){
    ctx.fillStyle='#03070b';ctx.fillRect(0,0,256,240);
    ctx.fillStyle='#0d2538';for(let y=0;y<240;y+=16)ctx.fillRect(0,y,256,1);
    ctx.fillStyle='#55e2ff';ctx.fillRect(8,pong.p1-18,6,36);ctx.fillRect(242,pong.p2-18,6,36);
    ctx.fillRect(pong.x-3,pong.y-3,6,6);ctx.font='16px monospace';ctx.fillText(String(pong.score1),112,24);ctx.fillText(String(pong.score2),140,24);
  }
  function reset(){Object.assign(pong,{x:128,y:120,vx:95,vy:62,p1:120,p2:120,score1:0,score2:0});draw();}
  function frame(t){const dt=Math.min(.033,(t-last)/1000||0);last=t;if(running&&!paused){if(keys['ArrowUp']||keys.w)pong.p1-=150*dt;if(keys['ArrowDown']||keys.s)pong.p1+=150*dt;pong.p2+=(pong.y-pong.p2)*Math.min(1,dt*4);pong.p1=Math.max(20,Math.min(220,pong.p1));pong.p2=Math.max(20,Math.min(220,pong.p2));pong.x+=pong.vx*dt;pong.y+=pong.vy*dt;if(pong.y<4||pong.y>236)pong.vy*=-1;if(pong.x<18&&Math.abs(pong.y-pong.p1)<24){pong.vx=Math.abs(pong.vx)*1.03;pong.x=18}if(pong.x>238&&Math.abs(pong.y-pong.p2)<24){pong.vx=-Math.abs(pong.vx)*1.03;pong.x=238}if(pong.x<0){pong.score2++;pong.x=128;pong.y=120;pong.vx=95}if(pong.x>256){pong.score1++;pong.x=128;pong.y=120;pong.vx=-95}draw();}raf=requestAnimationFrame(frame);}
  const keys={}; window.addEventListener('keydown',e=>{keys[e.key]=true;keys[e.key.toLowerCase()]=true;if(['ArrowUp','ArrowDown',' '].includes(e.key))e.preventDefault();});window.addEventListener('keyup',e=>{delete keys[e.key];delete keys[e.key.toLowerCase()];});
  $('#game-start').onclick=()=>{running=true;paused=false;$('#console-status').textContent='RUNNING · NEX8 PONG CARTRIDGE · PLAYER 1 W/S · PLAYER 2 AI';cancelAnimationFrame(raf);last=performance.now();raf=requestAnimationFrame(frame);};
  document.querySelectorAll('.launch-game').forEach(b=>b.onclick=()=>{$('#console').scrollIntoView({behavior:'smooth'});setTimeout(()=>$('#game-start').click(),250);});
  $('#game-reset').onclick=()=>{reset();running=false;$('#console-status').textContent='RESET · CARTRIDGE SLOT 1';};
  $('#game-pause').onclick=()=>{paused=!paused;$('#console-status').textContent=paused?'PAUSED · NEX8 CPU HALTED':'RUNNING · NEX8 PONG';};
  $('#game-fullscreen').onclick=()=>canvas.requestFullscreen?.();
  $('#assemble').onclick=runBuild;$('#save-project').onclick=save;
  $('#new-project').onclick=()=>{if(confirm('Reset browser project?')){Object.keys(files).forEach(k=>delete files[k]);Object.assign(files,{'main.cell8':defaultSource,'game.logic':'// game logic\n','README.n8':'# NEX8 PROJECT\n'});activeFile='main.cell8';load();log('new project created');}};
  $('#download-rom').onclick=()=>{runBuild();if(!rom.length)return;const blob=new Blob([rom],{type:'application/octet-stream'});const a=document.createElement('a');a.href=URL.createObjectURL(blob);a.download='nex8-program.bin';a.click();URL.revokeObjectURL(a.href);log('exported nex8-program.bin');};
  document.querySelectorAll('.open-ide').forEach(b=>b.onclick=()=>$('#ide').scrollIntoView({behavior:'smooth'}));
  const links=document.querySelectorAll('nav a[href^="#"]');links.forEach(link=>link.addEventListener('click',()=>{links.forEach(x=>x.removeAttribute('aria-current'));link.setAttribute('aria-current','page');}));
  editor.addEventListener('input',()=>{files[activeFile]=editor.value;status.textContent='EDITING';});
  load();reset();runBuild();
})();
