#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct cache{
	unsigned long tag;
	unsigned long address;
	int valid;
	int time;

};


int memread;
int memwrite;
int l1_miss;
int l2_miss;
int l1_hit;
int l2_hit;
int count;
//struct cache** l1;
//struct cache** l2;

unsigned long get_offset(int a){
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
			line[i][j].tag = 0;
			line[i][j].valid = 0;
			line[i][j].time = 0;
			line[i][j].address = 0;
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


int search(struct cache** line, int assoc, unsigned long tag, unsigned long s){
	//s->set index
	//if found, return 1; otherwise return 0;
	if(line==0) return 0;
	for(int i=0; i<assoc; i++){
			if(line[s][i].tag==tag&&line[s][i].valid==1){
				//target found;
				return 1;
		}
	}
	return 0;
}


void empty(struct cache** line, int assoc, unsigned long tag, unsigned long s){
	//clear the address if it is found in l2
	if(line==0){
		return;
	}
//	count = 0;
	for(int i=0; i<assoc; i++){
			if(line[s][i].tag==tag&&line[s][i].valid==1){
				//target found;
				line[s][i].tag = 0;
				line[s][i].valid = 0;
				line[s][i].time = 0;
			//	line[s][i].address = 0;
				return;
		}
	}
	return;
}


unsigned long l1_fifoRead(struct cache** l1, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l1==0) return 0;
	int min = 0;
	unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l1[set_index][i].tag==tag){
			//hit, nothing happens
			l1_hit++;
			return 0;
		}else{
			if(l1[set_index][i].valid==0){
				//unsigned long temp = l2_fifoRead()
			//	memread++;
				l1_miss++;
				count++;
				l1[set_index][i].tag = tag;
				l1[set_index][i].valid = 1;
				l1[set_index][i].time = count;
				l1[set_index][i].address = address;
				return 0;
			}else if(i+1==assoc){
				l1_miss++;
			//	memread++;
				min = find_min(l1,assoc,set_index,min);
				evict = l1[set_index][min].address;
				count++;
				l1[set_index][min].tag = tag;
				l1[set_index][min].valid = 1;
				l1[set_index][min].time = count;
				l1[set_index][min].address = address;
			//	printf("evict is: %zu\n",evict);
				return evict;
			}
		}
	}
	return 0;

}

unsigned long l1_fifoWrite(struct cache** l1, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l1==0) return 0;
	int min = 0;
	unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l1[set_index][i].tag==tag){
			l1_hit++;
			memwrite++;
			return 0;
		}else{
			if(l1[set_index][i].valid==0){
				l1_miss++;
			//	memread++;
				memwrite++;
				count++;
				l1[set_index][i].tag = tag;
				l1[set_index][i].valid = 1;
				l1[set_index][i].time = count;
				l1[set_index][i].address = address;
				return 0;
			}else if(i+1==assoc){
				l1_miss++;
			//	memread++;
				memwrite++;
				min = find_min(l1,assoc,set_index,min);
				count++;
				evict = l1[set_index][min].address;
				l1[set_index][min].tag = tag;
				l1[set_index][min].valid = 1;
				l1[set_index][min].time = count;
				l1[set_index][min].address = address;
			//	printf("evict is:%zu\n",evict);
				return evict;
			}
		}
	}
	return 0;
}

unsigned long l1_lruRead(struct cache** l1, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l1==0) return 0;
	int min = 0;
	unsigned long evict;
	for(int i=0; i<assoc; i++){
		 if(l1[set_index][i].tag==tag){
                        l1_hit++;
                     //   memread++;
			count++;
			l1[set_index][i].time = count;
                        return 0;
		}else{
			if(l1[set_index][i].valid==0){
				l1_miss++;
		//		memread++;
				l1[set_index][i].tag = tag;
				l1[set_index][i].valid = 1;
				l1[set_index][i].time = count;
				l1[set_index][i].address = address;
				return 0;
			}else if(i+1==assoc){
				l1_miss++;
		//		memread++;
				min = find_min(l1,assoc,set_index,min);
				evict = l1[set_index][i].address;
				count++;
				l1[set_index][min].tag = tag;
                                l1[set_index][min].valid = 1;
                                l1[set_index][min].time = count;
                                l1[set_index][min].address = address;
                                return evict;
			//	printf("evict is: %zu\n",evict);
			}
		}

	}
	return 0;

}

