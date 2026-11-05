/* ASHEN HOLLOW - Game Boy Color port (GBDK-2020, MBC1+RAM+BATTERY)
   Created and developed by ash&gold */
#include <gb/gb.h>
#include <gb/cgb.h>
#include <rand.h>
#include <string.h>
#include <stdint.h>

#ifndef RGB8
#define RGB8(r,g,b) RGB((r)>>3,(g)>>3,(b)>>3)
#endif
#ifndef VBK_TILES
#define VBK_TILES 0
#define VBK_ATTRIBUTES 1
#endif

#define BGT 128u            /* BG tile indices live in 128..255 (same address in both LCDC modes) */
#define END 0xFF
#define NMON 6
#define ALLCARDS 0x3F
#define CAPCOST 100

/* ---------- tile numbers (add BGT) ---------- */
#define T_FONT 1   /* 1..47 */
#define T_GRASS 48
#define T_BAR 54   /* 54..62 */
#define T_MON 64   /* 64..79 */

/* ---------- 5x5 font: A-Z 0-9 / : ! . ? + - > % & < ---------- */
static const uint8_t FONT[47][5]={
{0x0E,0x11,0x1F,0x11,0x11},{0x1E,0x11,0x1E,0x11,0x1E},{0x0F,0x10,0x10,0x10,0x0F},{0x1E,0x11,0x11,0x11,0x1E},
{0x1F,0x10,0x1E,0x10,0x1F},{0x1F,0x10,0x1E,0x10,0x10},{0x0F,0x10,0x13,0x11,0x0F},{0x11,0x11,0x1F,0x11,0x11},
{0x1F,0x04,0x04,0x04,0x1F},{0x01,0x01,0x01,0x11,0x0E},{0x11,0x12,0x1C,0x12,0x11},{0x10,0x10,0x10,0x10,0x1F},
{0x11,0x1B,0x15,0x11,0x11},{0x11,0x19,0x15,0x13,0x11},{0x0E,0x11,0x11,0x11,0x0E},{0x1E,0x11,0x1E,0x10,0x10},
{0x0E,0x11,0x15,0x12,0x0D},{0x1E,0x11,0x1E,0x12,0x11},{0x0F,0x10,0x0E,0x01,0x1E},{0x1F,0x04,0x04,0x04,0x04},
{0x11,0x11,0x11,0x11,0x0E},{0x11,0x11,0x11,0x0A,0x04},{0x11,0x11,0x15,0x1B,0x11},{0x11,0x0A,0x04,0x0A,0x11},
{0x11,0x0A,0x04,0x04,0x04},{0x1F,0x02,0x04,0x08,0x1F},
{0x0E,0x13,0x15,0x19,0x0E},{0x04,0x0C,0x04,0x04,0x0E},{0x0E,0x01,0x0E,0x10,0x1F},{0x1E,0x01,0x0E,0x01,0x1E},
{0x12,0x12,0x1F,0x02,0x02},{0x1F,0x10,0x1E,0x01,0x1E},{0x0E,0x10,0x1E,0x11,0x0E},{0x1F,0x01,0x02,0x04,0x04},
{0x0E,0x11,0x0E,0x11,0x0E},{0x0E,0x11,0x0F,0x01,0x0E},
{0x01,0x02,0x04,0x08,0x10},{0x00,0x04,0x00,0x04,0x00},{0x04,0x04,0x04,0x00,0x04},{0x00,0x00,0x00,0x00,0x04},
{0x0E,0x01,0x06,0x00,0x04},{0x00,0x04,0x0E,0x04,0x00},{0x00,0x00,0x0E,0x00,0x00},{0x08,0x04,0x02,0x04,0x08},
{0x19,0x02,0x04,0x08,0x13},{0x0C,0x12,0x0C,0x13,0x0D},{0x02,0x04,0x08,0x04,0x02}};

/* ---------- rectangle art: x,y,w,h,colour ---------- */
static const uint8_t A_GRASS[]={1,2,1,3,1, 5,4,1,3,1, 3,6,2,1,2, 6,1,1,1,2, END};
static const uint8_t A_TREE[] ={1,0,6,6,1, 2,1,3,2,2, 3,6,2,2,3, END};
static const uint8_t A_PATH[] ={1,2,1,1,1, 5,5,1,1,1, 3,6,2,1,2, 6,1,1,1,2, END};
static const uint8_t A_WATER[]={1,2,3,1,1, 4,5,3,1,1, 0,7,2,1,2, END};
static const uint8_t A_DUNG[] ={0,3,8,1,1, 3,0,1,8,1, 5,5,1,1,2, 1,1,1,1,2, END};
static const uint8_t A_INN[]  ={0,1,8,3,1, 1,4,6,4,2, 3,5,2,3,3, END};
static const uint8_t *const A_TERRAIN[6]={A_GRASS,A_TREE,A_PATH,A_WATER,A_DUNG,A_INN};

