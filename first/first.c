#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct cache{
	unsigned long tag;
	int valid;
	int time;
};

int mread;
int mwrite;
int hit;
int miss;
int count;

unsigned long  get_offset(int a){
	if(a==1) return 0;
	unsigned long bits = log(a)/log(2);
	return bits;

}

unsigned long get_snum(int a){
	if(a==1) return 0;
	unsigned long setnum = log(a)/log(2);
	return setnum;
}

struct cache** allocate(int n, int assoc){
	struct cache** line = malloc(n*sizeof(struct cache*));
	for(int i=0; i<n; i++){
		line[i] = malloc(sizeof(struct cache)*assoc);
	}
	for(int i=0; i<n; i++){
		for(int j=0; j<assoc; j++){
			line[i][j].valid = 0;
			line[i][j].tag = 0;
			line[i][j].time = 0;
		}

	}
	return line;
}


void free_cache(struct cache** line, int n){
	if(line==0) return;
	for(int i=0; i<n; i++){
		free(line[i]);
	}
	free(line);
}

int find_min(struct cache** line, int assoc, unsigned long set_index, int min){
	if(line==0) return 0;
	for(int j=0; j<assoc; j++){
		if(line[set_index][j].time<=line[set_index][min].time){
			min = j;
		}
	}
	return min;
}

struct cache** fifo_read(struct cache** line, int assoc, unsigned long tag, unsigned long set_index){
	if(line==0) return 0;
	int min = 0;
	for(int i=0; i<assoc; i++){
		if(line[set_index][i].tag==tag){
			//mread++;
			hit++;
			//count++;
			return line;
		}else{
			if(line[set_index][i].valid==0){
				mread++;
				miss++;
				count++;
				line[set_index][i].tag = tag;
				line[set_index][i].valid = 1;
				line[set_index][i].time = count;
				return line;
			}else if(i+1==assoc){
				miss++;
				mread++;
				min = find_min(line,assoc,set_index,min);
				count++;
				line[set_index][min].tag = tag;
				line[set_index][min].valid = 1;
				line[set_index][min].time = count;
				return line;	
			}
		}
	}
	return line;

}

struct cache** fifo_write(struct cache** line, int assoc, unsigned long tag, unsigned long set_index){
	int min = 0;
	if(line==0) return 0;
	for(int i=0; i<assoc; i++){
		if(line[set_index][i].tag==tag){
			hit++;
			mwrite++;
			return line;
		}else{

			if(line[set_index][i].valid==0){
				miss++;
				mread++;
				mwrite++;
				count++;
				line[set_index][i].tag = tag;
				line[set_index][i].valid = 1;
				line[set_index][i].time = count;
				return line;
			}else if(i+1==assoc){
				miss++;
				mread++;
				mwrite++;
				min = find_min(line,assoc,set_index,min);
				count++;
				line[set_index][min].tag = tag;
				line[set_index][min].valid = 1;
				line[set_index][min].time = count;
				return line;
			}
		}
	}
	return line;
}


struct cache** lru_read(struct cache** line, int assoc, unsigned long tag, unsigned long set_index){
	if(line==0) return 0;
	//int count = 0;
	int min = 0;
	for(int i=0; i<assoc; i++){
		if(line[set_index][i].tag==tag){
			//mread++;
			hit++;
			count++;
			line[set_index][i].time = count;
			return line;
		}else {
			if(line[set_index][i].valid==0){
				miss++;
				mread++;
				count++;
				line[set_index][i].tag = tag;
				line[set_index][i].valid = 1;
				line[set_index][i].time = count;
				return line;
		
			}else if(i+1==assoc){
				mread++;
				miss++;
		//		for(int j=0; j<assoc; j++){
		//			if(line[set_index][j].time<=line[set_index][min].time){
		//				min = j;
		//			}
		//		}
				min = find_min(line,assoc,set_index,min);
				//replace the least recently accessed line
				line[set_index][min].tag = tag;
				line[set_index][min].valid = 1;
				count++;
				line[set_index][min].time = count;
				return line;

			}
		}
	}
	return line;

}
/*
struct cache** lru_read(struct cache** line, int assoc, unsigned long tag, unsigned long set_index){
	if(line==0) return 0;
	int min = 0;
	for(int i=0;i<assoc;i++){
		if(line[set_index][i].valid==0){
			mread++;
			miss++;
			count++;
			line[set_index][i].tag = tag;
			line[set_index][i].valid = 1;
			line[set_index][i].time = count;
			return line;
		}else{
			if(line[set_index][i].tag==tag){
				hit++;
				count++;
				line[set_index][i].time = count;
				return line;
			}else{
				if(i+1==assoc){
					miss++;
					mread++;
					min = find_min(line,assoc,set_index,min);
					count++;
					
				}
			}
		}
	}

}
*/


