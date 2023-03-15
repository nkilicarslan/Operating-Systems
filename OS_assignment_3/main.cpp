#include <iostream>
#include "parser.h"
#include "fat32.h"
#include <sstream>
#include <string>
#include <vector>
#include <string.h>
#include <ctime>
#include <cmath>
using namespace std;

vector<string> path = {};
BPB_struct bpb;

vector<string> keep_months = {"January","February","March","April","May","June","July","August","September","October","November","December"};

parsed_input* take_the_input(bool* is_it_quit,string *input_param, string *input3){

    parsed_input *p = new parsed_input;
    string input;

    getline(cin,input);
    *input_param = input;
    /*int input_size = input.size();
    char input_array[input_size+1];
    for (int i = 0; i < input_size; ++i) {
        input_array[i] = input[i];
    }*/
    stringstream ss(input);
    string first_input;
    string second_input;
    string third_input;
    ss >> first_input >> second_input >> third_input;
    if(first_input == "quit"){
        *is_it_quit = true;
        p->type = QUIT;
        return p;
    }
    int char_size = first_input.size();
    if(second_input != ""){
        char_size += second_input.size() + 1;
    }
    if(third_input != ""){
        char_size += third_input.size() + 1;
    }
    int first_input_size = first_input.size();
    int second_input_size = second_input.size();
    int third_input_size = third_input.size();
    char input_array[char_size+1];
    for (int i = 0; i < first_input_size; ++i) {
        input_array[i] = first_input[i];
    }
    if(second_input != ""){
        input_array[first_input_size] = ' ';
        for (int i = 0; i < second_input_size; ++i) {
            input_array[first_input_size+1+i] = second_input[i];
        }
    }

    if(third_input != ""){
        input_array[first_input_size+second_input_size] = ' ';
        for (int i = 0; i < third_input_size; ++i) {
            input_array[first_input_size+second_input_size+1+i] = third_input[i];
        }
    }


    input_array[char_size] = '\0';
    parse(p,input_array);
    if(p->type == CD){
        *input_param = second_input;
    }
    if(p->type == LS){
        *input_param = second_input;
        *input3 = third_input;
    }
    if(p->type == MKDIR || p->type == TOUCH){
        *input_param = second_input;
    }
    if(p->type == CAT){
        *input_param = second_input;
    }
    return p;
}

int binaryToDecimal(string n)
{
    string num = n;
    int dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }

    return dec_value;
}





void print_the_path(vector<string> path){
    int size_of_vec = path.size();
    if(size_of_vec == 0) cout << "/";
    for (int i = 0; i < size_of_vec; ++i) {
        cout << "/" << path[i];
    }
    cout << "> ";
}

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void arrange_path(vector<string>* path,string input){
    vector<string> input_parameters;
    if(input[0] == '/'){ // if the full path is wanted, then do some arrangement.
        input = input.substr(1, input.size()-1);
        string delimiter = "/";
        input_parameters = split (input, delimiter);
        path->clear();

    }
    else{  // relative path
        string delimiter = "/";
        input_parameters = split (input, delimiter);

    }

    for (int i = 0; i < input_parameters.size(); ++i) { //arrange the path vector according to given input
        if(path->size() != 0 && input_parameters[i] == ".."){
            path->pop_back();
        }
        else{
            if(input_parameters[i] != "-l")path->push_back(input_parameters[i]);
        }
    }
}

vector<int> decToBinary(int n)
{
    // array to store binary number
    int binaryNum[32];
    vector<int> result_vec;
    vector<int> binary_num_reversed;

    // counter for binary array
    int i = 0;
    while (n > 0) {

        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--) binary_num_reversed.push_back(binaryNum[j]);
    for (int j = 0; j < 16-binary_num_reversed.size(); ++j) {
        result_vec.push_back(0);
    }

    for (int j = 0; j < binary_num_reversed.size(); ++j) {
        result_vec.push_back(binary_num_reversed[j]);
    }
    return result_vec;
}

vector<int> decToBinary_2(int n)
{
    // array to store binary number
    int binaryNum[32];
    vector<int> result_vec;
    vector<int> binary_num_reversed;

    // counter for binary array
    int i = 0;
    while (n > 0) {

        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--) binary_num_reversed.push_back(binaryNum[j]);

    for (int j = 0; j < binary_num_reversed.size(); ++j) {
        result_vec.push_back(binary_num_reversed[j]);
    }
    return result_vec;
}




uint16_t atou16(const char *s)
{
    uint16_t v = 0;
    while (*s) { v = (v << 1) + (v << 3) + (*(s++) - '0'); }
    return v;
}



void remove_last_mem(vector<string>* path){
    vector<string> tmp;
    for (int i = 0; i < path->size()-1; ++i) {
        tmp.push_back((*path)[i]);
    }
    *path = tmp;

}

int find_empty_cluster(int sub_cluster_num,FILE* disk){
    fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector+4*2, SEEK_SET);

    int count = 2;
    uint32_t fat_entry;
    while(1){
        fread(&fat_entry,4,1,disk);
        fat_entry &= 0x0FFFFFFF;
        if(fat_entry == 0){
            fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector+4*count, SEEK_SET);
            uint32_t  k = 268435448;
            fwrite(&k,4,1,disk);
            break;
        }
        count++;

    }

    return count;
}

