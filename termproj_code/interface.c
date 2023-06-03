#include "term_proj.h"

#define START_MAX 2
#define MENU_MAX 2
#define ONE_MAX 12
#define TWO_MAX 6

#define BMP_LIST_MAX 30
#define BMP_PRINT_MAX 10


bool search_done=FALSE;
bool select_done=FALSE;
bool trans_done=FALSE;

char file1_name[20]; 
char file2_name[20];

char* start_items[START_MAX] = {"START","QUIT"};
char* menu_items[MENU_MAX] = {"Enter File Name","Get File From Server"};
char* option_items[2] = {"Check Other Directory", "Get File From Server"};

char* num[2] = {"Use One Image","Use Two Image"};

char* one[ONE_MAX] = {"Brighten","Contrast Ratio","Gamma Encoding","Highboost Filter","Unsharpen","Inverse", "Translate", "Rotate  90", "Rotate 180", "Rotate 270", "Mirror", "Flip"};
char* two[TWO_MAX] = {"Add"," Average ","Subtract ","Difference","Bitwise And","Bitwise OR"};

int current_item = 0;
int current_item1 = 0;
int current_item2 = 0;
int current_server = 0;
int current_one = 0;
int current_two = 0;
int current_get;
int current_option = 0;

int height, width;

int main() {
    int pid;
    fileinfo file1;
    SignalSetup();
    initscr();
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);
    
    getmaxyx(stdscr, height, width);
    while(true){
        while (true) {
            clear();
            Highlight(START_MAX, start_items, current_item);

            int key = getch();

            switch (key) {
            case KEY_UP:
                current_item = (current_item - 1 + START_MAX) % START_MAX;
                continue;
            case KEY_DOWN:
                current_item = (current_item + 1) % START_MAX;
                continue;
            case '\n':
            case '\r':
                if (current_item == 0) { // 시작 부분
                    Start(&file1);
                }
                else { // 종료
                    endwin();
                    return 0;
                }
                break;
            default:
                continue;
            }

            break;
        }

        current_item = 0;
        while (true) {
            clear();
            
            Highlight(2, num, current_item);

            int key = getch();

            switch (key) {
            case KEY_UP:
                current_item = (current_item - 1 + 2) % 2;
                continue;
            case KEY_DOWN:
                current_item = (current_item + 1) % 2;
                continue;
            case '\n':
            case '\r':
                if (current_item == 0) { 
                    OnePhotoUsage(&file1);
                }
                else { 
                    TwoPhotoUsage(&file1);
                }
                break;
            default:
                continue;
            }
            
            break;
        }
        char* output_file_name;
        output_file_name = SaveBMPFile(&file1);
        if ((pid=fork()) == -1){
            oops("Failed to fork");
        }
        else if(pid == 0){
            char* args[] = {"eog", output_file_name , NULL};
            execvp("eog", args);
        }
        else{
            wait(NULL);
            free(output_file_name);
	        int key = getch();
            while (key != '\n' && key != '\r')
                key = getch();
        }
        
    }
}
void Start(fileinfo* file) {
    bool search_done = false;

    while (!search_done) {
        clear();
        Highlight(MENU_MAX, menu_items, current_item1);

        int key = getch();

        switch (key) {
        case KEY_UP:
            current_item1 = (current_item1 - 1 + MENU_MAX) % MENU_MAX;
            continue;
        case KEY_DOWN:
            current_item1 = (current_item1 + 1) % MENU_MAX;
            continue;
        case '\n':
        case '\r':
            if (current_item1 == 0) { // 파일 이름 입력
                keypad(stdscr, FALSE);
                int find_bmp = 0;
                find_bmp = InputFileName(file);
                clear();
		        keypad(stdscr, TRUE);
                curs_set(0);
                noecho();

                if(find_bmp) {
                    mvprintw(height / 2, (width - strlen("Succeed to find ") - strlen(file->name))/2 - 3, "Succeed to find \"%s\"", file->name);
                    key = getch();
                    while (key != '\n' && key != '\r')
                        key = getch();
                }
                else{
                    mvprintw(height / 2, (width - strlen("Failed to find  ") - strlen(file->name))/2, "Failed to find \"%s\"", file->name);
                    key = getch();
                    while (key != '\n' && key != '\r')
                        key = getch();
                    FileNotExist(file);
                } 

                search_done = true;
            }
            else { // 서버 이용
                UseServer(file);
                search_done = true;
            }
            break;
        }
    }
}

