/*
 *  Excalibur Computational Storage ioctl internals
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define FUSE_USE_VERSION 34

#include <fuse3/fuse_lowlevel.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/xattr.h>

#include "EXCBR_CSFS_fnct.h"
#include "EXCBR_CS_exec.h"

cs_fptr cs_cmd[CS_FNCT_END] = {
    [CS_UNDEF]           = cs_nop,
    [CS_COUNT_VOWEL]     = count_vowel,
    [CS_COUNT_CONSONANT] = count_consonant,
    [CS_GET_STATUS]      = cs_nop,
    [CS_SET_STATUS]      = cs_nop,
    [CS_MAX]             = d_cs_max,
    [CS_MIN]             = d_cs_min,
    [CS_SUM]             = cs_nop,
    [CS_PROD]            = cs_nop,
    [CS_LAND]            = cs_nop,
    [CS_LOR]             = cs_nop,
    [CS_BAND]            = cs_nop,
    [CS_BOR]             = cs_nop,
    [CS_MAXLOC]          = cs_nop,
    [CS_MINLOC]          = cs_nop,
    [CS_AVG_INT]         = i_cs_average,
    [CS_AVG_DOUBLE]      = d_cs_average,
    [CS_NOP]             = cs_nop,
};

void
cs_exec(fuse_req_t req, const cs_args_t *my_cs, void *read_bf)
{
	fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: fct_id  : %d\n", my_cs->fct_id);
	switch(my_cs->fct_id)
	{
    	case CS_GET_STATUS:
		case CS_SET_STATUS:
   		case CS_SUM:
   		case CS_PROD:
   		case CS_LAND:
   		case CS_LOR:
   		case CS_BAND:
   		case CS_BOR:
   		case CS_MAXLOC:
   		case CS_MINLOC:
			fuse_log(FUSE_LOG_DEBUG, "\ncs ioctl does non yet implements func id %d: exiting \n",
				     my_cs->fct_id);
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
			return;

		case CS_COUNT_VOWEL:
		case CS_COUNT_CONSONANT:
		case CS_AVG_INT:
		case CS_AVG_DOUBLE:
		case CS_MAX:
    	case CS_MIN:

   			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: in_bfsz : %ld\n", my_cs->in_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: offset : %ld\n", my_cs->offset);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: out_bfsz: %ld\n", my_cs->out_bfsz);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: executing command: %d: %s\n",
					 my_cs->fct_id, cs_get_fnct_desc(my_cs->fct_id));

			cs_args_t out_buf = *my_cs;

			cs_cmd[my_cs->fct_id](my_cs, &out_buf, read_bf);
			fuse_log(FUSE_LOG_DEBUG, "\n cs_ioctl: command executed");
			fuse_reply_ioctl(req, 0, &out_buf, sizeof(cs_args_t));
			break;

		default:
			fuse_log(FUSE_LOG_DEBUG, "\ncs ioctl does not support func id: %d: exiting \n",
					my_cs->fct_id);
			fuse_reply_ioctl(req, EINVAL, NULL , 0);
	}
}