static const uint8_t M_SLIME[]={10,12,12,2,2, 7,14,18,2,2, 5,16,22,4,2, 4,20,24,6,2, 5,26,22,3,1, 6,15,5,3,3, 11,20,3,4,0, 19,20,3,4,0, 14,26,4,1,0, END};
static const uint8_t M_BAT[]  ={2,10,10,3,1, 4,13,10,3,1, 8,16,6,3,1, 20,10,10,3,1, 18,13,10,3,1, 18,16,6,3,1, 12,10,8,12,2, 12,7,2,3,2, 18,7,2,3,2, 13,14,2,2,3, 17,14,2,2,3, 13,22,2,3,2, 17,22,2,3,2, END};
static const uint8_t M_WOLF[] ={4,14,18,8,1, 5,22,3,6,1, 17,22,3,6,1, 18,8,10,8,1, 19,5,3,4,1, 25,5,3,4,1, 26,12,5,4,2, 23,10,2,2,3, 0,10,5,3,2, 6,20,14,2,2, END};
static const uint8_t M_GOB[]  ={10,25,4,5,2, 16,25,4,5,2, 8,15,14,10,2, 9,5,12,10,1, 5,8,4,3,1, 21,8,4,3,1, 12,9,2,2,3, 17,9,2,2,3, 25,6,3,18,2, 23,2,7,6,2, END};
static const uint8_t M_SKEL[] ={10,3,12,10,1, 12,7,3,3,0, 17,7,3,3,0, 12,12,8,2,2, 15,14,2,10,1, 9,15,14,2,1, 9,19,14,2,1, 9,23,14,2,1, 11,25,3,6,1, 18,25,3,6,1, 26,4,2,22,2, 24,24,6,2,2, END};
static const uint8_t M_OGRE[] ={7,25,7,6,2, 18,25,7,6,2, 3,12,26,14,1, 3,22,26,3,2, 9,2,14,11,1, 12,6,3,2,3, 18,6,3,2,3, 12,10,2,3,3, 18,10,2,3,3, 0,13,4,10,1, 28,13,4,10,1, END};
static const uint8_t *const MONS[NMON]={M_SLIME,M_BAT,M_WOLF,M_GOB,M_SKEL,M_OGRE};
static const uint8_t H_BIG[]={5,0,6,3,1, 5,3,6,4,2, 4,7,8,5,3, 3,7,1,4,2, 12,7,1,4,2, 5,12,2,4,1, 9,12,2,4,1, 13,3,2,9,2, END};
static const uint8_t H_SMALL[]={2,0,4,2,1, 2,2,4,2,2, 1,4,6,2,3, 2,6,1,2,1, 5,6,1,2,1, END};

static const char *const NAMES[NMON]={"SLIME","CAVE BAT","WILD WOLF","GOBLIN","SKELETON","OGRE"};
/* hp, atk, def, exp, gold */
static const uint8_t EN[NMON][5]={{18,5,1,8,5},{14,7,0,9,6},{24,8,2,12,8},{34,10,3,20,14},{42,12,4,26,18},{64,15,5,45,40}};
static const uint8_t ZG[4]={0,0,1,2}, ZD[4]={3,4,3,5};

/* ---------- palettes ---------- */
#define NV RGB8(20,24,31)
static const uint16_t PWORLD[5][4]={
 {RGB8(79,127,58),RGB8(45,90,39),RGB8(106,160,74),RGB8(107,74,42)},
 {RGB8(183,154,99),RGB8(160,134,82),RGB8(203,178,124),RGB8(120,96,56)},
 {RGB8(47,106,168),RGB8(111,166,220),RGB8(30,70,120),RGB8(20,40,80)},
 {RGB8(74,68,88),RGB8(59,54,72),RGB8(92,85,112),RGB8(35,30,48)},
 {RGB8(79,127,58),RGB8(196,69,47),RGB8(216,201,160),RGB8(90,58,31)}};
static const uint16_t PUI[2][4]={
 {NV,RGB8(58,70,88),RGB8(232,163,61),RGB8(239,230,207)},
 {NV,RGB8(58,70,88),RGB8(239,230,207),RGB8(232,163,61)}};
