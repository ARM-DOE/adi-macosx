/*******************************************************************************
*
*  COPYRIGHT (C) 2010 Battelle Memorial Institute.  All Rights Reserved.
*
********************************************************************************
*
*  Author:
*     name:  Brian Ermold
*     phone: (509) 375-2277
*     email: brian.ermold@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 6128 $
*    $Author: ermold $
*    $Date: 2011-03-24 07:08:33 +0000 (Thu, 24 Mar 2011) $
*    $Version: dsdb-libdsdb3-1.2-0 $
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "dsdb3.h"

/*************************************************
*  Process Config
*/

void print_process_config_value(ProcConf *proc_conf)
{
    fprintf(stdout, " -> [");

    if (proc_conf->site)
        fprintf(stdout, "'%s', ", proc_conf->site);
    else
        fprintf(stdout, "'<NULL>', ");

    if (proc_conf->facility)
        fprintf(stdout, "'%s', ", proc_conf->facility);
    else
        fprintf(stdout, "'<NULL>', ");

    if (proc_conf->type)
        fprintf(stdout, "'%s', ", proc_conf->type);
    else
        fprintf(stdout, "'<NULL>', ");

    if (proc_conf->name)
        fprintf(stdout, "'%s', ", proc_conf->name);
    else
        fprintf(stdout, "'<NULL>', ");

    fprintf(stdout, "'%s', '%s']\n", proc_conf->key, proc_conf->value);
}

int get_process_config_values(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *key)
{
    ProcConf **conf_vals;
    int        status;
    int        row;

    if (!key) {
        key = "%";
    }

    fprintf(stdout,
        "get_process_config_values('%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, key);

    status = dsdb_get_process_config_values(
        dsdb, site, facility, proc_type, proc_name, key, &conf_vals);

    if (status == 1) {

        for (row = 0; conf_vals[row]; row++) {
            print_process_config_value(conf_vals[row]);
        }

        dsdb_free_process_config_values(conf_vals);
    }
    else if (status == 0) {
        fprintf(stdout, " -> No Matching Process Config Values Found\n");
    }

    return(status);
}

/*************************************************
*  Family Process
*/

void print_family_process(FamProc *fam_proc)
{
    fprintf(stdout, " -> ['%s', '%s', '%s', '%s', '%s', '%s']\n",
        fam_proc->category,
        fam_proc->class,
        fam_proc->site,
        fam_proc->facility,
        fam_proc->type,
        fam_proc->name);
}

int get_family_process(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    FamProc  *fam_proc;
    int       status;

    fprintf(stdout, "get_family_process('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_get_family_process(
        dsdb, site, facility, proc_type, proc_name, &fam_proc);

    if (status == 1) {
        print_family_process(fam_proc);
        dsdb_free_family_process(fam_proc);
    }
    else if (status == 0) {
        fprintf(stdout, " -> Family Process Not Found\n");
    }

    return(status);
}

int inquire_family_processes(
    DSDB       *dsdb,
    const char *category,
    const char *class,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    FamProc **fam_procs;
    int       status;
    int       row;

    fprintf(stdout,
        "inquire_family_processes('%s', '%s', '%s', '%s', '%s', '%s');\n",
        category, class, site, facility, proc_type, proc_name);

    status = dsdb_inquire_family_processes(dsdb,
        category, class, site, facility, proc_type, proc_name, &fam_procs);

    if (status == 1) {

        for (row = 0; fam_procs[row]; row++) {
            print_family_process(fam_procs[row]);
        }

        dsdb_free_family_processes(fam_procs);
    }
    else if (status == 0) {
        fprintf(stdout, " -> No Matching Family Processes Found\n");
    }

    return(status);
}

/*************************************************
*  Process Location
*/

int get_process_location(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    ProcLoc *proc_loc;
    int      status;

    fprintf(stdout, "get_process_location('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_get_process_location(
        dsdb, site, facility, proc_type, proc_name, &proc_loc);

    if (status == 1) {

        fprintf(stdout, " -> ['%s' '%.4f' '%.4f' '%.4f']\n",
            proc_loc->name,
            proc_loc->lat,
            proc_loc->lon,
            proc_loc->alt);

        dsdb_free_process_location(proc_loc);
    }
    else if (status == 0) {
        fprintf(stdout, " -> Process Not Found\n");
    }

    return(status);
}

/*************************************************
*  Process State
*/

int delete_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    int status;

    fprintf(stdout, "delete_process_state('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_delete_process_state(
        dsdb, site, facility, proc_type, proc_name);

    if (status == 1) {
        fprintf(stdout, " -> Deleted Process State\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Process State Not Found\n");
    }

    return(status);
}

