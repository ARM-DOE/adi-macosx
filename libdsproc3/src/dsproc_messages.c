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
*    $Revision: 15828 $
*    $Author: ermold $
*    $Date: 2012-11-08 02:42:30 +0000 (Thu, 08 Nov 2012) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc_messages.c
 *  Message Handling Functions.
 */

#include "dsproc3.h"
#include "dsproc_private.h"

extern DSProc *_DSProc; /**< Internal DSProc structure */

/** @publicsection */

/*******************************************************************************
 *  Public Functions
 */

/**
 *  Append a 'Bad File' message to the log file and warning mail message.
 *
 *  @param  func      - the name of the function sending the warning
 *  @param  src_file  - the source file the message came from
 *  @param  src_line  - the line number in the source file
 *  @param  file_name - name of the bad file
 *  @param  format    - message format string (see printf)
 *  @param  ...       - arguments for the format string
 */
void dsproc_bad_file_warning(
    const char *func,
    const char *src_file,
    int         src_line,
    const char *file_name,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    char        bf_format[128];
    char       *message;
    va_list     args;

    if (!format) return;

    snprintf(bf_format, 128, "Bad File:   %s -> %%s", file_name);

    va_start(args, format);
    message = msngr_format_va_list(format, args);
    va_end(args);

    if (message) {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            bf_format, message);

        free(message);
    }
    else {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            bf_format, "memory allocation error creating bad file message.\n");
    }
}

/**
 *  Append a 'Bad Line' message to the log file and warning mail message.
 *
 *  @param  func      - the name of the function sending the warning
 *  @param  src_file  - the source file the message came from
 *  @param  src_line  - the line number in the source file
 *  @param  file_name - name of the file containing the bad line
 *  @param  line_num  - line number of the bad line
 *  @param  format    - format string (see printf)
 *  @param  ...       - arguments for the format string
 */
void dsproc_bad_line_warning(
    const char *func,
    const char *src_file,
    int         src_line,
    const char *file_name,
    int         line_num,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    char        bl_format[128];
    char       *message;
    va_list     args;

    if (!format) return;

    snprintf(bl_format, 128, "Bad Line:   %s:%d -> %%s", file_name, line_num);

    va_start(args, format);
    message = msngr_format_va_list(format, args);
    va_end(args);

    if (message) {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            bl_format, message);

        free(message);
    }
    else {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            bl_format, "memory allocation error creating bad line message.\n");
    }
}

/**
 *  Append a 'Bad Record' message to the log file and warning mail message.
 *
 *  @param  func      - the name of the function sending the warning
 *  @param  src_file  - the source file the message came from
 *  @param  src_line  - the line number in the source file
 *  @param  file_name - name of the file containing the bad record
 *  @param  rec_num   - record number of the bad record
 *  @param  format    - message format string (see printf)
 *  @param  ...       - arguments for the format string
 */
void dsproc_bad_record_warning(
    const char *func,
    const char *src_file,
    int         src_line,
    const char *file_name,
    int         rec_num,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    char        br_format[128];
    char       *message;
    va_list     args;

    if (!format) return;

    snprintf(br_format, 128, "Bad Record:   %s:%d -> %%s", file_name, rec_num);

    va_start(args, format);
    message = msngr_format_va_list(format, args);
    va_end(args);

    if (message) {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            br_format, message);

        free(message);
    }
    else {

        msngr_send(sender, func, src_file, src_line, MSNGR_WARNING,
            br_format, "memory allocation error creating bad record message.\n");
    }
}

/**
 *  Generate a debug message.
 *
 *  This function will generate the specified message if debug mode is
 *  enabled and at a level greater than or equal to the specified debug
 *  level of the message. For example, if the debug level specifed on the
 *  command line is 2 all debug level 1 and 2 messages will be generated,
 *  but the debug level 3, 4, and 5 messages will not be.
 *
 *  @param  func   - the name of the function sending the message (__func__)
 *  @param  file   - the source file the message came from (__FILE__)
 *  @param  line   - the line number in the source file (__LINE__)
 *  @param  level  - debug message level [1-5]
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 */
void dsproc_debug(
    const char *func,
    const char *file,
    int         line,
    int         level,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    MessageType type;
    va_list     args;

    if (msngr_debug_level || msngr_provenance_level) {

        if (!format) return;

        if (!func) func = "null";
        if (!file) file = "null";

        va_start(args, format);

        switch (level) {
            case 1:  type = MSNGR_DEBUG_LV1; break;
            case 2:  type = MSNGR_DEBUG_LV2; break;
            case 3:  type = MSNGR_DEBUG_LV3; break;
            case 4:  type = MSNGR_DEBUG_LV4; break;
            case 5:  type = MSNGR_DEBUG_LV5; break;
            default: type = MSNGR_DEBUG_LV1;
        }

        msngr_vsend(sender, func, file, line, type, format, args);

        va_end(args);
    }
}