static const uint16_t PBAR[3][4]={
 {NV,RGB8(58,70,88),0,RGB8(123,176,74)},{NV,RGB8(58,70,88),0,RGB8(74,143,212)},{NV,RGB8(58,70,88),0,RGB8(212,81,58)}};
static const uint16_t PM[7][4]={
 {NV,RGB8(63,154,46),RGB8(108,194,74),RGB8(184,240,160)},
 {NV,RGB8(90,58,122),RGB8(58,37,82),RGB8(255,74,74)},
 {NV,RGB8(138,143,152),RGB8(108,112,120),RGB8(255,210,63)},
 {NV,RGB8(106,168,58),RGB8(138,90,42),RGB8(255,233,74)},
 {NV,RGB8(232,228,212),RGB8(154,168,176),RGB8(85,85,85)},
 {NV,RGB8(122,138,69),RGB8(74,90,42),RGB8(255,221,136)},
 {NV,RGB8(50,56,70),RGB8(50,56,70),RGB8(50,56,70)}};   /* [6] = silhouette */
static const uint16_t PHERO[4]={0,RGB8(138,75,42),RGB8(240,201,160),RGB8(58,111,181)};

/* ---------- state ---------- */
typedef struct{uint8_t lv,x,y,cards;uint16_t hp,mhp,mp,mmp,atk,def,exp,gold;}Hero;
static Hero P;
static uint8_t grace,msgt,zone_hit;
static uint8_t keys,prev;
static uint16_t fc;
static uint8_t tm[18][20],at[18][20];
static uint32_t dirty;
static uint8_t buf[1024],bw;
static char sb[24];static uint8_t sl;

/* ---------- input / frame ---------- */
static void vram_row(uint8_t y,const uint8_t*t,const uint8_t*a){
 VBK_REG=VBK_ATTRIBUTES;set_bkg_tiles(0,y,20,1,a);
 VBK_REG=VBK_TILES;set_bkg_tiles(0,y,20,1,t);
}
static void flush(void){
 uint8_t y;
 if(!dirty)return;
 for(y=0;y<18;y++)if(dirty&(1UL<<y))vram_row(y,tm[y],at[y]);
 dirty=0;
}
static void frame(void){vsync();flush();prev=keys;keys=joypad();fc++;}
#define HIT(b) ((keys&(b))&&!(prev&(b)))
static void wait(uint8_t n){while(n--)frame();}
static uint8_t R(uint8_t a,uint8_t b){return a+rand()%(b-a+1);}

/* ---------- text ---------- */
static void put(uint8_t x,uint8_t y,uint8_t t,uint8_t p){tm[y][x]=BGT+t;at[y][x]=p;dirty|=1UL<<y;}
static uint8_t gl(char c){
 if(c>='a'&&c<='z')c-=32;
 if(c>='A'&&c<='Z')return T_FONT+(c-'A');
 if(c>='0'&&c<='9')return 27+(c-'0');
 switch(c){case '/':return 37;case ':':return 38;case '!':return 39;case '.':return 40;case '?':return 41;
  case '+':return 42;case '-':return 43;case '>':return 44;case '%':return 45;case '&':return 46;case '<':return 47;}
 return 0;
}
static void txt(uint8_t x,uint8_t y,const char*s,uint8_t p){while(*s&&x<20){put(x,y,gl(*s),p);x++;s++;}}
static void ctxt(uint8_t y,const char*s,uint8_t p){txt((20-strlen(s))/2,y,s,p);}
static void clr(uint8_t y,uint8_t n){uint8_t x;while(n--){for(x=0;x<20;x++)put(x,y,0,6);y++;}}
static void num(uint8_t x,uint8_t y,uint16_t v,uint8_t d,uint8_t p){
 uint8_t i;
 for(i=d;i>0;i--){put(x+i-1,y,(v||i==d)?27+v%10:0,p);v/=10;}
}
static void sadd(const char*s){while(*s)sb[sl++]=*s++;sb[sl]=0;}
static void sset(const char*s){sl=0;sadd(s);}
static void snum(uint16_t v){char t[6];uint8_t n=0;do{t[n++]='0'+v%10;v/=10;}while(v);while(n)sb[sl++]=t[--n];sb[sl]=0;}