int get_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    ProcState *proc_state;
    int        status;
    char      *state_name;
    char      *state_text;
    char       state_enabled[8];
    char       last_updated[32];

    fprintf(stdout, "get_process_state('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_get_process_state(
        dsdb, site, facility, proc_type, proc_name, &proc_state);

    if (status == 1) {

        state_name = (proc_state->name) ? proc_state->name : "NULL";
        state_text = (proc_state->text) ? proc_state->text : "NULL";

        dsdb_bool_to_text(dsdb, proc_state->is_enabled, state_enabled);

        if (proc_state->last_updated) {
            dsdb_time_to_text(dsdb, proc_state->last_updated, last_updated);
        }
        else {
            strcpy(last_updated, "NULL");
        }

        fprintf(stdout, " -> ['%s' '%s' '%s' '%s']\n",
            state_name,
            state_enabled,
            state_text,
            last_updated);

        dsdb_free_process_state(proc_state);
    }
    else if (status == 0) {
        fprintf(stdout, " -> Process Not Found\n");
    }

    return(status);
}

int is_process_enabled(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    int status;

    fprintf(stdout, "is_process_enabled('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_is_process_enabled(
        dsdb, site, facility, proc_type, proc_name);

    if (status == 1) {
        fprintf(stdout, " -> Enabled\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Disabled\n");
    }

    return(status);
}

int update_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *state,
    const char *desc,
    time_t      state_time)
{
    int  status;
    char time_string[32];

    dsdb_time_to_text(dsdb, state_time, time_string);

    fprintf(stdout,
        "update_process_state('%s', '%s', '%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, state, desc,
        time_string);

    status = dsdb_update_process_state(
        dsdb, site, facility, proc_type, proc_name,
        state, desc, state_time);

    if (status == 1) {
        fprintf(stdout, " -> Updated Process State\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Unexpected NULL result from database query\n");
    }

    return(status);
}

/*************************************************
*  Process Status
*/

int delete_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    int status;

    fprintf(stdout, "delete_process_status('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_delete_process_status(
        dsdb, site, facility, proc_type, proc_name);

    if (status == 1) {
        fprintf(stdout, " -> Deleted Process Status\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Process Status Not Found\n");
    }

    return(status);
}

int get_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    ProcStatus *proc_status;
    int         status;
    char       *status_name;
    char       *status_text;
    char        is_successful[8];
    char        last_started[32];
    char        last_completed[32];
    char        last_successful[32];

    fprintf(stdout, "get_process_status('%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name);

    status = dsdb_get_process_status(
        dsdb, site, facility, proc_type, proc_name, &proc_status);

    if (status == 1) {

        status_name = (proc_status->name) ? proc_status->name : "NULL";
        status_text = (proc_status->text) ? proc_status->text : "NULL";

        dsdb_bool_to_text(dsdb, proc_status->is_successful, is_successful);

        if (proc_status->last_started) {
            dsdb_time_to_text(dsdb, proc_status->last_started, last_started);
        }
        else {
            strcpy(last_started, "NULL");
        }

        if (proc_status->last_completed) {
            dsdb_time_to_text(dsdb, proc_status->last_completed, last_completed);
        }
        else {
            strcpy(last_completed, "NULL");
        }

        if (proc_status->last_successful) {
            dsdb_time_to_text(dsdb, proc_status->last_successful, last_successful);
        }
        else {
            strcpy(last_successful, "NULL");
        }

        fprintf(stdout, " -> ['%s' '%s' '%s' '%s' '%s' '%s']\n",
            status_name,
            status_text,
            is_successful,
            last_started,
            last_completed,
            last_successful);

        dsdb_free_process_status(proc_status);
    }
    else if (status == 0) {
        fprintf(stdout, " -> Process Not Found\n");
    }

    return(status);
}

