#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>

#include "hw2_output.h"
using namespace std;

class privates{
public:
    int id;
    int row_size;
    int column_size;
    int waiting_time;
    vector<vector<int>> starting_points;
};

pthread_cond_t cond_var;
pthread_cond_t cond_for_stop;
pthread_cond_t cond_for_commander;
pthread_mutex_t break_priv_mutex = PTHREAD_MUTEX_INITIALIZER;
string current_command;
int break_priv_count = 0;
int akk = 0;
int asd = 0;
int qwe = 0;
int aw = 0;

class smokers{
public:
    int id;
    int time_to_smoke;
    int cell_count;
    vector<vector<int>> vec_loc_and_count;
};

class smoker_private_intersection{
public:
    int smoker_id;
    int private_id;
    int smoker_index;
    int private_index;
    pthread_mutex_t* mutex;
};

class smoker_intersection_areas{

public:
    int first_id;
    int second_id;
    int first_index;
    int second_index;
    pthread_mutex_t* mutex;
};



class intersection_areas{

public:
    int first_id;
    int second_id;
    int first_index;
    int second_index;
    pthread_mutex_t* mutex;
};

class commands{
public:
    int time;
    string command;
};

vector<vector<int>> grid;
vector<privates> keep_the_all_privates;
vector<intersection_areas> keep_all_intersection;
vector<commands> keep_all_commands;
vector<smokers> keep_all_smokers;
vector<smoker_private_intersection> keep_all_smoker_priv_intersection;
vector<smoker_intersection_areas> keep_all_smoker_intersect;

bool break_again = false;


