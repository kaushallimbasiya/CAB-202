#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
#include "helpers.h"





// GLobal Variable
bool game_over = false;
int delay = 100;
int timeout = INT_MAX;
double start_time;

bool p = false;
bool check = false;

// Vacuum state
#define VACUUM_HEIGHT 9
#define VACUUM_WIDTH 9
#define VACUMM_SPEED 01

char * vacuum =
"   @@@   "
"  @@@@@  "
" @@@@@@@ "
"@@@@@@@@@"
"@@@@@@@@@"
"@@@@@@@@@"
" @@@@@@@ "
"  @@@@@  "
"   @@@   "
;
// Charger state
#define CHARGER_HEIGHT 3
#define CHARGER_WIDTH 9
int chx;
int chy;

char * chrgstn = 
"#########"
"#########"
"#########"
;
 
int battery = 100;
int new_battery = 0;

double hx, hy;

double dx, dy, x, y;

//dust state
#define MAX_DUST 1000
#define DUST_WIDTH 1
#define DUST_HEIGHT 1

char * dust = ".";

//slime
#define MAX_SLIME 10
#define SLIME_WIDTH 5
#define SLIME_HEIGHT 5

char * slime = 
"  ~  "
" ~~~ "
"~~~~~"
" ~~~ "
"  ~  "
;

//Trash
#define MAX_TRASH 5
#define TRASH_WIDTH 11
#define TRASH_HEIGHT 6

char * trash = 
"     &     "
"    &&&    "
"   &&&&&   "
"  &&&&&&&  "
" &&&&&&&&& "
"&&&&&&&&&&&"
;

int num_dust = 1;
int count_dust = 0;
int num_slime = 1;
int count_slime = 0;
int num_trash = 1;
int count_trash = 0;
int rubbish_weight;

double dux [MAX_DUST], duy [MAX_DUST];
double sx[MAX_SLIME], sy[MAX_SLIME];
double tx[MAX_TRASH], ty[MAX_TRASH];

int Heading = 90;


bool collided(
    double x0, double y0, int w0, int h0,
    double x1, double y1, int w1, int h1
);


bool collided(
    double x0, double y0, int w0, int h0,
    double x1, double y1, int w1, int h1
){
    int left0 = round(x0) - w0 / 2, right0 = left0 + w0 - 1;
    int left1 = round(x1) - w1 / 2, right1 = left1 + w1 - 1;

    int top0 = round(y0) - h0 / 2, bottom0 = top0 + h0 - 1;
    int top1 = round(y1) - h1 / 2, bottom1 = top1 + h1 - 1;

    if (right1 < left0 || right0 < left1) return false;
    if (bottom1 < top0 || bottom0 < top1) return false;

    return true;
}


bool pixel_collision (int x0, int y0, int w0, int h0, char pixels0[], int x1, int y1, int w1, int h1, char pixels1[]) {

    if ( (x0 + w0) < x1 ) {
        return false;
    }
    if ( (x1 + w1) < x0 ) {
        return false;
    }
    if ( (y0 + h0) < y1 ) {
        return false;
    }
    if ( (y1 + h1) < y0) {
        return false;
    }
    

    for (int j = 0; j <= h0 - 1; j++) {
        for (int i = 0; i <= w0 - 1; i++) {
            if ( pixels0 [i+j*w0] != ' ' ) {
                for (int q = 0; q <= h1 - 1; q++) {
                    for (int k = 0; k <= w1 - 1; k++) {
                        if ( pixels1 [q + k * w1] != ' ' ) {
                            int relx0 = i + x0;
                            int rely0 = j + y0;
                            int relx1 = q + x1;
                            int rely1 = k + y1;
                            if (relx0 == relx1 && rely1 == rely0) {
                                return true;
                            }
                        }
                    }
                }
            }
            
        }
    }return false;
}


void back_base(){
    double by = 4 - y/2;
    double bx = (screen_width()/2 - x) + 2;
    double angle = Heading * atan2(by,bx);
    dy = sin((angle) * M_PI / 180) * VACUMM_SPEED;
    dx = cos((angle) * M_PI / 180) * VACUMM_SPEED;
    check = true;
}


