#include <iostream>
#include <math.h>
#include <string.h>
#include <istream>
#include <fstream>
#include <limits>
int X_MAX ;
int Y_MAX ;

int EAST=-1;  // Direction are taken as sign compliment
int WEST= 1;
int NORTH=-2;
int SOUTH=2;
int traceCount=0;
int DelX;
int DelY;
void printConMat();
void printTrace(int);
void printMapping(int,int);
bool checkOutBound(int s,int Max);
bool checkOutBoundL(int );
void XY_calculateMRs();

void printDir(int s);
void addMRtoFile();
void initCongMat();
int  congState(int );
int     MAPPING[128][128]={0};                  // For Mapping of tiles, Each point is a core and value is Task associated with it
bool    CongestionC[128][128]={0};              // If 1 Mapped to tile, 0 Not mapped

int Trace[10000][4];                            // <t, Src, Dest, PKTs >
int MRsWF[10000];
int MRsXY[10000];
void WestFirst_Routing();
int MRtotalXY=0;
int MRtotalWF=0;
using namespace std;

int main(int args, char *arg[])
{   
    
    ifstream fin;               //for input file
    ofstream fout;              //output file stream 
    //COMMAND LINE ARGUMENT Config.txt Trace.txt   
    fin.open("Config.txt");     // Taking Configuration 
    string str;
    int M,N, TaskCount=0;
    int MR_XY[100];
    int t=0;
 if(fin.is_open())
 {  
     fin.ignore(LONG_MAX,'\n');
     fin.ignore(LONG_MAX,'\n'); // Topology:
     fin>>M;
     fin>>N;
     X_MAX=N;
     Y_MAX=M;  // Updating Global Vars
     //fin.ignore(LONG_MAX,'\n');  // MAPPING
    string str;
    fin>>str;   
       for(int i=0;i<M;i++)
        {
            for(int j=0;j<N;j++)
            {   
                fin>>MAPPING[i][j];
            }
        }
 }
 else{
     perror("ERROR While Parsing\n Config.txt");
     exit(1);
 }
    fin.close();

// ADD Trace File 

int index_Trace=0;
    fin.open("trace.txt");
  if(fin.is_open())
 {  
     fin.ignore(LONG_MAX,'\n');
     fin.ignore(LONG_MAX,'\n'); 
            while(!fin.eof())
            {   //<t Src Dest Pkts>
                fin>>Trace[index_Trace][0];    // time
                fin>>Trace[index_Trace][1];    // SRC
                fin>>Trace[index_Trace][2];    // Destination
                fin>>Trace[index_Trace][3];    // Pkts
                index_Trace++;
            }
            traceCount=index_Trace;
 }
 else{
     perror("ERROR While Parsing\n Config.txt");
     exit(1);
 }

fin.close();
    //   printMapping(M,N);               // To print MAPPING
    //   printTrace(index_Trace);         // To print Trace File



   WestFirst_Routing();
   XY_calculateMRs();

    cout<<MRtotalWF<<endl;
    cout<<MRtotalXY<<endl;


   addMRtoFile();
   return 0;
}