static uint8_t choose(const char*const*it,uint8_t n,uint8_t y0,uint8_t canback){
 uint8_t c=0,i;
 clr(y0,n);
 for(i=0;i<n;i++)txt(3,y0+i,it[i],6);
 put(1,y0,44,7);
 for(;;){
  frame();
  if(HIT(J_DOWN)&&c+1<n){put(1,y0+c,0,6);c++;put(1,y0+c,44,7);}
  if(HIT(J_UP)&&c>0){put(1,y0+c,0,6);c--;put(1,y0+c,44,7);}
  if(HIT(J_A))return c;
  if(canback&&HIT(J_B))return 255;
 }
}

/* ---------- rasteriser: rect lists -> 2bpp tiles ---------- */
static void ras_begin(uint8_t w){bw=w;memset(buf,0,(uint16_t)w*w);}
static void rects(const uint8_t*p){
 uint8_t i,j;
 while(*p!=END){
  for(j=p[1];j<p[1]+p[3];j++)for(i=p[0];i<p[0]+p[2];i++)buf[(uint16_t)j*bw+i]=p[4];
  p+=5;
 }
}
static void upload(uint8_t first,uint8_t tw,uint8_t th,uint8_t obj){
 uint8_t tx,ty,r,c,p0,p1,v,td[16];
 for(ty=0;ty<th;ty++)for(tx=0;tx<tw;tx++){
  for(r=0;r<8;r++){
   p0=p1=0;
   for(c=0;c<8;c++){v=buf[(uint16_t)(ty*8+r)*bw+tx*8+c];p0=(p0<<1)|(v&1);p1=(p1<<1)|((v>>1)&1);}
   td[r*2]=p0;td[r*2+1]=p1;
  }
  if(obj)set_sprite_data(first+ty*tw+tx,1,td);else set_bkg_data(first+ty*tw+tx,1,td);
 }
}
static void load_monster(uint8_t k){ras_begin(32);rects(MONS[k]);upload(BGT+T_MON,4,4,0);}

static void init_gfx(void){
 uint8_t g,r,b,n,c,td[16];
 DISPLAY_OFF;
 memset(td,0,16);set_bkg_data(BGT,1,td);
 for(g=0;g<47;g++){
  for(r=0;r<8;r++){b=(r>=1&&r<=5)?(uint8_t)(FONT[g][r-1]<<2):0;td[r*2]=b;td[r*2+1]=b;}
  set_bkg_data(BGT+T_FONT+g,1,td);
 }
 for(g=0;g<6;g++){ras_begin(8);rects(A_TERRAIN[g]);upload(BGT+T_GRASS+g,1,1,0);}
 for(n=0;n<9;n++){ras_begin(8);for(r=2;r<6;r++)for(c=0;c<8;c++)buf[r*8+c]=(c<n)?3:1;upload(BGT+T_BAR+n,1,1,0);}
 ras_begin(16);rects(H_BIG);upload(0,2,2,1);
 ras_begin(8);rects(H_SMALL);upload(4,1,1,1);
 set_sprite_palette(0,1,PHERO);
 SPRITES_8x8;SHOW_BKG;SHOW_SPRITES;
}

/* ---------- scenes ---------- */
static void hide_sprites(void){uint8_t i;for(i=0;i<5;i++)move_sprite(i,0,0);}
static void scene_begin(void){
 DISPLAY_OFF;hide_sprites();
 memset(tm,BGT,sizeof tm);memset(at,6,sizeof at);
 SCX_REG=0;SCY_REG=0;dirty=0;
}
static void scene_end(void){dirty=0x3FFFFUL;flush();DISPLAY_ON;}
static void pal_ui(void){set_bkg_palette(6,2,&PUI[0][0]);}
static void pal_world(void){set_bkg_palette(0,5,&PWORLD[0][0]);pal_ui();}
static void pal_mon(uint8_t k){set_bkg_palette(0,1,PM[k]);set_bkg_palette(1,3,&PBAR[0][0]);pal_ui();}

