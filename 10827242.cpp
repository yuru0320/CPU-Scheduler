//
//  main.cpp
//  proj2
//
//  Created by Ruby on 2022/5/11.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include<sys/time.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <spawn.h>
# include <stdio.h>
# include <stdlib.h>
#include <string>


using namespace std;
typedef struct processData {
    int id;
    int cpuBurst;
    int ArrivalTime;
    int priority;
    int endT;
    int burst;
    bool last;
} pData;

typedef struct id_Time {
    int id;
    int time;
} idT;

typedef struct finalData {
    vector<string> outdata;
    vector<idT> waiting;
    vector<idT> turnaround;
    int method ;//1:FCFS 2:RR 3:SRTF 4:PPRR 5:HRRN
} fData;


string filename;
int method, time_slice, now_time =0 ;
vector<pData> ALLprocess;
vector<fData> outItem;

void readFile(){
    //init();
    ifstream txtfile;
    string dataLine;
    pData Ptemp;
    int num, count =1;
    string temp;
    cout << "輸入檔案名稱: "<<endl;
    cin >> filename;
    //filename += ".txt";
    txtfile.open(filename+ ".txt");
    while (!txtfile.is_open()) {
        cout << "Failed to open file.\n";
        cout << "輸入檔案名稱: "<<endl;
        cin >> filename;
        txtfile.open(filename + ".txt");
    }
    
    txtfile >> method >> time_slice;
    for(int i = 0 ; i < 6 ; i ++ ){
        txtfile >>temp;
    }
    
    while(txtfile >> num){
        
        if(count == 1){
            Ptemp.id = num;
        }
        else if(count == 2){
            Ptemp.cpuBurst = num;
            Ptemp.burst = num; // 不更改
        }
        else if(count == 3){
            Ptemp.ArrivalTime = num;
        }
        else if(count == 4){
            Ptemp.priority = num;
            ALLprocess.push_back(Ptemp);
            count = 0 ;
        }
        count ++;
    }
    
}
bool biggerID(idT p1 , idT p2) {
    return p1.id < p2.id;
}

bool biggerAT(pData p1 , pData p2) { //ok
    if(p1.ArrivalTime == p2.ArrivalTime){
        return p1.id < p2.id;
    }
    return p1.ArrivalTime < p2.ArrivalTime;
}

bool biggerPRI(pData p1 , pData p2) {
    return p1.priority < p2.priority;
}

bool biggerCPU(pData p1 , pData p2) {
    if(p1.cpuBurst == p2.cpuBurst){
        if(p1.ArrivalTime == p2.ArrivalTime){
            return p1.id < p2.id;
        }
        else return p1.ArrivalTime < p2.ArrivalTime;
    }
    return p1.cpuBurst < p2.cpuBurst;
}

bool biggerRR(pData p1 , pData p2) {
    float RR1 = 0, RR2 = 0;
    RR1 = (float)(now_time - p1.ArrivalTime + p1.burst)/ p1.burst;
    RR2 = (float)(now_time - p2.ArrivalTime + p2.burst)/ p2.burst;
    if(RR1 == RR2){
        if(p1.ArrivalTime == p2.ArrivalTime){
            return p1.id < p2.id;
        }
        else return p1.ArrivalTime < p2.ArrivalTime;
    }
    return RR1 > RR2;
}

string intTOstring(int num ){
    int ascii = 10;
    string s;
    if(num >= 10){
        ascii = num - ascii + 65;
        s += char(ascii);
    }
    else s = to_string(num);
    return s;
}