void WestFirst_Routing()
{   int Xmax=X_MAX;
    int Ymax=Y_MAX;
    int SourceX,SourceY;
    int DestX,DestY;
    int t;// add delay component here
    int tp=0;
    int MR_Total=0;
    int MR_c=0;
    int cX,cY;
    int incomingDirection=-1;  
    int MRprev=0;
    for(int i=0; i<traceCount;i++)         // for  each Communication edge
    {
        initCongMat();          // init congestion decision mat
        congState(tp);
 //       printConMat();

        // Coordinate of Source task
        SourceX= Trace[i][1]%X_MAX;  
        SourceY= Trace[i][1]/Y_MAX; 
        
        // Coordinate of Dest task
        DestX= Trace[i][2]%X_MAX;  
        DestY= Trace[i][2]/Y_MAX; 

        DelX= DestX-SourceX;
        DelY= DestY-SourceY;
        /*
        cout<< "Task: "<< i<<" to "<<i+1;
        */
        cout<<"\t("<<SourceX<<","<<SourceY<<")"<< "--"<<"("<<DestX<<","<<DestY<<")"<< endl;
        cout << "DelX "<< DelX <<  " ,DelY "<<DelY<<" ";
        
        // WEST FIRST     
        if(DelX<=0 ) // pure XY Routing && DelY= 0,+,-
        {
            if(DelX==0)
            MR_c=2;
            else
            MR_c=3;

            MR_Total=   MR_Total+ MR_c + (abs(DelX) +abs(DelY)-1)*2;  // Pure XY Routing

            if(MR_c==2)
            {   if(DelY<0)
                incomingDirection=SOUTH;
                else
                incomingDirection=NORTH;
            }
            else
            {
                incomingDirection= (DelY/abs(DelY)) * NORTH; // DelY POSitive then Incoming direction is NORTH else SOUTH
            }
        }

        if(DelX>0 && DelY >=0) // Destination lies right side Right Side Bottom
        {
            //    cout<<"\nInside Module 2"<<endl;
                int ite;
                
                ite=abs(DelX) + abs(DelY);
                //check Nearby Tiles
                while(!(DelX==0 && DelY==0))
                {
              //      cout<<" --inside while"<<endl;
                    
                    if(DelY>0) //Go South or East   Delx Positive Del Y Positive 
                    {
                //         cout<<"  -- M1"<<endl;
                      
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBound(SourceY,Ymax);            // Destination Lies South East
                        Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag1==true && (CongestionC[SourceX][SourceY+1]==0)) // No congestion good to go   SOUTH
                        {
                            if(incomingDirection==NORTH)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelY--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelY--;
                                incomingDirection=NORTH;
                            }
                            SourceY++;           
                        }

                        else if(Flag2==true && (CongestionC[SourceX+1][SourceY]==0) && DelX >0)  // GO EAST
                        {         
                           if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning
                                incomingDirection=WEST;
                                DelX--;
                            }
                            SourceX++;
                        }
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==NORTH)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelY--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelY--;
                                incomingDirection=NORTH;
                            }
                            SourceY++;
                    
                        }
                    }

                    else if(DelX>0) // Go East
                    {
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                         Flag1=checkOutBound(SourceY,Ymax);            // Destination Lies  East or South
                         Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag2==true && (CongestionC[SourceX+1][SourceY]==0)) // No congestion good to go  EAST
                        {
                            if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;           
                        }

                        else if(Flag1==true && (CongestionC[SourceX][SourceY+1]==0) && DelY >0)  // GO SOUTH
                        {         
                           if(incomingDirection==NORTH)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelY--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning
                                incomingDirection=NORTH;
                                DelY--;
                            }
                            SourceY++;
                        }
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;
                           }
                    
                    }
                }


        }

        if(DelX>0 && DelY <=0) // Destination lies right side Right Side TOP
        {     //  cout<<" Inside Module 3"<<endl;
                int ite;
                ite=abs(DelX) + abs(DelY);
                //check Nearby Tiles
                while(!(DelX==0 && DelY==0))
                {
                    if(DelY<=0) //Go NORTH or East   Delx Positive Del Y Positive 
                    {
             
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBoundL(SourceY);            // Destination Lies North East
                        Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag1==true && (CongestionC[SourceX][SourceY-1]==0) && DelY <0) // No congestion good to go   SOUTH
                        {       
                            //CongestionC[SourceX][SourceY-1]==Trace[tp][3]
                             // CongestionC[SourceX][SourceY-1]=1;  free path
                            if(incomingDirection==SOUTH)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelY++;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelY++;
                                incomingDirection=SOUTH;
                            }
                            SourceY--;           
                                             
                        }

                        else if(Flag2==true && (CongestionC[SourceX+1][SourceY]==0) && DelX >0)  // GO EAST
                        {         
                           if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning
                                incomingDirection=WEST;
                                DelX--;
                            }
                            SourceX++;
                         }
            
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
                             if(abs(DelY)>0){   
                                     if(incomingDirection==SOUTH)
                                    {
                                        MR_Total=MR_Total+2; //No Turning
                                        DelY++;
                                    }
                                 else
                                {
                                 MR_Total=MR_Total+3; //Turning    
                                 DelY++;
                                 incomingDirection=SOUTH;
                                }
                                SourceY--;
                            }

                         if(abs(DelX)>0){   
                             if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceY--;
                        }


                        }
                    }

                    else if(DelX>0) // Go East
                    {

                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBoundL(SourceY-1);            // Destination Lies  East or South
                        Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag2==true && (CongestionC[SourceX+1][SourceY]==0)) // No congestion good to go  EAST
                        {
                            if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;           
                        }

                        else if(Flag1==true && (CongestionC[SourceX][SourceY-1]==0) && DelY <0)  // GO SOUTH
                        {         
                           if(incomingDirection==SOUTH)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelY++;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning
                                incomingDirection=SOUTH;
                                DelY++;
                            }
                            SourceY--;
                        }
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==WEST)
                            {
                                MR_Total=MR_Total+2; //No Turning
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+3; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;
                        }
                    
                    }
                }


        }
        printDir(incomingDirection);
        cout<<endl;
        MRsWF[tp]=MR_Total- MRprev;
        MRprev=MR_Total;
        tp++;                 
        cout<<"MR_On= "<< MR_Total<<endl; 
    }
    MRtotalWF=MR_Total;
}

bool  checkOutBound(int s,int Max)
{
    if(s+1>=Max)
    {
        return false;
    }
    else 
    return true;
}