unsigned long l1_lruWrite(struct cache** l1, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l1==0) return 0;
	int min = 0;
	unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l1[set_index][i].tag==tag){
			l1_hit++;
			memwrite++;
			count++;
			l1[set_index][i].time = count;
			return 0;
		}else{
			if(l1[set_index][i].valid==0){
				l1_miss++;
		//		memread++;
				memwrite++;
				count++;
				l1[set_index][i].tag = tag;
				l1[set_index][i].valid = 1;
				l1[set_index][i].time = count;
				l1[set_index][i].address = address;
				return 0;
			}else if(i+1==assoc){
				l1_miss++;
		//		memread++;
				memwrite++;
				min = find_min(l1,assoc,set_index,min);
				count++;
				evict = l1[set_index][i].address;
				l1[set_index][min].tag = tag;
				l1[set_index][min].valid = 1;
				l1[set_index][min].time = count;
				l1[set_index][min].address = address;
				return evict;
			}
		}
	}
	return 0;
}

void l2_fifoRead(struct cache** l2, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l2==0) return;
	int min = 0;
	//unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l2[set_index][i].tag==tag){
			//hit, nothing happens
			//l2_hit++;
			return;
		}else{
			if(l2[set_index][i].valid==0){
				//unsigned long temp = l2_fifoRead()
		//		memread++;
			//	l2_miss++;
				count++;
				l2[set_index][i].tag = tag;
				l2[set_index][i].valid = 1;
				l2[set_index][i].time = count;
				l2[set_index][i].address = address;
			//	return 0;
				return;
			}else if(i+1==assoc){
			//	l2_miss++;
		//		memread++;
				min = find_min(l2,assoc,set_index,min);
				//evict = l2[set_index][min].address;
				count++;
				l2[set_index][min].tag = tag;
				l2[set_index][min].valid = 1;
				l2[set_index][min].time = count;
				l2[set_index][min].address = address;
				//return evict;
				return;
			}
		}
	}
	return;

}

void l2_fifoWrite(struct cache** l2, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l2==0) return;
	int min = 0;
	//unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l2[set_index][i].tag==tag){
		//	l2_hit++;
		//	memwrite++;
		//	return 0;
			return;
		}else{
			if(l2[set_index][i].valid==0){
		//		l2_miss++;
		//		memread++;
		//		memwrite++;
				count++;
				l2[set_index][i].tag = tag;
				l2[set_index][i].valid = 1;
				l2[set_index][i].time = count;
				l2[set_index][i].address = address;
			//	return 0;
				return;
			}else if(i+1==assoc){
		//		l2_miss++;
		//		memread++;
		//		memwrite++;
				min = find_min(l2,assoc,set_index,min);
				count++;
				//evict = l2[set_index][min].address;
				l2[set_index][min].tag = tag;
				l2[set_index][min].valid = 1;
				l2[set_index][min].time = count;
				l2[set_index][min].address = address;
				//return evict;
				return;
			}
		}
	}
	return;
}

void l2_lruRead(struct cache** l2, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l2==0) return;
	int min = 0;
	//unsigned long evict;
	for(int i=0; i<assoc; i++){
		 if(l2[set_index][i].tag==tag){
                  //      l2_hit++;
                        //memread++;
			count++;
			l2[set_index][i].time = count;
                //        return 0;
			return;
		}else{
			if(l2[set_index][i].valid==0){
		//		l2_miss++;
		//		memread++;
				l2[set_index][i].tag = tag;
				l2[set_index][i].valid = 1;
				l2[set_index][i].time = count;
				l2[set_index][i].address = address;
			//	return 0;
				return;
			}else if(i+1==assoc){
		//		l2_miss++;
		//		memread++;
				min = find_min(l2,assoc,set_index,min);
				//evict = l2[set_index][i].address;
				count++;
				l2[set_index][min].tag = tag;
                                l2[set_index][min].valid = 1;
                                l2[set_index][min].time = count;
                                l2[set_index][min].address = address;
                                //return evict;
				return;
			}
		}

	}
	return;

}


void l2_lruWrite(struct cache** l2, int assoc, unsigned long tag, unsigned long set_index, unsigned long address){
	if(l2==0) return;
	int min = 0;
	//unsigned long evict;
	for(int i=0; i<assoc; i++){
		if(l2[set_index][i].tag==tag){
		//	l2_hit++;
		//	memwrite++;
			count++;
			l2[set_index][i].time = count;
			//return 0;
			return;
		}else{
			if(l2[set_index][i].valid==0){
		//		l2_miss++;
		//		memread++;
		//		memwrite++;
				count++;
				l2[set_index][i].tag = tag;
				l2[set_index][i].valid = 1;
				l2[set_index][i].time = count;
				l2[set_index][i].address = address;
			//	return 0;
				return;
			}else if(i+1==assoc){
		//		l2_miss++;
		//		memread++;
		//		memwrite++;
				min = find_min(l2,assoc,set_index,min);
				count++;
	//			evict = l2[set_index][i].address;
				l2[set_index][min].tag = tag;
				l2[set_index][min].valid = 1;
				l2[set_index][min].time = count;
				l2[set_index][min].address = address;
	//			return evict;
				return;
			}
		}
	}
	return;
}