void draw_border() {
    int w, h;
    get_screen_size(&w, &h);
    
    
    //Left
    draw_line(0, 0, 0, h-2, '|');
    draw_char(0, 2, '+');
    draw_char(0, 4, '+');

// Header Line 1
    draw_line(1, 2, w - 2, 2, '-');
    draw_line((w/3), 1, (w/3), 3, '|');
    draw_char((w/3), 2, '+');
    

// Header Line 2
    draw_line(1, 4, w - 2, 4, '-');
    draw_line(2*(w/3), 1, 2*(w/3), 3, '|');
    draw_char((w/3), 4, '+');
    draw_char(2*(w/3), 4, '+');
    draw_char(2*(w/3), 2, '+');
    
    // corners of screen
    draw_char(0, 0,'+');
    draw_char(0, h-1,'+');
    draw_char(0, h-3, '+');
    draw_char(w-1, h-1,'+');
    
    draw_char(w-1, 0, '+');

    //Top
    draw_line(1, 0, w - 2, 0, '-');
    draw_char((w/3), 0, '+');
    draw_char(2*(w/3), 0, '+');

    //Bottom
    draw_line(1, h - 1, w - 2, h - 1, '-');
    
    //Right
    draw_line(w - 1, 1, w - 1, h-2, '|');
    draw_char(w-1, h-3, '+');
    draw_char(w-1, 2, '+');
    draw_char(w-1, 4, '+');

    draw_line(1, h-3, w-2, h-3, '-');
}

void show_state(){

    
    int w, h;
    get_screen_size(&w, &h);

    // Row one column one
    char * string = "N10032029";
    int stringlen = strlen(string);
    int horilength = stringlen/2;
    draw_string(((w-1)/6)-horilength, 1, string);

    // Row one column two

    // Generate random number between 30 and 60.
    
    stringlen = strlen(string);
    horilength = stringlen/2;
    draw_formatted(((w-1)/2)-horilength, 1, "Heading: %d", Heading);

    string = "Load weight (g): ";
    rubbish_weight = count_dust + 5 * (count_slime) + 20 * (count_trash);
    stringlen = strlen(string);
    horilength = stringlen/2;
    draw_formatted(((w-1)/2)-horilength, 3, "Load weight (g): %d", rubbish_weight);
    if (rubbish_weight >= 45){
        back_base();
    }
    

    int time_difference = (int)(get_current_time() - start_time);
    int seconds = time_difference % 60;
    int minutes = time_difference / 60;
    char*time = "Elapsed: %02d:%02d";
    stringlen = strlen(string);
    horilength = stringlen/2;
    draw_formatted(((w-1)/6)-horilength, 3, time, minutes, seconds); 

    battery = 100 - ((int)(get_current_time() - start_time)) + new_battery;
    if (battery > 0){
        battery = battery;
    }
    draw_formatted((.75*(w-1))+horilength, 1, "Battery: %d%%" , battery);
    if (battery <= 25){
        back_base();
        if (pixel_collision(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, chx, chy, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn) == true){
            battery = 100;
        }
    }
    

    
    stringlen = strlen(string);
    horilength = stringlen/2;
    draw_formatted((.75*(w-1))+horilength, 3, "Litter (d, s, l): %d, %d, %d", count_dust, count_slime, count_trash);
}

void draw_pixels (int left, int top, int width, int height, char bitmap[], bool space_is_transparent) {

    for (int j = 0; j <= height - 1; j++) {
        for (int i = 0; i <= width - 1; i++) {
            if (bitmap [ i + j*width ] != ' ') {
                draw_char ( left + i, top + j, bitmap [ i + j*width ]);
            }
            else if (space_is_transparent == false) {
                draw_char ( left + i, top + j, ' ');
            }
        }
    }
}









void draw_vacuum() {
    int w, h;
    get_screen_size(&w, &h);

    draw_pixels(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, false);
}

void setup_vacuum(){

    dx = 0;
    dy = VACUMM_SPEED;

    // dx = (rand() % 100) / 100;
    // dy = (rand() % 100) / 100;

    x = (zdk_screen->width + VACUUM_WIDTH)/2 - 8;
    y = (zdk_screen->height + VACUUM_HEIGHT)/2 - 8;
}

void draw_charger(){
    int w, h;
    get_screen_size(&w, &h);
    draw_pixels((w/2)- CHARGER_WIDTH/2, 5, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn, false);

}

void setup_charger(){
    int w, h;
    get_screen_size(&w, &h);

    chx = (w/2)- CHARGER_WIDTH/2;
    chy = 5;
}


bool vacuum_ctrl(int ch) {
    return (ch == 'j' || ch == 'l' || ch == 'k' || ch == 'i');
}