struct cache** lru_write(struct cache** line, int assoc, unsigned long tag,unsigned long set_index){
	if(line==0) return 0;
	int min = 0;
	for(int i=0; i<assoc; i++){
		if(line[set_index][i].valid==0){
			mread++;
			mwrite++;
			miss++;
			count++;
			line[set_index][i].tag = tag;
			line[set_index][i].valid = 1;
			line[set_index][i].time = count;
			return line;
		}else{
			if(line[set_index][i].tag==tag){
				//memory_read++;
				mwrite++;
				hit++;
				count++;
				line[set_index][i].time = count;
				return line;
			}else{
				if(i+1==assoc){
					miss++;
					mread++;
					mwrite++;
					
				//	for(int j=0; j<assoc; j++){
				//		if(line[set_index][j].time<=line[set_index][min].time){
				//			min = j;
				//		}
				//	}
					min = find_min(line,assoc,set_index,min);
					//mwrite++;
					count++;
					line[set_index][min].tag = tag;
					line[set_index][min].valid = 1;
					line[set_index][min].time = count;
					return line;
				}
			}
		}
	}
	return line;

}




/*
argv[1]->cache size
argv[2]->assoc
argv[3]->policy(fifo or lru)
argv[4]->block size
argv[5]->file
*/

int main(int argc, char** argv){
	if(argc<5){
		printf("Error\n");
		return EXIT_SUCCESS;
	}
	int cache_size = atoi(argv[1]);
	int assoc;
	int snum;
	int b_size = atoi(argv[4]);//block size;
//	int mr = 0;
//	int mw = 0;
//	int h = 0;
//	int m = 0;
//	int* mread = 0;
//	int* mwrite = 0;
//	int* hit = 0;
//	int* miss = 0;
	unsigned long set_index;
	unsigned long address;
	unsigned long b;
	unsigned long s;
	unsigned long tag;
	char op;
	if(argv[3][0]=='f'){
		//fifo
		FILE* file = fopen(argv[5],"r");
		if(argv[2][0]=='d'){
			//direct map
			assoc = 1;
			snum = cache_size/b_size;
		//	 printf("assoc:%d\n",assoc);

		}
		else if(argv[2][5]==':'){
		//	snum = 1;
		//	assoc = cache_size/b_size;
			sscanf(argv[2], "assoc:%d", &assoc);
		//	printf("assoc:%d\n",assoc);
                        int temp = cache_size/b_size;
                        snum = temp/assoc;
		}
		else{
			snum = 1;
			assoc = cache_size/b_size;
		//	 printf("assoc:%d\n",assoc);

		}
//		int mread = 0;
//		int mwrite = 0;
//		int hit = 0;
//		int miss = 0;
		b = get_offset(b_size);
		s = get_snum(snum);
//		printf("Block_offset:%lu, set_num: %lu\n", b, s);
		struct cache** line = allocate(snum, assoc);
		//masking
		//set_index = (address>>b)&((1<<s)-1);
		while(fscanf(file, "%c %lx\n", &op, &address)!= EOF){
			//masking
			set_index = (address>>b)&((1<<s)-1);
			tag = address>>(b+s);
			//printf("Mask:%lu, tag:%lu\n", set_index, tag);
			if(op=='R'){
				line = fifo_read(line,assoc,tag,set_index);
			}else if(op=='W'){
				line = fifo_write(line,assoc,tag,set_index);
			}else{
				printf("Error\n");
			}
		}
		printf("memread:%d\n", mread);
		printf("memwrite:%d\n", mwrite);
		printf("cachehit:%d\n", hit);
		printf("cachemiss:%d\n",miss);
		free_cache(line, snum);
		fclose(file);
	}
	else if(strcmp(argv[3],"lru") == 0){
		//lru
		FILE* fp = fopen(argv[5], "r");
		if(argv[2][0] == 'd'){
			assoc = 1;
			snum = cache_size/b_size;
		//	 printf("assoc:%d\n",assoc);

		}else if(argv[2][5]==':'){
			sscanf(argv[2], "assoc:%d", &assoc);
		//	 printf("assoc:%d\n",assoc);
			int temp = cache_size/b_size;
			snum = temp/assoc;
		}else{
			snum = 1;
			assoc = cache_size/b_size;
		//	printf("assoc:%d\n",assoc);
		}
		b = get_offset(b_size);
		s = get_snum(snum);
		struct cache** line = allocate(snum, assoc);
                //masking
                //set_index = (address>>b)&((1<<s)-1);
                while(fscanf(fp, "%c %lx\n", &op, &address)!=EOF){
                        //masking
                        set_index = (address>>b)&((1<<s)-1);
                        tag = address>>(b+s);
                        if(op=='R'){
                                line = lru_read(line,assoc,tag,set_index);
                        }else if(op=='W'){
                                line = lru_write(line,assoc,tag,set_index);
                        }else{
                                printf("Error\n");
                        }
                }
                printf("memread:%d\n", mread);
                printf("memwrite:%d\n", mwrite);
                printf("cachehit:%d\n", hit);
                printf("cachemiss:%d\n",miss);
                fclose(fp);
		free_cache(line,snum);
	}
	//sscanf(argv[2], "assoc:%d",&assoc);
	return EXIT_SUCCESS;

}




