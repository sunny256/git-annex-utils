/* Copyright 2012 Steve Seltzer

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#if HAVE_GETOPT_H
#include <getopt.h>
#endif /* HAVE_GETOPT_H */
#ifdef HAVE_GMP_H
#include <gmp.h>
#endif /* HAVE_GMP_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include <stdio.h>
#include <stdlib.h>

/* local headers */
#include <opts.h>
#include <usage.h>
#include <common/returncodes.h>

/* Process the command line
 *
 * Does not return on error, version, or help.
 * returns - index of first non-option argument in argv[]
 */
int procopts(int argc, char *argv[]){
  int c;
  int digit_optind = 0;

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;

#if HAVE_GETOPT_LONG
    static struct option long_options[] = LONG_OPTS;
    c = getopt_long(argc, argv, SHORT_OPTS,
		    long_options, &option_index);
#else
    c = getopt(argc, argv, SHORT_OPTS);
#endif
    if (c == -1)
      break;

    switch (c) {
    case 0: /* this is a long only option */
      if(!strcmp(long_options[option_index].name,"help")){
	usage();
	exit(RTRN_OK);
      } else if(!strcmp(long_options[option_index].name,"si")){
	opt_si=1;
      }
      else
	exit(RTRN_ERR_CMDLINE);
      break;

    case '0': /* use null instead of '\n' at end of lines */
      opt_outputnullsep=1; /* print totals for all files, not just dirs */
      break;

    case 'B': /* set blocksize */
      if(optarg)
	setblocksize(optarg);
      else
	exit(RTRN_ERR_CMDLINE);
      break;

    case 'L': /* count only files that are locally available */
      opt_countlocal=1;
      opt_countremote=0;
      break;

    case 'R': /* count only files that are *not* locally available */
      opt_countlocal=0;
      opt_countremote=1;
      break;

    case 'V': /* print version */
      printf("%s: %s\n", opt_progname, PACKAGE_STRING);
      exit(RTRN_OK);
      break;

    case 'a': /* print totals for all files */
      opt_outputall=1; /* print totals for all files, not just dirs */
      break;

    case 'b': /* output bytes */
      mpz_set_ui(opt_blocksize,1);
      break;

    case 'c': /* print total of all cmdline args */
      opt_printtotal=1;
      break;

    case 'h': /* output "human readable" sizes */
      opt_humanreadable=1;
      break;

    case 'k': /* set blocksize to 1M */
      mpz_set_ui(opt_blocksize,1024);
      break;

    case 'm': /* set blocksize to 1M */
      mpz_set_ui(opt_blocksize,1024*1024);
      break;

    case 's': /* only output sizes for command line args */
      opt_summarize=1;
      break;

    default: /* this shouldn't happen, only if I screwup the options strings */
      fprintf(stderr,"%s: Unknown option %c, use --help for help.\n", opt_progname, c);
    case '?':
      exit(RTRN_ERR_CMDLINE);
      break;
    }
  }

  return optind;
}

/* set the blocksize */
/* TODO: use gmp_scanf to remove digit length limitation */
void setblocksize(const char *arg){
  char buff[MAX_BLOCKSIZE_ARG_DIGITS+1];
  size_t arglen;
  size_t suffixlen;
  const char *suffix;
  mpz_t multiplier;

  /* special case empty string */
  if(!strlen(arg)){
    fprintf(stderr,"%s: invalid --block-size argument ''\n",opt_progname);
    exit(RTRN_ERR_CMDLINE);
  }

  /* check the number of digits */
  arglen=strspn(arg,"0123456789");
  if(arglen>MAX_BLOCKSIZE_ARG_DIGITS){
    fprintf(stderr,"%s: error, you may not specify more than %zu digits for the block size.\n",opt_progname,(size_t)MAX_BLOCKSIZE_ARG_DIGITS);
    exit(RTRN_ERR_CMDLINE);
  }

  /* handle the suffix */
  suffix=arg+arglen;
  suffixlen=strlen(suffix);
  mpz_init_set_ui(multiplier,1024);
  switch(suffixlen){
  case 0:
    mpz_set_ui(multiplier,1);
    break;
  case 2:
    if(suffix[1]!='B'){
      mpz_set_ui(multiplier,0);
      break;
    }
    mpz_set_ui(multiplier,1000);
    /* fall through to single char suffix */
  case 1:
    switch(suffix[0]){
    case 'Y':
    case 'y':
      mpz_pow_ui(multiplier,multiplier,8);
      break;
    case 'Z':
    case 'z':
      mpz_pow_ui(multiplier,multiplier,7);
      break;
    case 'E':
    case 'e':
      mpz_pow_ui(multiplier,multiplier,6);
      break;
    case 'P':
    case 'p':
      mpz_pow_ui(multiplier,multiplier,5);
      break;
    case 'T':
    case 't':
      mpz_pow_ui(multiplier,multiplier,4);
      break;
    case 'G':
    case 'g':
      mpz_pow_ui(multiplier,multiplier,3);
      break;
    case 'M':
    case 'm':
      mpz_pow_ui(multiplier,multiplier,2);
      break;
    case 'K':
    case 'k':
      break;
    default:
      mpz_set_ui(multiplier,0);
    }
    break;
  default:
    mpz_set_ui(multiplier,0);
  }
  if(!mpz_cmp_ui(multiplier,0)){
    fprintf(stderr,"%s: error, blocksize suffix '%s' unkown\n",opt_progname,suffix);
    exit(RTRN_ERR_CMDLINE);
  }

  if(arglen==0) /* special case for no digits */
    mpz_swap(opt_blocksize,multiplier);
  else{
    /* copy the digits of the argument into our buffer */
    strncpy(buff,arg,arglen);
    buff[arglen]=0;

    /* parse it */
    mpz_set_str(opt_blocksize,buff,10);

    /* apply the suffix */
    mpz_mul(opt_blocksize,opt_blocksize,multiplier);
  }

  /* free the multiplier */
  mpz_clear(multiplier);
}

/* global program options, generally set by the command line */
char *opt_progname; /* holds the program's name */
mpz_t opt_blocksize; /* what blocksize to use for counting */
char opt_humanreadable=0; /* display output in a "human readable" format */
char opt_summarize=0; /* only display output for command line arguments */
char opt_outputall=0; /* print totals for all files, not just dirs */
char opt_printtotal=0; /* print total for all arguments */
char opt_outputnullsep=0; /* use a null instead of '\n' at the end of lines */
char opt_countlocal=1; /* count local files */
char opt_countremote=1; /* count remote files */
char opt_si=0; /* human-readable, use powers of 1000 instead of 1024 */