void fill_empty_cluster(uint16_t myInt16,uint16_t myInt16_2,int sub_cluster_num,int count,FILE* disk){
    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+count*2)*bpb.BytesPerSector, SEEK_SET);
    FatFileEntry fatFileEntry;
    fatFileEntry.msdos.attributes = 16;
    fatFileEntry.msdos.firstCluster = count+2;
    fatFileEntry.msdos.filename[0] = '.';
    for (int i = 0; i < 7; ++i) {
        fatFileEntry.msdos.filename[i+1] = ' ';
    }
    for (int i = 0; i < 3; ++i) {
        fatFileEntry.msdos.filename[i] = ' ';
    }
    fatFileEntry.msdos.modifiedDate = myInt16;
    fatFileEntry.msdos.modifiedTime = myInt16_2;
    fwrite(&fatFileEntry,32,1,disk);

    FatFileEntry fatFileEntry_2;
    fatFileEntry_2.msdos.attributes = 16;
    fatFileEntry_2.msdos.firstCluster = sub_cluster_num;
    fatFileEntry_2.msdos.filename[0] = '.';
    fatFileEntry_2.msdos.filename[1] = '.';
    for (int i = 0; i < 6; ++i) {
        fatFileEntry_2.msdos.filename[i+2] = ' ';
    }
    for (int i = 0; i < 3; ++i) {
        fatFileEntry_2.msdos.filename[i] = ' ';
    }
    fatFileEntry_2.msdos.modifiedDate = myInt16;
    fatFileEntry_2.msdos.modifiedTime = myInt16_2;
    fwrite(&fatFileEntry_2,32,1,disk);
}


void write_new_clust_num(int old_cluster_num,int new_clus_num, FILE* disk){
    fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector+4*(old_cluster_num+2), SEEK_SET);
    fwrite(&new_clus_num,4,1,disk);
}


