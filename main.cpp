#include "calc.hpp"

#ifndef PC
	APP_NAME("CPpong")
	APP_DESCRIPTION("A simple pong game. Use '<-', '->', '1' and '3' to control, 5 to restart, EXE or Clear to quit.")
	APP_AUTHOR("SnailMath (inspiration by InterChan)")
	APP_VERSION("1.0.0")
#endif

//variables
bool running;

#ifdef PC
	#define calcspeed 1
#else
	#define calcspeed 3
#endif

#define pv 3*calcspeed //player velocity

//player and ball position and ball velocity
int bx,by,vx,vy;
int px[2];
//player scores
int score[2];
int firstmove = 1;
//ball and player size
#define pw 100
#define ph 15
#define br 10
//define colors
#define W 0xffff //white
#define B 0x0000 //black

bool waitkey;

void circle(int x, int y, int r, uint16_t c){
  for (int ix = -r; ix<=r ; ix++)
    for (int iy = -r; iy<=r ; iy++)
      if( ix*ix + iy*iy < r*r) //a circle are all points where x^2+y^2=r^2 is true.
        //setPixel takes care to check if the dot is on the screen.
        setPixel(x+ix,y+iy,c); 
}

void square(int x, int y, int w, int h, uint16_t c){
  for (int ix = 0; ix<w ; ix++)
    for (int iy = 0; iy<h ; iy++)
      //setPixel takes care to check if the dot is on the screen.
      setPixel(x+ix,y+iy,c);
}

void new_round(){
	px[0] = (width-pw)/2;
	px[1] = (width-pw)/2;
	bx    = width/2;
	by    = height/2;	
	vx    = 1*calcspeed;
	vy    = 2*calcspeed * firstmove;
	firstmove = -firstmove;
	running = true;
}

void initialize_game(){
	waitkey = true;
	firstmove = 1;
	score[0]=0;
	score[1]=0;
	new_round();
}

//draw a 7-segment display with the segments in n lit up
void segment(uint8_t n, int x, int y, int w, int h, uint16_t c){
	/*A*/ if(n&0b10000000) square(x,   y,     w+h, h,   c);
	/*B*/ if(n&0b01000000) square(x+w, y,     h,   w+h, c);
	/*C*/ if(n&0b00100000) square(x+w, y+w,   h,   w+h, c);
	/*D*/ if(n&0b00010000) square(x,   y+2*w, w+h, h,   c);
	/*E*/ if(n&0b00001000) square(x,   y+w,   h,   w+h, c);
	/*F*/ if(n&0b00000100) square(x,   y,     h,   w+h, c);
	/*G*/ if(n&0b00000010) square(x,   y+w,   w+h, h,   c);
}

//draw a 1 digit number on the screen
void digit(uint8_t n, int x, int y, int w, int h, uint16_t c){
	switch (n) {
		case 0:	segment(0b11111100, x, y, w, h, c); break;
		case 1:	segment(0b01100000, x, y, w, h, c); break;
		case 2:	segment(0b11011010, x, y, w, h, c); break;
		case 3:	segment(0b11110010, x, y, w, h, c); break;
		case 4:	segment(0b01100110, x, y, w, h, c); break;
		case 5:	segment(0b10110110, x, y, w, h, c); break;
		case 6:	segment(0b10111110, x, y, w, h, c); break;
		case 7:	segment(0b11100000, x, y, w, h, c); break;
		case 8:	segment(0b11111110, x, y, w, h, c); break;
		case 9:	segment(0b11110110, x, y, w, h, c); break;
	}
}
//draw a 4 digit decimal number on the screen
void number(uint16_t n, int x, int y, int w, int h, uint16_t c){
	if(n>=1000) digit((n/1000)%10, x    , y, w, h, c);
	if(n>= 100) digit((n/100 )%10, x+2*w, y, w, h, c);
	if(n>=  10) digit((n/10  )%10, x+4*w, y, w, h, c);
	digit            ((n     )%10, x+6*w, y, w, h, c);
}

void drawplayfield(){
	//clear the screen
	fillScreen(B);
	//draw th net in the middle
	for (int x=0; x<width; x+=28) square(x,height/2-3,14,6,W);
	//draw the scores
	number(score[0], width-(160), height/2-(3+20+20+5+10), 20, 5, color(128,128,128));
	number(score[1], width-(160), height/2+(3        +10), 20, 5, color(128,128,128));
	//draw player 0
	square(px[0],0,pw,ph,W);
	//draw player 1
	square(px[1],height-ph,pw,ph,W);
	//draw ball
	circle(bx,by,br,W);
	//refresh the screen
	LCD_Refresh();
}

#define handleInput() if(input()) break;
bool input(){
		uint32_t key1, key2;	//First create variables
		getKey(&key1, &key2);	//then read the keys
		//Use testKey() to test if a specific key is pressed
		if(testKey(key1, key2, KEY_CLEAR)){return true;}
		if(testKey(key1, key2, KEY_EXE  )){return true;}
		if(testKey(key1, key2, KEY_LEFT )){if(px[0]>0       )px[0]-=pv;}
		if(testKey(key1, key2, KEY_RIGHT)){if(px[0]<width-pw)px[0]+=pv;}
		if(testKey(key1, key2, KEY_1    )){if(px[1]>0       )px[1]-=pv;}
		if(testKey(key1, key2, KEY_3    )){if(px[1]<width-pw)px[1]+=pv;}
		if(testKey(key1, key2, KEY_5    )){if(waitkey){waitkey=false;new_round();}}else{waitkey=true;}
		return false;
}

//calculate collisions (for physics)
void collide(int p){
	vy = -vy;
	int absvy = vy>0?vy:-vy;
	int b = bx - px[p];
	int r05 = br/2;		// 0.5 * radius
	int r10 = br;		// 1.0 * radius
	int r15 = r10+r05;	// 1.5 * radius

	//avoid ball getting stuck in the corners
	if(bx<2*br) 
		vx = 1*calcspeed;
	if(bx>width-(2*br))	
		vx = -1*calcspeed;

	//collision with the player
	if(b<-r15){//more than 1.5*r off (waay wrong)
		running=false;
		score[1-p]++;
		//if(p){firstmove=-1;}else{firstmove= 1;}
	}else if(b<r15){//between 0r and 1r off (exactly on the edge)
		vx -= absvy;
	}else if(b<pw-r15){//between 0r and right side of the player
	
	}else if(b<pw+r15){//between pw and pw+1.5*r
		vx += absvy;
	}else{ //more thab 1.5*r to the right
		running=false;
		score[1-p]++;
		//if(p){firstmove=-1;}else{firstmove= 1;}
	}


	//if(bx<px[p]-br)running=false;
	//if(bx>px[p]+pw+br)running=false;
  
}

//do the physics
void physics(){
	if(!running)return;
	bx+=vx;
	by+=vy;
	//collide with left/right walls
	if(bx+br>=width || bx-br<0)  vx = -vx;
	//the ball won't reach the top or bottom wall.
	//if(by+br>=height || by-br<0) vy = -vy;
	//collide player 0
	if(by-br<ph)         collide(0);
	//collide player 1
	if(by+br>=height-ph) collide(1);
}

//The acutal main
void main2(){
	initialize_game();
	while(true){ //main loop
		handleInput();
		physics();
		drawplayfield();
	}

}
