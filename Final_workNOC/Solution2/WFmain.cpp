#include <iostream>
#include <math.h>
#include <string.h>
#include <istream>
#include <fstream>
#include <limits>

int X_MAX ;
int Y_MAX ;

int EAST=-1;  // Direction are taken as sign compliment
int WEST= 1;  // for Ports
int NORTH=-2;
int SOUTH=2;
int traceCount=0;
int max_clk=0;
int DelX;
int DelY;
int path[256][2]; // it carries the dynamic path for updation

/*typedef struct cruxRouteNode(){
bool east_west;
bool west_east;
bool north_south;
bool south_north;
bool injection;
bool ejection;
} CRUXnode;
*/
void updatePath(int t_nodes, int maxVal);
void printConMat();
void printTrace(int);
void printMapping(int,int);
bool checkOutBound(int s,int Max);
bool checkOutBoundL(int );
void XY_calculateMRs();
void findpath(int x,int y, int dx, int dy);
void printDir(int s);
void addMRtoFile();
void initCongMat();
void genInFileforSnR();
void Exec_clock(); // Clocks required to execute the tasks
int  congState(int );
int     MAPPING[128][128]={0};                  // For Mapping of tiles, Each point is a core and value is Task associated with it
int     CongestionC[128][128]={0};               // Carries the clk time of each time or rather to say the how many clocks it has executed
//-------- Modify Congestion Matrix -----------
int Trace[10000][4];                            // <t, Src, Dest, PKTs >
int MRsWF[10000];
int MRsXY[10000];
void WestFirst_Routing();
int MRtotalXY=0;
int MRtotalWF=0;

using namespace std;