int main(int argc, char** argv){
	if(argc<8){
		printf("Error\n");
		return EXIT_SUCCESS;
	}
	/*
	argv[1]->L1 cache size
	argv[2]->L1 assoc;
	argv[3]->L1 cache policy
	argv[4]->L1 block size
	argv[5]->L2 cache size
	argv[6]->L2 assoc
	argv[7]->L2 policy
	argv[8]->trace file
	*/
	int l1_cachesize = atoi(argv[1]);
	int block_size = atoi(argv[4]);
	int l2_cachesize = atoi(argv[5]);
	unsigned long address;
	unsigned long set_index1;
	unsigned long set_index2;
	unsigned long l1_tag;
	unsigned long l2_tag;
	unsigned long new;
	int b;
	int s1;
	int s2;
	int l1_setnum;
	int l2_setnum;
	int l1_assoc;
	int l2_assoc;
	char op;
	//for search in l2
	unsigned long tag_index;
	unsigned long set_bits;
	//FILE* file = fopen(argv[8], "r");
	//if(file==0){
	//	printf("Error\n");
	//	return EXIT_SUCCESS;
	//}
	if(argv[3][0] == 'f'){
		//fifo
		if(argv[2][5]==':'){
			sscanf(argv[2],"assoc:%d",&l1_assoc);
			l1_setnum = l1_cachesize/block_size/l1_assoc;
		}else if(argv[2][0] == 'd'){
			l1_assoc = 1;
			l1_setnum = l1_cachesize/block_size;
		}else{
			l1_setnum = 1;
			l1_assoc = l1_cachesize/block_size;
		}
		b = get_offset(block_size);
		s1 = get_snum(l1_setnum);
//		struct cache** l1 = allocate(l1_setnum,l1_assoc);
//		struct cache** l2 = allocate(l2_setnum,l2_assoc);
		if(argv[7][0]=='f'){
			//l2 fifo
			FILE* file = fopen(argv[8], "r");
			if(argv[6][5]==':'){
				sscanf(argv[6],"assoc:%d",&l2_assoc);
				l2_setnum = l2_cachesize/block_size/l2_assoc;
			}else if(argv[6][0]=='d'){
				l2_assoc = 1;
				l2_setnum =  l2_cachesize/block_size;
			}else{
				l2_setnum = 1;
				l2_assoc = l2_cachesize/block_size;
			}
			s2 = get_snum(l2_setnum);
			struct cache** l1 = allocate(l1_setnum, l1_assoc);
			struct cache** l2 = allocate(l2_setnum, l2_assoc);
			while(fscanf(file, "%c %lx\n", &op, &address)!=EOF){
				set_index1 = (address>>b)&((1<<s1)-1);
				l1_tag = address>>(b+s1);
				set_bits = (address>>b)&((1<<s2)-1);
				tag_index = address>>(b+s2);
			//	new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
			//	printf("new is:%zu\n",new);
				if(op=='R'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						//address found in l1
						new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
					//	memread++;
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
							//	memread++;
								l2_hit++;
								new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2,l2_assoc,tag_index,set_bits);
								if(new!=0){
                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_fifoRead(l2,l2_assoc,l2_tag,set_index2,new);
								}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							//evict things here in l1
								s2 = get_snum(l2_setnum);
                                                        	set_index2 = (new>>b)&((1<<s2)-1);
                                                        	l2_tag = new>>(b+s2);
								l2_fifoRead(l2,l2_assoc,l2_tag,set_index2,new);
			
							//	printf("New address is:%zu\n", new);
							}
						 }
					}
				}
				if(op=='W'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
					//	memread++;
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
						//	memread++;
							l2_hit++;
							new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
							empty(l2, l2_assoc, tag_index,set_bits);
							if(new!=0){
                                                        s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
                                                        l2_fifoWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
							l2_fifoWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
						//	printf("New address is:%zu\n", new);
						 }
					}

				}
			}
		//	memread = memread - l2_hit;
			printf("memread:%d\n",memread);
			printf("memwrite:%d\n",memwrite);
			printf("l1cachehit:%d\n",l1_hit);