void write_to_the_file(int sub_cluster_num,char** argv,string name, int type){
    std::time_t now = std::time(0);   // get time now
    std::tm* ltm = std::localtime(&now);
    /*std::cout << (ltm->tm_year + 1900) << '-'
              << (ltm->tm_mon + 1) << '-'
              <<  ltm->tm_mday
              << 5+ltm->tm_hour << ":"
              << 30+ltm->tm_min
              << ltm->tm_sec
              << "\n";
    */
    string creation_mod_date = "";
    vector<int> asd;
    asd = decToBinary_2(ltm->tm_year -80);
    for (int i = 0; i < 7-asd.size(); ++i) {
        creation_mod_date.append("0");
    }
    for (int i = 0; i < asd.size(); ++i) {
        creation_mod_date.append(to_string(asd[i]));
    }

    vector<int> sad;
    sad = decToBinary_2(ltm->tm_mon);
    for (int i = 0; i < 4-sad.size(); ++i) {
        creation_mod_date.append("0");
    }
    for (int i = 0; i < sad.size(); ++i) {
        creation_mod_date.append(to_string(sad[i]));
    }

    vector<int> dsa;
    dsa = decToBinary_2(ltm->tm_mday);
    for (int i = 0; i < 5-dsa.size(); ++i) {
        creation_mod_date.append("0");
    }
    for (int i = 0; i < dsa.size(); ++i) {
        creation_mod_date.append(to_string(dsa[i]));
    }
    char p[creation_mod_date.size()+1];
    for (int i = 0; i < creation_mod_date.size(); ++i) {
        p[i] = creation_mod_date[i];
    }
    p[creation_mod_date.size()] = '\0';
    uint16_t myInt16 = 0;
    for (int i = 0; i < 16; ++i) {
        if(p[15-i] == '1') myInt16 += pow(2,i);
    }
    //uint16_t myInt16 = atou16(p);

    /*int myInt(std::stoi(creation_mod_date));
    uint16_t myInt16(0);
    if (myInt <= static_cast<int>(UINT16_MAX) && myInt >=0) {
        myInt16 = static_cast<uint16_t>(myInt);
    }
    else {
        std::cout << "Error : Manage your error the way you want to\n";
    }*/

    string hour_minute_sec = "";

    vector<int> xyz;
    xyz = decToBinary_2(ltm->tm_hour);
    for (int i = 0; i < 5-xyz.size(); ++i) {
        hour_minute_sec.append("0");
    }
    for (int i = 0; i < xyz.size(); ++i) {
        hour_minute_sec.append(to_string(xyz[i]));
    }

    vector<int> yyy;
    yyy = decToBinary_2(ltm->tm_min);
    for (int i = 0; i < 6-yyy.size(); ++i) {
        hour_minute_sec.append("0");
    }
    for (int i = 0; i < yyy.size(); ++i) {
        hour_minute_sec.append(to_string(yyy[i]));
    }

    vector<int> zzz;
    zzz = decToBinary_2(ltm->tm_sec/2);
    for (int i = 0; i < 5-zzz.size(); ++i) {
        hour_minute_sec.append("0");
    }
    for (int i = 0; i < zzz.size(); ++i) {
        hour_minute_sec.append(to_string(zzz[i]));
    }

    char pi[hour_minute_sec.size()+1];
    for (int i = 0; i < hour_minute_sec.size(); ++i) {
        pi[i] = hour_minute_sec[i];
    }
    pi[hour_minute_sec.size()] = '\0';

    uint16_t myInt16_2 = 0;
    for (int i = 0; i < 16; ++i) {
        if(pi[15-i] == '1') myInt16_2 += pow(2,i);
    }


    /*int myIntt(std::stoi(hour_minute_sec));
    uint16_t myInt16_2(0);
    if (myIntt <= static_cast<int>(UINT16_MAX) && myInt >=0) {
        myInt16_2 = static_cast<uint16_t>(myIntt);
    }
    else {
        std::cout << "Error : Manage your error the way you want to\n";
    }*/




    FILE* disk = fopen(argv[1], "r+");
    if (!disk) {
        fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
        return ;
    }
    fread(&bpb, sizeof(bpb), 1, disk);
    FatFileEntry* fatFileEntry = new FatFileEntry;

    FatFileLFN* fatFileLfn = new FatFileLFN;
    int how_many_row = 0;
    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
    int for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
    for (int i = 0; i < for_count; ++i) {
        fread(fatFileLfn,32,1,disk);
        if(fatFileLfn->attributes == 0){
            break;
        }
        how_many_row++;
    }
    int how_many_LFN = (name.size() -1) / 13 +1;
    int name_index = 0;
    int lfn_loop_var = how_many_LFN-1;
    vector<char> tmp_name;
    for (int i = 0; i < name.size(); ++i) {
        tmp_name.push_back(name[i]);
    }
    tmp_name.push_back('\0');
    if((32-how_many_row)<how_many_LFN){
        if(type == 0){
            int new_clus_num = find_empty_cluster(sub_cluster_num,disk);
            fill_empty_cluster(myInt16,myInt16_2,sub_cluster_num,new_clus_num-2,disk);
            write_new_clust_num(sub_cluster_num,new_clus_num,disk);
            sub_cluster_num = new_clus_num-2;
        }
        how_many_row = 0;
    }
    for (int i = 0; i < how_many_LFN; ++i) {
        //FILE* disk = fopen(argv[1], "r+");
        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+(sub_cluster_num)*2)*bpb.BytesPerSector+(how_many_row+lfn_loop_var)*32, SEEK_SET);
        //fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
        FatFileLFN* write_to_file = new FatFileLFN;

        bool is_it_finished = false;
        for (int j = 0; j < 5; ++j) { // take the name1 parameters of the fatFileEntry
            write_to_file->name1[j] = tmp_name[name_index];
            if(name_index == tmp_name.size() -1){ // name1'yu çek
                is_it_finished = true;
                break;
            }
            name_index++;
        }

        if(is_it_finished == false){
            for (int j = 0; j < 6; ++j) { // take the name1 parameters of the fatFileEntry
                write_to_file->name2[j] = tmp_name[name_index];
                if(name_index == tmp_name.size() -1){ // name1'yu çek
                    is_it_finished = true;
                    break;
                }
                name_index++;
            }
        }

        if(is_it_finished == false){
            for (int j = 0; j < 2; ++j) { // take the name1 parameters of the fatFileEntry
                write_to_file->name3[j] = tmp_name[name_index];
                if(name_index == tmp_name.size() -1){ // name1'yu çek
                    is_it_finished = true;
                    break;
                }
                name_index++;
            }
        }
        write_to_file->reserved = 0;
        //if(i == 0) write_to_file->sequence_number = 0x40 + how_many_LFN;
        //else write_to_file->sequence_number = how_many_LFN - i;
        write_to_file->attributes = 15;
        lfn_loop_var--;
        fwrite(write_to_file,32,1,disk);




    }
    //disk = fopen(argv[1], "r+");
    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+(sub_cluster_num)*2)*bpb.BytesPerSector+(how_many_row+how_many_LFN)*32, SEEK_SET);
    FatFileEntry* fatFileEntry2 = new FatFileEntry;
    if(type == 0) fatFileEntry2->lfn.attributes = 16;
    else fatFileEntry2->lfn.attributes = 32;
    fatFileEntry2->msdos.modifiedDate = myInt16;
    fatFileEntry2->msdos.modifiedTime = myInt16_2;
    fatFileEntry2->msdos.creationTime = myInt16_2;
    fatFileEntry2->msdos.fileSize = 0;
    fatFileEntry2->msdos.creationTimeMs = 0;
    fatFileEntry2->msdos.lastAccessTime = myInt16_2;
    fatFileEntry2->msdos.creationDate = myInt16;
    if(type == 0){
        int new_clus_num = find_empty_cluster(sub_cluster_num,disk);
        fill_empty_cluster(myInt16,myInt16_2,sub_cluster_num,new_clus_num-2,disk);
        fatFileEntry2->msdos.firstCluster = new_clus_num;
    }
    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+(sub_cluster_num)*2)*bpb.BytesPerSector+(how_many_row+how_many_LFN)*32, SEEK_SET);
    fwrite(fatFileEntry2,32,1,disk);


    // -------------------------------------------------- this is test
    //disk = fopen(argv[1], "r+");

    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
    /*while(1){


        fread(fatFileEntry,32,1,disk);
    }*/



}