void update_vacuum(){

    // Generate random number between 30 and 60.
    int r = rand() % 31 + 30;
    
    int lx = round(x);
    int ly = round(y);
    bool hit = false;
    // bool check = true;

    if(lx >= (screen_width() - VACUUM_WIDTH - 2)){
        timer_pause(100);
        // dx = -dx;
        Heading = (r + Heading) % 360;
        dx = VACUMM_SPEED * cos((Heading) * M_PI / 180);
        // //  dy = VACUMM_SPEED * sin(r * M_PI / 180);
        hit = true;
        x = (screen_width() - VACUUM_WIDTH - 2);
    }
    

    if(ly >= (screen_height() - VACUUM_HEIGHT - 3)){
        timer_pause(100);
        // dy = -dy;
        // // dx = VACUMM_SPEED * cos(r * M_PI / 180);
        Heading = (r + Heading) % 360;
        dy = -VACUMM_SPEED * sin((Heading) * M_PI / 180);
        
        hit = true;
        y = (screen_height() - VACUUM_HEIGHT - 3);
    }
    if(lx < 2){
        timer_pause(100);
        // dx = -dx;
        Heading = (r + Heading) % 360;
        dx = VACUMM_SPEED * cos((Heading) * M_PI / 180);
        // // dy = VACUMM_SPEED * sin(r * M_PI / 180);
        hit = true;
        x = 2;
    }
    if(ly <= 6 ){
        timer_pause(100);
        Heading = (r + Heading) % 360;
        dy = VACUMM_SPEED * sin((Heading) * M_PI / 180);
        dx = VACUMM_SPEED * cos((Heading) * M_PI / 180);
        
        // dy = VACUMM_SPEED * sin((Heading) * M_PI / 180);
        
        hit = true;

        y = 6;
    }
    if(hit == true){
        timer_pause(100);
        if (dy == 0){
            dy = VACUMM_SPEED;
        }
        if (dx == 0){
            dx = VACUMM_SPEED;
        }
    }

   if (pixel_collision(lx, ly, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, chx, chy, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn) == true){   
        Heading = (r + Heading) % 360;
        //dx = VACUMM_SPEED * cos((Heading) * M_PI / 180);
        dy = VACUMM_SPEED * sin((Heading) * M_PI / 180);
    }
    
    x += dx;
    y += dy;

}





void update_manually(ch){
    if (ch == 'j' && x - VACUUM_WIDTH / 2 > 0){
        x--;
    }
    if (ch == 'l' && x + VACUUM_WIDTH < screen_width() - 2){
        x++;
    }
    if (ch == 'k' && y + VACUUM_HEIGHT < screen_height() - 3){
        y++;
    }
    if (ch == 'i' && (y - VACUUM_HEIGHT / 2 > 4)){
        y--;
    }
}

void draw_dust() {
    for (int i = 0; i < num_dust; i++) {
        if(duy[i] > -1 && dux[i] > -1){
            draw_pixels(dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT, dust, true);
        }
    }
}

void setup_dust(int i){
    int w, h;
    get_screen_size (&w, &h);

    dux[i] = 1 + DUST_WIDTH / 2  + rand() % (w - 1 - DUST_WIDTH);
    duy[i] = 1 + DUST_HEIGHT / 2 + rand() % (h - 7 - DUST_HEIGHT) + 4;


}

bool compare_dust(int d){
    for (int i = 0; i < num_dust; i++){
        if (i !=d && (dux[i] == dux[d] && duy[i] == duy[d])){
            return true;
        }
    }
    return false;
}

void setup_dusts(){
    num_dust = get_int("How many dust would you like?");

    if (num_dust < 1 || num_dust > MAX_DUST) {
        num_dust = 1;
    }

    for (int i = 0; i < num_dust; i++) {

        do {
            setup_dust(i);
        }
        while (compare_dust(i) == true || collided(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT));
    }   
}

void draw_slime() {
    for (int i = 0; i < num_slime; i++) {
        if(sy[i] > -1 && sx[i] > -1){
            draw_pixels(sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT, slime, true );
        }
    }
}

void setup_slime(int i){
    int w, h;
    get_screen_size (&w, &h);

    sx[i] = 1 + SLIME_WIDTH / 2  + rand() % (w - 1 - SLIME_WIDTH) - 1;
    sy[i] = 1 + SLIME_HEIGHT / 2 + rand() % (h - 8 - SLIME_HEIGHT) + 3;

}

bool compare_slime(int s){
    for (int i = 0; i < num_slime; i++){
        if (i != s && (sx[i] == sx[s] && sy[i] == sy[s])){
            return true;
        }
    }
    return false;
}

void setup_slimes(){
    num_slime = get_int("How many slime would you like?");

    if (num_slime < 1 || num_slime > MAX_SLIME) {
        num_slime = 1;
    }

    for (int i = 0; i < num_slime; i++) {

        do {
            setup_slime(i);
        }
        while (compare_slime (i) == true || collided(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT));
    }   
}