//			printf("l2cachehit:%d\n",l2_hit);
			printf("l1cachemiss:%d\n",l1_miss);
			printf("l2cachehit:%d\n",l2_hit);
			printf("l2cachemiss:%d\n",l2_miss);
			free_cache(l1,l1_setnum);
			free_cache(l2,l2_setnum);
			fclose(file);
		}
		if(argv[7][0]=='l'){
			//l2 lru
			FILE* file = fopen(argv[8], "r");
			if(argv[6][5]==':'){
				sscanf(argv[6],"assoc:%d",&l2_assoc);
				l2_setnum = l2_cachesize/block_size/l2_assoc;
			}else if(argv[6][0]=='d'){
				l2_assoc = 1;
				l2_setnum =  l2_cachesize/block_size;
			}else{
				l2_setnum = 1;
				l2_assoc = l2_cachesize/block_size;
			}
			struct cache** l1 = allocate(l1_setnum, l1_assoc);
			struct cache** l2 = allocate(l2_setnum, l2_assoc);
			while(fscanf(file, "%c %lx\n", &op, &address)!=EOF){
				set_index1 = (address>>b)&((1<<s1)-1);
				l1_tag = address>>(b+s1);
				s2 = get_snum(l2_setnum);
				set_bits = (address>>b)&((1<<s2)-1);
				tag_index = address>>(b+s2);
				if(op=='R'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
			//			memread++;
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
							//	memread++;
								l2_hit++;
								new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2, l2_assoc, tag_index,set_bits);

								if(new!=0){
                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_lruRead(l2,l2_assoc,l2_tag,set_index2,new);
								}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
							l2_tag = new>>(b+s2);
							l2_lruRead(l2,l2_assoc,l2_tag,set_index2,new);
							}

						 }
					}
				}
				if(op=='W'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
							//	memread++;
								l2_hit++;
								new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2, l2_assoc, tag_index,set_bits);

								if(new!=0){
                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_lruWrite(l2,l2_assoc,l2_tag,set_index2,new);
								}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
							l2_lruWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
						 }
					}

				}
			}
	//		memread = memread - l2_hit;
			printf("memread:%d\n",memread);
			printf("memwrite:%d\n",memwrite);
			printf("l1cachehit:%d\n",l1_hit);
			//printf("l2cachehit:%d\n",l2_hit);
			printf("l1cachemiss:%d\n",l1_miss);
		        printf("l2cachehit:%d\n",l2_hit);
			printf("l2cachemiss:%d\n",l2_miss);
			free_cache(l1,l1_setnum);
			free_cache(l2,l2_setnum);
			fclose(file);
		}

	}
		if(argv[3][0]=='l'){
			//lru
			if(argv[2][5]==':'){
			sscanf(argv[2],"assoc:%d",&l1_assoc);
			l1_setnum = l1_cachesize/block_size/l1_assoc;
		}else if(argv[2][0] == 'd'){
			l1_assoc = 1;
			l1_setnum = l1_cachesize/block_size;
		}else{
			l1_setnum = 1;
			l1_assoc = l1_cachesize/block_size;
		}
		b = get_offset(block_size);
		s1 = get_snum(l1_setnum);
//		struct cache** l1 = allocate(l1_setnum,l1_assoc);
//		struct cache** l2 = allocate(l2_setnum,l2_assoc);
		if(argv[7][0]=='f'){
			//l2 fifo
			FILE* file = fopen(argv[8], "r");
			if(argv[6][5]==':'){
				sscanf(argv[6],"assoc:%d",&l2_assoc);
				l2_setnum = l2_cachesize/block_size/l2_assoc;
			}else if(argv[6][0]=='d'){
				l2_assoc = 1;
				l2_setnum =  l2_cachesize/block_size;
			}else{
				l2_setnum = 1;
				l2_assoc = l2_cachesize/block_size;
			}
			s2 = get_snum(l2_setnum);
			struct cache** l1 = allocate(l1_setnum, l1_assoc);
			struct cache** l2 = allocate(l2_setnum, l2_assoc);
			while(fscanf(file, "%c %lx\n", &op, &address)!=EOF){
				set_index1 = (address>>b)&((1<<s1)-1);
				l1_tag = address>>(b+s1);
				set_bits = (address>>b)&((1<<s2)-1);
				tag_index = address>>(b+s2);
			//	new = l1_fifoRead(l1,l1_assoc,l1_tag,set_index1,address);
			//	printf("new is:%zu\n",new);
				if(op=='R'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						//address found in l1
						new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
					//	memread++;
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
							//	memread++;
								l2_hit++;
								new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2,l2_assoc,tag_index,set_bits);

								if(new!=0){
							//	memread++;
                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_fifoRead(l2,l2_assoc,l2_tag,set_index2,new);
								}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
							//evict things here in l1
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
							l2_fifoRead(l2,l2_assoc,l2_tag,set_index2,new);
							}
						//	printf("New address is:%zu\n", new);
						 }
					}
				}
				if(op=='W'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_lruWrite(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
					//	memread++;
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
						//	memread++;
							l2_hit++;
							new = l1_lruWrite(l1,l1_assoc,l1_tag,set_index1,address);
							empty(l2, l2_assoc, tag_index,set_bits);

							if(new!=0){
                                                        s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
                                                        l2_fifoWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_lruWrite(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
							l2_fifoWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
						//	printf("New address is:%zu\n", new);
						 }
					}

				}
			}
		//	l2_miss = memread;
			printf("memread:%d\n",memread);
			printf("memwrite:%d\n",memwrite);
			printf("l1cachehit:%d\n",l1_hit);
