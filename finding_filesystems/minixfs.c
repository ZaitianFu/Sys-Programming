/**
* Finding Filesystems Lab
* CS 241 - Fall 2018
*/

#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode *inode = get_inode(fs, path);
    struct stat buffer;
    int i = minixfs_stat(fs, path, &buffer);
    if(inode == NULL) {
      if(i == -1){
        errno = ENOENT;
      }
      return -1;
    }
    
      // set mode
      uint16_t mode = inode->mode;
     
      mode = new_permissions & 0x3ff;
      int t = mode >> 9;
      mode = mode | (t << 9);
      inode->mode = mode;

	int ret=clock_gettime(CLOCK_REALTIME, &inode->ctim);

      return ret;
    

}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
      inode *inode = get_inode(fs, path);
    struct stat buffer;
    int i = minixfs_stat(fs, path, &buffer);
    if(inode == NULL) {
      if(i == -1){
        errno = ENOENT;
      }
      return -1;
    }
    
      if(owner != (uid_t)-1){
        inode->uid = owner;
      }
      if(group != (gid_t)-1){
        inode->gid = group;
      }

	int ret=clock_gettime(CLOCK_REALTIME, &inode->ctim);
      return ret;
          

}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
/*
 inode *node = get_inode(fs, path);
if (node) return NULL;
if ((int)first_unused_inode(fs)==-1) return NULL;
inode* new=fs->inode_root+(int)first_unused_inode(fs);
new->nlink=1;
const char* buffer;
inode* parent=parent_directory(fs,path, &buffer);
if (! is_directory(parent)) return NULL;
init_inode(parent,new);
minixfs_dirent temp;
temp.name= (char*)buffer;
temp.inode_num=(int)first_unused_inode(fs);


clock_gettime(CLOCK_REALTIME, &parent->mtim);
    return new;
if (get_inode(fs, path) != NULL) {
	clock_gettime(CLOCK_REALTIME, &get_inode(fs, path)->ctim);
        return NULL;}

    const char *filename;
    inode *parentd = parent_directory(fs, path, &filename);
    inode *parent = parentd;

    if (!valid_filename(filename) || strlen(filename) > FILE_NAME_LENGTH || !is_directory(parentd))
        return NULL;

    data_block_number data = (parentd->size) / sizeof(data_block);
    data_block_number *blockarr = parentd->direct;
    int is_indirect = 0;
    if (data >= NUM_DIRECT_INODES) {
        if (parentd->indirect == UNASSIGNED_NODE) {
            inode_number b = add_single_indirect_block(fs, parentd);
            if (b == -1)
                return NULL;
        }
        blockarr = (data_block_number *)(fs->data_root + parentd->indirect);
        is_indirect = 1;
        data -= NUM_DIRECT_INODES;
    }

    int new = first_unused_inode(fs);
    init_inode(parentd, fs->inode_root + new);
    size_t lo = parent->size % sizeof(data_block);
    data_block *newblock = fs->data_root + blockarr[data];
    if (lo == 0) { 
        data_block_number b;
        if (is_indirect)
            b = add_data_block_to_indirect_block(fs, blockarr);
        else
            b = add_data_block_to_inode(fs, parentd);
        if (b == -1)
            return NULL;
        
        newblock = fs->data_root + b;
    }
    char *cpy = (char*)malloc(FILE_NAME_LENGTH);
    strncpy(cpy, filename,FILE_NAME_LENGTH);
    memcpy(((char *)newblock) + lo, cpy, FILE_NAME_LENGTH);
    free(cpy);
    parent->size += FILE_NAME_ENTRY; 

    inode* ret=fs->inode_root + new;
    return ret; */ 

if(get_inode(fs,path)) {
    clock_gettime(CLOCK_REALTIME, &get_inode(fs,path) -> ctim); 
    return NULL;
}
inode *new =  fs->inode_root + (int)first_unused_inode(fs);
const char *filename;
inode * parent = parent_directory(fs, path, &filename);

if((int)first_unused_inode(fs) == -1 || !parent || !is_directory(parent)) 
    return NULL;


minixfs_dirent newdirect;
char * ptr = NULL;
data_block * ptr2 = NULL;
newdirect.name = (char*)filename;
newdirect.inode_num = (inode_number)(int)first_unused_inode(fs);
init_inode(parent, new);

int numofblock = (int)parent->size /16; 
int offset = (int)parent->size % 16; 

if(offset == 0) {
    int a = -241;
    if(numofblock < NUM_DIRECT_INODES) 
      a = add_data_block_to_inode(fs, parent);
    
    if(a == -1) {
        free_inode(fs, new);
        return NULL;
    }
    int data_offset = (int)(parent -> direct[numofblock]);
    memset(fs -> data_root + data_offset, 0, sizeof(data_block));
    //  ptr = (char *)(fs -> data_root + data_offset);
    ptr = (char *)(fs -> data_root + data_offset);
} 

if(offset > 0) {
    
    if(numofblock < NUM_DIRECT_INODES) {
        int data_offset = (int)(parent -> direct[numofblock]);
        ptr2 = fs -> data_root + data_offset;
    }
    ptr = ((char *)ptr2) + offset;
}

make_string_from_dirent(ptr, (const minixfs_dirent)newdirect);
parent -> size = (int)parent->size + MAX_DIR_NAME_LEN;
clock_gettime(CLOCK_REALTIME, &parent -> mtim);