void FCFS(){
    vector<string> temp;
    vector<pData> queue, temProcess;
    fData ftemp;
    idT idtimeItem;
    ftemp.method = 1;
    sort(ALLprocess.begin(), ALLprocess.end(), biggerAT );
    temProcess = ALLprocess;
    while(1){
        for(int i = 0 ; i < temProcess.size() ; i++){
            if(now_time >= temProcess[0].ArrivalTime){
                //push = true;
                queue.push_back(temProcess[0]);//先放新來的
                temProcess.erase(temProcess.begin()); //刪掉放完的
                
            }
            else{
                break;
            }

        }
        if(queue.size() == 0 ){
            ftemp.outdata.push_back("-");
            now_time++;
        }
        else{
            sort(queue.begin(), queue.end(), biggerAT);
            for(int j =0 ; j < queue[0].burst ; j++){
                string ID = intTOstring(queue[0].id);
                ftemp.outdata.push_back(ID); //放到甘特圖
            }
            now_time = now_time + queue[0].burst;
            idtimeItem.id = queue[0].id;
            idtimeItem.time = now_time - queue[0].ArrivalTime;
            ftemp.turnaround.push_back(idtimeItem);
            idtimeItem.time = now_time - queue[0].ArrivalTime- queue[0].burst;
            idtimeItem.id = queue[0].id;
            ftemp.waiting.push_back(idtimeItem);
            queue.erase(queue.begin());
            
        }
        if(queue.size()==0 && temProcess.size()==0) break;
        
    }
    outItem.push_back(ftemp);
    
}

void RR(){
    now_time =0;
    vector<pData> queue, temProcess;
    pData qtemp;
    fData ftemp;
    idT idtimeItem;
    int idle;
    bool needToPush = false, push = false;
    int remainingT =0 ;
    sort(ALLprocess.begin(), ALLprocess.end(), biggerAT );
    idle = ALLprocess[0].ArrivalTime - 0 ;
    now_time+=idle;
    for(int i=0; i < idle ; i++){
        ftemp.outdata.push_back("-");
    }
    
    temProcess = ALLprocess;
    while(1){
        for(int i = 0 ; i < temProcess.size() ; i++){
            if(now_time >= temProcess[0].ArrivalTime){
                push = true;
                queue.push_back(temProcess[0]);//先放新來的
                temProcess.erase(temProcess.begin()); //刪掉放完的
                
            }
            else{
                break;
            }

        }
        if(needToPush) {
            queue.push_back(qtemp); //上一個再放沒做完的
            needToPush = false;
        }

        if(queue.size() == 0 ){
            ftemp.outdata.push_back("-");
            now_time++;
        }
        else{
            if(queue[0].cpuBurst <= time_slice){
                if(queue[0].cpuBurst == time_slice) {
                    remainingT = time_slice;
                }
                else remainingT = queue[0].cpuBurst;
                now_time = now_time + remainingT;
                idtimeItem.id = queue[0].id;
                idtimeItem.time = now_time - queue[0].ArrivalTime;
                ftemp.turnaround.push_back(idtimeItem);
                idtimeItem.time = now_time - queue[0].ArrivalTime- queue[0].burst;
                idtimeItem.id = queue[0].id;
                ftemp.waiting.push_back(idtimeItem);
                for(int j = 0 ; j < remainingT; j++){
                    string ID = intTOstring(queue[0].id);
                    ftemp.outdata.push_back(ID); //放到甘特圖
                }
                queue.erase(queue.begin());
                
            }
            else{
                queue[0].cpuBurst -= time_slice ;
                qtemp = queue[0];
                for(int j = 0 ; j < time_slice; j++){
                    string ID = intTOstring(queue[0].id);
                    ftemp.outdata.push_back(ID); //放到甘特圖
                }
                queue.erase(queue.begin());
                now_time += time_slice;
                needToPush = true;
            }
            
        }
        if(queue.size()==0 && temProcess.size()==0) break;
    }
    outItem.push_back(ftemp);
//    for(int j =0 ; j < outItem[0].outdata.size() ;j++){
//        cout <<outItem[0].outdata[j];
//
//    }
}

