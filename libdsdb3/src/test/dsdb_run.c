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

#include <unistd.h>

#include "dsdb_run_commands.h"

const char *gProgramName;

int run_test(DSDB *dsdb);

void exit_usage()
{

fprintf(stdout,
"  USAGE: %s -a db_alias command\n"
"\n"
"    -a db_alias   => Database alias in the user's .db_connect file\n"
"    -d desc       => Description\n"
"    -f fac        => Facility Name\n"
"    -k key        => Config Key\n"
"    -n name:level => Datastream Class Name and Level\n"
"    -p type:name  => Process Type and Name\n"
"    -s site       => Site Name\n"
"    -t time       => Time string in the form 'YYYY-MM-DD hh:mm:ss'\n"
"    -v value      => Value\n"
"    -x ds_name:ds_level  => Datastraem Name and Level\n"
"\n"
"    command:\n"
"\n"
"        get_process_config_values -s site -f fac -p type:name [-k key]\n"
"\n"
"        get_family_process        -s site -f fac -p type:name\n"
"        inquire_family_processes  -s site -f fac -p type:name\n"
"\n"
"        get_process_location      -s site -f fac -p type:name\n"
"\n"
"        delete_process_state      -s site -f fac -p type:name\n"
"        get_process_state         -s site -f fac -p type:name\n"
"        is_process_enabled        -s site -f fac -p type:name\n"
"        update_process_state      -s site -f fac -p type:name -v state -d desc -t time\n"
"\n"
"        delete_process_status     -s site -f fac -p type:name\n"
"        get_process_status        -s site -f fac -p type:name\n"
"        update_process_started    -s site -f fac -p type:name -t time\n"
"        update_process_completed  -s site -f fac -p type:name -t time\n"
"        update_process_status     -s site -f fac -p type:name -v status -d desc -t time\n"
"\n"
"        get_process_inputs        -p type:name\n"
"        get_process_outputs       -p type:name\n"
"\n"
"        fill_retriever_values     -p type:name\n"
"        get_datastream_config_values -s site -f fac -x ds_name:ds_level[-k key]\n"
"\n"
"        delete_datastream_times   -s site -f fac -p type:name -n name:level\n"
"        get_datastream_times      -s site -f fac -p type:name -n name:level\n"
"        update_datastream_times   -s site -f fac -p type:name -n name:level -t first -t last\n"
"\n",
gProgramName);

    exit(1);
}

/*******************************************************************************
*  Main
*/