/* ---------- save (battery SRAM) ---------- */
typedef struct{uint8_t m0,m1;Hero h;uint8_t sum;}Save;
#define SV ((Save*)0xA000)
static uint8_t chk(const uint8_t*p,uint8_t n){uint8_t s=0x5A;while(n--)s+=*p++;return s;}
static uint8_t has_save(void){
 uint8_t ok;
 ENABLE_RAM;
 ok=(SV->m0=='A'&&SV->m1=='H'&&SV->sum==chk((const uint8_t*)&SV->h,sizeof(Hero)));
 DISABLE_RAM;return ok;
}
static void save_game(void){
 ENABLE_RAM;
 SV->m0='A';SV->m1='H';memcpy(&SV->h,&P,sizeof(Hero));SV->sum=chk((const uint8_t*)&P,sizeof(Hero));
 DISABLE_RAM;
}
static uint8_t load_game(void){
 uint8_t ok=has_save();
 if(ok){ENABLE_RAM;memcpy(&P,&SV->h,sizeof(Hero));DISABLE_RAM;}
 return ok;
}
static void newgame(void){
 P.lv=1;P.hp=P.mhp=40;P.mp=P.mmp=15;P.atk=8;P.def=3;P.exp=0;P.gold=0;P.x=2;P.y=3;P.cards=0;grace=2;
}
static uint16_t need(void){return 14+6*(uint16_t)P.lv;}
static uint8_t ncards(void){uint8_t i,n=0;for(i=0;i<NMON;i++)if(P.cards&(1<<i))n++;return n;}

/* ---------- card dex ---------- */
static void cards(void){
 uint8_t sel=0,own,x,y;
 for(;;){
  own=(P.cards&(1<<sel))?1:0;
  scene_begin();
  set_bkg_palette(0,1,PM[own?sel:6]);pal_ui();
  load_monster(sel);
  ctxt(0,"CARD DEX",7);
  sset("FOUND ");snum(ncards());sadd("/6");ctxt(2,sb,6);
  for(y=0;y<4;y++)for(x=0;x<4;x++)put(8+x,4+y,T_MON+y*4+x,0);
  ctxt(9,own?NAMES[sel]:"???",7);
  ctxt(11,own?"CAPTURED":"NOT FOUND",6);
  ctxt(13,"CAPTURE COSTS 100G",6);
  ctxt(16,"< > BROWSE  B BACK",6);
  scene_end();
  for(;;){
   frame();
   if(HIT(J_B))return;
   if(HIT(J_RIGHT)){sel=(sel+1)%NMON;break;}
   if(HIT(J_LEFT)){sel=(sel+NMON-1)%NMON;break;}
  }
 }
}

static void stats(void){
 scene_begin();pal_ui();
 ctxt(1,"HERO STATUS",7);
 txt(2,3,"LEVEL",6);num(10,3,P.lv,3,6);
 txt(2,5,"HP",6);num(10,5,P.hp,3,6);txt(13,5,"/",6);num(14,5,P.mhp,3,6);
 txt(2,6,"MP",6);num(10,6,P.mp,3,6);txt(13,6,"/",6);num(14,6,P.mmp,3,6);
 txt(2,8,"ATTACK",6);num(10,8,P.atk,3,6);
 txt(2,9,"DEFENSE",6);num(10,9,P.def,3,6);
 txt(2,11,"EXP",6);num(10,11,P.exp,3,6);txt(13,11,"/",6);num(14,11,need(),3,6);
 txt(2,13,"GOLD",6);num(10,13,P.gold,4,6);
 ctxt(16,"B: BACK",7);
 scene_end();
 do frame();while(!HIT(J_B)&&!HIT(J_A));
}

static const char *const PMENU[4]={"CARDS","SAVE","STATS","BACK"};
static void pause_menu(void){
 uint8_t c;
 for(;;){
  scene_begin();pal_ui();ctxt(2,"MENU",7);scene_end();
  c=choose(PMENU,4,5,1);
  if(c==255||c==3)return;
  if(c==0)cards();
  else if(c==1){save_game();ctxt(12,"GAME SAVED!",7);wait(60);}
  else stats();
 }
}

/* ---------- exploration ---------- */
static const char *const MAP[16]={
"################","#pppp..#...w...#","#pHpp..#.....w.#","#pppp....##..ww#",
"#.pp.#...##....#","#..p.#..ww.....#","#..pppppp......#","#......p..###..#",
"#..##..p..#dd..#","#..##..p....dd.#","#......pppp.dd.#","#.w....p..#dd..#",
"#ww..#.p..#..d.#","#w...#.p.......#","#......p.......#","################"};
static const char TCH[]=".#pwdH";
static const uint8_t TPAL[6]={0,0,1,2,3,4};