void SRTF(){
    now_time =0;
    vector<pData> queue, temProcess;
    pData qtemp;
    fData ftemp;
    idT idtimeItem;
    int idle;
    sort(ALLprocess.begin(), ALLprocess.end(), biggerAT );
    idle = ALLprocess[0].ArrivalTime - 0 ;
    now_time+=idle;
    for(int i=0; i < idle ; i++){
        ftemp.outdata.push_back("-");
    }
    temProcess = ALLprocess;
    while(1){
        for(int i = 0 ; i < temProcess.size() ; i++){
            if(now_time >= temProcess[0].ArrivalTime){
                queue.push_back(temProcess[0]);//先放新來的
                temProcess.erase(temProcess.begin()); //刪掉放完的
            }
            else break;
        }
        if(queue.size() == 0 ){
            ftemp.outdata.push_back("-");
            now_time++;
        }
        else{
            sort(queue.begin(), queue.end(), biggerCPU );
            if(queue[0].cpuBurst == 1){//即將做完
                now_time ++;
                idtimeItem.id = queue[0].id;
                idtimeItem.time = now_time  - queue[0].ArrivalTime;
                ftemp.turnaround.push_back(idtimeItem);
                idtimeItem.time = now_time - queue[0].ArrivalTime- queue[0].burst;
                idtimeItem.id = queue[0].id;
                ftemp.waiting.push_back(idtimeItem);
                string ID = intTOstring(queue[0].id);
                ftemp.outdata.push_back(ID); //放到甘特圖
                queue.erase(queue.begin());
                
            }
            else{
                queue[0].cpuBurst -- ;
                qtemp = queue[0];
                string ID = intTOstring(queue[0].id);
                ftemp.outdata.push_back(ID); //放到甘特圖
                queue.erase(queue.begin());
                queue.push_back(qtemp);
                now_time ++;
            }
            
        }
        if(queue.size()==0 && temProcess.size()==0) break;
        
    }
    outItem.push_back(ftemp);
    
}


void PPRR(){
    now_time =0;
    vector<pData> queue, temProcess, RRqueue, RR2queue;
    fData ftemp;
    pData qtemp,q0;
    idT idtimeItem;
    int idle;
    bool needToPush = false, needToPush2 = false, doingRR = false, notEnough = false, notEnough2 = false;
    sort(ALLprocess.begin(), ALLprocess.end(), biggerAT );
    idle = ALLprocess[0].ArrivalTime - 0 ;
    now_time+=idle;
    for(int i=0; i < idle ; i++){
        ftemp.outdata.push_back("-");
    }
    temProcess = ALLprocess;
    while(1){
        for(int i = 0 ; i < temProcess.size() ; i++){
            
            if(now_time >= temProcess[0].ArrivalTime){
                queue.push_back(temProcess[0]);//先放新來的
                temProcess.erase(temProcess.begin()); //刪掉放完的
            }
            else break;
        }
        
        if(needToPush2) {
            queue.push_back(qtemp); //上一個再放沒做完的
            needToPush2 = false;
        }
        
        if(queue.size() == 0 && RRqueue.size() == 0){
            ftemp.outdata.push_back("-");
            now_time++;
        }
        else{
            if(queue[0].last) {
                queue[0].last =false;
                queue.push_back(queue[0]);
                queue.erase(queue.begin());
            }
            sort(queue.begin(), queue.end(), biggerPRI );
            if(RRqueue.size()>0){
                for(int j = 0; j < RRqueue.size(); j++){
                    // 將all相同priority的process加入RRqueue
                    queue.push_back(RRqueue[j]);
                    //將所有RRqueue放回queue
                }
                RRqueue.clear();
            }
            if(needToPush) {
                queue.push_back(qtemp); //上一個再放沒做完的
                needToPush = false;
            }
            sort(queue.begin(), queue.end(), biggerPRI );
            if(queue.size() >1 && queue[0].priority == queue[1].priority  ){ //多個priority相同的process do Run Robin
                if(RRqueue.size() == 0) doingRR = false;
                q0 =queue[0];
                for(int j = 0; j < queue.size() && !doingRR ; j++){ // 將all相同priority的process加入RRqueue
                    if(queue[j].priority == q0.priority){
                        RRqueue.push_back(queue[j]);
                        queue.erase(queue.begin());
                        j=-1;
                    }
                }
                
                // 開始做RR
                doingRR = true;
                for(int k = 0 ; k < time_slice ; k++){
                    
                    if(now_time >= temProcess[0].ArrivalTime && temProcess[0].priority < RRqueue[0].priority && temProcess.size()>0 && RRqueue.size() >0) {
                        break;
                    }
                    else{
                        now_time++;
                        string ID = intTOstring(RRqueue[0].id);
                        ftemp.outdata.push_back(ID); //放到甘特圖
                        if(RRqueue[0].cpuBurst == 1){
                            idtimeItem.id = RRqueue[0].id;
                            idtimeItem.time = now_time - RRqueue[0].ArrivalTime;
                            ftemp.turnaround.push_back(idtimeItem);
                            idtimeItem.time = now_time - RRqueue[0].ArrivalTime- RRqueue[0].burst;
                            idtimeItem.id = RRqueue[0].id;
                            ftemp.waiting.push_back(idtimeItem);
                            RRqueue.erase(RRqueue.begin());
                            notEnough =false;
                            break;
                        }
                        else if(RRqueue[0].cpuBurst > 1){
                            notEnough =true;
                            RRqueue[0].cpuBurst -- ;
                            qtemp = RRqueue[0];
                        }
                    }
                                
                }
                if(notEnough) {
                    RRqueue.erase(RRqueue.begin());
                    if(RRqueue.size() == 0 ){
                        qtemp.last =true;
                        queue.push_back(qtemp);
                        needToPush = false;
                    }
                    else needToPush = true;
                    notEnough = false;
                }
            }
            else{ // 目前只有一個最小priority process要做
                                
                for(int k = 0 ; k < time_slice ; k++){
                    
                    if( temProcess.size() > 0 && queue.size() > 0 && now_time >= temProcess[0].ArrivalTime && temProcess[0].priority < queue[0].priority) {
                        break;
                    }
                    else{
                        now_time++;
                        string ID = intTOstring(queue[0].id);
                        ftemp.outdata.push_back(ID); //放到甘特圖
                        if(queue[0].cpuBurst == 1){
                            idtimeItem.id = queue[0].id;
                            idtimeItem.time = now_time - queue[0].ArrivalTime;
                            ftemp.turnaround.push_back(idtimeItem);
                            idtimeItem.time = now_time - queue[0].ArrivalTime- queue[0].burst;
                            idtimeItem.id = queue[0].id;
                            ftemp.waiting.push_back(idtimeItem);
                            queue.erase(queue.begin());
                            notEnough2 =false;
                            break;
                        }
                        else if(queue[0].cpuBurst > 1){
                            notEnough2 =true;
                            queue[0].cpuBurst -- ;
                            qtemp = queue[0];
                        }
                    }
                }
                
                if(notEnough2) {
                    queue.erase(queue.begin());
                    if(queue.size() == 0 ){
                        qtemp.last =true;
                        queue.push_back(qtemp);
                        needToPush2 = false;
                    }
                    else needToPush2 = true;
                    notEnough2 = false;
                }
            }
        }
        if(queue.size()==0 && temProcess.size()==0 && RRqueue.size() == 0 ) {
            break;
            
        }
    }
    outItem.push_back(ftemp);
    
}

