#include <iostream>
#include "parser.h"
#include "hw1.h"
#include <cstring>
#include <vector>
#include <unistd.h>
#include "sys/wait.h"
#include "fcntl.h"
#include <stdio.h>
#include <string.h>
using namespace std;
void main_helper(){
    int is_bundle_creation = 0;
    vector<char *> keep_the_bundle_names;
    bool flag = true;
    vector<vector<char **>> keep_the_commands;
    int vector_index = 0;
    while(1){
        parsed_input *parsedInput = new parsed_input();
        char * take_the_input = new char(256);
        cin.get(take_the_input,256);
        cin.get();
        strcat(take_the_input," ");
        parse(take_the_input,is_bundle_creation,parsedInput);
        if(parsedInput->command.type == PROCESS_BUNDLE_CREATE){ //check for the input pbc
            keep_the_bundle_names.push_back(parsedInput->command.bundle_name);
            keep_the_commands.resize(keep_the_commands.size()+1);
            is_bundle_creation = 1;

        }
        else if(parsedInput->command.type == PROCESS_BUNDLE_STOP){ //check for input pbs
            vector_index++;

            is_bundle_creation = 0;

        }
        else if(parsedInput->command.type == PROCESS_BUNDLE_EXECUTION){ //check for the bundle execution
            int name_count;
            int bundle_vec_index;
            int number_of_command;
            if(parsedInput->command.bundle_count == 1){ // for the first three case
                name_count = keep_the_bundle_names.size();
                for (int i = 0; i < name_count; ++i) { // find the index of the bundle_name in the given vector
                    if(strcmp(parsedInput->command.bundles->name,keep_the_bundle_names[i] ) == 0){
                        bundle_vec_index = i;
                        break;
                    }
                }
                number_of_command = keep_the_commands[bundle_vec_index].size();
            }
            if(parsedInput->command.bundles->input == NULL && // execute command without "|" or "<" or ">"
            parsedInput->command.bundles->output == NULL &&
            parsedInput->command.bundle_count == 1){
                for (int i = 0; i < number_of_command; ++i) {
                    if(fork() == 0){
                        execvp(keep_the_commands[bundle_vec_index][i][0],keep_the_commands[bundle_vec_index][i]);
                    }
                }
                for (int i = 0; i < number_of_command; ++i) {
                    wait(nullptr);
                }

            }
            else if(parsedInput->command.bundle_count == 1 && parsedInput->command.bundles->input != nullptr){ // if input contains "<"

                if(parsedInput->command.bundles->output != nullptr){                    // if the input contains both "<" and ">"

                    for (int i = 0; i < number_of_command; ++i) {
                        if(fork() == 0){
                            int file_descp_index = open(parsedInput->command.bundles->input, O_RDWR | O_CREAT,0666);
                            int open_output_file = open(parsedInput->command.bundles->output, O_RDWR | O_CREAT, 0666);
                            dup2(file_descp_index,0);
                            dup2(open_output_file,1);
                            close(file_descp_index);
                            close(open_output_file);
                            execvp(keep_the_commands[bundle_vec_index][i][0],keep_the_commands[bundle_vec_index][i]);
                        }
                    }
                }
                else{                                               // if the input contains only "<"
                    for (int i = 0; i < number_of_command; ++i) {
                        if(fork() == 0){
                            int file_descp_index = open(parsedInput->command.bundles->input, O_RDWR | O_CREAT,0666);
                            dup2(file_descp_index,0);
                            close(file_descp_index);
                            execvp(keep_the_commands[bundle_vec_index][i][0],keep_the_commands[bundle_vec_index][i]);
                        }
                    }

                }
                for (int i = 0; i < number_of_command; ++i) {
                    wait(nullptr);
                }

            }
            else if(parsedInput->command.bundle_count == 1 && parsedInput->command.bundles->input == nullptr && parsedInput->command.bundles->output != nullptr){ //if input contains ">"

                for (int i = 0; i < number_of_command; ++i) {
                    if(fork() == 0){
                        int file_descp_index = open(parsedInput->command.bundles->output, O_RDWR | O_APPEND | O_CREAT,0666);
                        dup2(file_descp_index,1);
                        close(file_descp_index);
                        execvp(keep_the_commands[bundle_vec_index][i][0],keep_the_commands[bundle_vec_index][i]);
                    }
                }
                for (int i = 0; i < number_of_command; ++i) {
                    wait(nullptr);
                }

            }
            else if(parsedInput->command.bundle_count >= 2 && flag == true){
                int name_count_;
                int bundle_vec_index_for_the_first_one = 0;
                int bundle_vec_index_for_the_second_one = 0;

                int arr[parsedInput->command.bundle_count];
                int arr_count = 0;
                name_count_ = keep_the_bundle_names.size();
                for (int i = 0; i < name_count_; ++i) { // find the index of the bundle_name in the given vector
                    if(arr_count == parsedInput->command.bundle_count) break;
                    if(strcmp(parsedInput->command.bundles[arr_count].name,keep_the_bundle_names[i] ) == 0){
                        arr[arr_count] = i;
                        arr_count++;
                        i = -1;
                    }


                }
                int bund_count = parsedInput->command.bundle_count;
                int pipes[(bund_count-1)*2];
                for (int i = 0; i < (bund_count-1)*2; i += 2) {
                    pipe(pipes+i);
                }
                for (int i = 0; i < bund_count; ++i) {
                    if(fork() == 0){
                        if(i == 0 && parsedInput->command.bundles[0].input != nullptr){
                            int file_descp_index = open(parsedInput->command.bundles[0].input, O_RDWR | O_CREAT,0666);
                            dup2(file_descp_index,0);
                            close(file_descp_index);
                        }
                        if(i == 0){

                            dup2(pipes[1],1);
                            for (int j = 0; j < (bund_count-1)*2; ++j) {
                                close(pipes[j]);
                            }
                            execvp(keep_the_commands[arr[i]][0][0],keep_the_commands[arr[i]][0]);
                        }
                        else if(i == bund_count-1){
                            if(parsedInput->command.bundles[bund_count-1].output != nullptr){
                                int file_descp_index = open(parsedInput->command.bundles[bund_count-1].output, O_RDWR | O_APPEND | O_CREAT,0666);
                                dup2(file_descp_index,1);
                                close(file_descp_index);

                            }
                            close(pipes[1]);
                            dup2(pipes[0],0);
                            close(pipes[0]);
                            for (int j = 0; j < (bund_count-1)*2; ++j) {
                                close(pipes[j]);
                            }
                            execvp(keep_the_commands[arr[i]][0][0],keep_the_commands[arr[i]][0]);
                        }
                        else{
                            dup2(pipes[(i-1)*2],0);
                            dup2(pipes[i*2+1],1);
                            for (int j = 0; j < (bund_count-1)*2; ++j) {
                                close(pipes[j]);
                            }
                            execvp(keep_the_commands[arr[i]][i][0],keep_the_commands[arr[i]][i]);
                        }

                    }


                }

                for (int j = 0; j < (bund_count-1)*2; ++j) {
                    close(pipes[j]);
                }
                while(wait(NULL) > 0);

            }

            else if(parsedInput->command.bundle_count >= 2 && flag == false){
                int name_count_;
                int bundle_vec_index_for_the_first_one = 0;
                int bundle_vec_index_for_the_second_one = 0;

                int arr[parsedInput->command.bundle_count];
                int arr_count = 0;
                name_count_ = keep_the_bundle_names.size();
                for (int i = 0; i < name_count_; ++i) { // find the index of the bundle_name in the given vector
                    if(arr_count == parsedInput->command.bundle_count) break;
                    if(strcmp(parsedInput->command.bundles[arr_count].name,keep_the_bundle_names[i] ) == 0){
                        arr[arr_count] = i;
                        arr_count++;
                        i = -1;
                    }


                }

                int count_var_for_pipe = 0;
                for (int i = 1; i < parsedInput->command.bundle_count; ++i) {
                    count_var_for_pipe += 1 + keep_the_commands[arr[i]].size();
                }
                int fd[count_var_for_pipe][2];
                for (int i = 0; i < count_var_for_pipe; ++i) {
                    pipe(fd[i]);
                }
                int tmp_count = 0;
                int bund_count = parsedInput->command.bundle_count;
                for (int i = 0; i < bund_count; ++i) {


                    for (int j = 0; j < keep_the_commands[i].size(); ++j) {
                        if(fork() == 0){
                            if(i == 0 && parsedInput->command.bundles[0].input != nullptr){
                                int file_descp_index = open(parsedInput->command.bundles[0].input, O_RDWR | O_CREAT,0666);
                                dup2(file_descp_index,0);
                                close(file_descp_index);
                            }
                            if(i == 0){

                                dup2(fd[0][1],1);
                                for (int l = 0; l < count_var_for_pipe; ++l) {
                                    close(fd[l][0]);
                                    close(fd[l][1]);
                                }
                                execvp(keep_the_commands[arr[i]][j][0],keep_the_commands[arr[i]][j]);
                            }
                            else if(i == bund_count-1){
                                if(parsedInput->command.bundles[bund_count-1].output != nullptr){
                                    int file_descp_index = open(parsedInput->command.bundles[bund_count-1].output, O_WRONLY | O_APPEND | O_CREAT,0666);
                                    dup2(file_descp_index,1);
                                    close(file_descp_index);

                                }
                                //cout << tmp_count << endl;
                                //cout << "Reading from pipe: " << tmp_count-keep_the_commands[arr[i]].size()+j << endl;
                                     dup2(fd[tmp_count-keep_the_commands[arr[i]].size()+j][0],0);
                                for (int l = 0; l < count_var_for_pipe; ++l) {
                                    close(fd[l][0]);
                                    close(fd[l][1]);
                                }
                                execvp(keep_the_commands[arr[i]][j][0],keep_the_commands[arr[i]][j]);
                            }
                            else{
                                //cout << "Reading from pipe: " << tmp_count-keep_the_commands[arr[i]].size()+j << endl;
                                //cout << tmp_count << endl;
                                dup2(fd[tmp_count-keep_the_commands[arr[i]].size()+j][0],0);
                                dup2(fd[tmp_count][1],1);
                                for (int l = 0; l < count_var_for_pipe; ++l) {
                                    close(fd[l][0]);
                                    close(fd[l][1]);
                                }
                                execvp(keep_the_commands[arr[i]][j][0],keep_the_commands[arr[i]][j]);
                            }


                        }




                    }
                    if(i != bund_count-1)
                    tmp_count += keep_the_commands[arr[i+1]].size()+1;
                    //cout << "temp count : " << tmp_count << endl;
                }
                if(fork() == 0) {
                    tmp_count = 0;
                    for (int i = 0; i < parsedInput->command.bundle_count - 1; ++i) {
                        char buffer[512];
                        //cout << "rep reads from : " << tmp_count << endl;
                        close(fd[tmp_count][1]);
                        ssize_t read_size = read(fd[tmp_count][0], buffer, 511);
                        //cout << buffer << endl;
                        close(fd[tmp_count][0]);
                        buffer[read_size] = '\0';
                        for (int j = 0; j < keep_the_commands[arr[i+1]].size(); ++j) {
                            //cout << "rep writes to " << tmp_count+1+j << endl;
                            close(fd[tmp_count+1+j][0]);
                            write(fd[tmp_count+1+j][1],buffer,read_size);
                            close(fd[tmp_count+1+j][1]);
                        }
                        tmp_count += keep_the_commands[arr[i+1]].size()+1;
                    }
                    exit(0);
                }
                for (int i = 0; i < count_var_for_pipe; ++i) {
                    close(fd[i][0]);
                    close(fd[i][1]);
                }
            }


        }
        else if(parsedInput->command.type == QUIT){
            break;
        }
        else{
            // this is for parsing command inputs such as ls -l, cat
            keep_the_commands[vector_index].push_back(parsedInput->argv);
            if(keep_the_commands[vector_index].size() >= 2) flag = false;



        }
        delete[] take_the_input;
        delete parsedInput; // maybe in the future we can take the commands here
    }
}