bool  checkOutBoundL(int s)
{
    if(s-1 < 0)
    {
        return false;
    }
    else 
    return true;
}

void printDir(int s)
{
    switch(s)
    {
    case -1:
    cout<<" EAST"<<endl;
    break;
    
    case 1:
    cout<<" WEST"<<endl;
    break;
    case -2:
    cout<<" NORTH"<<endl;
    break;
    case 2:
    cout<<" SOUTH"<<endl;
    break;
    }
}

void initCongMat()
{
        for(int i=0;i<Y_MAX;i++)
        { 
            for(int j=0;j<X_MAX;j++)
            {
                CongestionC[i][j]=0;
            }
        }
  
}

int congState(int tp)
{
    int i;
    int start_clk,end_clk;
    start_clk = Trace[tp][0];
    end_clk= Trace[tp][0]+Trace[tp][3];

    for(i=0;i<traceCount;i++)
    {
        if(i<tp)
        {
                if(Trace[i][0]+Trace[i][3] >=start_clk) // current start with last task
                {
                    CongestionC[Trace[i][1]/Y_MAX ][ Trace[i][1]%X_MAX]=1; // Source sending packets to destination
                    CongestionC[Trace[i][2]/Y_MAX ][ Trace[i][2]%X_MAX]=1; // Dest receiving packets to destination
                }

        }
        else if(Trace[i][0]<=end_clk && Trace[i][0]>=start_clk)
        {
            // More tiles are communicating 
            // Set Path variable (UPDATE)
//            CongestionC[Trace[i][1]/Y_MAX ][ Trace[i][1]%X_MAX]=1; // Source sending packets to destination
  //          CongestionC[Trace[i][2]/Y_MAX ][ Trace[i][2]%X_MAX]=1; // Dest receiving packets to destination
            //Track path Add
        }else break;
     }
    return i;
}


void printConMat()
{      cout<<"Congest MAT :"<<endl;
    for(int i=0;i<Y_MAX;i++) // rows Y_max
    {
         for(int j=0;j<X_MAX;j++)
        {
            cout<<CongestionC[i][j]<<" ";
        }
        cout<<endl;
    }
}


void printTrace(int ind)
{
     cout<<"TRACE:"<<endl;
    for(int i=0;i<ind;i++)
    {
        cout<<Trace[i][0]<<" ";
        cout<<Trace[i][1]<<" ";
        cout<<Trace[i][2]<<" ";
        cout<<Trace[i][3]<<endl;
    }
}

void printMapping(int m,int n)
{
 
 cout<<"MAPPING"<<endl;
 for(int i=0;i<m;i++)
 {
     for(int j=0;j<n;j++)
    {
     cout<<MAPPING[i][j]<<" ";
    }
    cout<<endl;
 }
}

void addMRtoFile()
{
    ofstream ff;
    ff.open("output.txt");
    ff<<"Trace : with corresponding MRs On West First - XY"<<endl;
    ff<<"t\t Src\t Dest\t Pkts\t MRs-WF\t MRs-XY\n";
      if(ff.is_open())
    for(int i=0;i<traceCount;i++)
    {
        ff<<Trace[i][0]<<"\t";
        ff<<Trace[i][1]<<"\t";
        ff<<Trace[i][2]<<"\t";
        ff<<Trace[i][3]<<"\t";
        ff<<MRsWF[i]<<"\t";
        ff<<MRsXY[i]<<"\n";
    }
    else{
        //error handling
    }

    ff<<"Total MRs turned On XY-Routing---- "<<endl;
    ff<<MRtotalXY<<endl;
    ff<<"Total MRs Turned On West First---- "<<endl;
    ff<<MRtotalWF<<endl;
   cout<<"File Successfully Created"<<endl;
   ff.close(); 
}
 void XY_calculateMRs()
 {
     int SourceX,SourceY;
     int DestX,DestY;
     int DelX,DelY;
        int MR_c=0;
        int MR_Total=0;
        for(int i=0;i<traceCount;i++)
        {
        SourceX= Trace[i][1]%X_MAX;  
        SourceY= Trace[i][1]/Y_MAX; 
        
        // Coordinate of Dest task
        DestX= Trace[i][2]%X_MAX;  
        DestY= Trace[i][2]/Y_MAX; 

        DelX= DestX-SourceX;
        DelY= DestY-SourceY;

        if(abs(DelX)==0 || abs(DelY)==0)
        {
            MR_c= (abs(DelX)+ abs(DelY) )*2;
        }
        else
        {
            MR_c= (abs(DelX)+ abs(DelY) )*2 +1; // one turn 
        }

         MRsXY[i]= MR_c;
         MR_Total= MR_c + MR_Total;
        }

    MRtotalXY=MR_Total;        
 }