void HRRN(){
    now_time =0;
    vector<pData> queue, temProcess;
    fData ftemp;
    idT idtimeItem;
    int idle;
    sort(ALLprocess.begin(), ALLprocess.end(), biggerAT );
    idle = ALLprocess[0].ArrivalTime - 0 ;
    now_time+=idle;
    for(int i=0; i < idle ; i++){
        ftemp.outdata.push_back("-");
    }
    temProcess = ALLprocess;
    while(1){
        for(int i = 0 ; i < temProcess.size() ; i++){
            if(now_time >= temProcess[0].ArrivalTime){
                queue.push_back(temProcess[0]);//先放新來的
                temProcess.erase(temProcess.begin()); //刪掉放完的
                
            }
            else break;
        }
        if(queue.size() == 0 ){
            ftemp.outdata.push_back("-");
            now_time++;
        }
        else{
            sort(queue.begin(), queue.end(), biggerRR );
            for(int j =0 ; j < queue[0].cpuBurst ; j++ ){
                string ID = intTOstring(queue[0].id);
                ftemp.outdata.push_back(ID); //放到甘特圖
            }
            
            now_time = now_time + queue[0].burst;
            idtimeItem.id = queue[0].id;
            idtimeItem.time = now_time - queue[0].ArrivalTime;
            ftemp.turnaround.push_back(idtimeItem);
            idtimeItem.time = now_time - queue[0].ArrivalTime- queue[0].burst;
            idtimeItem.id = queue[0].id;
            ftemp.waiting.push_back(idtimeItem);
            queue.erase(queue.begin());
        }
        if(queue.size()==0 && temProcess.size()==0) break;
        
    }
    outItem.push_back(ftemp);
    
}