void UseServer(fileinfo* file) {
    char** bmp_list;
    int token_num = 0;
    bmp_list = GetBMPList("192.168.0.5", &token_num);
    int current_bmp = 0;

    while(1){
        clear();
        Highlight2(token_num, bmp_list, current_bmp);

        int key = getch();
        switch (key) {
		case KEY_UP:
		    current_bmp = (current_bmp - 1 + token_num) % token_num;
		    continue;
		case KEY_DOWN:
		    current_bmp = (current_bmp + 1) % token_num;
		    continue;
		case '\n':
		case '\r':
		    GetBMPFromServer("192.168.0.5", bmp_list[current_bmp] ,file);
		    break;
		default:
			continue;
		}
		break;
	}
    clear();
      
    free(bmp_list);
}


void FileNotExist(fileinfo* file) {
	while(true) {
        clear();
        Highlight(2, option_items, current_option);

        int key = getch();

		switch (key) {
		case KEY_UP:
		    current_option = (current_option - 1 + 2) % 2;
		    continue;
		case KEY_DOWN:
		    current_option = (current_option + 1) % 2;
		    continue;
		case '\n':
		case '\r':
		    if (current_option == 0) { // 찾아보기 선택한 경우
                SearchPath(file);
		    }
		    else { // 서버
		        UseServer(file);
		    }
		    break;
		default:
			continue;
		}
		break;
	}
}

void SearchPath(fileinfo* file){
    char save_path[256];
    if (getcwd(save_path, sizeof(save_path)) == NULL){
        oops("Failed to getcwd");
    }
    int path_num = 0;
	char** path_list;
    path_list = GetPathList(file->name, &path_num);
    int current_path = 0;
    while(1){
        clear();
        Highlight(path_num, path_list, current_path);

        int key = getch();
    
        switch (key) {
		case KEY_UP:
		    current_path = (current_path - 1 + path_num) % path_num;
		    continue;
		case KEY_DOWN:
		    current_path = (current_path + 1) % path_num;
		    continue;
		case '\n':
		case '\r':
		    ReadFileFromName(path_list[current_path] ,file);
            clear();
            mvprintw(height / 2, (width - strlen("Succeed to read file from absolute path"))/2, "Succeed to read file from absolute path");
            mvprintw(height / 2 + 2, (width - strlen(path_list[current_path]))/2, "\"%s\"", path_list[current_path]);
            key = getch();
            while (key != '\n' && key != '\r')
                key = getch();

		    break;
		default:
			continue;
		}
		break;
	}
    clear();
    chdir(save_path);
    for(int i = 0; i < path_num; i++){
        free(path_list[i]);
    }   
    free(path_list);
}

