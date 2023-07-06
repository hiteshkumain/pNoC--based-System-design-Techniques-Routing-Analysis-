//Communication matrix Generator
/*
Copyright Hitesh Kumain
raise request for modification 
email h20210068@goa.bits-pilani.ac.in
*/


#include <iostream>
#include <fstream>
int M;  // topology M*N 
int N;  


int MAT[128][128]={0};
using namespace std;
int main()
{   

    int pkt, totalcores=0;
    cout<<"Enter Topology Values as M N "<<endl;
    cin>>M>>N;
    cout<<"Max Packet Size = "<<endl;
    cin>>pkt;
    totalcores=M*N;
    for(int i=0;i<totalcores;i++ ) // M ROWS i.e Y
    {   for(int j=0;j<totalcores;j++)
        {
            if(i!=j)
            {
                MAT[i][j]=rand()%pkt;
            }
            else
            MAT[i][j]=0;
        }

    }
    // ANY MODIFICATION

// STORING IN comm1.txt file
string str;
ofstream ff;
str="CommGraph.txt"; // take from user FILE NAME 
ff.open("CommGraph.txt");
    if(ff.is_open())
    {
    ff<<"Communication Matrix 2D : M*N cores "<<endl;
    ff<<M <<" " <<N<<endl;

    for(int i=0;i<totalcores;i++ ) // M ROWS i.e Y
    {   ff<<"Core"<<i<<"|  \t";
        for(int j=0;j<totalcores;j++)
        {   
            ff<<MAT[i][j]<<"\t";
        }
        ff<<"\n";
    }
    cout<<"Success: Saved to file "<< str<<endl;
    ff.close();
    }
    else
    {
        perror("File Open Failed");
    }
}

