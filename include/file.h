#define buffer_size 	255 /* maximum valid length of filename */
struct filehandle
{
	struct  vnode *node;
	int 	flags;
	int		offset;
	char 	filename_buffer[buffer_size]; // file name from copyinstr   
};