static void panel(void){
 txt(16,0,"LV",7);num(18,0,P.lv,2,6);
 txt(16,2,"HP",7);num(16,3,P.hp,3,6);txt(16,4,"/",6);num(17,4,P.mhp,3,6);
 txt(16,6,"MP",7);num(16,7,P.mp,3,6);txt(16,8,"/",6);num(17,8,P.mmp,3,6);
 txt(16,10,"G",7);num(16,11,P.gold,4,6);
 txt(16,13,"XP",7);num(16,14,P.exp,3,6);txt(16,15,"/",6);num(17,15,need(),3,6);
}
static void say(const char*s){clr(16,1);txt(0,16,s,7);msgt=100;}
static void world_setup(void){
 uint8_t x,y,i;
 scene_begin();pal_world();
 for(y=0;y<16;y++)for(x=0;x<16;x++){
  for(i=0;i<6;i++)if(MAP[y][x]==TCH[i])put(x,y,T_GRASS+i,TPAL[i]);
 }
 panel();
 txt(0,17,"START: MENU",6);
 set_sprite_tile(0,4);move_sprite(0,P.x*8+8,P.y*8+16);
 scene_end();
}
static void inn(void){
 if(P.hp==P.mhp&&P.mp==P.mmp)say("YOU FEEL FINE.");
 else if(P.gold<10)say("INN COSTS 10G.");
 else{P.gold-=10;P.hp=P.mhp;P.mp=P.mmp;save_game();panel();say("RESTED. GAME SAVED!");}
}
static void world(void){
 int8_t dx,dy;uint8_t hold=0,nx,ny,ch;
 world_setup();
 for(;;){
  frame();
  if(msgt&&--msgt==0)clr(16,1);
  if(HIT(J_START)){pause_menu();world_setup();continue;}
  dx=dy=0;
  if(keys&J_UP)dy=-1;else if(keys&J_DOWN)dy=1;else if(keys&J_LEFT)dx=-1;else if(keys&J_RIGHT)dx=1;
  if(!(dx|dy)){hold=0;continue;}
  if(hold==0||hold==12){
   if(hold==12)hold=6;
   nx=(uint8_t)(P.x+dx);ny=(uint8_t)(P.y+dy);
   if(nx<16&&ny<16){
    ch=MAP[ny][nx];
    if(ch!='#'&&ch!='w'){
     P.x=nx;P.y=ny;move_sprite(0,P.x*8+8,P.y*8+16);
     if(ch=='H')inn();
     else if(grace)grace--;
     else if((ch=='.'&&rand()<33)||(ch=='d'&&rand()<56)){
      zone_hit=(ch=='d');
      HIDE_BKG;wait(4);SHOW_BKG;wait(4);HIDE_BKG;wait(4);SHOW_BKG;
      return;
     }
    }
   }
  }
  hold++;
 }
}

/* ---------- battle ---------- */
static uint8_t ek,esp;
static uint16_t ehp,emhp,eatk,edef,eexp,egold;
static void bar(uint8_t x,uint8_t y,uint8_t w,uint16_t v,uint16_t m,uint8_t p){
 uint16_t px=m?(uint16_t)((uint32_t)v*w*8/m):0;uint8_t i,n;
 if(v&&!px)px=1;
 for(i=0;i<w;i++){
  if(px>=(uint16_t)(i+1)*8)n=8;else if(px>(uint16_t)i*8)n=px-i*8;else n=0;
  put(x+i,y,T_BAR+n,p);
 }
}
static void bt_upd(void){
 bar(3,1,8,ehp,emhp,3);
 txt(1,9,"HERO LV",6);num(9,9,P.lv,2,6);txt(15,9,"G",7);num(16,9,P.gold,4,6);
 txt(1,10,"HP",6);bar(3,10,8,P.hp,P.mhp,1);num(12,10,P.hp,3,6);txt(15,10,"/",6);num(16,10,P.mhp,3,6);
 txt(1,11,"MP",6);bar(3,11,8,P.mp,P.mmp,2);num(12,11,P.mp,3,6);txt(15,11,"/",6);num(16,11,P.mmp,3,6);
}
static void art_show(uint8_t on){uint8_t x,y;for(y=0;y<4;y++)for(x=0;x<4;x++)put(11+x,1+y,on?T_MON+y*4+x:0,0);}
static void hero_pos(uint8_t x,uint8_t y){move_sprite(0,x,y);move_sprite(1,x+8,y);move_sprite(2,x,y+8);move_sprite(3,x+8,y+8);}
static void blog(const char*s){
 uint8_t x;
 for(x=0;x<20;x++){tm[12][x]=tm[13][x];at[12][x]=at[13][x];}
 dirty|=1UL<<12;clr(13,1);txt(0,13,s,6);wait(32);
}
static void shake(void){uint8_t i;for(i=0;i<6;i++){SCX_REG=(i&1)?3:(uint8_t)-3;wait(2);}SCX_REG=0;}
static void flash_enemy(void){art_show(0);wait(3);art_show(1);wait(3);}

