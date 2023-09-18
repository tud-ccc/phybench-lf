/*******************************************************************************
 *                      LTE UPLINK RECEIVER PHY BENCHMARK                      *
 *                                                                             *
 * This file is distributed under the license terms given by LICENSE.TXT       *
 *******************************************************************************
 * Author: Magnus Sjalander                                                    *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uplink.h"
#include "uplink_parameters.h"
#include "def.h"


/* Create Data Frames */
void init_data(input_data *data) {
    int ofdm, frame;
    int i,slot;

    for (frame=0; frame<NMB_DATA_FRAMES; frame++) {
        for (slot=0; slot<NMB_SLOT; slot++) { 
            for (ofdm=0; ofdm<OFDM_IN_SLOT; ofdm++) {
                for (i=0; i<MAX_SC; i++) {
                    data[frame].in_data[slot][ofdm][0][i].re = rand();
                    data[frame].in_data[slot][ofdm][0][i].im = rand();
                    data[frame].in_data[slot][ofdm][1][i].re = rand();
                    data[frame].in_data[slot][ofdm][1][i].im = rand();
                    data[frame].in_data[slot][ofdm][2][i].re = rand();
                    data[frame].in_data[slot][ofdm][2][i].im = rand();
                    data[frame].in_data[slot][ofdm][3][i].re = rand();
                    data[frame].in_data[slot][ofdm][3][i].im = rand();
                }
            }
            for (i=0; i<MAX_SC; i++) {
                data[frame].fftw[slot][i].re = rand();
                data[frame].fftw[slot][i].im = rand();
            }
        }
        /* One reference symbol per layer */
        for (i=0; i<MAX_LAYERS; i++){
            RsGen(MAX_RB, data[frame].in_rs[slot][i]);
        }
    }
}

int subframe_data(int *subframe) {
  return ((*subframe)++)%NMB_DATA_FRAMES;
}

#define L1_QPSK  15
#define L2_QPSK  28
#define L3_QPSK  40
#define L4_QPSK  55
#define L1_16QAM 23
#define L2_16QAM 43
#define L3_16QAM 65
#define L4_16QAM 85
#define L1_64QAM 27
#define L2_64QAM 53
#define L3_64QAM 78
#define L4_64QAM 98.0

float user_load(int nmbRB, int nmbLayer, int mod) {
  switch(mod) {
  case MOD_QPSK:
    switch(nmbLayer) {
    case 1:
      return nmbRB*L1_QPSK/100;
    case 2:
      return nmbRB*L2_QPSK/100;
    case 3:
      return nmbRB*L3_QPSK/100;
    case 4:
      return nmbRB*L4_QPSK/100;
    }
  case MOD_16QAM:
    switch(nmbLayer) {
    case 1:
      return nmbRB*L1_16QAM/100;
    case 2:
      return nmbRB*L2_16QAM/100;
    case 3:
      return nmbRB*L3_16QAM/100;
    case 4:
      return nmbRB*L4_16QAM/100;
    }
  case MOD_64QAM:
    switch(nmbLayer) {
    case 1:
      return nmbRB*L1_64QAM/100;
    case 2:
      return nmbRB*L2_64QAM/100;
    case 3:
      return nmbRB*L3_64QAM/100;
    case 4:
      return nmbRB*L4_64QAM/100;
    }
  }
  return -1;
}


#if PARAMETER_MODEL_STEP_NEW
/* This model changes the probabillity for the number of layers and
  modulation that is used by a user. The probability starts low
  resulting in almost all users having one layer and using QPSK as
  modulation. The probabillity is increased over 170 steps until all
  users will have four layers and use 64 QAM modulation. When maximum
  load has been reached the probabillity is instead being reduced over
  170 steps until it reaches the same level as in the beginning. */

#define COUNTDOWN 1