return new;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
	
    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    size_t countori = count;
  unsigned long block_count = (count + *off) / (16 * KILOBYTE);
  if((count + *off) % (16 * KILOBYTE) != 0) block_count++;
  if(block_count > NUM_DIRECT_INODES + NUM_INDIRECT_INODES){
    errno = ENOSPC;
    return -1;
  }
  if(minixfs_min_blockcount(fs, path, block_count) == -1){
    errno = ENOSPC;
    return -1;
  }

 inode *inodeget = get_inode(fs, path);
 int id = *off / (16 * KILOBYTE);
 int completed = 0;
 if(id < NUM_DIRECT_INODES){
    int offsetnum = *off % (16 * KILOBYTE);
  for(int i = id; i < NUM_DIRECT_INODES; i++){
   size_t writeamount = 16 * KILOBYTE - offsetnum;
   if(count > 16 * KILOBYTE) {
        memcpy(&fs->data_root[inodeget->direct[i]].data[offsetnum], buf + completed, writeamount);
         offsetnum = 0;
        completed += writeamount;
     count -= writeamount;
      }
   else {
    memcpy(&fs->data_root[inodeget->direct[i]].data[offsetnum], buf + completed, count);
    offsetnum = 0;
        completed += count;
     
     count -= count;
    break;
   }
  }
  if(count > 0){
   data_block indirec = fs->data_root[inodeget->indirect];
   for(int i = 0; i < (int)NUM_INDIRECT_INODES; i++){
    size_t writeamount = 16 * KILOBYTE - offsetnum;
    if(count > 16 * KILOBYTE) {
          memcpy(&fs->data_root[(int)indirec.data[4* i]].data[0], buf + completed, writeamount);
          completed += writeamount;
      count -= writeamount;

        }
    else {
     memcpy(&fs->data_root[(int)indirec.data[4* i]].data[0], buf + completed, count);
          completed += count;
          count -= count;
     break;
    }
   }
  }
 }

 else{
  int remain = *off - NUM_DIRECT_INODES * 16 *KILOBYTE;
  data_block indirec = fs->data_root[inodeget->indirect];
  int indirec_block_id = remain / (16 * KILOBYTE);

    int offsetnum = remain % (16 * KILOBYTE);

    for(int i = indirec_block_id; i < (int)NUM_INDIRECT_INODES; i++){
      size_t writeamount = 16 * KILOBYTE - offsetnum;
      if(count > 16 * KILOBYTE) {
        memcpy(&fs->data_root[(int)indirec.data[4* i]].data[offsetnum], buf + completed, writeamount);
        completed += writeamount;
        offsetnum = 0;
        count -= writeamount;
      }
      else {
        memcpy(&fs->data_root[(int)indirec.data[4* i]].data[offsetnum], buf + completed, count);
        completed += count;
        count -= count;
        break;
      }

    }
 }
if (*off + countori >= inodeget->size)
	inodeget->size=*off + countori ;
else
	inodeget->size=inodeget->size ;


    *off += completed;
    if(clock_gettime(CLOCK_REALTIME, &inodeget->mtim) == -1 || clock_gettime(CLOCK_REALTIME, &inodeget->atim) == -1 )
      return -1;
    

    return completed;

}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!
inode *inodeget = get_inode(fs, path);
    if(inodeget == NULL) {
      errno = ENOENT;
      return -1;
    }
  
    if(*off > (long)inodeget->size) 
      return 0;
      ssize_t readsize;
      if (count<= inodeget->size- *off)
        readsize=count;
      else
        readsize=inodeget->size- *off;
        
    int blockid = *off / (16 * KILOBYTE);
    int completed = 0;

    if(blockid < NUM_DIRECT_INODES){

      int offset = *off % (16 * KILOBYTE);
      for(int i = blockid; i < NUM_DIRECT_INODES; i++){
        int numread = 16 * KILOBYTE - offset;
        if(readsize > 16 * KILOBYTE) {
          memcpy(buf + completed, &fs->data_root[inodeget->direct[i]].data[offset], (size_t)numread);
          offset = 0;
          
          readsize-= (ssize_t)numread;
          completed += numread;

        }
        else {

          memcpy(buf + completed, &fs->data_root[inodeget->direct[i]].data[offset], readsize);
            offset = 0;
              readsize -= (ssize_t)readsize;
          completed += readsize;
        
          break;
        }

      }
              
      if(readsize > 0){
          
       data_block indirect = fs->data_root[inodeget->indirect];

        int block_num;
        for(int i = 0; i < (int)NUM_INDIRECT_INODES; i++){
          size_t numread = 16 * KILOBYTE/* - offset*/;
          block_num = indirect.data[i * 4];
          if(readsize > 16 * KILOBYTE) {
            memcpy(buf + completed, &fs->data_root[block_num].data[0], numread);
            completed += numread/* - offset*/;
            //offset = 0;
            readsize -= numread;
          }
          else {
            memcpy(buf + completed, &fs->data_root[block_num].data[0], readsize);
            completed += readsize;
//            offset = 0;
            readsize -= readsize;
            break;
          }
        }
      }
    }
    else{
      int remain = *off - NUM_DIRECT_INODES * 16 *KILOBYTE;
 data_block indirect = fs->data_root[inodeget->indirect];

      int offset = remain % (16 * KILOBYTE);
    int block_num;
      for(int i = remain / (16 * KILOBYTE); i < (int)NUM_INDIRECT_INODES; i++){
        size_t numread = 16 * KILOBYTE - offset;
        block_num = indirect.data[i * 4];
        if(readsize > 16 * KILOBYTE) {
          memcpy(buf + completed, &fs->data_root[block_num].data[offset], numread);
          completed += numread;
          offset = 0;
          readsize -= (ssize_t)numread;
        }
        else {
          memcpy(buf + completed, &fs->data_root[block_num].data[offset], readsize);
          //
          completed += readsize;
          readsize -= readsize;
          break;
        }

      }
    }
    if(clock_gettime(CLOCK_REALTIME, &inodeget->atim) == -1){
      return -1;
    }
    *off += completed;
    return completed;
}
