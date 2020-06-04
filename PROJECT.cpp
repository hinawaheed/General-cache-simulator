#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define BYTES_PER_WORD 1
#define DATA 1
using namespace std;
int current_clk=0;


int access_count = 0 , read_hits = 0 , read_misses = 0 , write_hits = 0 , write_misses = 0 , no_of_writes = 0 , no_of_reads = 0 ;

class Cache 
{
	public:
		
	int ** Cache_Data;
	long long unsigned ** Upper_Data;
	long long unsigned ** Time_Access;
	long long unsigned ** Time_Load;	
};

int startCache(Cache cache ,int number_of_sets, int associativity)
 {
      for (int i=0; i<number_of_sets; i++) 
	  {
          for (int j=0; j<associativity; j++ )
		   {
              cache.Cache_Data[i][j] = 0;
              cache.Upper_Data[i][j]= 0;
              cache.Time_Access[i][j] = 0;
              cache.Time_Load[i][j] = 0;
          }
      }
      return 0;
}

//this upper is equal to the tag information
int make_upper(long long unsigned address, int block_size) 
{
    int tag;
    tag = (address/block_size) ;
   // cout<<"Tag: "<<tag<<endl;
    return tag;
}

// this gives the index of the set 
int make_index (int number_of_sets, long long unsigned tag) 
{
    int index;
    index = tag % number_of_sets;
    //cout<<"Index: "<<index<<endl;
    return index;                               // index of the set in the cache
}

 //this function is finding the block inside the set
int getPos(Cache cache , int index , long long unsigned tag , int associativity )
{
	int pos=0 ;
	for(pos=0 ; pos<associativity ; pos ++)
	{
		if (cache.Upper_Data[index][pos] == tag)
		{
            return pos;               // block inside of the set found
   	 	}
	}
	return -1 ;	 // block inside of the set not found
}

// this function tells wether there are free blocks in a set or not 
bool IsFull(Cache cache ,int index , int associativity)
{
	int pos;
    for (pos=0; pos<associativity; pos++) {
        if (cache.Cache_Data[index][pos]==0) {
            return -1;
        }
    }
    return 1;
}

// this returns a free block in the set 
int random_free_space_set (Cache cache, int index, int associativity) {
    int i;
    for (i=0; i<associativity; i++) {
        if (cache.Cache_Data[index][i]==0) {
            return i;
        }
    }
    return -1;                                   // No block free to be filled
}

int findLessAccessTSset (Cache cache, int index, int associativity) {
    int p_lessAc=0; // Position when is the less Time_Access in the set (by index)
    long unsigned lessAc = cache.Time_Access[index][0];
    for (int i=0; i<associativity; i++) {
        if (cache.Time_Access[index][i] < lessAc) {
            lessAc = cache.Time_Access[index][i];
            p_lessAc = i;
        }
    }
    return p_lessAc;
}

int findLessLoadTSset (Cache cache, int index, int associativity) {
    int i;
    int p_lessLd=0;  // Position when is the less Time_Load in the set (by index)
    long unsigned lessLd = cache.Time_Load[index][0];
    for (i=0; i<associativity; i++) {
        if (cache.Time_Load[index][i] < lessLd) {         
            lessLd = cache.Time_Load[index][i];
            p_lessLd = i;
        }
    }
    return p_lessLd;
}

void write (Cache cache , int index , long long unsigned tag , int associativity , string replacement_policy)
{
	int pos = getPos (cache,index,tag,associativity);
	 if (pos == -1 ) //write misses
	 {
	 	write_misses++;
	 	bool full = IsFull(cache,index,associativity);
	 	if(full!=1)  //set is full so a replacement policy is needed 
		{ 
			if(replacement_policy=="FIFO")
			{
				int p_lLd = findLessLoadTSset(cache, index, associativity);
                cache.Upper_Data[index][p_lLd] = tag;
                cache.Cache_Data[index][p_lLd] = DATA;
                cache.Time_Access[index][p_lLd] = current_clk; // Update T_Access
                cache.Time_Load[index][p_lLd] = current_clk;   // Update T_Load
			}
			else if (replacement_policy=="LRU")
			{
				int p_lAc = findLessAccessTSset(cache, index, associativity);
                cache.Upper_Data[index][p_lAc] = tag; 
                cache.Cache_Data[index][p_lAc] = DATA;
                cache.Time_Access[index][p_lAc] = current_clk;  // Update T_Access
                cache.Time_Load[index][p_lAc]   = current_clk ; // Update T_Load
				
			}
	 	}
	 	else   // set is not full
	 	{
	 		int free_block = random_free_space_set (cache, index, associativity);
            cache.Upper_Data[index][free_block] = tag;
            cache.Cache_Data[index][free_block] = DATA;
            cache.Time_Access[index][free_block] = current_clk; 
            cache.Time_Load[index][free_block] = current_clk;   
		}
	}
	 else  //write hits 
	 {
	 	write_hits++;
	 	cache.Upper_Data[index][pos] = tag ;
	 	cache.Time_Access[index][pos] = current_clk;
	 }
}

