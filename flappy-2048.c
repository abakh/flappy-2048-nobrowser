#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "config.h"
typedef signed char byte;
ALLEGRO_FONT *font, *smallfont;
ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *backbuffer,*buffer;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_TIMER *timer;
float sqlen= 60;//can be changed
float size,insize,startx,starty,endx,endy,sprtr;
float sqrsx[2]; // in fraction of insize
byte holes[2];//y that starts the hole
float heroy= 0.5;// protagonist's y in fraction of insize
float v=0; //protagonist's velocity

int score=0,best=0;
ALLEGRO_COLOR bg_c,fg_c,fg2_c,sqr_c,text_c,text2_c;
ALLEGRO_COLOR colors[12];
char scorestr[7]={0};//(ab)used to print the best score and the header score too
void er(int x,char* msg){
	if(!x){
		fprintf(stderr,"Failed at %s.\n",msg);
		exit(EXIT_FAILURE);
	}
}
void set_coordinates(void){
	sprtr=sqlen/10;
	size= sqlen*4+5*sprtr;
	insize= size-2*sprtr;//size of inner buffer
        startx=sprtr;
        endx=startx+size;
	starty= (size)/2;
        endy=starty+size;
}
byte what_color(void){
	int s=2;
	byte ret=0;
	while(s<score && ret<12){
		if(!s)
			s=1;
		else
			s*=2;
		ret++;
	}
	return ret;
}
void set_colors(void){
	bg_c=al_map_rgb(250,248,239);
	fg_c=al_map_rgb(187,173,160);
	fg2_c=al_map_rgb(205,193,180);
	sqr_c=al_map_rgb(238,228,218);
	text_c=al_map_rgb(119,110,101);
	text2_c=al_map_rgb(250,248,239);
	colors[0]=al_map_rgb(238,228,218);
	colors[1]=al_map_rgb(236,224,202);
	colors[2]=al_map_rgb(243,176,117);
	colors[3]=al_map_rgb(245,149,99);
	colors[4]=al_map_rgb(247,122,93);
	colors[5]=al_map_rgb(245,93,59);
	colors[6]=al_map_rgb(236,205,113);
	colors[7]=al_map_rgb(237,203,97);
	colors[8]=al_map_rgb(236,199,78);
	colors[9]=al_map_rgb(237,196,63);
	colors[10]=al_map_rgb(235,194,45);
	colors[11]=al_map_rgb(63,62,61);
}
void init(void){
	er(al_init(),"init");
	er(al_init_primitives_addon(),"primitives' init");
	er(al_init_image_addon(), "image init");
	al_init_font_addon(); //returns void
	er(al_init_ttf_addon() , "ttf init");
	er(al_install_mouse() , "installing mouse");
	er(al_install_keyboard() , "installing keyboard");
	set_coordinates();
	set_colors();
	if( display= al_create_display((int) size+sprtr*2,(int)(size+sprtr*2)*3/2)){
		backbuffer= al_get_backbuffer(display);
		al_set_window_title(display,"Flappy 2048!");
	}
	else
		goto Display;
	if( queue= al_create_event_queue() )
		;
	else
		goto Queue;
	if( timer = al_create_timer(1.0/FPS) )
		;
	else
		goto Timer;
	if( buffer = al_create_bitmap(insize,insize) )
		;
	else 
		goto Buffer;
	er( (font=al_load_ttf_font(FONT_ADDRESS,FONT_SIZE,0)) || (font=al_create_builtin_font()) , "loading ttf font or allegro's builtin");
	er( (smallfont=al_load_ttf_font(FONT_ADDRESS,SMALL_SIZE,0)) || (smallfont=al_create_builtin_font()) , "loading large ttf font or allegro's builtin");

	al_register_event_source(queue,al_get_display_event_source(display));
	al_register_event_source(queue,al_get_timer_event_source(timer));
	al_register_event_source(queue,al_get_keyboard_event_source());
	al_register_event_source(queue,al_get_mouse_event_source());
	return;

	Buffer:
	al_destroy_bitmap(buffer);
	perror("Destroying buffer.");
	Timer:
	al_destroy_timer(timer);
	perror("Destroying timer.");
	Queue:
	al_destroy_event_queue(queue);
	perror("Destroying event queue.");
	Display:
	al_destroy_display(display);
	perror("Destroying display (don't fear, not like that ;).");
	exit(EXIT_FAILURE);
}