void fill_the_fat_vector(vector<int>* keep_fat_index, int sub_cluster_num, FILE* disk){
    //fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector, SEEK_SET);
    //keep_fat_index->push_back(sub_cluster_num);
    sub_cluster_num += 2;
    uint32_t* fat_entry;
    while(1){
        fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector+4*sub_cluster_num, SEEK_SET);
        fread(fat_entry,4,1,disk);
        *fat_entry &= 0x0FFFFFFF;
        if(*fat_entry == 268435448 || *fat_entry == 0){
            break;
        }
        sub_cluster_num = *fat_entry;
        keep_fat_index->push_back(sub_cluster_num-2);


    }
    keep_fat_index->push_back(-1);
}

void fill_the_fat_vector_for_spe(vector<int>* keep_fat_index, int sub_cluster_num, FILE* disk){
    //fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector, SEEK_SET);
    //keep_fat_index->push_back(sub_cluster_num);
    //sub_cluster_num += 2;
    uint32_t* fat_entry;
    while(1){
        fseek(disk, bpb.ReservedSectorCount*bpb.BytesPerSector+4*sub_cluster_num, SEEK_SET);
        fread(fat_entry,32,1,disk);
        *fat_entry &= 0x0FFFFFFF;
        if(*fat_entry == 268435448 || *fat_entry == 0){
            break;
        }
        sub_cluster_num = *fat_entry;
        keep_fat_index->push_back(sub_cluster_num-2);


    }
    keep_fat_index->push_back(-1);
}