static const char *const BMAIN[4]={"ATTACK","SKILL","GUARD","RUN"};
static const char *const BSKILL[4]={"FIREBALL 6MP","HEAL 5MP","CAPTURE 100G","BACK"};

/* returns 0 = normal end, 1 = game over, 2 = collection complete */
static uint8_t battle(uint8_t z){
 uint8_t m,s,guard=0,ups,mult;int16_t d;uint16_t ch,s1,s2;const uint8_t*b;
 ek=(z?ZD:ZG)[rand()&3];b=EN[ek];
 s1=100+15*(P.lv-1);s2=100+10*(P.lv-1);
 emhp=ehp=(uint16_t)((uint32_t)b[0]*s1/100);eatk=(uint16_t)((uint32_t)b[1]*s1/100);
 edef=(uint16_t)((uint32_t)b[2]*s2/100);eexp=(uint16_t)((uint32_t)b[3]*s1/100);egold=(uint16_t)((uint32_t)b[4]*s2/100);esp=0;
 scene_begin();pal_mon(ek);load_monster(ek);
 txt(1,0,NAMES[ek],7);txt(1,1,"HP",6);art_show(1);bt_upd();
 set_sprite_tile(0,0);set_sprite_tile(1,1);set_sprite_tile(2,2);set_sprite_tile(3,3);hero_pos(32,64);
 scene_end();
 sset(NAMES[ek]);sadd(" APPEARS!");blog(sb);
 for(;;){
  m=choose(BMAIN,4,14,0);
  if(m==0){
   hero_pos(42,64);wait(5);hero_pos(32,64);
   d=P.atk+R(0,3);d-=edef;if(d<1)d=1;
   if(rand()<26){d*=2;sset("CRITICAL! ");snum(d);sadd(" DMG!");}else{sset("YOU HIT FOR ");snum(d);sadd("!");}
   ehp=(d>=(int16_t)ehp)?0:ehp-d;flash_enemy();bt_upd();blog(sb);
  }else if(m==1){
   s=choose(BSKILL,4,14,1);
   if(s==255||s==3)continue;
   if(s==0){
    if(P.mp<6){blog("NOT ENOUGH MP!");continue;}
    P.mp-=6;d=(int16_t)(P.atk*17/10)+R(0,4)-(int16_t)(edef/2);if(d<3)d=3;
    ehp=(d>=(int16_t)ehp)?0:ehp-d;flash_enemy();bt_upd();sset("FIREBALL: ");snum(d);sadd(" DMG!");blog(sb);
   }else if(s==1){
    if(P.mp<5){blog("NOT ENOUGH MP!");continue;}
    P.mp-=5;ch=14+4*(uint16_t)P.lv;if(ch>P.mhp-P.hp)ch=P.mhp-P.hp;P.hp+=ch;
    bt_upd();sset("HEALED ");snum(ch);sadd(" HP!");blog(sb);
   }else{
    if(P.cards&(1<<ek)){blog("ALREADY OWNED!");continue;}
    if(P.gold<CAPCOST){blog("NEED 100G!");continue;}
    P.gold-=CAPCOST;
    ch=25+(uint16_t)(65UL*(emhp-ehp)/emhp);if(ek==5)ch-=10;if(ch>95)ch=95;
    bt_upd();sset("CARD THROWN! ");snum(ch);sadd("%");blog(sb);
    if(R(0,99)<ch){
     P.cards|=(uint8_t)(1<<ek);art_show(0);
     sset("CAPTURED ");sadd(NAMES[ek]);sadd("!");blog(sb);
     save_game();
     if(P.cards==ALLCARDS){blog("ALL CARDS FOUND!");return 2;}
     return 0;
    }
    blog("IT BROKE FREE!");
   }
  }else if(m==2){guard=1;blog("YOU GUARD!");}
  else{
   if(rand()<141){blog("YOU ESCAPED!");return 0;}
   blog("CANNOT ESCAPE!");
  }
  clr(14,4);
  if(ehp==0){
   sset(NAMES[ek]);sadd(" IS DOWN!");art_show(0);blog(sb);
   P.exp+=eexp;P.gold+=egold;if(P.gold>9999)P.gold=9999;
   sset("+");snum(eexp);sadd(" EXP +");snum(egold);sadd("G");blog(sb);
   ups=0;
   while(P.exp>=need()&&P.lv<99){
    P.exp-=need();P.lv++;P.mhp+=8;P.mmp+=3;P.atk+=2;P.def+=1;
    P.hp+=8;if(P.hp>P.mhp)P.hp=P.mhp;P.mp+=3;if(P.mp>P.mmp)P.mp=P.mmp;ups++;
   }
   if(ups){sset("LEVEL UP! LV ");snum(P.lv);blog(sb);blog("STATS INCREASED!");}
   bt_upd();save_game();return 0;
  }
  /* enemy turn */
  mult=10;
  if(ehp*10<emhp*3&&!esp&&R(0,99)<70){esp=1;mult=18;sset(NAMES[ek]);sadd(" GOES WILD!");}
  else if(R(0,99)<22){mult=14;sset(NAMES[ek]);sadd(" SLAMS YOU!");}
  else{sset(NAMES[ek]);sadd(" ATTACKS!");}
  blog(sb);
  d=(int16_t)(eatk*mult/10)+R(0,3)-1-(int16_t)(P.def/2);if(d<1)d=1;
  if(guard){d=(d+1)/2;sset("GUARDED! ");snum(d);sadd(" DMG");}
  else{sset("YOU TAKE ");snum(d);sadd(" DMG!");}
  guard=0;
  P.hp=(d>=(int16_t)P.hp)?0:P.hp-d;
  shake();bt_upd();blog(sb);
  if(P.hp==0){blog("YOU COLLAPSE...");return 1;}
 }
}