void* main_for_thread(void* arg){
    int index = *(int*)arg;
    break_again = false;
    int privates_id = keep_the_all_privates[index].id;
    hw2_notify(PROPER_PRIVATE_CREATED,privates_id,0,0);
    int area_size = keep_the_all_privates[index].starting_points.size();
    for (int i = 0; i < area_size; ++i) {
        kal:
        // lock the necessary mutex with other privates
        int lal = keep_all_intersection.size();
        for (int j = 0; j < lal; ++j) {
            if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_lock(keep_all_intersection[j].mutex);
            }
        }
        int lul = keep_all_smoker_priv_intersection.size();
        // lock the necessary mutex between the privates and smokers
        for (int j = 0; j < lul; ++j) {
            if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_lock(keep_all_smoker_priv_intersection[j].mutex);
            }
        }



        if(current_command != "break" && current_command != "stop"){
            hw2_notify(PROPER_PRIVATE_ARRIVED,privates_id,keep_the_all_privates[index].starting_points[i][0],keep_the_all_privates[index].starting_points[i][1]);
        }
        // count down the grid area one by one
        int a = keep_the_all_privates[index].starting_points[i][0];
        int b = keep_the_all_privates[index].row_size;
        for (int j = a; j < a + b; ++j) {
            int a1 = keep_the_all_privates[index].starting_points[i][1];
            int b1 = keep_the_all_privates[index].column_size;
            for (int k = a1; k < a1 + b1; ++k) {
                while(grid[j][k] > 0){
                    //usleep(keep_the_all_privates[index].waiting_time);
                    struct timespec expected_wait;

                    clock_gettime(CLOCK_REALTIME, &expected_wait);
                    expected_wait.tv_sec += keep_the_all_privates[index].waiting_time / 1000;
                    if(expected_wait.tv_nsec + keep_the_all_privates[index].waiting_time % 1000 * 1000000 < 999999999){
                        expected_wait.tv_nsec += keep_the_all_privates[index].waiting_time % 1000 * 1000000;
                    }
                    else{
                        expected_wait.tv_nsec = expected_wait.tv_nsec + keep_the_all_privates[index].waiting_time % 1000 * 1000000 - 999999999;
                        expected_wait.tv_sec++;
                    }

                    pthread_mutex_t unnecessary_mutex;
                    pthread_mutex_init(&unnecessary_mutex,NULL);
                    pthread_mutex_lock(&unnecessary_mutex);
                    if(current_command == "stop"){
                        hw2_notify(PROPER_PRIVATE_STOPPED, privates_id, 0, 0);
                        int ala = keep_all_intersection.size();
                        for (int j = 0; j < ala; ++j) {
                            if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                                // found a intersection in this area(i)
                                pthread_mutex_unlock(keep_all_intersection[j].mutex);
                            }
                        }
                        int qwe = keep_all_smoker_priv_intersection.size();
                        // unlock the necessary mutex between the privates and smokers
                        for (int j = 0; j < qwe; ++j) {
                            if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                                // found a intersection in this area(i)
                                pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
                            }
                        }

                        pthread_exit(NULL);
                    }
                    if(current_command == "break" || !pthread_cond_timedwait(&cond_var, &unnecessary_mutex, &expected_wait)){
                        pthread_mutex_unlock(&unnecessary_mutex);
                        if(current_command == "break"){
                            int ow = keep_all_intersection.size();
                            for (int j = 0; j < ow; ++j) {
                                if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                                    // found a intersection in this area(i)
                                    pthread_mutex_unlock(keep_all_intersection[j].mutex);
                                }
                            }
                            int pp = keep_all_smoker_priv_intersection.size();
                            // unlock the necessary mutex between the privates and smokers
                            for (int j = 0; j < pp; ++j) {
                                if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                                    // found a intersection in this area(i)
                                    pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
                                }
                            }
                            if(break_again == false){
                                hw2_notify(PROPER_PRIVATE_TOOK_BREAK, privates_id, 0, 0);
                                pthread_mutex_lock(&break_priv_mutex);
                                break_priv_count++;
                                pthread_mutex_unlock(&break_priv_mutex);
                                akk++;
                                asd++;
                                qwe++;
                                aw++;
                                break_again = false;
                            }
                            if(break_priv_count == keep_the_all_privates.size()){
                                break_priv_count = 0;
                                pthread_cond_signal(&cond_for_commander);
                            }

                            pthread_cond_wait(&cond_var,&unnecessary_mutex);
                            if(current_command == "break"){
                                break_again = true;
                            }
                            if(current_command == "stop"){
                                int gg = keep_all_intersection.size();
                                for (int j = 0; j < gg; ++j) {
                                    if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                                        // found a intersection in this area(i)
                                        pthread_mutex_unlock(keep_all_intersection[j].mutex);
                                    }
                                }
                                int gga = keep_all_smoker_priv_intersection.size();
                                // unlock the necessary mutex between the privates and smokers
                                for (int j = 0; j < gga; ++j) {
                                    if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                                        // found a intersection in this area(i)
                                        pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
                                    }
                                }
                                hw2_notify(PROPER_PRIVATE_STOPPED, privates_id, 0, 0);
                                pthread_exit(NULL);
                            }
                            if(current_command == "continue"){
                                break_again = false;
                                hw2_notify(PROPER_PRIVATE_CONTINUED, privates_id, 0, 0);
                                goto kal;
                                for (int j = 0; j < keep_all_intersection.size(); ++j) {
                                    if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                                        // found a intersection in this area(i)
                                        pthread_mutex_lock(keep_all_intersection[j].mutex);
                                    }
                                }
                                // lock the necessary mutex between the privates and smokers
                                for (int j = 0; j < keep_all_smoker_priv_intersection.size(); ++j) {
                                    if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                                        // found a intersection in this area(i)
                                        pthread_mutex_lock(keep_all_smoker_priv_intersection[j].mutex);
                                    }
                                }

                            }

                        }

                        if(current_command == "stop"){
                            int qwet = keep_all_intersection.size();
                            for (int j = 0; j < qwet; ++j) {
                                if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                                    // found a intersection in this area(i)
                                    pthread_mutex_unlock(keep_all_intersection[j].mutex);
                                }
                            }
                            int kn = keep_all_smoker_priv_intersection.size();
                            // unlock the necessary mutex between the privates and smokers
                            for (int j = 0; j < kn; ++j) {
                                if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                                    // found a intersection in this area(i)
                                    pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
                                }
                            }
                            hw2_notify(PROPER_PRIVATE_STOPPED, privates_id, 0, 0);
                            pthread_exit(NULL);
                        }

                    }
                    
                    if(current_command != "break" && current_command != "stop"){
                        if(grid[j][k] > 0){
                            grid[j][k] = grid[j][k] - 1;
                            hw2_notify(PROPER_PRIVATE_GATHERED,privates_id,j,k);
                        }

                    }
                    

                }
                
            }


        }
        
        while(1){
            if(current_command != "break") break;
        }


        int iok = keep_all_intersection.size();
        // unlock again the necessary mutex;
        for (int j = 0; j < iok; ++j) {
            if((keep_all_intersection[j].first_id == privates_id && keep_all_intersection[j].first_index == i) || (keep_all_intersection[j].second_id == privates_id && keep_all_intersection[j].second_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_unlock(keep_all_intersection[j].mutex);
            }
        }
        int ij = keep_all_smoker_priv_intersection.size();
        // unlock the necessary mutex between the privates and smokers
        for (int j = 0; j < ij; ++j) {
            if((keep_all_smoker_priv_intersection[j].private_id == privates_id && keep_all_smoker_priv_intersection[j].private_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
            }
        }
        if(current_command !="stop"){
            hw2_notify(PROPER_PRIVATE_CLEARED,privates_id,0,0);
        }
        else{
            hw2_notify(PROPER_PRIVATE_STOPPED, privates_id, 0, 0);
            pthread_exit(NULL);
        }

    }
    hw2_notify(PROPER_PRIVATE_EXITED,privates_id,0,0);
    free(arg);
}


