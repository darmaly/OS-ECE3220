/*
  Brandon White
  Deeb Armaly
*/

#include "btrfs.h"

/* implementation of assigned functions */

/* you are welcome to use helper functions of your own */

/* btrfs_delete()
 *
 * deletes a closed directory entry having the given file descriptor
 *   (changes the status of the entry to unused) and releases all
 *   allocated file blocks
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is closed
 *
 * postconditions:
 *   (1) the status of the directory entry is set to unused
 *   (2) all file blocks have been set to free
 *
 * input parameter is a file descriptor
 *
 * return value is TRUE when successful or FALSE when failure
 */

unsigned int btrfs_delete(unsigned int file_descriptor) {
  /* your code here */
  if (!btrfs_check_fd_in_range(file_descriptor)) return (FALSE);
  if (directory[file_descriptor].status != CLOSED) return (FALSE);
  
  unsigned short i, j, fat_index, prev_index;
  
  fat_index = directory[file_descriptor].first_block;
  j = 0;
  for (i = 0; i < directory[file_descriptor].size; i++) {
    if (i % BLOCK_SIZE == 0 && i != 0) {
      prev_index                          = fat_index;
      fat_index                           = file_allocation_table[fat_index];
      file_allocation_table[prev_index]   = FREE;
      j                                   = 0;
    }
    blocks[fat_index].bytes[j] = 0;
  }
  file_allocation_table[fat_index]        = FREE;
  free_directory(file_descriptor);

  return TRUE;
}

/* btrfs_read()
 *
 * reads a specified number of bytes from a file starting
 *   at the byte offset in the directory into the specified
 *   buffer; the byte offset in the directory entry is
 *   incremented by the number of bytes transferred
 *
 * depending on the starting byte offset and the specified
 *   number of bytes to transfer, the transfer may cross two
 *   or more file blocks
 *
 * the function will read fewer bytes than specified if the
 *   end of the file is encountered before the specified number
 *   of bytes have been transferred
 *
 * preconditions:
 *   (1) the file descriptor is in range
 *   (2) the directory entry is open
 *   (3) the file has allocated file blocks
 *
 * postconditions:
 *   (1) the buffer contains bytes transferred from file blocks
 *   (2) the specified number of bytes has been transferred
 *         except in the case that end of file was encountered
 *         before the transfer was complete
 *
 * input parameters are a file descriptor, the address of a
 *   buffer of bytes to transfer, and the count of bytes to
 *   transfer
 *
 * return value is the number of bytes transferred
 */

unsigned int btrfs_read(unsigned int file_descriptor, char *buffer, unsigned int byte_count){
  /* your code here */
  if (!btrfs_check_fd_in_range (file_descriptor))     return (FALSE);
  if (!btrfs_check_file_is_open(file_descriptor))     return (FALSE);
  if (directory[file_descriptor].first_block == 0)  return (FALSE);

  unsigned int i;
  unsigned int count        = directory[file_descriptor].byte_offset / BLOCK_SIZE;
  unsigned int offset       = directory[file_descriptor].byte_offset % BLOCK_SIZE;
  unsigned int fat_index    = directory[file_descriptor].first_block;
  unsigned int return_size  = directory[file_descriptor].size - count * BLOCK_SIZE; 
  
  // get to the correct FAT index
  for (i = 0; i < count; i++) {
    fat_index = file_allocation_table[fat_index];
    if (fat_index == 0) return 0;
  }
  i = 0;
  for (; i < byte_count; i++) {
    if (offset == return_size) break;
    // iterate until end of file
    if (offset == BLOCK_SIZE) {
      fat_index = file_allocation_table[fat_index];
      offset = 0;
      return_size -= BLOCK_SIZE;
      if (fat_index == LAST_BLOCK) break;
    }
    buffer[i] = blocks[fat_index].bytes[offset];
    offset++;
  }
  return i;
}


   /* btrfs_write()
    *
    * writes a specified number of bytes from a specified buffer
    *   into a file starting at the byte offset in the directory;
    *   the byte offset in the directory entry is incremented by
    *   the number of bytes transferred
    *
    * depending on the starting byte offset and the specified
    *   number of bytes to transfer, the transfer may cross two
    *   or more file blocks
    *
    * furthermore, depending on the starting byte offset and the
    *   specified number of bytes to transfer, additional file
    *   blocks, if available, will be added to the file as needed;
    *   in this case, the size of the file will be adjusted
    *   based on the number of bytes transferred beyond the
    *   original size of the file
    *
    * the function will read fewer bytes than specified if file
    *   blocks are not available
    *
    * preconditions:
    *   (1) the file descriptor is in range
    *   (2) the directory entry is open
    *
    * postconditions:
    *   (1) the file contains bytes transferred from the buffer
    *   (2) the specified number of bytes has been transferred
    *         except in the case that file blocks needed to
    *         complete the transfer were not available
    *   (3) the size of the file is increased as appropriate
    *         when transferred bytes extend beyond the previous
    *         end of the file
    *
    * input parameters are a file descriptor, the address of a
     byte_index = directory[file_descriptor].byte_offset;
    *   transfer
    *
    * return value is the number of bytes transferred
    */

   unsigned int btrfs_write(unsigned int file_descriptor, char *buffer, unsigned int byte_count){

    /* your code here */
    if (!btrfs_check_fd_in_range (file_descriptor)) return (0);
    if (!btrfs_check_file_is_open(file_descriptor)) return (0);
    unsigned int i, fat_index, byte_index;

    // if first block not set, set it and the fat_index
    if (directory[file_descriptor].first_block == 0)
      directory[file_descriptor].first_block = fat_index = btrfs_new_block();
    else {
      fat_index   = directory[file_descriptor].first_block;
      while (file_allocation_table[fat_index] != LAST_BLOCK) {
        fat_index = file_allocation_table[fat_index];
      }
    }

    // byte_index starts @ the remaineder of the block size
    byte_index = directory[file_descriptor].byte_offset % BLOCK_SIZE;
    for (i = 0; i < byte_count; i++, byte_index++) {
      if (byte_index == BLOCK_SIZE) {
        byte_index                          = 0;
        file_allocation_table[fat_index]    = LAST_BLOCK;
        file_allocation_table[fat_index]    = btrfs_new_block();
        fat_index                           = btrfs_new_block();
        if (fat_index == 0) break;
      }
      blocks[fat_index].bytes[byte_index]  = buffer[i];
     }
     // keep track of the last FAT index
    directory[file_descriptor].byte_offset = i;
    file_allocation_table[fat_index]       = LAST_BLOCK;
    directory[file_descriptor].size += i;
    return i;
   }
   

void free_directory(unsigned int fd) {
  directory[fd].byte_offset  = 0;
  directory[fd].first_block  = 0;
  directory[fd].name[0]      = '\0';
  directory[fd].size         = 0;
  directory[fd].status       = UNUSED;
}