void drawbg(void){
	al_clear_to_color(bg_c);
	al_draw_filled_rounded_rectangle(startx,starty,endx,endy,8,8,fg_c);
	al_draw_text(font,text_c,sprtr,1.4*sqlen,ALLEGRO_ALIGN_LEFT,"Flappy 2048");
	float sx,sy;
	for(byte x=0;x<4;x++){
		for(byte y=0;y<4;y++){
			sx=startx+sprtr*(x+1)+sqlen*x;
			sy=starty+sprtr*(y+1)+sqlen*y;
			al_draw_filled_rounded_rectangle(sx,sy,sx+sqlen,sy+sqlen,2,2,fg2_c);
		}
	}
}
void drawscores(void){
	#define TEXTY (int) (3*sqlen/8)+sprtr
	al_draw_filled_rounded_rectangle(sprtr,sprtr,sprtr+sqlen,sprtr+sqlen,2,2,fg_c);
	al_draw_filled_rounded_rectangle(size+sprtr-sqlen,sprtr,size+sprtr,sprtr+sqlen,2,2,fg_c);
	al_draw_text(smallfont,text_c,sprtr+sqlen/2,sprtr+sprtr,ALLEGRO_ALIGN_CENTRE,"SCORE");
	sprintf(scorestr,"%d",score);
	al_draw_text(smallfont,text_c,sprtr+sqlen/2,TEXTY,ALLEGRO_ALIGN_CENTRE,scorestr);
	al_draw_text(smallfont,text_c,size-sqlen/2+sprtr,sprtr+sprtr,ALLEGRO_ALIGN_CENTRE,"BEST");
	sprintf(scorestr,"%d",best);
	al_draw_text(smallfont,text_c,size-sqlen/2+sprtr,TEXTY,ALLEGRO_ALIGN_CENTRE,scorestr);
}
void draw(void){//draws to buffer
	al_clear_to_color(al_map_rgba(0,0,0,0));
	float sx,sy;
	for(byte y=0;y<4;y++){
		for(byte barrier=0;barrier<2;barrier++){
			if(y!= holes[barrier] && y!= holes[barrier]+1){
				sx=sqrsx[barrier]*insize;
				sy=y*(sqlen+sprtr);
				al_draw_filled_rounded_rectangle(sx,sy,sx+sqlen,sy+sqlen,2,2,sqr_c);
			}
		}
	}
	byte w=what_color();
	al_draw_filled_rounded_rectangle(0,heroy*size,sqlen,(heroy*size)+sqlen,2,2,colors[w]);
	sprintf(scorestr,"%d",score);
	ALLEGRO_COLOR c;
	if(w<2)
		c=text_c;
	else
		c=text2_c;
	if(score < 1000)
		al_draw_text(font,c,sqlen/2,(heroy+0.045)*size,ALLEGRO_ALIGN_CENTRE,scorestr);
	else
		al_draw_text(smallfont,c,sqlen/2,(heroy+0.085)*size,ALLEGRO_ALIGN_CENTRE,scorestr);
}
bool collision(void){
	float tilestart;
	byte y;
	if(heroy< -0.22)//higher than the visible square
		return 1;
	if(sqrsx[0] < 0.22 && sqrsx[0] > -0.22){// 10/(10+1)*4 = the first 10/44= 0.22 is sqlen in fraction of insize
		for(y=0;y<4;y++){
			if(y!=holes[0] && y!=holes[0]+1){
				tilestart=y*0.24;//0.22+0.02
				if(heroy-tilestart<0.22 && heroy-tilestart>-0.22 )
					return 1;
			}
		}
	}
	return 0;
}
int main(void){
	FILE* bestrec=fopen(BEST_ADDRESS,"r");
	if(bestrec){
		fscanf(bestrec,"%d",&best);
	}
	init();
	al_start_timer(timer);
	sqrsx[0]=0.5;
	sqrsx[1]=NOTHING;
	holes[0]=rand()%3;
	holes[1]=NOHOLE;
	bool redraw=1,hitend=0;
	ALLEGRO_EVENT ev;
	byte foo,pressed;
	while(1){
		al_wait_for_event(queue,&ev);
		if(ev.type==ALLEGRO_EVENT_TIMER){
				redraw=true;
				if(sqrsx[0] <= NOTHING){
					holes[0]=holes[1];
					holes[1]=NOHOLE;
					sqrsx[0]=sqrsx[1];
					sqrsx[1]=NOTHING;

					score++;
					if(score>best)
						best=score;
				}
				if(sqrsx[0]<=0.25 && sqrsx[1]<=NOTHING){//sqrsx[1] gets less than nothing
					sqrsx[1]=1;
					holes[1]=rand()%3;
				}
				for(foo=0;foo<2;foo++){
					sqrsx[foo]-=WORLDSPEED;
				}
				heroy+=v;
				v+=GRAVITY;
				if(heroy > 0.95 ){
					if(!hitend){
						v=BOUNCE-(v*FRICTION);
						hitend=1;
					}
				}
				else
					hitend=0;
				
				if(collision() )
					score=0;
		}
		else if(ev.type==ALLEGRO_EVENT_KEY_DOWN || ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			pressed++;
		else if(ev.type==ALLEGRO_EVENT_KEY_UP || ev.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			pressed--;
		else if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
			bestrec=fopen(BEST_ADDRESS,"w");
			if(bestrec)
				fprintf(bestrec,"%d",best);
			fflush(bestrec);
			break;
		}
		if(pressed)
			v=JUMP;
		if(redraw && al_is_event_queue_empty(queue)){
			drawbg();
			al_set_target_bitmap(buffer);
			draw();
			al_set_target_bitmap(backbuffer);
			drawscores();
			al_draw_bitmap(buffer,startx+sprtr,starty+sprtr,0);
			al_flip_display();
		}
	}
	al_destroy_bitmap(buffer);
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
        al_destroy_display(display);
	return EXIT_SUCCESS;
}
