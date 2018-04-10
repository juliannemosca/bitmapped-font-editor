#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MODE_NOT_SET	0
#define MODE_CREAT	1
#define MODE_WRITE	2

#define CREAT_ARGS_NUM	3
#define WRITE_ARGS_NUM	3
#define MAX_ARGS_NUM 	3

/**
 * get the character's width in bytes
 *
 * @param ch_width:	the font character's "real" width
 * @return:		how many bytes it takes to store a row
 */
static int
get_char_width_size(int ch_width) {
  int width = ch_width / 8;
  if (ch_width % 8 > 0)
    width += 1;
  return width;
}

/**
 * get the character's size in bytes
 *
 * @param ch_width:	the font character's "real" width
 * @param ch_height:	the font character's "real" height
 * @return:		how many bytes it takes to store a character
 */
static int
get_char_size(int ch_width, int ch_height)
{
  if (ch_width == 0 || ch_height == 0)
    {
      fprintf(stderr, "error: Invalid value or values for character width/height (%d,%d).\n", ch_width, ch_height);
      exit(EXIT_FAILURE);
    }

  // font height could actually be arbitrary
  // so it is not necessary to make it a multiple of 16.
  //
  // if necessary uncomment and modify the following
  // commented parts.
  //
  //int height = ch_height / 16;
  //if (ch_height % 16 > 0)
  //  height += 1;

  //height = height * 16;

  return get_char_width_size(ch_width) * ch_height; //height;
}

/**
 * get the size of the padding for a new file
 *
 * @param ch_num:	the number of characters in the char. set
 * @param ch_width:	the font character's "real" width
 * @param ch_height:	the font character's "real" height
 * @return:		the size of the required padding in bytes
 */
static int
get_pad_size(int ch_num, int ch_width, int ch_height)
{
  if (ch_num == 0)
    {
      fprintf(stderr, "error: Invalid value for number of characters (%d).\n", ch_num);
      exit(EXIT_FAILURE);
    }
  return (get_char_size(ch_width, ch_height) * ch_num);
}

/**
 * fail and exit if a flag is set with another operation
 *
 * @param mode_flag:	current flag value
 */
static void
fail_if_set(int mode_flag)
{
  if (mode_flag != MODE_NOT_SET)
    {
      fprintf(stderr, "Invalid argument, cannot mix create and write modes.\n");
      exit(EXIT_FAILURE);
    }
}

/**
 * make a copy of the value of an arg.
 *
 * @param dst:		pointer by ref. to dst string
 * @param src:		pointer to src string
 * @return:		pointer to dst string
 */
static char *
cpy_opt_arg_val(char ** dst, const char * src)
{
  *dst = strdup(src);
  if (!*dst)
    {
      fprintf(stderr, "error: Cannot copy argument %s.", src);
      exit(EXIT_FAILURE);
    }
  return *dst;
}

/**
 *
 *
 *
 */