void* main_for_smoker(void* arg){
    int index = *(int*)arg;
    int smoker_id = keep_all_smokers[index].id;
    hw2_notify(SNEAKY_SMOKER_CREATED, smoker_id, 0, 0);
    int area_size = keep_all_smokers[index].vec_loc_and_count.size();
    for (int i = 0; i < area_size; ++i){
        // lock the necessary mutex with the privates
        for (int j = 0; j < keep_all_smoker_priv_intersection.size(); ++j) {
            if((keep_all_smoker_priv_intersection[j].smoker_id == smoker_id && keep_all_smoker_priv_intersection[j].smoker_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_lock(keep_all_smoker_priv_intersection[j].mutex);
            }
        }
        // lock the mutex with other smokers
        for (int j = 0; j < keep_all_smoker_intersect.size(); ++j) {
            if((keep_all_smoker_intersect[j].first_id == smoker_id && keep_all_smoker_intersect[j].first_index == i) || (keep_all_smoker_intersect[j].second_id == smoker_id && keep_all_smoker_intersect[j].second_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_lock(keep_all_smoker_intersect[j].mutex);
            }
        }

        int before_x = keep_all_smokers[index].vec_loc_and_count[i][0]-1;
        int before_y = keep_all_smokers[index].vec_loc_and_count[i][1]-1;
        int instant_x = keep_all_smokers[index].vec_loc_and_count[i][0]-1;
        int instant_y = keep_all_smokers[index].vec_loc_and_count[i][1]-1;
        if(current_command != "stop"){
            hw2_notify(SNEAKY_SMOKER_ARRIVED, smoker_id, keep_all_smokers[index].vec_loc_and_count[i][0], keep_all_smokers[index].vec_loc_and_count[i][1]);
        }
        for (int j = 0; j < keep_all_smokers[index].vec_loc_and_count[i][2]; ++j) {
            //usleep(keep_all_smokers[index].time_to_smoke);
            struct timespec expected_wait;
            clock_gettime(CLOCK_REALTIME, &expected_wait);
            expected_wait.tv_sec += keep_all_smokers[index].time_to_smoke / 1000;
            if(expected_wait.tv_nsec + keep_all_smokers[index].time_to_smoke % 1000 * 1000000 < 999999999){
                expected_wait.tv_nsec += keep_all_smokers[index].time_to_smoke % 1000 * 1000000;
            }
            else{
                expected_wait.tv_nsec = expected_wait.tv_nsec + keep_all_smokers[index].time_to_smoke % 1000 * 1000000 - 999999999;
                expected_wait.tv_sec++;
            }
            pthread_mutex_t unnecessary_mutex;
            pthread_mutex_init(&unnecessary_mutex,NULL);
            pthread_mutex_lock(&unnecessary_mutex);
            if(current_command == "stop" || !pthread_cond_timedwait(&cond_for_stop, &unnecessary_mutex, &expected_wait)){
                pthread_mutex_unlock(&unnecessary_mutex);
                // unlock the mutex with other smokers
                for (int j = 0; j < keep_all_smoker_intersect.size(); ++j) {
                    if((keep_all_smoker_intersect[j].first_id == smoker_id && keep_all_smoker_intersect[j].first_index == i) || (keep_all_smoker_intersect[j].second_id == smoker_id && keep_all_smoker_intersect[j].second_index == i)){
                        // found a intersection in this area(i)
                        pthread_mutex_unlock(keep_all_smoker_intersect[j].mutex);
                    }
                }
                // unlock the necessary mutex with the privates
                for (int j = 0; j < keep_all_smoker_priv_intersection.size(); ++j) {
                    if((keep_all_smoker_priv_intersection[j].smoker_id == smoker_id && keep_all_smoker_priv_intersection[j].smoker_index == i)){
                        // found a intersection in this area(i)
                        pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
                    }
                }
                hw2_notify(SNEAKY_SMOKER_STOPPED, smoker_id, 0, 0);
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&unnecessary_mutex);

            grid[instant_x][instant_y] = grid[instant_x][instant_y] + 1;
            hw2_notify(SNEAKY_SMOKER_FLICKED, smoker_id, instant_x, instant_y);
           if(instant_x == before_x && (before_y +1 == instant_y || before_y == instant_y)){ // row sabit sağa kay
               instant_y += 1;
           }
           else if(before_y +2 == instant_y && ( before_x == instant_x || before_x +1 == instant_x )){ // column sabit aşağı kay
               instant_x += 1;
           }
           else if(before_x +2 == instant_x && (before_y +2 == instant_y || before_y +1 == instant_y)){ // column sabit sola kay
               instant_y -= 1;
           }
           else if(before_y == instant_y && (before_x +2 == instant_x || before_x +1 == instant_x)){ // row sabit yukarı kay
               instant_x -= 1;
           }

        }
        // unlock the mutex with other smokers
        for (int j = 0; j < keep_all_smoker_intersect.size(); ++j) {
            if((keep_all_smoker_intersect[j].first_id == smoker_id && keep_all_smoker_intersect[j].first_index == i) || (keep_all_smoker_intersect[j].second_id == smoker_id && keep_all_smoker_intersect[j].second_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_unlock(keep_all_smoker_intersect[j].mutex);
            }
        }
        // unlock the necessary mutex with the privates
        for (int j = 0; j < keep_all_smoker_priv_intersection.size(); ++j) {
            if((keep_all_smoker_priv_intersection[j].smoker_id == smoker_id && keep_all_smoker_priv_intersection[j].smoker_index == i)){
                // found a intersection in this area(i)
                pthread_mutex_unlock(keep_all_smoker_priv_intersection[j].mutex);
            }
        }
        hw2_notify(SNEAKY_SMOKER_LEFT, smoker_id, 0, 0);

    }
    hw2_notify(SNEAKY_SMOKER_EXITED, smoker_id, 0, 0);




}



void* main_for_commander(void* arg){
    for (int i = 0; i < keep_all_commands.size(); ++i) {
        if(i == 0){
            usleep(keep_all_commands[i].time * 1000);
        }
        else{
            usleep((keep_all_commands[i].time - keep_all_commands[i-1].time)*1000);
        }
        current_command = keep_all_commands[i].command;
        if(current_command == "break"){
            hw2_notify(ORDER_BREAK,0, 0, 0);
            pthread_mutex_t unnecessary_mutex;
            pthread_mutex_init(&unnecessary_mutex,NULL);
            pthread_mutex_lock(&unnecessary_mutex);
            if(break_again == false){
                pthread_cond_wait(&cond_for_commander,&unnecessary_mutex);
            }
            pthread_mutex_unlock(&unnecessary_mutex);
        }
        if(current_command == "stop"){
            hw2_notify(ORDER_STOP, 0, 0, 0);
            pthread_cond_broadcast(&cond_for_stop);
        }
        if(current_command == "continue") hw2_notify(ORDER_CONTINUE, 0, 0, 0);
        pthread_cond_broadcast(&cond_var);
    }
    free(arg);
}


bool intersect_or_not(int a, int b, int x , int y, int first_row, int first_column, int second_row, int second_column){
    bool flag_for_x = false;
    int first_start_x = a;
    int first_finish_x = a+first_row-1;
    int first_start_y = b;
    int first_finish_y = b+first_column-1;
    int second_start_x = x;
    int second_finish_x = x+second_row-1;
    int second_start_y = y;
    int second_finish_y = y+second_column-1;
    for (int i = first_start_x; i < first_finish_x+1; ++i) {
        for (int j = second_start_x; j < second_finish_x+1; ++j) {
            if(i == j) flag_for_x = true;
            for (int k = first_start_y; k < first_finish_y+1; ++k) {
                for (int l = second_start_y; l < second_finish_y+1; ++l) {
                    if(k==l && flag_for_x) return true;
                }
            }
            flag_for_x = false;
        }
    }
    return false;
}


int main() {
    hw2_init_notifier();
    // take the input and parse the input
    string str_for_line;
    getline(cin,str_for_line);
    string word;
    stringstream s(str_for_line);
    s >> word;
    int row_count = stoi(word);
    s >> word;
    int column_count = stoi(word);
    grid.resize(row_count);
    for (int i = 0; i < row_count; ++i){
        grid[i].resize(column_count);
        getline(cin,str_for_line);
        stringstream s(str_for_line);
        for (int j = 0; j < column_count; ++j) {
            s >> word;
            grid[i][j] = stoi(word);
        }
    }
    getline(cin,str_for_line);
    stringstream s1(str_for_line);
    string kk;
    s1 >> kk;
    int privates_count = stoi(kk);
    for (int i = 0; i < privates_count; ++i) {
        privates a;
        string tmp;
        getline(cin,tmp);
        stringstream s1(tmp);
        s1 >> tmp;
        a.id = stoi(tmp);
        s1 >> tmp;
        a.row_size = stoi(tmp);
        s1 >> tmp;
        a.column_size = stoi(tmp);
        s1 >> tmp;
        a.waiting_time = stoi(tmp);
        s1 >> tmp;
        int for_count = stoi(tmp);
        for (int k = 0; k < for_count; ++k) {
            vector<int> tmp_vector(2);
            getline(cin,tmp);
            stringstream s1(tmp);
            s1 >> word;
            tmp_vector[0] = stoi(word);
            s1 >> word;
            tmp_vector[1] = stoi(word);
            a.starting_points.push_back(tmp_vector);
        }
        keep_the_all_privates.push_back(a);
    }
    //check the commander give any order or not
    getline(cin,str_for_line);
    stringstream s2(str_for_line);
    string kkk;
    s2 >> kkk;
    int smoker_count = 0;
    if(kkk != ""){
        int command_count = stoi(kkk);
        for (int i = 0; i < command_count; ++i) {
            getline(cin,str_for_line);
            stringstream s3(str_for_line);
            s3 >> word;
            commands c;
            c.time = stoi(word);
            s3 >> word;
            c.command = word;
            keep_all_commands.push_back(c);
        }
        // take the cigbuts inputs
        getline(cin,str_for_line);
        stringstream s3(str_for_line);
        string ak;
        s3 >> ak;
        if(ak != ""){
            smoker_count = stoi(ak);
            for (int i = 0; i < smoker_count; ++i) {
                smokers k;
                string tmp_k;
                getline(cin,tmp_k);
                stringstream s4(tmp_k);
                s4 >> tmp_k;
                k.id = stoi(tmp_k);
                s4 >> tmp_k;
                k.time_to_smoke = stoi(tmp_k);
                s4 >> tmp_k;
                k.cell_count = stoi(tmp_k);
                for (int j = 0; j < k.cell_count; ++j) {
                    vector<int> tmp_vector_2(3);
                    getline(cin,tmp_k);
                    stringstream s5(tmp_k);
                    s5 >> tmp_k;
                    tmp_vector_2[0] = stoi(tmp_k);
                    s5 >> tmp_k;
                    tmp_vector_2[1] = stoi(tmp_k);
                    s5 >> tmp_k;
                    tmp_vector_2[2] = stoi(tmp_k);
                    k.vec_loc_and_count.push_back(tmp_vector_2);

                }
                keep_all_smokers.push_back(k);



            }
        }

    }


    // find the intersection areas between all smokers
    int baa = 0;
    for (int i = 0; i < smoker_count; ++i) {
        int gather_area_count = keep_all_smokers[i].vec_loc_and_count.size();
        for (int j = 1; j < smoker_count; ++j) {
            if(i == j) break;
            int gather_area_count_2 = keep_all_smokers[j].vec_loc_and_count.size();
            for (int k = 0; k < gather_area_count; ++k) {
                for (int l = 0; l < gather_area_count_2; ++l) {
                    if(intersect_or_not(keep_all_smokers[i].vec_loc_and_count[k][0],keep_all_smokers[i].vec_loc_and_count[k][1],
                                        keep_all_smokers[j].vec_loc_and_count[l][0],keep_all_smokers[j].vec_loc_and_count[l][1],
                                        1,1,1,
                                        1) == true){
                        smoker_intersection_areas k9;
                        pthread_mutex_t* mutex = new pthread_mutex_t;
                        pthread_mutex_init(mutex,NULL);
                        k9.mutex = mutex;
                        k9.first_id = keep_all_smokers[i].id;
                        k9.second_id = keep_all_smokers[j].id;
                        k9.first_index = k;
                        k9.second_index = l;
                        keep_all_smoker_intersect.push_back(k9);
                        baa++;
                    }
                }
            }
        }
    }
    










    // find the intersection areas between all privates
    for (int i = 0; i < privates_count; ++i) {
        int gather_area_count = keep_the_all_privates[i].starting_points.size();
        for (int j = 1; j < privates_count; ++j) {
            if(i == j) break;
            int gather_area_count_2 = keep_the_all_privates[j].starting_points.size();
            for (int k = 0; k < gather_area_count; ++k) {
                for (int l = 0; l < gather_area_count_2; ++l) {
                    if(intersect_or_not(keep_the_all_privates[i].starting_points[k][0],keep_the_all_privates[i].starting_points[k][1],
                                        keep_the_all_privates[j].starting_points[l][0],keep_the_all_privates[j].starting_points[l][1],
                                        keep_the_all_privates[i].row_size,keep_the_all_privates[i].column_size,keep_the_all_privates[j].row_size,
                                        keep_the_all_privates[j].column_size) == true){
                        intersection_areas k1;
                        pthread_mutex_t* mutex = new pthread_mutex_t;
                        pthread_mutex_init(mutex,NULL);
                        k1.mutex = mutex;
                        k1.first_id = keep_the_all_privates[i].id;
                        k1.second_id = keep_the_all_privates[j].id;
                        k1.first_index = k;
                        k1.second_index = l;
                        keep_all_intersection.push_back(k1);
                    }
                }
            }
        }
    }
    int kale = 0;

    // find the intersection areas between the privates and smokers
    for (int i = 0; i < privates_count; ++i) {
        int gather_area_count = keep_the_all_privates[i].starting_points.size();
        for (int j = 0; j < smoker_count; ++j) {
            int litter_area_count = keep_all_smokers[j].vec_loc_and_count.size();
            for (int k = 0; k < gather_area_count; ++k) {
                for (int l = 0; l < litter_area_count; ++l) {

                    if(intersect_or_not(keep_the_all_privates[i].starting_points[k][0],keep_the_all_privates[i].starting_points[k][1],
                                        keep_all_smokers[j].vec_loc_and_count[l][0] -1,keep_all_smokers[j].vec_loc_and_count[l][1] -1,
                                        keep_the_all_privates[i].row_size,keep_the_all_privates[i].column_size,3,
                                        3) == true){
                        smoker_private_intersection k2;
                        pthread_mutex_t* mutex = new pthread_mutex_t;
                        pthread_mutex_init(mutex,NULL);
                        k2.mutex = mutex;
                        k2.private_id = keep_the_all_privates[i].id;
                        k2.smoker_id = keep_all_smokers[j].id;
                        k2.private_index = k;
                        k2.smoker_index = l;
                        keep_all_smoker_priv_intersection.push_back(k2);
                        kale++;
                    }
                }
            }
        }
    }





    pthread_cond_init(&cond_var, NULL);
    pthread_cond_init(&cond_for_stop, NULL);
    pthread_cond_init(&cond_for_commander, NULL);

    pthread_t th[privates_count];
    pthread_t thr[1];
    pthread_t thre[smoker_count];

    for (int i = 0; i < privates_count; ++i) {
        int* a = (int*)malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &main_for_thread, a) != 0) {
            perror("Failed to create thread");
        }
    }
    for (int i = 0; i < smoker_count; ++i) {
        int* a = (int*)malloc(sizeof(int));
        *a = i;
        if (pthread_create(&thre[i], NULL, &main_for_smoker, a) != 0) {
            perror("Failed to create thread");
        }
    }


    if (pthread_create(&thr[0], NULL, &main_for_commander, NULL) != 0) {
        perror("Failed to create thread");
    }
    for (int i = 0; i < privates_count; ++i) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    for (int i = 0; i < smoker_count; ++i) {
        if (pthread_join(thre[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    if (pthread_join(thr[0], NULL) != 0) {
        perror("Failed to join thread");
    }




    return 0;
}