void init_parameter_model(parameter_model *pmodel, int *subframe) {
    *subframe = 0;
    pmodel->active = TASK_THREADS;
    pmodel->countdown = COUNTDOWN;
    pmodel->count = 1;
    pmodel->inc = 1;
    pmodel->active_min = 100;
    pmodel->active_max = 0;
    pmodel->active_sum = 0;
    pmodel->active_frames = 0;
    pmodel->activity[0] = 0;
    pmodel->activity[1] = 0;
    pmodel->activity[2] = 0;

    //printf("\n");
    //printf("#Users TotalRB MaxRB MinRB MaxLayer MinLayer MaxMod MinMod Count\n");
}


user_parameters *uplink_parameters(
  parameter_model *pmodel,
  input_data *data,
  int *subframe,
  current_line *cline,
  FILE** trc
) {
    int userid = 0;
    struct userS *user = NULL;
    //int seqid=-1;
    char line[1024];
    static bool is_empty = false;
    int current_subframe = cline->subframe;
    int current_bs = cline->bs;
    user_parameters *parameters = NULL;

    if (is_empty) {
      fclose(*trc);
      return NULL;
    }

    do{
      if(cline->bs >= 0){
        /* Allocate a new user and add it to the parameter list*/
        if(user == NULL) {
          user = (struct userS*)malloc(sizeof(struct userS));
          user->next = NULL;
          parameters = (user_parameters*)malloc(sizeof(user_parameters));
          parameters->first = user;
        }
        else {
            user->next = (struct userS*)malloc(sizeof(struct userS));
            user = user->next;
            user->next = NULL;
        }

        if(user) {
          /* Assign the values read to the users  */
          user->subframe = cline->subframe;
          user->startRB  = 0;     // Assuming different users will have different data
          user->nmbRB	   = cline->prbs;
          switch (cline->mod) {
              case 0 : user->mod = MOD_PSK;
                          break;
              case 1 : user->mod = MOD_QPSK;
                          break;
              case 2 : user->mod = MOD_16QAM;
                          break;
              case 3 : user->mod = MOD_64QAM;
                          break;
              default : user->mod = MOD_QPSK;
          }
          user->nmbLayer = cline->layers;
          user->data     = &data[cline->subframe%NMB_DATA_FRAMES]; // &data[subframe_data(subframe)];
          user->next 	   = NULL;
          userid++;
        }
        else {
            fprintf(stderr,"user creation failed\n");
            exit(EXIT_FAILURE);
        }
      }
      else {
        user = (struct userS*)malloc(sizeof(struct userS));
        user->nmbRB = -1;
        parameters = (user_parameters*)malloc(sizeof(user_parameters));
        parameters->first = user;
      }
      parameters->last = user;

      //read next line
      if (is_empty = fgets(line, 1024, *trc) == NULL)
        break;
      getfield(line, cline);
    }
    while (current_subframe == cline->subframe);

    return parameters;
}

void init_trace_file(const char *filename, current_line *cline, FILE** trc) {
      if(!(*trc = fopen(filename,"r")))
        perror(filename);
      char log[BUFSIZ];
      char line[1024];
      if(!trc) {
          printf("Error opening trace file : %s",filename);
      }
      else {
          fgets(line, 1024, *trc); // remove header line
          bool is_empty = false;
          fgets(line, 1024, *trc); // read first valid line
          getfield(line, cline);
      }
}

bool getfield(char* line, current_line *cline)
{
    cline->subframe = atoi(strsep(&line, ","));
    char* valid = strsep(&line, ",");
    if (*valid == '\0'){
        cline->bs = -1;
        return false;
    }
    else {
        cline->bs = atoi(valid);
        cline->crnti = atoi(strsep(&line, ","));
        cline->prbs = atoi(strsep(&line, ","));
        cline->layers = atoi(strsep(&line, ","));
        cline->mod = atoi(strsep(&line, ","));
        cline->typ = atoi(strsep(&line, ","));
    }
    //printf("%d, %d, %d, %d, %d, %d, %d\n", cline->subframe, cline->bs, cline->crnti, cline->prbs, cline->layers, cline->mod, cline->typ);
    return true;
}


#endif