void draw_trash() {
    for (int i = 0; i < num_trash; i++) {
        if(ty[i] > -1 && tx[i] > -1){
            draw_pixels(tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT, trash, true );
        }
    }
}

void setup_trash(int i){
    int w, h;
    get_screen_size (&w, &h);

    tx[i] = 1 + TRASH_WIDTH / 2  + rand() % (w - 2 - TRASH_WIDTH) - 5;
    ty[i] = 1 + TRASH_HEIGHT / 2 + rand() % (h - 8 - TRASH_HEIGHT) + 2;

}

bool compare_trash(int t){
    for (int i = 0; i < num_trash; i++){
        if (i != t && (tx[i] == tx[t] && ty[i] == ty[t])){
            return true;
        }
    }
    return false;
}

void setup_trashes(){
    num_trash = get_int("How many trash would you like?");

    if (num_trash < 1 || num_trash > MAX_TRASH) {
        num_trash = 1;
    }

    for (int i = 0; i < num_trash; i++) {

        do {
            setup_trash(i);
        }
        while (compare_trash(i) == true || collided(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT));
    }   
}

void update_rubbish(){

    int w, h;
    get_screen_size (&w, &h);

    // Collision for Dusts:

    for(int i = 0; i < num_dust; i++){
        if(pixel_collision(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT, dust) == true){
            dux[i] = -1;
            duy[i] = -1;
            
            count_dust++;
        }
        else if (pixel_collision(chx, chy, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn, dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT, dust) == true) {
            if(!(dux[i] == -1 && duy[i] == -1)){
                dux[i] = 1 + DUST_WIDTH / 2  + rand() % (w - 1 - DUST_WIDTH);
                duy[i] = 1 + DUST_HEIGHT / 2 + rand() % (h - 7 - DUST_HEIGHT) + 4;
            }
        }

        else if(pixel_collision(dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT, dust, sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT, slime) == true){
            if(!(dux[i] == -1 && duy[i] == -1)){
                dux[i] = 1 + DUST_WIDTH / 2  + rand() % (w - 1 - DUST_WIDTH);
                duy[i] = 1 + DUST_HEIGHT / 2 + rand() % (h - 8 - DUST_HEIGHT) + 4;
            }
        }

        else if(pixel_collision(dux[i], duy[i], DUST_WIDTH, DUST_HEIGHT, dust, tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT, trash) == true){
            if(!(dux[i] == -1 && duy[i] == -1)){
                dux[i] = 1 + DUST_WIDTH / 2  + rand() % (w - 1 - DUST_WIDTH);
                duy[i] = 1 + DUST_HEIGHT / 2 + rand() % (h - 8 - DUST_HEIGHT) + 4;
            }
        }

        
    }
    // Collision for Slimes:

    for (int i = 0; i < num_slime; i++){
        if(pixel_collision(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT, slime) == true){
            sx[i] = -1;
            sy[i] = -1;
            count_slime++;
        }
        else if (pixel_collision(chx, chy, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn, sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT, slime) == true) {
            if (!(sx[i] == -1 && sy[i] == -1)){
                sx[i] = 1 + SLIME_WIDTH / 2  + rand() % (w - 1 - SLIME_WIDTH) - 1;
                sy[i] = 1 + SLIME_HEIGHT / 2 + rand() % (h - 8 - SLIME_HEIGHT) + 3;
            }
        }

        else if(pixel_collision(sx[i], sy[i], SLIME_WIDTH, SLIME_HEIGHT, slime, tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT, trash) == true){
            if (!(sx[i] == -1 && sy[i] == -1)){
                sx[i] = 1 + SLIME_WIDTH / 2  + rand() % (w - 1 - SLIME_WIDTH) - 1;
                sy[i] = 1 + SLIME_HEIGHT / 2 + rand() % (h - 8 - SLIME_HEIGHT) + 3;
            }
        }
    }
    // Collision for Trashes:

    for (int i = 0; i < num_trash; i++){
        if(pixel_collision(x, y, VACUUM_WIDTH, VACUUM_HEIGHT, vacuum, tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT, trash) == true){
            tx[i] = -1;
            ty[i] = -1;
            count_trash++;
        }
        else if (pixel_collision(chx, chy, CHARGER_WIDTH, CHARGER_HEIGHT, chrgstn, tx[i], ty[i], TRASH_WIDTH, TRASH_HEIGHT, trash) == true) {
            if (!(tx[i] == -1 && ty[i] == -1)) {
            tx[i] = 1 + TRASH_WIDTH / 2  + rand() % (w - 2 - TRASH_WIDTH) - 5;
            ty[i] = 1 + TRASH_HEIGHT / 2 + rand() % (h - 8 - TRASH_HEIGHT) + 2;    
            }
        }
    }

}