int main(int args, char *arg[]) // input args as Config.txt Communication.tx Trace.txt
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
    cout<<"MRs WF : ";
    cout<<MRtotalWF<<endl;
    cout<<"MRs XY : ";
    cout<<MRtotalXY<<endl;

   genInFileforSnR();
   cout<<"Total Trace :" <<traceCount-1<<endl;
   Exec_clock();
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
    int clk_init,clk_end;
    int maxOfPath=0;
    int path_len=0;
    initCongMat();                          // init congestion decision mat
        
    for(int i=0; i<traceCount;i++)         // for  each Communication edge
    {   
        path_len=0;
//        congState(tp);
        // Coordinate of Source task
        
        SourceX= Trace[i][1]%X_MAX;  
        SourceY= Trace[i][1]/Y_MAX; 
        
        path[path_len][0]=SourceX;
        path[path_len][1]=SourceY;

        // Coordinate of Dest task
        DestX= Trace[i][2]%X_MAX;  
        DestY= Trace[i][2]/Y_MAX; 

        DelX= DestX-SourceX;
        DelY= DestY-SourceY;

        clk_init=Trace[i][0];
        clk_end=clk_init+Trace[i][3]; 
        
        maxOfPath=clk_init;
        /*
        cout<< "Task: "<< i<<" to "<<i+1;
        */
        cout<<"\t("<<SourceX<<","<<SourceY<<")"<< "--"<<"("<<DestX<<","<<DestY<<")"<< endl;
        cout << "DelX "<< DelX <<  " ,DelY "<<DelY<<" ";
        
        // WEST FIRST     
        if(DelX<=0 ) // pure XY Routing 
        {
            if(DelX==0)
            MR_c=2;
            else
            MR_c=3;
            MR_Total=   MR_Total+ MR_c ;  // Pure XY Routing

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
        
        for(int i=0;i<abs(DelX);i++)
        {   
            SourceX--;
            path_len++;
            path[path_len][0]=SourceX;
            path[path_len][1]=SourceY;
            maxOfPath=max(maxOfPath,CongestionC[SourceX][SourceY]);
        }

    if(DelY<0)    
    {
        for(int i=0;i<abs(DelY);i++)
        {   SourceY--;
            path_len++;
            path[path_len][0]=SourceX;
            path[path_len][1]=SourceY;
            maxOfPath=max(maxOfPath,CongestionC[SourceX][SourceY]);            
        }
    }
    else
    {
        for(int i=0;i<abs(DelY);i++)
        {
            SourceY++;
            path_len++;
            path[path_len][0]=SourceX;
            path[path_len][1]=SourceY;
            maxOfPath=max(maxOfPath,CongestionC[SourceX][SourceY]);
        }
    }

        updatePath(path_len,maxOfPath+Trace[i][3]);
        // UPDATE PATH VALUES TO MAX VALUES OF TILES LIES IN BETWEEN

    }

        else if(DelX>0 && DelY >=0) // Destination lies right side and Bottom
        {
                int ite;
                ite=abs(DelX) + abs(DelY);
       
                //check Nearby Tiles
                while(!(DelX==0 && DelY==0))
                {
                         if(DelX>0) // Go East
                    {
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                         Flag1=checkOutBound(SourceY,Ymax);            // Destination Lies  East or South
                         Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag2==true && (clk_init>= CongestionC[SourceX+1][SourceY])) // No congestion good to go  EAST
                        {
                            if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;           
                        }

                        else if(Flag1==true && (clk_init>= CongestionC[SourceX][SourceY+1]) && DelY >0)  // GO SOUTH
                        {         
                           if(incomingDirection==NORTH)
                            {
                                DelY--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning
                                incomingDirection=NORTH;
                                DelY--;
                            }
                            SourceY++;
                        }
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;
                            
                           }
                
                    }
               
                    else if(DelY>0) 
                    {
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBound(SourceY,Ymax);            // Destination Lies South East
                        Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag1==true && (clk_init >= CongestionC[SourceX][SourceY+1])) // No congestion good to go SOUTH
                        {
                            
                            if(incomingDirection==NORTH)
                            {    
                                DelY--;                     // No turning
                             }
                            else
                            {
                                MR_Total=MR_Total+1;        //Turning    
                                DelY--;
                                incomingDirection=NORTH;
                            }
                            SourceY++;
                        }

                        else if(Flag2==true && (clk_init >= CongestionC[SourceX+1][SourceY]) && DelX >0)  // GO EAST
                        {                                  
                            //  maxVal=max(maxVal,CongestionC[SourceX+1][SourceY]);
                           if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning
                                incomingDirection=WEST;
                                DelX--;
                            }
                            SourceX++;

                        }

                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==NORTH)
                            {
                                DelY--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelY--;
                                incomingDirection=NORTH;
                            }
                            SourceY++;                  
                        }
                               
                    }
                
                    path_len++;
                    path[path_len][0]=SourceX;
                    path[path_len][1]=SourceY;
                    maxOfPath=max(maxOfPath,CongestionC[SourceX][SourceY]);
                    updatePath(path_len,maxOfPath+Trace[i][3]); // MAX of Path + Packet Size  Assuming each packet takes 1 CC     
                }
                // EXiting While Loop
                if(DelX==0 && DelY==0)
                {
                path_len++;
                MR_Total++; // Ejection Point
                path[path_len][0]=DestX;
                path[path_len][1]=DestY;
                }
            }