void writeFile(string method_Name){
    ofstream outFile;
    outFile.open("out_"+filename+".txt");
    // 輸出甘特圖
    outFile << method_Name << "\r\n";
    for(int i = 0 ; i < outItem.size(); i ++ ){
        if(outItem.size() > 1 ){
            if(i == 0 ) outFile << "==        FCFS==\r\n";
            else if(i == 1) outFile << "==          RR==\r\n";
            else if(i == 2) outFile << "==        SRTF==\r\n";
            else if(i == 3) outFile << "==        PPRR==\r\n";
            else if(i == 4) outFile << "==        HRRN==\r\n";
        }
        else if(method_Name == "Priority RR") outFile << "==        "<< "PPRR" <<"==\r\n";
        else outFile << "==        "<< method_Name <<"==\r\n";
        for(int j = 0 ; j < outItem[i].outdata.size(); j++ ){
            outFile << outItem[i].outdata[j] ;
        }
        outFile << "\r\n" ;
    }
    outFile   << "===========================================================\r\n";
    //waiting time
    
    if(outItem.size() > 1 )outFile << "\r\n" << "Waiting Time\r\nID\tFCFS\tRR\tSRTF\tPPRR\tHRRN\r\n";
    else if(method_Name == "Priority RR") outFile << "\r\n" << "Waiting Time\r\nID\t" << "PPRR"<<"\r\n";
    else outFile << "\r\n" << "Waiting Time\r\nID\t" << method_Name<<"\r\n";
    outFile   << "===========================================================\r\n";
    //outFile   << "===========================================================\r\n";
    for(int i = 0 ; i < outItem.size(); i++ ){
        sort(outItem[i].waiting.begin(), outItem[i].waiting.end(), biggerID );
        sort(outItem[i].turnaround.begin(), outItem[i].turnaround.end(), biggerID );
    }
    for(int j = 0 ; j <outItem[0].waiting.size() ; j++ ){
        outFile << outItem[0].waiting[j].id<< "\t";
        for(int i = 0 ; i < outItem.size(); i++ ){
            outFile << outItem[i].waiting[j].time;
            if(i != outItem.size()-1 )outFile <<"\t";
        }
        outFile << "\r\n" ;
    }
    outFile   << "===========================================================\r\n";
    if(outItem.size() > 1 )outFile << "\r\n" << "Turnaround Time\r\nID\tFCFS\tRR\tSRTF\tPPRR\tHRRN\r\n";
    else if(method_Name == "Priority RR" ) outFile << "\r\n" << "Turnaround Time\r\nID\t" << "PPRR"<<"\r\n";
    else outFile << "\r\n" << "Turnaround Time\r\nID\t" << method_Name<<"\r\n";
    outFile   << "===========================================================\r\n";
    for(int j = 0 ; j <outItem[0].turnaround.size() ; j++ ){
        outFile << outItem[0].turnaround[j].id<< "\t";
        for(int i = 0 ; i < outItem.size(); i++ ){
            outFile << outItem[i].turnaround[j].time;
            if(i != outItem.size()-1 )outFile <<"\t";
        }
        outFile << "\r\n" ;
    }
    outFile   << "===========================================================\r\n\r\n";
    outFile.close();
}


 int main(int argc, const char * argv[]) {
    // insert code here...
    cout<<"**********************\n";
    cout<<"*1. FCFS             *\n";
    cout<<"*2. RR               *\n";
    cout<<"*3. SRTF             *\n";
    cout<<"*4. PPRR             *\n";
    cout<<"*5. HRRN             *\n";
    cout<<"*6. ALL              *\n";
    cout<<"**********************\n";
    readFile();
    if(method == 1){ // FCFS
        FCFS();
        writeFile("FCFS");
    }
    else if(method == 2){
        RR();
        writeFile("RR");
    }
    else if(method == 3){
        SRTF();
        writeFile("SRTF");
    }
    else if(method == 4){
        PPRR();
        writeFile("Priority RR");
    }
    else if(method == 5){
        HRRN();
        writeFile("HRRN");
    }
    else if(method == 6){
        //string m_name = All;
        FCFS();
        RR();
        SRTF();
        PPRR();
        HRRN();
        writeFile("All");
    }
    else {
        cout << "Method ERROR !\n";
    }
   
}
