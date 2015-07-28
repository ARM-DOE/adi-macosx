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

#include "dsdb3.h"

/*  Process Config */

int get_process_config_values(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *key);

/*  Family Process  */

void print_family_process(FamProc *fam_proc);

int get_family_process(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int inquire_family_processes(
    DSDB       *dsdb,
    const char *category,
    const char *class,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

/*  Process Location  */

int get_process_location(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

/*  Process State  */

int delete_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int get_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int is_process_enabled(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int update_process_state(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *state,
    const char *desc,
    time_t      state_time);

/*  Process Status  */

int delete_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int get_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name);

int update_process_started(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    time_t      started_time);

int update_process_completed(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    time_t      completed_time);

int update_process_status(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *status,
    const char *desc,
    time_t      status_time);

/*  Process Input/Output Datastream Classes  */

int get_process_inputs(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name);

int get_process_outputs(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name);

/*  Process Output Datastreams  */

int delete_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level);

int get_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level);

int update_datastream_times(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name,
    const char *dsc_name,
    const char *dsc_level,
    time_t      first_time,
    time_t      last_time);

/*  Retriever  */

int fill_retriever_values(
    DSDB       *dsdb,
    const char *proc_type,
    const char *proc_name);


/*  Datastream Config */

int get_datastream_config_values(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *ds_name,
    const char *ds_type,
    const char *key);