void OnePhotoUsage(fileinfo* file) {
    int img_w = (file->header).biWidth;
    int img_h = (file->header).biHeight;
    clear();

    while (true) {
        clear();
        Highlight(ONE_MAX, one, current_one);
        int key = getch();

        switch (key) {
        case KEY_UP:
            current_one = (current_one - 1 + ONE_MAX) % ONE_MAX;
            continue;
        case KEY_DOWN:
            current_one = (current_one + 1) % ONE_MAX;
            continue;
        case '\n':
        case '\r':
            if (current_one == 0) { // Brighten
                int bright_val = GetBrightenVal();
                clear();
                Brighten(file->pixels, img_w, img_h, bright_val);
                return;
            }
            else if (current_one == 1) { // Contrast
                int cont_val = GetContVal();
                clear();
                Contrast(file->pixels, img_w, img_h, cont_val);
                return;
            }
            else if (current_one == 2) { // Gamma
                float gamma_val = GetGammaVal();
                clear();
                GammaFunc(file->pixels, img_w, img_h, gamma_val);
                return;
            }
            else if (current_one == 3) { // Highboost
                float alpha = GetHighVal();
                file->pixels = HighBoost(file->pixels, img_w, img_h, alpha);
                clear();
                return;
            }
            else if (current_one == 4) { // Unsharpen
                clear();
                file->pixels = UnSharpMask(file->pixels, img_w, img_h);
                return;
            }
            else if (current_one == 5) { // Inverse
                Inverse(file->pixels, img_w, img_h);
                clear();
                return;
            }
            else if (current_one == 6) { // Translate
                int delta_x, delta_y;
                char val_buffer[100];
                clear();
                mvprintw(height/2, (width - strlen("Enter x value: "))/2 - 4, "Enter x value: ");
                curs_set(1);
	            echo();
                getstr(val_buffer);
                delta_x = atoi(val_buffer);
                clear();
                mvprintw(height/2, (width - strlen("Enter y value: "))/2 - 4, "Enter y value: ");
                getstr(val_buffer);
                delta_y = atoi(val_buffer);
                curs_set(0);
                noecho();
                file->pixels = Translate(file->pixels, img_w, img_h, delta_x, delta_y);
                clear();
                return;
            }
            else if (current_one == 7) { // Rotate 90
                file->pixels = Rotate90(file->pixels, img_w, img_h);
                int temp;
                temp = (file->header).biWidth;
                (file->header).biWidth = (file->header).biHeight;
                (file->header).biHeight = temp;
                clear();
                return;
            }
            else if (current_one == 8) { // Rotate 180
                file->pixels = Rotate180(file->pixels, img_w, img_h); 
                clear();
                return;
            }
            else if (current_one == 9) { // Rotate 270
                file->pixels = Rotate270(file->pixels, img_w, img_h);
                int temp;
                temp = (file->header).biWidth;
                (file->header).biWidth = (file->header).biHeight;
                (file->header).biHeight = temp; 
                clear();
                return;
            }
            else if (current_one == 10) { // Mirror
                file->pixels = Mirror(file->pixels, img_w, img_h);
                clear();
                return;
            }
            else if (current_one == 11) { // Filp
                file->pixels = Flip(file->pixels, img_w, img_h);
                clear();
                return;
            }
        }
    }
}
void TwoPhotoUsage(fileinfo* file1) {
    clear();
    fileinfo file2;
    Start(&file2);
    if((file1->header).biWidth != (file2.header).biWidth || (file1->header).biHeight != (file2.header).biHeight){
        oops("Sizes of Two images are different");
    }
    int img_w = (file1->header).biWidth;
    int img_h = (file1->header).biHeight;
    

    while (true) {
        clear();
        Highlight(TWO_MAX, two, current_two);
        int key = getch();

        switch (key) {
        case KEY_UP:
            current_two = (current_two - 1 + TWO_MAX) % TWO_MAX;
            break;
        case KEY_DOWN:
            current_two = (current_two + 1) % TWO_MAX;
            break;
        case '\n':
        case '\r':
            if (current_two == 0) { // Add
                clear();
                Add(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
            else if (current_two == 1) { // Average
                clear();
                Avg(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
            else if (current_two == 2) { // Subtract
                clear();
                Subtract(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
            else if (current_two == 3) { // Difference
                clear();
                Diff(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
            else if (current_two == 4) { // Bitwise And
                clear();
                And(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
            else if (current_two == 5){ // Bitwise Or 
                clear();
                Or(file1->pixels, file2.pixels, img_w, img_h);
                return;
            }
        }
    }
}
void Highlight(int max_num, char** menu, int num){
	for (int i = 0; i < max_num; i++) { 
            int x = width / 2 - strlen(menu[i]) / 2;
            int y = height / 2 - max_num / 2 + i;
            if (i == num) {
                attron(A_REVERSE); 
                mvprintw(y, x, "%s", menu[i]);
                attroff(A_REVERSE);
            }
            else {
                mvprintw(y, x, "%s", menu[i]);
            }
        }
}

void Highlight2(int max_num, char** menu, int num) {
    clear();
    mvprintw(1, 0, "Searched 'BMP' File List");

    for (int i = 0; i < max_num; i++) {
        int x = (i/BMP_PRINT_MAX)*20;
        int y = 4 + i%BMP_PRINT_MAX;
        if (i == num) {
            attron(A_REVERSE);
            mvprintw(y, x, "%s", menu[i]);
            attroff(A_REVERSE);
        }
        else {
            mvprintw(y, x, "%s", menu[i]);
        }
    }
}

void HLBright(int num){
	int x = width/2;
	int y = height/3*2;
	
	mvprintw(height/2, (width-12)/2, "Bright Value");

	mvprintw(y, x-3, "[");
	
	attron(A_REVERSE);
	mvprintw(y, x-2, "%4d", num*5);
	attroff(A_REVERSE);
	
	mvprintw(y, x+2, "]");
}

int GetBrightenVal(){
	int num = 0;
		
	 while (true) {
        clear();
        HLBright(num);
		int key = getch();
	
        switch (key) {
        case KEY_LEFT:
        	if(num>=-50)
        		num--;
            continue;
        case KEY_RIGHT: 
        	if(num<=50)
        		num++;
            continue;
        case '\n':  
        case '\r':
        	return num*5;
        }
	}
}
void HLCont(int num){
	int x = width/2;
	int y = height/3*2;
	
	mvprintw(height/2, (width-14)/2, "Contrast Value");

	mvprintw(y, x-3, "[");
	
	attron(A_REVERSE);
	mvprintw(y, x-2, "%4d", num*5);
	attroff(A_REVERSE);
	
	mvprintw(y, x+2, "]");
}
int GetContVal(){
	int num = 0;
		
	 while (true) {
        clear();
        HLCont(num);
		int key = getch();
	
        switch (key) {
        case KEY_LEFT:
        	if(num>-20)
        		num--;
            continue;
        case KEY_RIGHT: 
        	if(num<20)
        		num++;
            continue;
        case '\n':  
        case '\r':
        	return num*5;
        }
	}
}
void HLGamma(float val){
	int x = width/2;
	int y = height/3*2;
	
	mvprintw(height/2, (width-11)/2, "Gamma Value");

	mvprintw(y, x-2, "[");
	
	attron(A_REVERSE);
	mvprintw(y, x-1, "%.1lf", val);
	attroff(A_REVERSE);
	
	mvprintw(y, x+2, "]");
}
float GetGammaVal(){
	float val = 0.1;
	
	 while (true) {
        clear();
        HLGamma(val);
		int key = getch();
	
        switch (key) {
        case KEY_LEFT:
        	if(val>0.1)
        		val-=0.1;
            continue;
        case KEY_RIGHT: 
        	if(val<=2.9)
        		val+=0.1;
            continue;
        case '\n':  
        case '\r':
        	return val;
        }
	}
}
void HLHigh(float val){
	int x = width/2;
	int y = height/3*2;
	
	mvprintw(height/2, (width-10)/2, "High Value");

	mvprintw(y, x-2, "[");
	
	attron(A_REVERSE);
	mvprintw(y, x-1, "%.1lf", val);
	attroff(A_REVERSE);
	
	mvprintw(y, x+2, "]");
}
float GetHighVal(){
	float val = 0.1;
	
	 while (true) {
        clear();
        HLHigh(val);
		int key = getch();
	
        switch (key) {
        case KEY_LEFT:
        	if(val>0.1)
        		val-=0.1;
            continue;
        case KEY_RIGHT: 
        	if(val<2.0)
        		val+=0.1;
            continue;
        case '\n':  
        case '\r':
        	return val;
        }
	}

}

void SignalSetup(){
    signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
}
