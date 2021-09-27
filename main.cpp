#include "calc.hpp"

#ifndef PC
	APP_NAME("CPpong")
	APP_DESCRIPTION("A simple pong game")
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
//ball and player size
#define pw 100
#define ph 15
#define br 10
//define colors
#define W 0xffff //white
#define B 0x0000 //black

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
	vy    = 2*calcspeed;
	running = true;
}

void initialize_game(){
	new_round();
}

void drawplayfield(){
	//clear the screen
	fillScreen(B);
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
		if(testKey(key1, key2, KEY_5    )){new_round();}
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
	}else if(b<r15){//between 0r and 1r off (exactly on the edge)
		vx -= absvy;
	}else if(b<pw-r15){//between 0r and right side of the player
	
	}else if(b<pw+r15){//between pw and pw+1.5*r
		vx += absvy;
	}else{ //more thab 1.5*r to the right
		running=false;
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