int update_process_started(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    time_t      started_time)
{
    int  status;
    char time_string[32];

    dsdb_time_to_text(dsdb, started_time, time_string);

    fprintf(stdout, "update_process_started('%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, time_string);

    status = dsdb_update_process_started(
        dsdb, site, facility, proc_type, proc_name, started_time);

    if (status == 1) {
        fprintf(stdout, " -> Updated Process Started\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Unexpected NULL result from database query\n");
    }

    return(status);
}

int update_process_completed(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    time_t      completed_time)
{
    int  status;
    char time_string[32];

    dsdb_time_to_text(dsdb, completed_time, time_string);

    fprintf(stdout,
        "update_process_completed('%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, time_string);

    status = dsdb_update_process_completed(
        dsdb, site, facility, proc_type, proc_name, completed_time);

    if (status == 1) {
        fprintf(stdout, " -> Updated Process Completed\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Unexpected NULL result from database query\n");
    }

    return(status);
}

int update_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *proc_status,
    const char *desc,
    time_t      status_time)
{
    int  status;
    char time_string[32];

    dsdb_time_to_text(dsdb, status_time, time_string);

    fprintf(stdout,
        "update_process_status('%s', '%s', '%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, proc_status, desc, time_string);

    status = dsdb_update_process_status(
        dsdb, site, facility, proc_type, proc_name, proc_status, desc, status_time);

    if (status == 1) {
        fprintf(stdout, " -> Updated Process Status\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Unexpected NULL result from database query\n");
    }

    return(status);
}

/*************************************************
*  Process Input/Output Datastream Classes
*/

int get_process_inputs(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name)
{
    DSClass **ds_classes;
    int       status;
    int       row;

    fprintf(stdout, "get_process_inputs('%s', '%s');\n",
        proc_type, proc_name);

    status = dsdb_get_process_dsc_inputs(
        dsdb, proc_type, proc_name, &ds_classes);

    if (status > 0) {

        for (row = 0; ds_classes[row]; row++) {
            fprintf(stdout, " -> ['%s', '%s']\n",
                ds_classes[row]->name,
                ds_classes[row]->level);
        }
        
        dsdb_free_ds_classes(ds_classes);
    }
    else if (status == 0) {
        fprintf(stdout, " -> No Process Inputs Found\n");
    }

    return(status);
}

int get_process_outputs(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name)
{
    DSClass **ds_classes;
    int       status;
    int       row;

    fprintf(stdout, "get_process_outputs('%s', '%s');\n",
        proc_type, proc_name);

    status = dsdb_get_process_dsc_outputs(
        dsdb, proc_type, proc_name, &ds_classes);

    if (status > 0) {

        for (row = 0; ds_classes[row]; row++) {
            fprintf(stdout, " -> ['%s', '%s']\n",
                ds_classes[row]->name,
                ds_classes[row]->level);
        }

        dsdb_free_ds_classes(ds_classes);
    }
    else if (status == 0) {
        fprintf(stdout, " -> No Process Outputs Found\n");
    }

    return(status);
}

/*************************************************
*  Process Output Datastream Times
*/

int delete_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level)
{
    int status;

    fprintf(stdout,
        "delete_datastream_times('%s', '%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, dsc_name, dsc_level);

    status = dsdb_delete_process_output_ds_times(
        dsdb, site, facility, proc_type, proc_name, dsc_name, dsc_level);

    if (status == 1) {
        fprintf(stdout, " -> Deleted Datastream Times\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Datastream Not Found\n");
    }

    return(status);
}

int get_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level)
{
    DSTimes *ds_times;
    int      status;
    char     first[32];
    char     last[32];

    fprintf(stdout,
        "get_datastream_times('%s', '%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, dsc_name, dsc_level);

    status = dsdb_get_process_output_ds_times(dsdb,
        site, facility, proc_type, proc_name, dsc_name, dsc_level, &ds_times);

    if (status == 1) {

        if (ds_times->first.tv_sec) {
            dsdb_timeval_to_text(dsdb, &ds_times->first, first);
        }
        else {
            strcpy(first, "NULL");
        }

        if (ds_times->last.tv_sec) {
            dsdb_timeval_to_text(dsdb, &ds_times->last, last);
        }
        else {
            strcpy(last, "NULL");
        }

        fprintf(stdout, " -> ['%s' '%s']\n", first, last);

        dsdb_free_ds_times(ds_times);
    }
    else if (status == 0) {
        fprintf(stdout, " -> Datastream Not Found\n");
    }

    return(status);
}

int update_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level,
    time_t      first_time,
    time_t      last_time)
{
    int       status;
    char      first_time_str[32];
    char      last_time_str[32];
    timeval_t tv_first = {0,0};
    timeval_t tv_last  = {0,0};

    tv_first.tv_sec = first_time;
    tv_last.tv_sec  = last_time;

    fprintf(stdout,
        "update_datastream_times('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');\n",
        site, facility, proc_type, proc_name, dsc_name, dsc_level,
        dsdb_timeval_to_text(dsdb, &tv_first, first_time_str),
        dsdb_timeval_to_text(dsdb, &tv_last, last_time_str));

    status = dsdb_update_process_output_ds_times(
        dsdb, site, facility, proc_type, proc_name, dsc_name, dsc_level,
        &tv_first, &tv_last);

    if (status == 1) {
        fprintf(stdout, " -> Updated Datastream Times\n");
    }
    else if (status == 0) {
        fprintf(stdout, " -> Unexpected NULL result from database query\n");
    }

    return(status);
}

/*************************************************
*  Retriever
*/

int fill_retriever_values(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name)
{
    Retriever *retriever;
    int        status;
    int       i,j,k,ii,jj,kk,iii;
    struct tm    *ts;
    char        buf[80];
    time_t     tmptime;

    fprintf(stdout, "dsdb_get_retriever('%s', '%s');\n",
        proc_type, proc_name);

    status = dsdb_get_retriever(
        dsdb, proc_type, proc_name, &retriever);

    if (status == 1) {

         /* Print ds_groups data */
         fprintf(stdout, " -> ['%s', '%s','num groups = %d']\n",
              retriever->proc_type,
              retriever->proc_name,
              retriever->nret_ds_groups);

         for (i = 0; i < retriever->nret_ds_groups; i++) {
              if (retriever->ret_ds_groups[i]->name)
                fprintf(stdout, " #, group_name -> %d,['%s']\n",
                i, retriever->ret_ds_groups[i]->name);
              else
                fprintf(stdout, "'<NULL>'\n, ");

              /* Print ds_subgroups data */
              fprintf(stdout, "    -> ['num subgroups = %d']\n",
              retriever->ret_ds_groups[i]->nret_ds_subgroups);

              for (j = 0; j < retriever->ret_ds_groups[i]->nret_ds_subgroups; j++) {
                if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->name)
                  fprintf(stdout, "    #, subgrp_name -> %d, ['%s']\n",
                  j, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->name);
                else
                  fprintf(stdout, "'<NULL>'\n, ");

                /* Print ret_datastreams data */
                fprintf(stdout, "       -> ['num datastreams = %d']\n",
                retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->nret_datastreams);
                for (k = 0; k < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->nret_datastreams; k++) {
                  if (k==0) 
                    fprintf(stdout, "       ds name, level, subgrp prior, site, fac, site_dep, fac_dep, beg_dt_dep, end_dt_dep\n");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->name)
                    fprintf(stdout, "       #, ds -> %d, ['%s', '%s', '%d']",
                    k, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->name,
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->level,
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_ds_subgrp_priority);
                  else
                    fprintf(stdout, "'<NULL>', ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->site)
                    fprintf(stdout, " -> ['%s']",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->site);
                  else
                    fprintf(stdout, "'<NULL>', ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->facility)
                    fprintf(stdout, " -> ['%s']",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->facility);
                  else
                    fprintf(stdout, "'<NULL>', ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->site_dependency)
                    fprintf(stdout, " -> ['%s']",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->site_dependency);
                  else
                    fprintf(stdout, "'<NULL>', ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->facility_dependency)
                    fprintf(stdout, " -> ['%s']",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->facility_dependency);
                  else
                    fprintf(stdout, "'<NULL>', ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->begin_date_dependency) {
                    tmptime = retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->begin_date_dependency;
                    ts = localtime(&tmptime);
                    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts); 
                    fprintf(stdout, " -> ['%s']",buf);  
 
                  }
                  else
                    fprintf(stdout, "'<NULL>' ");
                  if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->end_date_dependency) {
                    tmptime = retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->end_date_dependency;
                    ts = localtime(&tmptime);
                    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts); 
                    fprintf(stdout, " -> ['%s']\n",buf);  
 
                  }
                  else
                    fprintf(stdout, "'<NULL>'\n ");

                  /* Print ret_vargroups data */
                  fprintf(stdout, "        ->['num vargroups = %d']\n",
                  retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->nret_vargroups);
                  for (ii = 0; ii < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->nret_vargroups; ii++) {
                    if (ii==0) 
                      fprintf(stdout, "         vrgp name, data_type, units, sample_period, start_offset, end_offset, max,min, delta, sys_name,req_to_run_flag, ret_qc_flag, qc_req_to_run_flag\n");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->name)
                      fprintf(stdout, "         #, vargrps ->%d, ['%s']",
                      ii, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->name);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->data_type)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->data_type);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->units)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->units);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->sample_period)
                      fprintf(stdout, " -> ['%d']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->sample_period);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->start_offset)
                      fprintf(stdout, " -> ['%d']",
                      *retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->start_offset);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->end_offset)
                      fprintf(stdout, " -> ['%d']",
                      *retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->end_offset);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->max)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->max);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->min)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->min);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->delta)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->delta);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_system_name)
                      fprintf(stdout, " -> ['%s']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_system_name);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->req_to_run_flag)
                      fprintf(stdout, " -> ['%d']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->req_to_run_flag);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_qc_flag)
                      fprintf(stdout, " -> ['%d']",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_qc_flag);
                    else
                      fprintf(stdout, "'<NULL>', ");
                    if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->qc_req_to_run_flag)
                      fprintf(stdout, " -> ['%d']\n",
                      retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->qc_req_to_run_flag);
                    else
                      fprintf(stdout, "'<NULL>'\n");

                    /* Print ret_varnames data */
                    fprintf(stdout, "            -> ['num varnames = %d']\n",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->nret_varnames);
                    for (jj = 0; jj < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->nret_varnames; jj++) {
                      if (jj==0) 
                        fprintf(stdout, "            varname name, priority \n");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_varnames[jj]->name)
                        fprintf(stdout, "            #, varname -> %d, ['%s']",
                        jj, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_varnames[jj]->name);
                      else
                        fprintf(stdout, "'<NULL>', ");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_varnames[jj]->priority)
                        fprintf(stdout, " -> ['%d']\n",
                        retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_varnames[jj]->priority);
                      else
                        fprintf(stdout, "'<NULL>'\n");
                    }/* end jj */


                    /* Print ret_coord_dims data */
                    fprintf(stdout, "                -> ['num coord_dims = %d']\n",
                    retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->nret_coord_dims);
                    if(retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims){
                      for (jj = 0; jj < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->nret_coord_dims; jj++) {
                      if (jj==0) 
                          fprintf(stdout, "                coorddim order, name, dim_intvl, dim_units \n");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->name)
                          fprintf(stdout, "                # coorddim -> %d, ['%s'] ",
                          jj,retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->name);
                       else
                          fprintf(stdout, "'<NULL>' ");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_order)
                          fprintf(stdout, "   -> ['%d']",
                          retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_order);
                      else
                        fprintf(stdout, "'<NULL>', ");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_interval)
                          fprintf(stdout, " -> ['%f']",
                          retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_interval);
                      else
                          fprintf(stdout, "'<NULL>', ");
                      if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->units)
                          fprintf(stdout, " -> ['%s']\n",
                          retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->units);
                       else
                          fprintf(stdout, "'<NULL>'\n");

                            fprintf(stdout, "                   -> ['num dim_datastreams = %d']\n",
                        retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->nret_dim_datastreams);
                        if(retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams){
                          for (kk = 0; kk < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->nret_dim_datastreams; kk++) {
                            if (kk==0) 
                              fprintf(stdout, "                   dim ds name, level, subgrp prior, site, fac, site_dep, fac_dep, beg_dt_dep, end_dt_dep\n");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->name)
                              fprintf(stdout, "                   #, dim_ds -> %d, ['%s', '%s', '%d']",
                              kk, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->name,
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->level,
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->ret_ds_subgrp_priority);
                            else
                              fprintf(stdout, "'<NULL>', ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->site)
                              fprintf(stdout, " -> ['%s']",
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->site);
                            else
                              fprintf(stdout, "'<NULL>', ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->facility)
                              fprintf(stdout, " -> ['%s']",
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->facility);
                            else
                              fprintf(stdout, "'<NULL>', ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->site_dependency)
                              fprintf(stdout, " -> ['%s']",
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->site_dependency);
                            else
                              fprintf(stdout, "'<NULL>', ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->facility_dependency)
                              fprintf(stdout, " -> ['%s']",
                              retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->facility_dependency);
                            else
                              fprintf(stdout, "'<NULL>', ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->begin_date_dependency) {
                              tmptime = retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->begin_date_dependency;
                              ts = localtime(&tmptime);
                              strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
                              fprintf(stdout, " -> ['%s']",buf);
                            }
                            else
                            fprintf(stdout, "'<NULL>' ");
                            if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->end_date_dependency) {
                              tmptime = retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->end_date_dependency;
                              ts = localtime(&tmptime);
                              strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
                              fprintf(stdout, " -> ['%s']\n",buf);
                            }
                            else
                              fprintf(stdout, "'<NULL>'\n ");
                           /* Print ret_varnames data */
                           fprintf(stdout, "                     -> ['num vardimnames = %d']\n",
                           retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->nret_vardimnames);
                           for (iii = 0; iii < retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->nret_vardimnames; iii++) {
                           if (iii==0)
                             fprintf(stdout, "                      vardimname name, priority \n");
                           if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->ret_vardimnames[iii]->name)
                             fprintf(stdout, "                      #, vardims -> %d, ['%s']",
                             iii, retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->ret_vardimnames[iii]->name);
                           else
                             fprintf(stdout, "'<NULL>', ");
                           if (retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->ret_vardimnames[iii]->priority)
                             fprintf(stdout, " -> ['%d']\n",
                             retriever->ret_ds_groups[i]->ret_ds_subgroups[j]->ret_datastreams[k]->ret_vargroups[ii]->ret_coord_dims[jj]->ret_dim_datastreams[kk]->ret_vardimnames[iii]->priority);
                           else
                             fprintf(stdout, "'<NULL>'\n");
                            }/* end iii */
                          } /* end kk */
                        } /* end ret_dim_datastreams exists */
                      }/* end jj */

                    } /* end ret_coord_dims exists */
                  } /* end ii */
                } /* end loop over datastreams */
              } /* end loop over subgroups */
         } /* end loop over groups */

        dsdb_free_retriever(retriever);
    } 
    else if (status == 0) {
        fprintf(stdout, " -> No Process Inputs Found\n");
    }
    
    return(status);
}