int
main(int argc, char *argv[])
{
  int ix 	= 0;
  int mode_flag = MODE_NOT_SET;
  long int option_arg_vals[MAX_ARGS_NUM] = {0};
  char * val 	= 0;
  int ini_pos 	= 0;

  long int * arg_width 	= NULL;
  long int * arg_height = NULL;

  // Pointer to the font filename.
  char *fontfname;

  // Vars. for getopt
  int c;
  opterr = 0;

  //
  // Parse command line options.
  //
  while((c = getopt(argc, argv, "n:w:")) != -1)
    {
      switch(c)
	{
	case 'n':
	  fail_if_set(mode_flag);
	  mode_flag = MODE_CREAT;

	  ini_pos = optind-1;
	  ix = ini_pos;
          while(ix < argc)
	    {
	      cpy_opt_arg_val(&val, argv[ix]);
	      option_arg_vals[ix-ini_pos] = strtol(val, NULL, 10);
	      free(val);

	      ix++;
	      if ((ix-ini_pos) == CREAT_ARGS_NUM)
		break;
	    }

	  break;
	case 'w':
	  fail_if_set(mode_flag);
	  mode_flag = MODE_WRITE;

	  ini_pos = optind-1;
	  ix = ini_pos;

          while(ix < argc)
	    {
	      cpy_opt_arg_val(&val, argv[ix]);
	      option_arg_vals[ix-ini_pos] = strtol(val, NULL, 10);
	      free(val);

	      ix++;
	      if ((ix-ini_pos) == WRITE_ARGS_NUM)
		break;
	    }

	  break;
	case '?':
	  if (optopt == 'n' || optopt == 'w')
	    fprintf (stderr, "error: Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
	    fprintf (stderr, "error: Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr,
		     "error: Unknown option character `\\x%x'.\n",
		     optopt);
	  exit(EXIT_FAILURE);
	default:
	  exit(EXIT_FAILURE);
	}
    }

  // lastly read in the font filename
  if (ix == (argc-1))
    {
      fontfname = argv[ix];
    }
  else if (ix < argc)
    {
      fprintf(stderr, "error: Too many arguments.\n");
      exit(EXIT_FAILURE);
    }
  else
    {
      fprintf(stderr, "error: Missing font file name after option arguments.\n");
      exit(EXIT_FAILURE);
    }

  //
  // Perform required operation
  //

  switch(mode_flag)
    {      
    case MODE_CREAT:
      {
	arg_width = &option_arg_vals[1];
	arg_height = &option_arg_vals[2];

	char * padding 	= 0;

	int pad_size = get_pad_size(option_arg_vals[0],		// num. of ch.
				    *arg_width,			// char. width
				    *arg_height);		// char. height

	FILE * fp = fopen(fontfname, "wbx");
	if (NULL == fp)
	  {
	    fprintf(stderr, "error: Cannot create file %s.\n", fontfname);
	    exit(EXIT_FAILURE);
	  }

	// create padding
	//
	if ( (padding = (char *)malloc(sizeof(char) * pad_size)) == NULL)
	  {
	    fprintf(stderr, "error: Cannot allocate memory on the heap.\n");

	    fclose(fp);
	    exit(EXIT_FAILURE);
	  }
	memset(padding, 0x00, pad_size);

	// write padding to file
	fwrite(padding, sizeof(char), pad_size, fp);

	// cleanup
	fclose(fp);
	free(padding);
      }
      break;
    case MODE_WRITE:
      {
	arg_width = &option_arg_vals[0];
	arg_height = &option_arg_vals[1];
	long int * arg_offset = &option_arg_vals[2];

	int ch_size = get_char_size(*arg_width,		// char. width
				    *arg_height);	// char. height

	FILE * fp = fopen(fontfname, "r+b");
	if (NULL == fp)
	  {
	    fprintf(stderr, "error: Cannot open file %s.\n", fontfname);
	    exit(EXIT_FAILURE);
	  }

	// calculate position for entering char
	//
	// (offset was not checked when parsed
	//  with strtol, but we don't care for now,
	//  assuming a 0 value is always valid).
	//
	int w_pos = *arg_offset * ch_size;

	fseek(fp, 0L, SEEK_END);
	long int sz = ftell(fp);
	if (sz < (w_pos + ch_size))
	  {
	    fprintf(stderr, "error: Invalid offset (%ld).\n", (*arg_offset));

	    fclose(fp);
	    exit(EXIT_FAILURE);
	  }

	// reset the pos indicator to the char's position
	fseek(fp, w_pos, SEEK_SET);

	// prepare to read from stdin as many rows as needed
	//
	char *line_buf;
	size_t line_len = 0;

	int rows_read = 0;
	long int * w_vals = 0;
	char * w_byte_vals = 0;

	int width_size = get_char_width_size(*arg_width);
	int bits_per_row = width_size * 8; // width * bits per byte

	// allocate memory as needed
	//
	if ( (w_vals = (long int*)malloc(sizeof(long int*) * (*arg_height))) == NULL )
	  {
	    fprintf(stderr, "error: Cannot allocate memory on the heap.\n");

	    fclose(fp);
	    exit(EXIT_FAILURE);
	  }
	if ( (w_byte_vals = (char*)malloc(sizeof(char*) * (*arg_width) * (*arg_height))) == NULL )
	  {
	    fprintf(stderr, "error: Cannot allocate memory on the heap.\n");

	    fclose(fp);
	    free(w_vals);
	    exit(EXIT_FAILURE);
	  }

	// read loop
	//
	while (rows_read < (*arg_height))
	  {
	    if (getline(&line_buf, &line_len, stdin) == -1)
	      {
		fprintf(stderr, "error: Invalid input for row (%d).\n", (rows_read+1));
		if (line_buf != NULL)
		  free(line_buf);

		fclose(fp);
		free(w_vals);
		free(w_byte_vals);
		exit(EXIT_FAILURE);
	      }

	    // trim any newline
	    if (line_buf[strlen(line_buf)-1] == '\n')
	      line_buf[strlen(line_buf)-1] = '\0';

	    // make sure to discard any exceeding input
	    // but allow to enter a padding up to the
	    // whole row
	    //
	    if (strlen(line_buf) > bits_per_row)
	      line_buf[bits_per_row] = 0;

	    // convert bits enterd in binary format to value
	    //
	    // (again, we're taking any 0 value returned
	    //  by strtol as valid)
	    w_vals[rows_read] = strtol(line_buf, NULL, 2);

	    rows_read++;
	  }

	// prepare to write each row to the file
	//
	char * cp_byte_p = NULL;
	int copied_bytes = 0;

	int rows_wr;
	int byte_offset;

	for (rows_wr = 0; rows_wr < rows_read; rows_wr++)
	  {
	    // we need to take one byte at a time,
	    // so the trick here is to use a char pointer
	    // to read the long int's bytes one by one
	    //
	    // being specific here to x86-64 little endianness (!)
	    // this means the first byte read is from the
	    // little end, so it has to be written to the back
	    //
	    cp_byte_p = (char*)&(w_vals[rows_wr]);
	    int position = copied_bytes;
	    for (byte_offset = (width_size-1); byte_offset >= 0; byte_offset--)
	      {
		w_byte_vals[position+byte_offset] = (*cp_byte_p) & 255;
		cp_byte_p++;
		copied_bytes++;
	      }
	  }

	// write the values to the file
	//
	fwrite(w_byte_vals, sizeof(char), copied_bytes, fp);

	// cleanup
	fclose(fp);
	free(w_vals);
	free(w_byte_vals);
      }
      break;
    default:
      return -1;
    }

  return 0;
}
