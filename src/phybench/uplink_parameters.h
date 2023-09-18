#ifndef _UPLINK_PARAMETERS_H
#define _UPLINK_PARAMETERS_H

#include "uplink.h"
#include "ulRsGen.h"

#define COUNTDOWN 1

typedef struct {
    userS *first;
    userS *last;
} user_parameters;

typedef struct {
  short active;
  short inc;
  short count;
  short countdown;
  int active_min;
  int active_max;
  int active_sum;
  int active_frames;
  int activity[5];
} parameter_model;

typedef struct {
    int subframe;
    int bs;
    int crnti;
    int prbs;
    int layers;
    int mod;
    int typ;
} current_line;

void init_trace_file(const char *filename, current_line *cline, FILE** trc);
bool getfield(char* line, current_line *cline);
void init_parameter_model(parameter_model *pmodel, int *subframe);
void init_data(input_data *data);

#if PARAMETER_MODEL_VERIFICATION
user_parameters *uplink_parameters(parameter_model *pmodel, input_data *data, int *subframe, FILE **file);
#else
//user_parameters *uplink_parameters(parameter_model *pmodel, input_data *data, int *subframe);
user_parameters *uplink_parameters(parameter_model *pmodel, input_data *data, int *subframe, current_line *cline, FILE** trc);
#endif

#endif /* _UPLINK_PARAMETERS_H */