//---------------------------------------------------------------------------------//
        if(DelX>0 && DelY <=0) // Destination lies right side Right Side TOP
        {     //  cout<<" Inside Module 3"<<endl;
                int ite;
                bool turn= false;

                ite=abs(DelX) + abs(DelY);
                //check Nearby Tiles
                while(!(DelX==0 && DelY==0))
                {
                    
                    if(DelX>0) // Go East
                    {

                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBoundL(SourceY-1);            // Destination Lies  East or South
                        Flag2=checkOutBound(SourceX,Xmax);
                        if(Flag2==true && (CongestionC[SourceX+1][SourceY]<=clk_init)) // No congestion good to go  EAST
                        {
                            if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;
                            turn =true;
                        }

                        else if(Flag1==true && (CongestionC[SourceX][SourceY-1]<=clk_init) && DelY <0)  // GO SOUTH
                        {         
                           if(incomingDirection==SOUTH)
                            {
                                DelY++;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning
                                incomingDirection=SOUTH;
                                DelY++;
                            }
                            SourceY--;
                            
                        }
                        //Forcefully XY Routing ... will wait until tile is free
                        else {  
  
                             if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceX++;
                        }
                       
                    }
                
                    else if(DelY<=0) //Go NORTH or East   Delx Positive Del Y Positive 
                    {
             
                        bool Flag1=true,Flag2=true;
                        //CheckOutBound of Tiles
                        Flag1=checkOutBoundL(SourceY);            // Destination Lies North East
                        Flag2=checkOutBound(SourceX,Xmax);
                        
                        if(Flag1==true && (CongestionC[SourceX][SourceY-1]<=clk_init) && DelY <0) // No congestion good to go   SOUTH
                        {       
                               if(incomingDirection==SOUTH)
                            {
                                DelY++;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelY++;
                                incomingDirection=SOUTH;
                            }
                            SourceY--;           
                 
                        }

                        else if(Flag2==true && (CongestionC[SourceX+1][SourceY]<=clk_init) && DelX >0)  // GO EAST
                        {         
                           if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning
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
                                        DelY++;
                                    }
                                 else
                                {
                                 MR_Total=MR_Total+1; //Turning    
                                 DelY++;
                                 incomingDirection=SOUTH;
                                }
                                SourceY--;
                            }

                         if(abs(DelX)>0){   
                             if(incomingDirection==WEST)
                            {
                                DelX--;
                            }
                            else
                            {
                                MR_Total=MR_Total+1; //Turning    
                                DelX--;
                                incomingDirection=WEST;
                            }
                            SourceY--;
                        }


                        }
                    }

                    path_len++;
                    path[path_len][0]=SourceX;
                    path[path_len][1]=SourceY;
                    maxOfPath=max(maxOfPath,CongestionC[SourceX][SourceY]);
            
                }
            
            if(DelX==0 && DelY==0)
                {
                path_len++;
                MR_Total++; // Ejection Point
                path[path_len][0]=DestX;
                path[path_len][1]=DestY;
                }
        }
        updatePath(path_len,maxOfPath+Trace[i][3]);
      //  printConMat();
       // printDir(incomingDirection);
       // cout<<endl;
        MRsWF[tp]=MR_Total- MRprev;
        MRprev=MR_Total;
        tp++;                 
        cout<<"MR_On= "<< MR_Total<<endl; 
    }   // ---- END OF FORLOOP TRACE ---


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
    ff<<"ClKs: ";
    ff<<max_clk<<endl;
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

        if(abs(DelX)!=0 && abs(DelY)!=0)
        {
            MR_c= 3 ; // Injecction , Ejection and a Turn
        }
        else if(abs(DelX)==0 && abs(DelY==0))
        {
                MR_c=0; // No injection and ejection as the destination and source are same 
        }
        else
        {
            MR_c= 2; // Injection and Ejection port 
        }

         MRsXY[i]= MR_c;
         MR_Total= MR_c + MR_Total;
        }

    MRtotalXY=MR_Total;        
 }


void updatePath(int t_nodes,int max)
{
    int x,y;
    for(int i=0;i<=t_nodes;i++)
    {
        x=path[i][0];
        y=path[i][1];

        CongestionC[y][x]=max;
    }
}

void genInFileforSnR()
{

    ofstream ff;
    ff.open("inputs.txt"); // SnR input for CLAP
    
    //cout << "Set WDM 8 16 32 64 etc"<< endl;
    //cin>> str;

    if(ff.is_open())
    {
        //-------inputs.txt---------- for the Snr calculation

        ff<<"arch_type=mesh;"<<endl;
        ff<<"set_wdm 64;"<<endl;

// NAMING CONVENTIONS FOR clap TOOL HAVE CORES TILES FROM 1 ONWARDS
    for(int i=0;i<traceCount-1;i++)
    {
        ff<<"from "; 
        ff<< 1+ Trace[i][1]%X_MAX <<","<< 1+(Trace[i][1])/Y_MAX; 
        ff<<" to ";
        // Coordinate of Dest task
        ff<< 1+Trace[i][2]%X_MAX<<","<<1+Trace[i][2]/Y_MAX<<";"<<endl; 
    }

    }else{
        perror( "Error In Function genInFileSnr" );
    }
}
   
   void Exec_clock() // Clocks required to execute the tasks
   {
       for(int i=0;i<Y_MAX;i++)
       {
           for(int j=0;j<X_MAX;j++)
           {
               max_clk= max(CongestionC[i][j],max_clk);
           }
       }
    cout<<"Total Clocks Requied : "<<max_clk<<endl;
    
   }