void draw_all() {
    clear_screen();
    draw_border();
    show_state();
    draw_vacuum();
    draw_charger();
    draw_dust();
    draw_slime();
    draw_trash();

    
    
    //draw_dust();
    show_screen();
    
    
}

void setup (void) {
    start_time =  get_current_time();
    setup_vacuum();
    setup_charger();
    setup_dusts();
    setup_slimes();
    setup_trashes();
    draw_all();
}

void dust_cheat(){
    int dust_id = get_int("How many dust would you like?");
    int dust_x = get_int("x?");
    int dust_y = get_int("y?");

    if(dust_id >= 0 && dust_id < num_dust){
        dux[dust_id] = dust_x;
        duy[dust_id] = dust_y;
    }
}


void slime_cheat(){
    int slime_id = get_int("How many slime would you like?");
    int slime_x = get_int("x?");
    int slime_y = get_int("y?");

    if(slime_id >= 0 && slime_id < num_slime){
        sx[slime_id] = slime_x;
        sy[slime_id] = slime_y;
    }
}


void trash_cheat(){
    int trash_id = get_int("How many trash would you like?");
    int trash_x = get_int("x?");
    int trash_y = get_int("y?");

    if(trash_id >= 0 && trash_id < num_trash){
        tx[trash_id] = trash_x;
        ty[trash_id] = trash_y;
    }
}

void do_vacuum_cheat() {
    x = get_int("Vacuum x?");
    y = get_int("Vacuum y?");
}

void do_battery_cheat() {
    int temp_battery = get_int("Enter a new battery level? "); 
    new_battery = (new_battery + temp_battery) - battery;
}

void reset(){
    clear_screen();
    setup();
}

void do_timeout() {
    timeout = get_int("How many seconds shall the program run?");
}

bool timed_out() {
    return get_current_time() - start_time >= timeout;
}

void do_delay_cheat(){
    delay = get_int("New delay (milliseconds)?");

    if (delay < 0) delay = 100;
}


void do_help() {
    clear_screen();
    draw_formatted_lines(screen_width()/2, screen_height()/2,
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "!!!! Vacuum Simulation Command !!!!\n"
        "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
        "\n"
        "b: Immediate return to base.\n"
        "j, k, l, i: Move Vacuum left, down, right, up respectively\n"
        "d: Dust cheat\n"
        "s: Slime cheat\n"
        "t: Trash cheat\n"
        "m: Change the millisecond loop delay\n"
        "r: restart\n"
        "o: Specify time-out (seconds) after which the program terminates\n"
        "p: pause or resume Vacuum movement\n"
        "q: quit\n"
        "v: Vacuum cheat 1 - Override the position of the vacuum\n"
        "w: Vacuum cheat 2 - Override the load of the Vacuum (grams)\n"
        "y: Vacuum cheat 3 - Override the battery level\n"
        "?: display this help screen\n\n\n"
        "Press a key to return to the game."
    );
    show_screen();

    while (get_char() > 0) {}
    while (!timed_out() && wait_char() <= 0) {}
}

void do_operation(int ch){
    if (ch == 'r'){
        reset();
    }
    else if (vacuum_ctrl(ch)) {
        update_manually(ch);
    }
    else if (ch == 'b' || battery <= 25 || rubbish_weight >= 45) {
        back_base();
    }
    else if (ch == 'o') {
        do_timeout();
    }
    else if (ch == 'm') {
        do_delay_cheat();
    }
    else if (ch == 'd') {
        dust_cheat();
    }
    else if (ch == 's') {
        slime_cheat();
    }
    else if (ch == 't') {
        trash_cheat();
    }
    else if (ch == 'p') {
        p = !p;
    }
    else if (ch == 'v') {
        do_vacuum_cheat();
    }
    else if (ch == 'y') {
        do_battery_cheat();
    }
    else if (ch == '?') {
        do_help();
    }
}




void loop() {
    
    char ch = get_char();

    if (ch == 'q'){
        game_over = true;
        return;
    }
    else if (ch >= ' ') {
        do_operation(ch);
    }
    if (p){
        update_vacuum();
    }
    
    update_rubbish();
    draw_all();




}
int main(void) {

    setup_screen();
    setup();
    while (!game_over && !timed_out()) {
        loop();
        timer_pause( delay );
    }
    return 0;
}