int main(int argc, char** argv) {
    while (true){
        string input = "";
        print_the_path(path);
        bool is_it_quit = false;
        parsed_input *pars = new parsed_input;
        string third_param = "";
        pars = take_the_input(&is_it_quit,&input,&third_param);
        //if(is_it_quit == true) break;
        if(pars->type == QUIT){
            delete pars;
            return 0;
        }
        if(pars->type == CD){ // if the given commend is CD
            if(input == "/"){
                path.clear();
                continue;
            }
            int path_vec_size = path.size();
            vector<string> tmp;
            for (int i = 0; i < path_vec_size; ++i) { // take the backup of the path vector
                tmp.push_back(path[i]);
            }
            arrange_path(&path,input);

            bool i_am_done = false;
            int sub_cluster_num = 0;
            for (int i = 0; i < path.size(); ++i) { //search for the all parameters of the path one by one.
                FILE* disk = fopen(argv[1], "rb");
                if (!disk) {
                    fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
                    return -1;
                }

                fread(&bpb, sizeof(bpb), 1, disk);
                FatFileEntry* fatFileEntry = new FatFileEntry;
                FatFileLFN* fatFileLfn = new FatFileLFN;

                //fseek(disk,512,SEEK_SET);
                vector<char> name;
                int for_count = 0;
                vector<char> before_step_name;

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }

                vector<int> keep_fat_index;
                int vec_fat_index = 0;
                fill_the_fat_vector(&keep_fat_index, sub_cluster_num, disk);

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }


                bool is_it_finished = false;
                for (int j = 0; j < for_count; ++j) { // read the data
                    fread(fatFileLfn,32,1,disk);
                    //fread(fatFileEntry, 32, 1, disk);
                    if(is_it_finished == false){
                        for (int k = 0; k < 5; ++k) { // take the name1 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name1[k]);
                            if(fatFileLfn->name1[k] == 0){ // name1'yu çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 6; ++k) {  // take the name2 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name2[k]);
                            if(fatFileLfn->name2[k] == 0){ //name2'yi çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 2; ++k) { // take the name3 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name3[k]);
                            if(fatFileLfn->name3[k] == 0){   //name3'ü çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(name.size() == 13){ //check the vector is full or not
                        for (int k = 0; k < before_step_name.size(); ++k) {
                            is_it_finished = true;
                            name.push_back(before_step_name[k]);
                        }
                        is_it_finished = true;
                    }
                    else{
                        before_step_name.clear();
                    }
                    bool is_it_equal = true;
                    if( (is_it_finished == true && name.size()-1 == path[i].size())  || (path[i].size() % 13 == 0 && name.size() == path[i].size()) ){ // if the string is taken and the size of the string and the real string is matched
                        for (int k = 0; k < path[i].size(); ++k) {
                            if(path[i][k] != name[k]){
                                is_it_equal = false; //set flag to false to identify, the two string does not match
                                break;
                            }
                        }
                        if(is_it_equal == true){
                            if(i == path.size() -1){ //if the last searching path is found, we are done!
                                fread(fatFileEntry, 32, 1, disk);
                                if(fatFileEntry->msdos.attributes == 32) break;
                                i_am_done = true;
                            }
                            else{  //if the given parameter found and if still paramaters are waiting to be search, do some arrangements
                                fread(fatFileEntry, 32, 1, disk);
                                sub_cluster_num = fatFileEntry->msdos.firstCluster-2;
                                is_it_finished = false;
                                break;
                            }
                        }
                    }
                    else{ // it is finished and the size is not equal to each other that means thats two string does not match
                        is_it_equal = false;
                    }
                    if(is_it_finished == true && is_it_equal == false){ //if we take the all string and it does not macth, clear the vector and set the flag.
                        before_step_name = name;
                        name.clear();

                        if(fatFileLfn->attributes == 16) before_step_name.clear();
                        is_it_finished = false;
                    }
                    if(i_am_done == true){
                        break;
                    }
                    if(j == for_count-1 && keep_fat_index[vec_fat_index] != -1){
                        j = -1;
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[vec_fat_index]*2)*bpb.BytesPerSector, SEEK_SET);
                        vec_fat_index++;

                    }
                }



            }
            if(i_am_done == false && path.size() != 0){ //for bitip aradığım şeyi bulamadıysam wrong input almışımdır pathi eski path yap buna ek olarak eğer roota gittiysem güncellememek için bir size checkleyen and attım.
                path = tmp;
            }

        }

        else if(pars->type == LS){
            vector<string> tmp;
            for (int i = 0; i < path.size(); ++i) {
                tmp.push_back(path[i]);
            }
            if(1){ // if the command is ls without -l
                if(input != "" && third_param == ""){ //if the given command ls with path
                    int path_vec_size = path.size();
                    tmp.clear();
                    for (int i = 0; i < path_vec_size; ++i) { // take the backup of the path vector
                        tmp.push_back(path[i]);
                    }
                    arrange_path(&path,input);

                }
                else if(input != "" && third_param != ""){
                    int path_vec_size = path.size();
                    tmp.clear();
                    for (int i = 0; i < path_vec_size; ++i) { // take the backup of the path vector
                        tmp.push_back(path[i]);
                    }
                    arrange_path(&path,third_param);
                }
                bool is_it_printed_anything = false;
                bool i_am_done = false;
                int sub_cluster_num = 0;
                bool flag_for_exit = false;
                //if(path.size() != 0 && path[0] == "-l") path.clear();
                for (int i = 0; i < path.size()+1; ++i) { //search for the all parameters of the path one by one.
                    if(flag_for_exit == false && i != 0) break;
                    if(flag_for_exit == true) flag_for_exit = false;
                    FILE* disk = fopen(argv[1], "rb");
                    if (!disk) {
                        fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
                        return -1;
                    }

                    fread(&bpb, sizeof(bpb), 1, disk);
                    FatFileEntry* fatFileEntry = new FatFileEntry;


                    FatFileLFN* fatFileLfn = new FatFileLFN;

                    //fseek(disk,512,SEEK_SET);
                    vector<char> name;
                    int for_count = 0;
                    vector<char> before_step_name;

                    if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                        for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                    }
                    else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                        for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                    }

                    vector<int> keep_fat_index;
                    int vec_fat_index = 0;
                    fill_the_fat_vector(&keep_fat_index, sub_cluster_num, disk);

                    if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                        for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                    }
                    else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                        for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                    }

                    bool is_it_finished = false;
                    for (int j = 0; j < for_count; ++j) { // read the data
                        fread(fatFileLfn,32,1,disk);
                        //fread(fatFileEntry, 32, 1, disk);

                        if(is_it_finished == false){
                            for (int k = 0; k < 5; ++k) { // take the name1 parameters of the fatFileEntry
                                name.push_back(fatFileLfn->name1[k]);
                                if(fatFileLfn->name1[k] == 0){ // name1'yu çek
                                    is_it_finished = true;
                                    break;
                                }
                            }
                        }
                        if(is_it_finished == false){
                            for (int k = 0; k < 6; ++k) {  // take the name2 parameters of the fatFileEntry
                                name.push_back(fatFileLfn->name2[k]);
                                if(fatFileLfn->name2[k] == 0){ //name2'yi çek
                                    is_it_finished = true;
                                    break;
                                }
                            }
                        }
                        if(is_it_finished == false){
                            for (int k = 0; k < 2; ++k) { // take the name3 parameters of the fatFileEntry
                                name.push_back(fatFileLfn->name3[k]);
                                if(fatFileLfn->name3[k] == 0){   //name3'ü çek
                                    is_it_finished = true;
                                    break;
                                }
                            }
                        }
                        if(name.size() == 13){ //check the vector is full or not
                            for (int k = 0; k < before_step_name.size(); ++k) {
                                is_it_finished = true;
                                name.push_back(before_step_name[k]);
                            }
                            is_it_finished = true;
                        }
                        else{
                            if(before_step_name.size() != 0 && i == path.size() && (fatFileLfn->attributes == 16 || fatFileLfn->attributes == 32)){
                                if(input == "-l"){
                                    vector<int> month_day;
                                    vector<int> hour_minute;
                                    memcpy(fatFileEntry,fatFileLfn,32);
                                    if(fatFileEntry->msdos.attributes == 16){
                                        cout << "d";
                                    }
                                    else if(fatFileEntry->msdos.attributes == 32){
                                        cout << "-";
                                    }
                                    cout << "rwx------ 1 root root ";
                                    if(fatFileEntry->msdos.attributes == 16){
                                        cout << "0 ";
                                    }
                                    else if(fatFileEntry->msdos.attributes == 32){
                                        cout << fatFileEntry->msdos.fileSize << " ";
                                    }
                                    month_day = decToBinary(fatFileEntry->msdos.modifiedDate);
                                    string month = "";
                                    for (int k = 0; k < 4; ++k) {
                                        month.append(to_string(month_day[7+k]));
                                    }
                                    int month_num = binaryToDecimal(month);
                                    month = keep_months[month_num];
                                    cout << month << " ";
                                    string day = "";
                                    for (int k = 0; k < 5; ++k) {
                                        day.append(to_string(month_day[11+k]));
                                    }
                                    int day_num = binaryToDecimal(day);
                                    cout << day_num << " ";

                                    hour_minute = decToBinary(fatFileEntry->msdos.modifiedTime);
                                    string hour = "";
                                    for (int k = 0; k < 5; ++k) {
                                        hour.append(to_string(hour_minute[k]));
                                    }
                                    int hour_num = binaryToDecimal(hour);
                                    if(hour_num < 10) cout << "0" << hour_num << ":";
                                    else cout << hour_num << ":";

                                    string minute = "";
                                    for (int k = 0; k < 6; ++k) {
                                        minute.append(to_string(hour_minute[5+k]));
                                    }
                                    int min_num = binaryToDecimal(minute);
                                    if(min_num < 10) cout << "0" << min_num << " ";
                                    else cout << min_num << " ";
                                    for (int k = 0; k < before_step_name.size()-1; ++k) {
                                        cout << before_step_name[k];
                                    }
                                    is_it_printed_anything = true;
                                    if(before_step_name.size() != 0 && before_step_name.size() % 13 == 0) cout << before_step_name[before_step_name.size()-1];
                                    cout << " ";


                                    cout << endl;
                                }
                                else{
                                    for (int k = 0; k < before_step_name.size()-1; ++k) {
                                        cout << before_step_name[k];
                                    }
                                    is_it_printed_anything = true;
                                    if(before_step_name.size() != 0 && before_step_name.size() % 13 == 0) cout << before_step_name[before_step_name.size()-1];
                                    cout << " ";
                                }

                            }
                            before_step_name.clear();
                        }
                        bool is_it_equal = true;
                        if(i == path.size()){ //this for not searching just for ls part to print out everything
                            before_step_name = name;
                            name.clear();
                            if(fatFileLfn->attributes == 16 || fatFileLfn->attributes == 32) before_step_name.clear();
                            is_it_finished = false;
                            if(j == for_count-1 && keep_fat_index[vec_fat_index] != -1){
                                j = -1;
                                fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[vec_fat_index]*2)*bpb.BytesPerSector, SEEK_SET);
                                vec_fat_index++;

                            }
                            continue;
                        }
                        // this is the part for going to correct place
                        if(path.size() != 0 && ((is_it_finished == true && name.size()-1 == path[i].size()) || (path[i].size() % 13 == 0 && name.size() == path[i].size()))){ // if the string is taken and the size of the string and the real string is matched
                            for (int k = 0; k < path[i].size(); ++k) {
                                if(path[i][k] != name[k]){
                                    is_it_equal = false; //set flag to false to identify, the two string does not match
                                    break;
                                }
                            }
                            if(is_it_equal == true){
                                if(i == path.size()){ //if the last searching path is found, we are done!
                                    i_am_done = true;
                                }
                                else{  //if the given parameter found and if still paramaters are waiting to be search, do some arrangements
                                    fread(fatFileEntry, 32, 1, disk);
                                    sub_cluster_num = fatFileEntry->msdos.firstCluster-2;
                                    is_it_finished = false;
                                    flag_for_exit = true; // this is flag check for the input which the folder does not contain
                                    break;
                                }
                            }
                        }
                        else{ // it is finished and the size is not equal to each other that means thats two string does not match
                            is_it_equal = false;
                        }
                        if(is_it_finished == true && is_it_equal == false){ //if we take the all string and it does not macth, clear the vector and set the flag.
                            before_step_name = name;
                            name.clear();
                            if(fatFileLfn->attributes == 16 || fatFileLfn->attributes == 32) before_step_name.clear();
                            is_it_finished = false;
                        }
                        if(i_am_done == true){
                            break;
                        }
                        if(j == for_count-1 && keep_fat_index[vec_fat_index] != -1){
                            j = -1;
                            fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[vec_fat_index]*2)*bpb.BytesPerSector, SEEK_SET);
                            vec_fat_index++;

                        }
                    }



                }




                path = tmp;
                if(is_it_printed_anything == true && input != "-l") cout << endl;

            }
        }
        else if(pars->type == MKDIR || pars->type == TOUCH){
            int path_vec_size = path.size();
            vector<string> tmp;
            for (int i = 0; i < path_vec_size; ++i) { // take the backup of the path vector
                tmp.push_back(path[i]);
            }
            arrange_path(&path,input);
            string name_to_be_created = path[path.size()-1];
            remove_last_mem(&path);


            bool i_am_done = false;
            int sub_cluster_num = 0;
            for (int i = 0; i < path.size(); ++i) { //search for the all parameters of the path one by one.
                FILE* disk = fopen(argv[1], "rb");
                if (!disk) {
                    fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
                    return -1;
                }

                fread(&bpb, sizeof(bpb), 1, disk);
                FatFileEntry* fatFileEntry = new FatFileEntry;
                FatFileLFN* fatFileLfn = new FatFileLFN;

                //fseek(disk,512,SEEK_SET);
                vector<char> name;
                int for_count = 0;
                vector<char> before_step_name;

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }

                vector<int> keep_fat_index;
                int vec_fat_index = 0;
                fill_the_fat_vector(&keep_fat_index, sub_cluster_num, disk);

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }

                bool is_it_finished = false;
                for (int j = 0; j < for_count; ++j) { // read the data
                    fread(fatFileLfn,32,1,disk);
                    //fread(fatFileEntry, 32, 1, disk);

                    if(is_it_finished == false){
                        for (int k = 0; k < 5; ++k) { // take the name1 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name1[k]);
                            if(fatFileLfn->name1[k] == 0){ // name1'yu çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 6; ++k) {  // take the name2 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name2[k]);
                            if(fatFileLfn->name2[k] == 0){ //name2'yi çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 2; ++k) { // take the name3 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name3[k]);
                            if(fatFileLfn->name3[k] == 0){   //name3'ü çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(name.size() == 13){ //check the vector is full or not
                        for (int k = 0; k < before_step_name.size(); ++k) {
                            is_it_finished = true;
                            name.push_back(before_step_name[k]);
                        }
                        is_it_finished = true;
                    }
                    else{
                        before_step_name.clear();
                    }
                    bool is_it_equal = true;
                    if((is_it_finished == true && name.size()-1 == path[i].size())  || (path[i].size() % 13 == 0 && name.size() == path[i].size())){ // if the string is taken and the size of the string and the real string is matched
                        for (int k = 0; k < path[i].size(); ++k) {
                            if(path[i][k] != name[k]){
                                is_it_equal = false; //set flag to false to identify, the two string does not match
                                break;
                            }
                        }
                        if(is_it_equal == true){
                            if(i == path.size() -1){ //if the last searching path is found, we are done!
                                i_am_done = true;
                            }
                            else{  //if the given parameter found and if still paramaters are waiting to be search, do some arrangements
                                fread(fatFileEntry, 32, 1, disk);
                                sub_cluster_num = fatFileEntry->msdos.firstCluster-2;
                                is_it_finished = false;
                                break;
                            }
                        }
                    }
                    else{ // it is finished and the size is not equal to each other that means thats two string does not match
                        is_it_equal = false;
                    }
                    if(is_it_finished == true && is_it_equal == false){ //if we take the all string and it does not macth, clear the vector and set the flag.
                        before_step_name = name;
                        name.clear();
                        if(fatFileLfn->attributes == 16) before_step_name.clear();
                        is_it_finished = false;
                    }
                    if(i_am_done == true){
                        fread(fatFileEntry, 32, 1, disk);
                        sub_cluster_num = fatFileEntry->msdos.firstCluster-2;
                        if(pars->type == MKDIR) write_to_the_file(sub_cluster_num,argv,name_to_be_created,0);
                        else write_to_the_file(sub_cluster_num,argv,name_to_be_created,1);
                        break;
                    }
                    if(j == for_count-1 && keep_fat_index[vec_fat_index] != -1){
                        j = -1;
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[vec_fat_index]*2)*bpb.BytesPerSector, SEEK_SET);
                        vec_fat_index++;

                    }
                }



            }
            if(path.size() == 0){
                FILE* disk = fopen(argv[1], "rb");
                if (!disk) {
                    fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
                    return -1;
                }

                fread(&bpb, sizeof(bpb), 1, disk);
                fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);



                vector<int> keep_fat_index;
                fill_the_fat_vector(&keep_fat_index, sub_cluster_num, disk);
                fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[keep_fat_index.size()-2]*2)*bpb.BytesPerSector, SEEK_SET);
                fclose(disk);
                if(keep_fat_index.size() >1){
                    if(pars->type == MKDIR) write_to_the_file(sub_cluster_num,argv,name_to_be_created,0);
                    else write_to_the_file(sub_cluster_num,argv,name_to_be_created,1);
                }
                else{
                    if(pars->type == MKDIR) write_to_the_file(sub_cluster_num,argv,name_to_be_created,0);
                    else write_to_the_file(sub_cluster_num,argv,name_to_be_created,1);
                }
            }
                path = tmp;


        }


        else if(pars->type == CAT){

            int path_vec_size = path.size();
            vector<string> tmp;
            for (int i = 0; i < path_vec_size; ++i) { // take the backup of the path vector
                tmp.push_back(path[i]);
            }
            arrange_path(&path,input);

            bool i_am_done = false;
            int sub_cluster_num = 0;
            for (int i = 0; i < path.size()+1; ++i) { //search for the all parameters of the path one by one.
                FILE* disk = fopen(argv[1], "rb");
                if (!disk) {
                    fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
                    return -1;
                }

                fread(&bpb, sizeof(bpb), 1, disk);
                FatFileEntry* fatFileEntry = new FatFileEntry;
                FatFileLFN* fatFileLfn = new FatFileLFN;

                //fseek(disk,512,SEEK_SET);
                vector<char> name;
                int for_count = 0;
                vector<char> before_step_name;

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }

                vector<int> keep_fat_index;
                int vec_fat_index = 0;
                fill_the_fat_vector(&keep_fat_index, sub_cluster_num, disk);

                if(i == 0){ //if the first parameter is searching, we have to seach from the start of the given data
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                else{ //if the not first parameter is searching, we have do same arrangements in order to search efficiently.
                    fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+sub_cluster_num*2)*bpb.BytesPerSector, SEEK_SET);
                    for_count = bpb.BytesPerSector * bpb.SectorsPerCluster/32;
                }
                bool is_it_print_any = false;
                if(i == path.size() && i_am_done == true){
                    vector<int> fat_vec;
                    fat_vec.push_back(sub_cluster_num);
                    fill_the_fat_vector_for_spe(&fat_vec, sub_cluster_num, disk);
                    for (int j = 0; j < fat_vec.size()-1; ++j) {
                        char k[1024];
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+fat_vec[j]*2)*bpb.BytesPerSector, SEEK_SET);
                        fread(k,1023,1,disk);
                        k[1023] = '\0';
                        for (int l = 0; l < 1024; ++l) {
                            if(k[l] != '\0') is_it_print_any = true;
                            cout << k[l];
                        }
                    }
                    if(is_it_print_any == true) cout << endl;
                    continue;

                }

                bool is_it_finished = false;
                for (int j = 0; j < for_count; ++j) { // read the data
                    fread(fatFileLfn,32,1,disk);
                    //fread(fatFileEntry, 32, 1, disk);
                    if(is_it_finished == false){
                        for (int k = 0; k < 5; ++k) { // take the name1 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name1[k]);
                            if(fatFileLfn->name1[k] == 0){ // name1'yu çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 6; ++k) {  // take the name2 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name2[k]);
                            if(fatFileLfn->name2[k] == 0){ //name2'yi çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(is_it_finished == false){
                        for (int k = 0; k < 2; ++k) { // take the name3 parameters of the fatFileEntry
                            name.push_back(fatFileLfn->name3[k]);
                            if(fatFileLfn->name3[k] == 0){   //name3'ü çek
                                is_it_finished = true;
                                break;
                            }
                        }
                    }
                    if(name.size() == 13){ //check the vector is full or not
                        for (int k = 0; k < before_step_name.size(); ++k) {
                            is_it_finished = true;
                            name.push_back(before_step_name[k]);
                        }
                        is_it_finished = true;
                    }
                    else{
                        before_step_name.clear();
                    }
                    bool is_it_equal = true;
                    if((is_it_finished == true && name.size()-1 == path[i].size())  || (path[i].size() % 13 == 0 && name.size() == path[i].size())){ // if the string is taken and the size of the string and the real string is matched
                        for (int k = 0; k < path[i].size(); ++k) {
                            if(path[i][k] != name[k]){
                                is_it_equal = false; //set flag to false to identify, the two string does not match
                                break;
                            }
                        }
                        if(is_it_equal == true){
                            if(i == path.size() -1){ //if the last searching path is found, we are done!
                                sub_cluster_num = fatFileEntry->msdos.firstCluster;
                                i_am_done = true;
                            }
                            else{  //if the given parameter found and if still paramaters are waiting to be search, do some arrangements
                                fread(fatFileEntry, 32, 1, disk);
                                sub_cluster_num = fatFileEntry->msdos.firstCluster-2;
                                is_it_finished = false;
                                break;
                            }
                        }
                    }
                    else{ // it is finished and the size is not equal to each other that means thats two string does not match
                        is_it_equal = false;
                    }
                    if(is_it_finished == true && is_it_equal == false){ //if we take the all string and it does not macth, clear the vector and set the flag.
                        before_step_name = name;
                        name.clear();

                        if(fatFileLfn->attributes == 16) before_step_name.clear();
                        is_it_finished = false;
                    }
                    if(i_am_done == true){
                        fread(fatFileLfn,32,1,disk);
                        sub_cluster_num = fatFileLfn->firstCluster;
                        break;
                    }
                    if(j == for_count-1 && keep_fat_index[vec_fat_index] != -1){
                        j = -1;
                        fseek(disk, (bpb.ReservedSectorCount + bpb.NumFATs*bpb.extended.FATSize+keep_fat_index[vec_fat_index]*2)*bpb.BytesPerSector, SEEK_SET);
                        vec_fat_index++;

                    }
                }



            }
                path = tmp;


        }











        delete pars;
    }
    return 0;
}