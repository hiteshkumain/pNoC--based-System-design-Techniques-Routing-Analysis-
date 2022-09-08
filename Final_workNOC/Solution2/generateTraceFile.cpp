// Generate Trace file by taking commGraph.txt as an input and generate trace accordingly
// INPUT File = CommGraph.txt
// Output File = Trace.txt
// TRACE = t, Source,Destination , pkt

#include <iostream>
#include <fstream>
#include <string>
int M;  // topology M*N 
int N;  


int MAT[128][128]={0};
bool MATbool[128][128]={false};

using namespace std;

void createTrace(string);

void printMat(int);

int main(int arg, char ** args)
{
    string file_name;    
    if(arg <2)
    {
        file_name="trace.txt"; // Default File Name 
    }
    else
    {
        file_name=args[1];
    }

    ifstream ff;
    string str;
    
    ff.open("CommGraph.txt");
    ff.ignore(LONG_MAX,'\n');
    ff>>M>>N;

    cout<<"M*N= "<<M<<" "<<N <<endl;
    int totalcores;

    totalcores=M*N;

    for(int i=0;i<totalcores;i++ ) // M ROWS i.e Y
    {   ff>>str;
        for(int j=0;j<totalcores;j++)
        {
            ff>>MAT[i][j];
            MATbool[i][j]=false;
        }

    }
//printMat(totalcores);  TO PRINT MAT ON CONSOLE 

// ANY MODIFICATION
createTrace(file_name);

return 1;
}

///CREATE TRACE FILE ///

void createTrace(string file_name)
{

int entries=0;
cout<<"Pseudo Random Trace File Entries= \t";
cin>> entries; 

int t_clock=0; // trace clk
int tuner=8;
cout<<"Tuning Value for Clock_t= \t";
cin>> tuner; 
int cores;
cores = M*N;
string str;
ofstream ff;
str= file_name; // take from user FILE NAME 

int val=0;
int src, dest;


ff.open(str);
    if(ff.is_open())
    {
       ff<<"Trace File carries "<<endl;
       ff<<"<t\t Src\t Dest\t PKTs>"<<endl; 
       //CHECK FOR BUGS
       for(int i=0;i<entries;i++)
       {    if(i==0)
            t_clock=1;
            val++;
            src = rand()%cores;
            dest= rand()%cores;
            if(MAT[src][dest]!=0 && MATbool[src][dest]== false)           // if MAT[][]!=0 means there is packet to send 
                                                                           // if MATbool[][]==false and task is not yet include in trace
                {       
                        ff<<" ";
                        ff<<t_clock<<"\t";
                        ff<<src<<"\t";
                        ff<<dest<<"\t";
                        ff<<MAT[src][dest]<<"\n";
                        MATbool[src][dest]=true;
                        t_clock= t_clock + rand()%tuner;
                }

       } 
        cout<<"Success: Trace Saved to file "<< str<<endl;
        ff.close();
    }
}






void printMat(int totalcores)
{
    for(int i=0;i<totalcores;i++ ) // M ROWS i.e Y
        {   for(int j=0;j<totalcores;j++)
             {
                 cout<< MAT[i][j]<<" \t";
                }   
        cout<<endl;
        }
}