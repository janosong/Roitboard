/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../mjpg_streamer.h"
#include "../../utils.h"

#define INPUT_PLUGIN_NAME "FILE input plugin"

/* private functions and variables to this plugin */
static pthread_t   reader;
static pthread_t   worker;
static globals     *pglobal;

void *worker_thread(void *);
void *reader_thread(void *);
void worker_cleanup(void *);
void help(void);

static int delay = 0;
static char *defPict = NULL;
static int plugin_number;

#define FIFOSTREAM "/tmp/myFIFO"
#define JPEGBUFFERSIZE 5000000
unsigned char buf1[JPEGBUFFERSIZE];
unsigned char buf2[JPEGBUFFERSIZE];

static int camJpegSize = 0;
pthread_mutex_t flagMutex = PTHREAD_MUTEX_INITIALIZER;

//static int test = 0;


/*** plugin interface functions ***/
int input_init(input_parameter *param, int id)
{
    int i;
    plugin_number = id;

    param->argv[0] = INPUT_PLUGIN_NAME;

    /* show all parameters for DBG purposes */
    for(i = 0; i < param->argc; i++) {
        DBG("argv[%d]=%s\n", i, param->argv[i]);
    }

    reset_getopt();
    while(1) {
        int option_index = 0, c = 0;
        static struct option long_options[] = {
            {"h", no_argument, 0, 0},
            {"help", no_argument, 0, 0},
            {"d", required_argument, 0, 0},
            {"delay", required_argument, 0, 0},
            {"p", required_argument, 0, 0},
            {"pict", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long_only(param->argc, param->argv, "", long_options, &option_index);

        /* no more options to parse */
        if(c == -1) break;

        /* unrecognized option */
        if(c == '?') {
            help();
            return 1;
        }

        switch(option_index) {
            /* h, help */
        case 0:
        case 1:
            DBG("case 0,1\n");
            help();
            return 1;
            break;

            /* d, delay */
        case 2:
        case 3:
            DBG("case 2,3\n");
            delay = atoi(optarg);
            break;
        case 4:
        case 5:
            DBG("case 4,5\n");
            defPict = optarg;
            break;


        default:
            DBG("default case\n");
            help();
            return 1;
        }
    }

    pglobal = param->global;

    IPRINT("forced delay......: %i\n", delay);

    return 0;
}

int input_stop(int id)
{
    DBG("will cancel input thread\n");
    pthread_cancel(worker);
    pthread_cancel(reader);

    return 0;
}

int input_run(int id)
{
    pglobal->in[id].buf = NULL;

    if(pthread_create(&worker, 0, worker_thread, NULL) != 0) {
        free(pglobal->in[id].buf);
        fprintf(stderr, "could not start worker thread\n");
        exit(EXIT_FAILURE);
    }
    pthread_detach(worker);

    if(pthread_create(&reader, 0, reader_thread, NULL) != 0) {
        fprintf(stderr, "could not start reader thread\n");
        exit(EXIT_FAILURE);
    }
    pthread_detach(reader);

    return 0;
}

/*** private functions for this plugin below ***/
void help(void)
{
    fprintf(stderr, " ---------------------------------------------------------------\n" \
    " Help for input plugin..: "INPUT_PLUGIN_NAME"\n" \
    " ---------------------------------------------------------------\n" \
    " The following parameters can be passed to this plugin:\n\n" \
    " [-d | --delay ]........: delay to pause between frames\n" \
    " ---------------------------------------------------------------\n");
}


/* the single writer thread */
void *reader_thread(void *arg)
{
  int i, size;

  ssize_t nr, off;
  int fp;

  if(mkfifo(FIFOSTREAM, 0666) != 0) {
    unlink(FIFOSTREAM);  // probably file already exists, delete the file
    if(mkfifo(FIFOSTREAM, 0666) != 0) {
      fprintf(stderr, "Create named pipe error %s\n", FIFOSTREAM);
      exit(-1);
    }
  }


  if ((fp = open(FIFOSTREAM, O_RDONLY, 0)) < 0 ) {
    fprintf(stderr, "Cannot open %s\n", FIFOSTREAM);
    exit(1);
  }

  i = 0;

  while(1) {

    off = 0;
    usleep(100);
    do {
      nr = read(fp, buf1 + off, 8192);
      off = off + nr;
    } while(nr == 8192);
    size = off;

    if (buf1[0] == 0xff && buf1[1] == 0xd8 && size != 0) {
      pthread_mutex_lock(&flagMutex);
      camJpegSize = size;
      memcpy(buf2, buf1, size);
      pthread_mutex_unlock(&flagMutex);

      printf("Frame %d, size %d\n", i, camJpegSize);
      i++;
    }

  }
}

/* the single writer thread */
void *worker_thread(void *arg)
{
//    char buffer[1<<16];
    int file, rv,  csize;
    size_t filesize = 0;
    struct stat stats;
    unsigned char *buf, *cbuf;
    char cwd[64];

    /* set cleanup handler to cleanup allocated ressources */
    pthread_cleanup_push(worker_cleanup, NULL);

	rv = file = open(defPict, O_RDONLY);
    if (rv == -1) {
			perror("could not open file for reading");
			goto exitWorker;
		}

    /* approximate size of file */
    rv = fstat(file, &stats);
    if(rv == -1) {
      perror("could not read statistics of file");
      close(file);
      goto exitWorker;
    }

    filesize = stats.st_size;

    buf = malloc(filesize + (1 << 16));
    if (buf == NULL) {
      fprintf(stderr, "could not allocate memory\n");
      close(file);
      goto exitWorker;
    }
   
    read(file, buf, filesize);
        
    while(!pglobal->stop) {

        // copy frame from file to global buffer 
        pthread_mutex_lock(&pglobal->in[plugin_number].db);

        cbuf = buf;
        csize = filesize;


        pthread_mutex_lock(&flagMutex);
        if (camJpegSize != 0) {
          cbuf = buf2;
          csize = camJpegSize;
        //camJpegSize = 0;
        }
        else {
          cbuf = buf;
          csize = filesize;
        }
        pthread_mutex_unlock(&flagMutex);


        // allocate memory for frame 
        if(pglobal->in[plugin_number].buf != NULL) 
          free(pglobal->in[plugin_number].buf);

        pglobal->in[plugin_number].buf = malloc(csize + (1 << 16));
        if(pglobal->in[plugin_number].buf == NULL) {
            fprintf(stderr, "could not allocate memory\n");
            break;
        }
        memcpy(pglobal->in[plugin_number].buf, cbuf, csize);
        pglobal->in[plugin_number].size = csize;


//        IPRINT("new frame copied (size: %d)\n", pglobal->in[plugin_number].size);
        DBG("new frame copied (size: %d)\n", pglobal->in[plugin_number].size);

        // signal fresh_frame 
        pthread_cond_broadcast(&pglobal->in[plugin_number].db_update);
        pthread_mutex_unlock(&pglobal->in[plugin_number].db);

        if(delay != 0) usleep(1000 * delay);

//        printf("== TEST WORKER %d ==\n", test);

    }

exitWorker:
    DBG("leaving input thread, calling cleanup function now\n");
    /* call cleanup handler, signal with the parameter */
    pthread_cleanup_pop(1);
    close(file);
    return NULL;
}

void worker_cleanup(void *arg)
{
    static unsigned char first_run = 1;

    if(!first_run) {
        DBG("already cleaned up ressources\n");
        return;
    }

    first_run = 0;
    DBG("cleaning up ressources allocated by input thread\n");

    if(pglobal->in[plugin_number].buf != NULL) free(pglobal->in[plugin_number].buf);
}