//			printf("l2cachehit:%d\n",l2_hit);
			printf("l1cachemiss:%d\n",l1_miss);
			printf("l2cachehit:%d\n",l2_hit);
			printf("l2cachemiss:%d\n",l2_miss);
			free_cache(l1,l1_setnum);
			free_cache(l2,l2_setnum);
			fclose(file);
		}
		if(argv[7][0]=='l'){
			//l2 lru
			FILE* file = fopen(argv[8], "r");
			if(argv[6][5]==':'){
				sscanf(argv[6],"assoc:%d",&l2_assoc);
				l2_setnum = l2_cachesize/block_size/l2_assoc;
			}else if(argv[6][0]=='d'){
				l2_assoc = 1;
				l2_setnum =  l2_cachesize/block_size;
			}else{
				l2_setnum = 1;
				l2_assoc = l2_cachesize/block_size;
			}
			struct cache** l1 = allocate(l1_setnum, l1_assoc);
			struct cache** l2 = allocate(l2_setnum, l2_assoc);
			while(fscanf(file, "%c %lx\n", &op, &address)!=EOF){
				set_index1 = (address>>b)&((1<<s1)-1);
				l1_tag = address>>(b+s1);
				s2 = get_snum(l2_setnum);
				set_bits = (address>>b)&((1<<s2)-1);
				tag_index = address>>(b+s2);
				if(op=='R'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
		//						memread++;
								l2_hit++;
								new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2, l2_assoc, tag_index,set_bits);

                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_lruRead(l2,l2_assoc,l2_tag,set_index2,new);
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_lruRead(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
							l2_tag = new>>(b+s2);
							l2_lruRead(l2,l2_assoc,l2_tag,set_index2,new);
							}
						 }
					}
				}
				if(op=='W'){
					int temp = search(l1,l1_assoc,l1_tag,set_index1);
					if(temp==1){
						new = l1_fifoWrite(l1,l1_assoc,l1_tag,set_index1,address);
					}else{
						int check = search(l2,l2_assoc,tag_index,set_bits);
						if(check==1){
							//hit in l2
                                                        //evicts occur
							//	memread++;
								l2_hit++;
								new = l1_lruWrite(l1,l1_assoc,l1_tag,set_index1,address);
								empty(l2, l2_assoc, tag_index,set_bits);

                                                                s2 = get_snum(l2_setnum);
                                                                set_index2 = (new>>b)&((1<<s2)-1);
                                                                l2_tag = new>>(b+s2);
                                                                l2_lruWrite(l2,l2_assoc,l2_tag,set_index2,new);
                                                 }else{
							memread++;
							l2_miss++;
							new = l1_lruWrite(l1,l1_assoc,l1_tag,set_index1,address);
							if(new!=0){
							s2 = get_snum(l2_setnum);
                                                        set_index2 = (new>>b)&((1<<s2)-1);
                                                        l2_tag = new>>(b+s2);
							l2_lruWrite(l2,l2_assoc,l2_tag,set_index2,new);
							}
						 }
					}

				}
			}
		//	l2_miss = memread;
			printf("memread:%d\n",memread);
			printf("memwrite:%d\n",memwrite);
			printf("l1cachehit:%d\n",l1_hit);
			//printf("l2cachehit:%d\n",l2_hit);
			printf("l1cachemiss:%d\n",l1_miss);
		        printf("l2cachehit:%d\n",l2_hit);
			printf("l2cachemiss:%d\n",l2_miss);
			free_cache(l1,l1_setnum);
			free_cache(l2,l2_setnum);
			fclose(file);

		}

	}
	return EXIT_SUCCESS;
}