/**
 *  Generate an error message.
 *
 *  This function will set the status of the process and append an Error
 *  message to the log file and error mail message.
 *
 *  The status string should be a brief one line error message that will
 *  be used as the process status in the database.  This is the message
 *  that will be displayed in DSView. If the status string is NULL the
 *  error message specified by the format string and args will be used.
 *
 *  The format string and args will be used to generate the complete and
 *  more detailed log and error mail messages. If the format string is
 *  NULL the status string will be used.
 *
 *  @param  func   - the name of the function sending the message (__func__)
 *  @param  file   - the source file the message came from (__FILE__)
 *  @param  line   - the line number in the source file (__LINE__)
 *  @param  status - brief error message to use for the process status.
 *  @param  format - format string for full error message (see printf)
 *  @param  ...    - arguments for the format string
 */
void dsproc_error(
    const char *func,
    const char *file,
    int         line,
    const char *status,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    va_list     args;

    if (!func) func = "null";
    if (!file) file = "null";

    if (status) {
        dsproc_set_status(status);
    }

    if (!format) {
        if (!status) return;
        format = status;
    }

    va_start(args, format);

    if (!status) {
        status = msngr_format_va_list(format, args);
        if (status) {
            dsproc_set_status(status);
            free((void *)status);
        }
        else {
            dsproc_set_status(DSPROC_ENOMEM);
        }
    }

    msngr_vsend(sender, func, file, line, MSNGR_ERROR, format, args);

    va_end(args);
}

/**
 *  Append a message to the log file.
 *
 *  @param  func   - the name of the function sending the message (__func__)
 *  @param  file   - the source file the message came from (__FILE__)
 *  @param  line   - the line number in the source file (__LINE__)
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 */
void dsproc_log(
    const char *func,
    const char *file,
    int         line,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    va_list     args;

    if (!format) return;

    if (!func) func = "null";
    if (!file) file = "null";

    va_start(args, format);
    msngr_vsend(sender, func, file, line, MSNGR_LOG, format, args);
    va_end(args);
}

/**
 *  Send a mail message to the mentor of the software or data.
 *
 *  @param  func   - the name of the function sending the message (__func__)
 *  @param  file   - the source file the message came from (__FILE__)
 *  @param  line   - the line number in the source file (__LINE__)
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 */
void dsproc_mentor_mail(
    const char *func,
    const char *file,
    int         line,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    va_list     args;

    if (!format) return;

    if (!func) func = "null";
    if (!file) file = "null";

    va_start(args, format);
    msngr_vsend(sender, func, file, line, MSNGR_MAINTAINER, format, args);
    va_end(args);
}

/**
 *  Generate a warning message.
 *
 *  This function will append a Warning message to the log file
 *  and warning mail message.
 *
 *  @param  func   - the name of the function sending the message (__func__)
 *  @param  file   - the source file the message came from (__FILE__)
 *  @param  line   - the line number in the source file (__LINE__)
 *  @param  format - format string (see printf)
 *  @param  ...    - arguments for the format string
 */
void dsproc_warning(
    const char *func,
    const char *file,
    int         line,
    const char *format, ...)
{
    const char *sender = (_DSProc) ? _DSProc->full_name : "null";
    va_list     args;

    if (!format) return;

    if (!func) func = "null";
    if (!file) file = "null";

    va_start(args, format);
    msngr_vsend(sender, func, file, line, MSNGR_WARNING, format, args);
    va_end(args);
}

/**
 *  Get the current debug level.
 *
 *  @return
 */
int dsproc_get_debug_level(void)
{
    return(msngr_debug_level);
}
