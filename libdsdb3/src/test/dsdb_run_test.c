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

#include "dsdb_run_commands.h"

#define TEST_SITE      "xyz"
#define TEST_FAC       "T1"
#define TEST_PROC_TYPE "Ingest"
#define TEST_PROC_NAME "test"

#define BAD_SITE      "jnk"
#define BAD_FAC       "J1"
#define BAD_PROC_TYPE "Junk"
#define BAD_PROC_NAME "junk"

void print_testing_functions_header(
    const char *site,
    const char *fac,
    const char *type,
    const char *name)
{
    fprintf(stdout,
        "\n"
        "=================================================================\n"
        "testing process functions for: ['%s', '%s', '%s', '%s']\n"
        "=================================================================\n",
        site, fac, type, name);
}

void print_connect_status(DSDB *dsdb)
{
    if (dsdb_is_connected(dsdb)) {
        fprintf(stdout, "CONNECTED:    ");
    }
    else {
        fprintf(stdout, "DISCONNECTED: ");
    }

    fprintf(stdout, "nreconnect = %d\n\n", dsdb->nreconnect);
}

int test_reconnect(DSDB *dsdb)
{
    fprintf(stdout,
        "\n"
        "=================================================================\n"
        "Performing dsdb_reconnect() test...\n\n");

    print_connect_status(dsdb);

    dsdb_disconnect(dsdb);

    print_connect_status(dsdb);

    if (!dsdb_connect(dsdb)) {
        fprintf(stdout,
        "=================================================================\n");
        return(0);
    }

    print_connect_status(dsdb);

    if (!dsdb_connect(dsdb)) {
        fprintf(stdout,
        "=================================================================\n");
        return(0);
    }

    print_connect_status(dsdb);

    dsdb_disconnect(dsdb);

    print_connect_status(dsdb);

    dsdb_disconnect(dsdb);

    print_connect_status(dsdb);

    if (!dsdb_connect(dsdb)) {
        fprintf(stdout,
        "=================================================================\n");
        return(0);
    }

    print_connect_status(dsdb);

    fprintf(stdout,
        "PASSED\n"
        "=================================================================\n");

    return(1);
}