int main(int argc, char *argv[])
{
    const char *db_alias   = NULL;
    const char *command    = NULL;
    const char *site       = NULL;
    const char *facility   = NULL;
    const char *proc_type  = NULL;
    const char *proc_name  = NULL;
    const char *dsc_name   = NULL;
    const char *dsc_level  = NULL;
    const char *key        = NULL;
    const char *value      = NULL;
    const char *desc       = NULL;
    const char *time_text1 = NULL;
    const char *time_text2 = NULL;
    const char *ds_name    = NULL;
    const char *ds_level   = NULL;

    char   *switches;
    char    c;
    char   *colon;
    time_t  time1;
    time_t  time2;

    DSDB *dsdb;

    gProgramName = argv[0];

    while(--argc > 0) {
        if ((*++argv)[0] == '-') {
            switches = *argv;
            while (c = *++switches)
            switch(c) {
                case 'a': /* DB Alias */
                    db_alias = *++argv;
                    argc--;
                    break;
                case 'd': /* Description */
                    desc = *++argv;
                    argc--;
                    break;
                case 'f': /* Facility */
                    facility = *++argv;
                    argc--;
                    break;
                case 'k': /* Config Key */
                    key = *++argv;
                    argc--;
                    break;
                case 'n': /* Name:Level (Datastream Class) */
                    dsc_name  = *++argv;
                    colon     = strchr(dsc_name, ':');
                    *colon    = '\0';
                    dsc_level = ++colon;
                    argc--;
                    break;
                case 'p': /* Process Type:Name */
                    proc_type = *++argv;
                    colon     = strchr(proc_type, ':');
                    *colon    = '\0';
                    proc_name = ++colon;
                    argc--;
                    break;
                case 'x': /* Datastream Name:Level */
                    ds_name = *++argv;
                    colon     = strchr(ds_name, ':');
                    *colon    = '\0';
                    ds_level = ++colon;
                    argc--;
                    break;
                case 's': /* Site */
                    site = *++argv;
                    argc--;
                    break;
                case 't': /* Time */
                    if (!time_text1) {
                        time_text1 = *++argv;
                    }
                    else {
                        time_text2 = *++argv;
                    }
                    argc--;
                    break;
                case 'v': /* Value */
                    value = *++argv;
                    argc--;
                    break;
                default:
                    exit_usage();
            }
        }
        else if (!command) {
            command = *argv;
        }
        else {
            exit_usage();
        }
    }

    if (!db_alias || !command) {
        exit_usage();
    }

    /* redirect stderr to stdout */

    dup2(1, 2);

    /* make stdout unbuffered */

    setvbuf(stdout, NULL, _IONBF, 0);

    /* Create new DSDB "object" */

    dsdb = dsdb_create(db_alias);
    if (!dsdb) {
        return(1);
    }

    /* Connect to the database */

    if (!dsdb_connect(dsdb)) {
        dsdb_destroy(dsdb);
        return(1);
    }

    /* Convert times to seconds since 1970 */

    if (time_text1) {
        if (!dsdb_text_to_time(dsdb, time_text1, &time1)) {
            dsdb_destroy(dsdb);
            return(1);
        }
    }

    if (time_text2) {
        if (!dsdb_text_to_time(dsdb, time_text2, &time2)) {
            dsdb_destroy(dsdb);
            return(1);
        }
    }

    /* Run the predefined test procedures */

    if (strcmp(command, "test") == 0) {
        if (run_test(dsdb)) {
            dsdb_destroy(dsdb);
            return(0);
        }
        else {
            dsdb_destroy(dsdb);
            return(1);
        }
    }

    /* Process Functions */

    if (strcmp(command, "get_process_config_values") == 0) {
        get_process_config_values(
            dsdb, site, facility, proc_type, proc_name, key);
    }
    else if (strcmp(command, "get_family_process") == 0) {
        get_family_process(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "inquire_family_processes") == 0) {
        inquire_family_processes(
            dsdb, "%", "%", site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "get_process_location") == 0) {
        get_process_location(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "delete_process_state") == 0) {
        delete_process_state(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "get_process_state") == 0) {
        get_process_state(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "is_process_enabled") == 0) {
        is_process_enabled(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "update_process_state") == 0) {
        update_process_state(
            dsdb, site, facility, proc_type, proc_name,
            value, desc, time1);
    }
    else if (strcmp(command, "delete_process_status") == 0) {
        delete_process_status(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "get_process_status") == 0) {
        get_process_status(
            dsdb, site, facility, proc_type, proc_name);
    }
    else if (strcmp(command, "update_process_started") == 0) {
        update_process_started(
            dsdb, site, facility, proc_type, proc_name, time1);
    }
    else if (strcmp(command, "update_process_completed") == 0) {
        update_process_completed(
            dsdb, site, facility, proc_type, proc_name, time1);
    }
    else if (strcmp(command, "update_process_status") == 0) {
        update_process_status(
            dsdb, site, facility, proc_type, proc_name,
            value, desc, time1);
    }
    else if (strcmp(command, "get_process_inputs") == 0) {
        get_process_inputs(
            dsdb, proc_type, proc_name);
    }
    else if (strcmp(command, "get_process_outputs") == 0) {
        get_process_outputs(
            dsdb, proc_type, proc_name);
    }
    else if (strcmp(command, "delete_datastream_times") == 0) {
        delete_datastream_times(
            dsdb, site, facility, proc_type, proc_name, dsc_name, dsc_level);
    }
    else if (strcmp(command, "get_datastream_times") == 0) {
        get_datastream_times(
            dsdb, site, facility, proc_type, proc_name, dsc_name, dsc_level);
    }
    else if (strcmp(command, "update_datastream_times") == 0) {
        update_datastream_times(
            dsdb, site, facility, proc_type, proc_name, dsc_name, dsc_level,
            time1, time2);
    }
    else if (strcmp(command, "fill_retriever_values") == 0) {
        fill_retriever_values(
            dsdb, proc_type, proc_name);
    }
    else if (strcmp(command, "get_datastream_config_values") == 0) {
        get_datastream_config_values(
            dsdb, site, facility, ds_name, ds_level, key);
    }
    else {
        fprintf(stdout, "Unsupported command: %s\n", command);
    }

    /* Disconnect from the database */

    dsdb_disconnect(dsdb);
    dsdb_destroy(dsdb);

    return(0);
}