/* ---------- title, game over, credits ---------- */
static const char *const CR[]={"*ASHEN HOLLOW","","EVERY CREATURE,","CAPTURED.","","","CREATED AND","DEVELOPED BY","","*ASH&GOLD","","","THANK YOU FOR","PLAYING!"};
#define NCR 14
static void credits(void){
 uint8_t i,x,len,g,p,rt[20],ra[20];uint16_t sc;
 scene_begin();pal_ui();
 for(i=0;i<32;i++)vram_row(i,tm[0],at[0]);
 DISPLAY_ON;
 for(sc=0;sc<(uint16_t)(NCR+36)*8;sc++){
  vsync();keys=joypad();
  if(keys&J_START)break;
  if((sc&7)==0){
   g=(uint8_t)(sc>>3);
   memset(rt,BGT,20);memset(ra,6,20);
   if(g<NCR){
    const char*s=CR[g];p=6;if(*s=='*'){p=7;s++;}
    len=strlen(s);x=(20-len)/2;
    for(i=0;i<len;i++){rt[x+i]=BGT+gl(s[i]);ra[x+i]=p;}
   }
   vram_row((uint8_t)(((sc>>3)+18)&31),rt,ra);
  }
  SCY_REG=(uint8_t)sc;
  vsync();
 }
 scene_begin();pal_ui();
 ctxt(7,"THE END",7);ctxt(9,"A GAME BY ASH&GOLD",6);ctxt(12,"PRESS START",6);
 scene_end();
 do frame();while(!HIT(J_START));
}

static const char *const TNEW[1]={"NEW GAME"};
static const char *const TCONT[2]={"CONTINUE","NEW GAME"};
static void title(void){
 uint8_t x,y,c,keep,hs=has_save();
 scene_begin();set_bkg_palette(0,1,PM[5]);pal_ui();load_monster(5);
 ctxt(1,"ASHEN",7);ctxt(3,"HOLLOW",7);
 for(y=0;y<4;y++)for(x=0;x<4;x++)put(8+x,6+y,T_MON+y*4+x,0);
 ctxt(16,"A GAME BY ASH&GOLD",6);
 scene_end();
 c=hs?choose(TCONT,2,12,0):choose(TNEW,1,12,0);
 initrand(fc^((uint16_t)DIV_REG<<8));
 if(hs&&c==0){load_game();grace=2;}
 else{keep=0;if(hs){load_game();keep=P.cards;}newgame();P.cards=keep;}
}
static void gameover(void){
 uint8_t keep_cards;
 scene_begin();pal_ui();ctxt(7,"GAME OVER",7);ctxt(10,"PRESS A",6);scene_end();
 do frame();while(!HIT(J_A));
 keep_cards = P.cards; // Preserve your monster collection cards if desired
 newgame();            // Resets level, stats, HP/MP, gold, and x=2, y=3
 P.cards = keep_cards; // Restore collected cards
 save_game();          // Save the fresh lvl 1 respawn state
}
static void play(void){
 uint8_t r;
 for(;;){
  world();
  r=battle(zone_hit);grace=3;
  if(r==1)gameover();
  else if(r==2){credits();return;}
 }
}
void main(void){
 init_gfx();
 for(;;){title();play();}
}