int test_process_procedures(
    DSDB       *dsdb,
    const char *site,
    const char *facility,
    const char *proc_type,
    const char *proc_name)
{
    char enable_time_str[]    = "2005-12-04 12:04:32";
    char disable_time_str[]   = "2006-04-20 05:16:47";
    char started_time_str[]   = "2005-12-31 23:59:59";
    char success_time_str[]   = "2006-01-01 01:07:23";
    char failed_time_str[]    = "2006-01-01 02:01:11";
    char completed_time_str[] = "2006-01-01 03:11:06";

    char ds_first_time_str1[] = "2005-12-31 23:59:59";
    char ds_last_time_str1[]  = "2006-01-01 00:00:00";

    char ds_first_time_str2[] = "2005-12-30 15:28:05";
    char ds_last_time_str2[]  = "2005-12-30 16:32:18";

    char ds_first_time_str3[] = "2005-12-30 20:20:20";
    char ds_last_time_str3[]  = "2006-01-01 01:23:38";

    char ds_first_time_str4[] = "2006-01-01 01:23:38";
    char ds_last_time_str4[]  = "2006-01-01 02:23:38";

    time_t    enable_time;
    time_t    disable_time;
    time_t    started_time;
    time_t    success_time;
    time_t    failed_time;
    time_t    completed_time;

    timeval_t ds_first_time1;
    timeval_t ds_first_time2;
    timeval_t ds_first_time3;
    timeval_t ds_first_time4;
    timeval_t ds_last_time1;
    timeval_t ds_last_time2;
    timeval_t ds_last_time3;
    timeval_t ds_last_time4;

    dsdb_text_to_time(dsdb, enable_time_str,    &enable_time);
    dsdb_text_to_time(dsdb, disable_time_str,   &disable_time);
    dsdb_text_to_time(dsdb, started_time_str,   &started_time);
    dsdb_text_to_time(dsdb, success_time_str,   &success_time);
    dsdb_text_to_time(dsdb, failed_time_str,    &failed_time);
    dsdb_text_to_time(dsdb, completed_time_str, &completed_time);

    dsdb_text_to_timeval(dsdb, ds_first_time_str1, &ds_first_time1);
    dsdb_text_to_timeval(dsdb, ds_first_time_str2, &ds_first_time2);
    dsdb_text_to_timeval(dsdb, ds_first_time_str3, &ds_first_time3);
    dsdb_text_to_timeval(dsdb, ds_first_time_str4, &ds_first_time4);
    dsdb_text_to_timeval(dsdb, ds_last_time_str1,  &ds_last_time1);
    dsdb_text_to_timeval(dsdb, ds_last_time_str2,  &ds_last_time2);
    dsdb_text_to_timeval(dsdb, ds_last_time_str3,  &ds_last_time3);
    dsdb_text_to_timeval(dsdb, ds_last_time_str4,  &ds_last_time4);

    fprintf(stdout, "\n");
    get_family_process(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    get_process_location(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout,
    "\n"
    "--------------------------------\n"
    "Testing Process State Procedures\n"
    "--------------------------------\n");

    fprintf(stdout, "\n");
    delete_process_state(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    get_process_state(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    is_process_enabled(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_state(dsdb, site, facility, proc_type, proc_name,
        "Enabled", "Test Enable", enable_time);

    fprintf(stdout, "\n");
    get_process_state(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    is_process_enabled(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_state(dsdb, site, facility, proc_type, proc_name,
        "Disabled", "Test Disable", disable_time);

    fprintf(stdout, "\n");
    get_process_state(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    is_process_enabled(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout,
    "\n----------\n"
    "\n"
    "** This next command should fail with a DBError **\n"
    "\n");
    update_process_state(dsdb, site, facility, proc_type, proc_name,
        "Disable", "Test DBError", enable_time);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    delete_process_state(dsdb, site, facility, proc_type, proc_name);

    if (!test_reconnect(dsdb)) {
        return(0);
    }

    fprintf(stdout,
    "\n"
    "--------------------------------\n"
    "Testing Process Status Procedures\n"
    "--------------------------------\n");

    fprintf(stdout, "\n");
    delete_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n");
    get_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_started(dsdb, site, facility, proc_type, proc_name,
        started_time);

    fprintf(stdout, "\n");
    get_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_status(dsdb, site, facility, proc_type, proc_name,
        "Success", "Test Success", success_time);

    fprintf(stdout, "\n");
    get_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_status(dsdb, site, facility, proc_type, proc_name,
        "Failure", "Test Failure", failed_time);

    fprintf(stdout, "\n");
    get_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_process_completed(dsdb, site, facility, proc_type, proc_name,
        completed_time);

    fprintf(stdout, "\n");
    get_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout,
    "\n----------\n"
    "\n"
    "** This next command should fail with a DBError **\n"
    "\n");
    update_process_status(dsdb, site, facility, proc_type, proc_name,
        "Failed", "Test Failed", failed_time);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    delete_process_status(dsdb, site, facility, proc_type, proc_name);

    fprintf(stdout,
    "\n"
    "----------------------------------------------------------\n"
    "Testing Process Input/Output Datastream Classes Procedures\n"
    "----------------------------------------------------------\n");

    fprintf(stdout, "\n");
    get_process_inputs(dsdb, proc_type, proc_name);

    fprintf(stdout, "\n");
    get_process_outputs(dsdb, proc_type, proc_name);

    fprintf(stdout,
    "\n"
    "--------------------------------------------\n"
    "Testing Process Output Datastream Procedures\n"
    "--------------------------------------------\n");

    fprintf(stdout, "\n");
    delete_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout, "\n");
    get_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1", ds_first_time1.tv_sec, ds_last_time1.tv_sec);

    fprintf(stdout, "\n");
    get_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1", ds_first_time2.tv_sec, ds_last_time2.tv_sec);

    fprintf(stdout, "\n");
    get_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1", ds_first_time3.tv_sec, ds_last_time3.tv_sec);

    fprintf(stdout, "\n");
    get_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    update_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1", ds_first_time4.tv_sec, ds_last_time4.tv_sec);

    fprintf(stdout, "\n");
    get_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    fprintf(stdout,
    "\n----------\n"
    "\n"
    "** This next command should fail with a DBError **\n"
    "\n");
    update_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "junk", "b1", ds_first_time4.tv_sec, ds_last_time4.tv_sec);

    fprintf(stdout, "\n----------\n");

    fprintf(stdout, "\n");
    delete_datastream_times(dsdb, site, facility, proc_type, proc_name,
        "test", "b1");

    return(1);
}

int run_test(DSDB *dsdb)
{
    /* Test inquire_family_processes */

    print_testing_functions_header("%", "%", "%", "%");

    fprintf(stdout, "\n");
    inquire_family_processes(dsdb, "Test", "%", "%", "%", "%", "%");

    fprintf(stdout, "\n");
    inquire_family_processes(dsdb, "Junk", "%", "%", "%", "%", "%");

    /* Test DB Reconnect */

    if (!test_reconnect(dsdb)) {
        return(0);
    }

    /* Test Process Procedures */

    print_testing_functions_header(
        TEST_SITE, TEST_FAC, TEST_PROC_TYPE, TEST_PROC_NAME);

    test_process_procedures(
        dsdb, TEST_SITE, TEST_FAC, TEST_PROC_TYPE, TEST_PROC_NAME);

    return(1);
}