/*************************************************
*  Datastream Config
*/

void print_datastream_config_value(DSConf *ds_conf)
{
    fprintf(stdout, " -> [");

    if (ds_conf->site)
        fprintf(stdout, "'%s', ", ds_conf->site);
    else
        fprintf(stdout, "'<NULL>', ");

    if (ds_conf->facility)
        fprintf(stdout, "'%s', ", ds_conf->facility);
    else
        fprintf(stdout, "'<NULL>', ");

    if (ds_conf->name)
        fprintf(stdout, "'%s', ", ds_conf->name);
    else
        fprintf(stdout, "'<NULL>', ");

    if (ds_conf->level)
        fprintf(stdout, "'%s', ", ds_conf->level);
    else
        fprintf(stdout, "'<NULL>', ");

    fprintf(stdout, "'%s', '%s']\n", ds_conf->key, ds_conf->value);
}

int get_datastream_config_values(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *ds_name,
    const char *ds_level,
    const char *key)
{
    DSConf  **conf_vals;
    int       status;
    int       row;

    if (!key) {
        key = "%";
    }

    fprintf(stdout,
        "get_datastream_config_values('%s', '%s', '%s', '%s', '%s');\n",
        site, facility, ds_name, ds_level, key);

    status = dsdb_get_datastream_config_values(
        dsdb, site, facility, ds_name, ds_level, key, &conf_vals);

    if (status == 1) {

        for (row = 0; conf_vals[row]; row++) {
            print_datastream_config_value(conf_vals[row]);
        }

        dsdb_free_datastream_config_values(conf_vals);
    }
    else if (status == 0) {
        fprintf(stdout, " -> No Matching Datastream Config Values Found\n");
    }

    return(status);
}