void read (Cache cache , int index , long long unsigned tag , int associativity , string replacement_policy)
{
	int pos = getPos (cache,index,tag,associativity);
	 if (pos == -1 ) //read misses
	 {
	 	read_misses++;
	 	bool full = IsFull(cache,index,associativity);
	 	if(full!=1)  //set is full so a replacement policy is needed 
		{ 
			if(replacement_policy=="FIFO")
			{
				int p_lLd = findLessLoadTSset(cache, index, associativity);
                cache.Upper_Data[index][p_lLd] = tag;
                cache.Cache_Data[index][p_lLd] = DATA;
                cache.Time_Access[index][p_lLd] = current_clk; // Update T_Access
                cache.Time_Load[index][p_lLd] = current_clk;   // Update T_Load
			}
			else if (replacement_policy=="LRU")
			{
				int p_lAc = findLessAccessTSset(cache, index, associativity);
                cache.Upper_Data[index][p_lAc] = tag; 
                cache.Cache_Data[index][p_lAc] = DATA;
                cache.Time_Access[index][p_lAc] = current_clk;  // Update T_Access
                cache.Time_Load[index][p_lAc]   = current_clk ; // Update T_Load
				
			}
	 	}
	 	else   // set is not full
	 	{
	 		int free_block = random_free_space_set (cache, index, associativity);
            cache.Upper_Data[index][free_block] = tag;
            cache.Cache_Data[index][free_block] = DATA;
            cache.Time_Access[index][free_block] = current_clk; 
            cache.Time_Load[index][free_block] = current_clk;   
		}
	}
	 else  //read hit
	 {
	 	read_hits++;
	 	cache.Upper_Data[index][pos] = tag ;
	 	cache.Time_Access[index][pos] = current_clk;
	 }
}


int main ()
{
	
	Cache cache ; 
	int block_size , total_blocks , associativity , descfile , inpfile , i , no_of_lines , index ;
	char op ;
	long long unsigned tag , address ;
	ifstream desc , input ;
////long long int main_memory=;
//long long int physical_bit;

	cout<<"Choose a description file :\n 1. Associativity = 2 \n 2. Associativity = 4 \n 3. Associativity = 8 \n 4. Associativity = 16 \n";
	cin>>descfile;
	switch(descfile)
	{
		case 1:
			{
				desc.open("description1.txt");
				break;
			}
		case 2:
			{
				desc.open("description2.txt");
				break;
			}
		case 3:
			{
				desc.open("description3.txt");
				break;
			}
		case 4:
			{
				desc.open("description4.txt");
				break;
			}
	}
	 
	// reading the description file 
	
	desc>>block_size;
	desc>>total_blocks;
	desc>>associativity;
	desc.close();




	
	string policy ;
	cout<<"Select a replacement policy: ";
	cin>>policy;
	
	int number_of_sets = total_blocks/associativity ;
	
	// allocating space for cache
	
	cache.Cache_Data = new int *[number_of_sets];
	for(i=0 ; i < number_of_sets ; i++)
	cache.Cache_Data[i] = new int [associativity] ; 
	
	// allocating space for access time stamp
	
	cache.Time_Access = new long long unsigned *[number_of_sets];
	for(i=0 ; i < number_of_sets ; i++)
	cache.Time_Access[i] = new long long unsigned [associativity] ; 
	
	// allocating space for load time stamp
	
	cache.Time_Load = new long long unsigned *[number_of_sets];
	for(i=0 ; i < number_of_sets ; i++)
	cache.Time_Load[i] = new long long unsigned [associativity] ; 
	
	// allocating space for cache upper
	
	cache.Upper_Data = new long long unsigned *[number_of_sets];
	for(i=0 ; i < number_of_sets ; i++)
	cache.Upper_Data[i] = new long long unsigned [associativity] ; 
	
	startCache(cache, number_of_sets, associativity);
	
	// opening the input trace file 
	
	cout<<"\nChoose an input file :\n 1.\n 2.\n 3. \n 4. \n";
	cin>>inpfile;
	switch(inpfile)
	{
		case 1:
			{
				input.open("input1.txt");
				break;
			}
		case 2:
			{
				input.open("input2.txt");
				break;
			}
		case 3:
			{
				input.open("input3.txt");
				break;
			}
		case 4:
			{
				input.open("input4.txt");
				break;
			}
	}
	input >> no_of_lines ;
	srand (time(NULL));
	clock_t start=clock();

	for (int i=0 ; i<no_of_lines ; i++)
	{
		if(input!=" ")
		input>>address; 
		if(input!=" ")
		input>>op;
		current_clk += 1;
		access_count++;
	
		if(op=='W')  // for write access
		{
			tag = make_upper(address, block_size);
			index = make_index (number_of_sets, tag);
			no_of_writes++;
			write(cache,index,tag,associativity,policy);
		}
		else if (op=='R')  // for read access
		{ 
			tag = make_upper(address, block_size);
			index = make_index (number_of_sets, tag);
			no_of_reads++;
			read(cache,index,tag,associativity,policy);
		}
	}
	clock_t endt=clock();
    
    clock_t ctotaltime=endt-start;
    double finalt=ctotaltime/(double)CLOCKS_PER_SEC;
    cout.precision(8) ;
	 cout<<"\ntotal time is "<< finalt << "\n";
	cout<<"Access Counts: "<<access_count<<endl;
	cout<<"Read Hits: "<<read_hits<<endl;
	cout<<"Read Misses: "<<read_misses<<endl;
	cout<<"Write Hits: "<<write_hits<<endl;
	cout<<"Write Misses: "<<write_misses<<endl;

	
}

