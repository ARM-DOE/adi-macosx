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
*    $Revision: 57311 $
*    $Author: ermold $
*    $Date: 2014-10-07 17:47:53 +0000 (Tue, 07 Oct 2014) $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file dsproc3_internal.h
 *  Internal libdsproc3 functions and structures
 */

#ifndef _DSPROC3_INTERNAL_H
#define _DSPROC3_INTERNAL_H

#include "dsdb3.h"
#include "ncds3.h"

/** DSPROC library name. */
#define DSPROC_LIB_NAME "libdsproc3"

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_MAIN Internal: Process Framework
 */
/*@{*/

void dsproc_initialize(
        int          argc,
        char       **argv,
        ProcModel    proc_model,
        const char  *proc_version,
        int          nproc_names,
        const char **proc_names);

int dsproc_start_processing_loop(
        time_t *interval_begin,
        time_t *interval_end);

int dsproc_retrieve_data(
        time_t     begin_time,
        time_t     end_time,
        CDSGroup **ret_data);

int dsproc_merge_retrieved_data(void);

int dsproc_transform_data(
        CDSGroup **trans_data);

int dsproc_create_output_datasets(void);

int dsproc_store_output_datasets(void);

int dsproc_finish(void);

int dsproc_force_rename_bad(
        const char *file_path,
        const char *file_name);

int dsproc_is_fatal(int last_errno);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_MESSAGES Internal: Process Messages
 */
/*@{*/

void dsproc_bad_file_warning(
        const char *func,
        const char *src_file,
        int         src_line,
        const char *file_name,
        const char *format, ...);

void dsproc_bad_line_warning(
        const char *func,
        const char *src_file,
        int         src_line,
        const char *file_name,
        int         line_num,
        const char *format, ...);

void dsproc_bad_record_warning(
        const char *func,
        const char *src_file,
        int         src_line,
        const char *file_name,
        int         rec_num,
        const char *format, ...);

void dsproc_debug(
        const char *func,
        const char *file,
        int         line,
        int         level,
        const char *format, ...);

void dsproc_error(
        const char *func,
        const char *file,
        int         line,
        const char *status,
        const char *format, ...);

void dsproc_log(
        const char *func,
        const char *file,
        int         line,
        const char *format, ...);

void dsproc_mentor_mail(
        const char *func,
        const char *file,
        int         line,
        const char *format, ...);

void dsproc_warning(
        const char *func,
        const char *file,
        int         line,
        const char *format, ...);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_CONTROL Internal: Process Control
 */
/*@{*/

void dsproc_abort(
        const char *func,
        const char *file,
        int         line,
        const char *status,
        const char *format, ...);

void dsproc_disable(const char *message);
void dsproc_disable_db_updates(void);
void dsproc_disable_dynamic_metric_vars(int flag);
void dsproc_disable_lock_file(void);
void dsproc_disable_mail_messages(void);

int  dsproc_get_dynamic_dods_mode(void);
int  dsproc_get_force_mode(void);
int  dsproc_get_real_time_mode(void);
int  dsproc_get_reprocessing_mode(void);

void dsproc_set_dynamic_dods_mode(int mode);
void dsproc_set_force_mode(int mode);
int  dsproc_set_log_dir(const char *log_dir);
void dsproc_set_processing_interval(time_t begin_time, time_t end_time);
void dsproc_set_real_time_mode(int mode, float max_wait);
void dsproc_set_reprocessing_mode(int mode);
void dsproc_set_retriever_time_offsets(
        int    ds_id,
        time_t begin_offset,
        time_t end_offset);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_INFO Internal: Process Information
 */
/*@{*/

const char *adi_version(void);
const char *dsproc_lib_version(void);

const char *dsproc_get_input_dir(void);
const char *dsproc_get_input_file(void);
const char *dsproc_get_input_source(void);

const char *dsproc_get_status(void);
const char *dsproc_get_type(void);
const char *dsproc_get_version(void);

time_t      dsproc_get_max_run_time(void);
time_t      dsproc_get_start_time(void);
time_t      dsproc_get_time_remaining(void);
time_t      dsproc_get_min_valid_time(void);
time_t      dsproc_get_data_interval(void);
time_t      dsproc_get_processing_interval(time_t *begin, time_t *end);

void        dsproc_set_input_dir(const char *input_dir);
void        dsproc_set_input_source(const char *input_file);
void        dsproc_set_status(const char *status);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DATASTREAM Internal: DataStreams
 */
/*@{*/

/** Check for overlap with previously processed data before storing new data. */
#define DS_OVERLAP_CHECK    0x01

/** Run standard QC checks before storing new data. */
#define DS_STANDARD_QC      0x02

/** Preserve distinct observations when retrieving and storing data. */
#define DS_PRESERVE_OBS     0x04

/** Replace NaN and Inf values with missing values when data is stored. */
#define DS_FILTER_NANS      0x08

/** Do not merge multiple observations in retrieved data. */
#define DS_DISABLE_MERGE    0x10

/** Skip the transformation logic for all variables in this datastream. */
#define DS_SKIP_TRANSFORM   0x20

/** Consolidate the transformation QC bits for all variables when mapped
 *  to the output dataset. */
#define DS_ROLLUP_TRANS_QC  0x40

/**
 *  DataStream File Formats.
 */
typedef enum {

    DSF_NETCDF =  1, /**< netcdf data file format */
    DSF_RAW    = 10, /**< generic raw data format */
    DSF_PNG    = 11, /**< png image format        */
    DSF_JPG    = 12  /**< jpg image format        */

} DSFormat;

int     dsproc_init_datastream(
            const char  *site,
            const char  *facility,
            const char  *dsc_name,
            const char  *dsc_level,
            DSRole       role,
            const char  *path,
            DSFormat     format,
            int          flags);

void    dsproc_set_datastream_flags(int ds_id, int flags);
void    dsproc_set_datastream_format(int ds_id, DSFormat format);
void    dsproc_unset_datastream_flags(int ds_id, int flags);

void    dsproc_update_datastream_data_stats(
            int              ds_id,
            int              num_records,
            const timeval_t *begin_time,
            const timeval_t *end_time);

void    dsproc_update_datastream_file_stats(
            int              ds_id,
            double           file_size,
            const timeval_t *begin_time,
            const timeval_t *end_time);

int     dsproc_validate_datastream_data_time(
            int              ds_id,
            const timeval_t *data_time);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DATASTREAM_FILES Internal: DataStream Files
 */
/*@{*/

void    dsproc_close_untouched_files(void);

void    dsproc_free_file_list(char **file_list);
int     dsproc_get_datastream_files(int ds_id, char ***file_list);

int     dsproc_set_datastream_path(int ds_id, const char *path);

void    dsproc_set_file_name_compare_function(
            int     ds_id,
            int    (*function)(const void *, const void *));

void    dsproc_set_file_name_time_function(
            int      ds_id,
            time_t (*function)(const char *));

void    dsproc_set_max_open_files(int ds_id, int max_open);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DATASET Internal: Datasets
 */
/*@{*/

/** Flag specifying that all bad and indeterminate bits in the input
 *  QC variable should be consolidated into single bad or indeterminate
 *  bits in the output QC variable.
 */
#define MAP_ROLLUP_TRANS_QC  0x1

CDSGroup *dsproc_create_output_dataset(
            int      ds_id,
            time_t   data_time,
            int      set_location);

int     dsproc_dataset_pass_through(
            CDSGroup *in_dataset,
            CDSGroup *out_dataset,
            int       flags);

int     dsproc_map_datasets(
            CDSGroup *in_parent,
            CDSGroup *out_dataset,
            int       flags);

int     dsproc_map_var(
            CDSVar *in_var,
            size_t  in_sample_start,
            size_t  sample_count,
            CDSVar *out_var,
            size_t  out_sample_start,
            int     flags);

int     dsproc_set_dataset_location(CDSGroup *dataset);

void    dsproc_set_map_time_range(
            time_t begin_time,
            time_t end_time);

int     dsproc_store_dataset(
            int ds_id,
            int newfile);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DATASET_COMPARE Internal: Dataset Compare
 */
/*@{*/

int dsproc_compare_dod_versions(
        CDSGroup *prev_ds,
        CDSGroup *curr_ds,
        int       warn);

int dsproc_compare_dod_dims(
        CDSGroup *prev_ds,
        CDSGroup *curr_ds,
        int       warn);

int dsproc_compare_dod_atts(
        CDSGroup *prev_ds,
        CDSGroup *curr_ds,
        int       warn);

int dsproc_compare_dod_vars(
        CDSGroup *prev_ds,
        CDSGroup *curr_ds,
        int       warn);

int dsproc_compare_dods(
        CDSGroup *prev_ds,
        CDSGroup *curr_ds,
        int       warn);

int dsproc_exclude_from_dod_compare(
    const char  *var_name,
    int          exclude_data,
    int          natts,
    const char **att_names);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DATASET_FETCH Internal: Dataset Fetch
 */
/*@{*/

int     dsproc_fetch_dataset(
            int          ds_id,
            timeval_t   *begin_timeval,
            timeval_t   *end_timeval,
            size_t       nvars,
            const char **var_names,
            int          merge_obs,
            CDSGroup    **dataset);

timeval_t *dsproc_fetch_timevals(
            int          ds_id,
            timeval_t   *begin_timeval,
            timeval_t   *end_timeval,
            size_t      *ntimevals,
            timeval_t   *timevals);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DATASET_FILTERS Internal: Dataset Filters
 */
/*@{*/

void dsproc_disable_nan_filter_warnings(void);

int dsproc_filter_var_nans(CDSVar *var);
int dsproc_filter_dataset_nans(CDSGroup *dataset, int warn);

/** Flag to resest overlap filtering back to duplicate records only */
#define FILTER_DUPS_ONLY   0x00
/** Flag to filter records that are not in chronological order */
#define FILTER_TIME_SHIFTS 0x01
/** Flag to filter records that have the same times but different data values */
#define FILTER_OVERLAPS    0x02
/** Same as FILTER_TIME_SHIFTS | FILTER_OVERLAPS */
#define FILTER_ALL         FILTER_TIME_SHIFTS | FILTER_OVERLAPS

void dsproc_set_overlap_filtering_mode(int mode);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DATASET_QC Internal: Dataset QC
 */
/*@{*/

int dsproc_exclude_from_standard_qc_checks(
        const char *var_name);

int dsproc_qc_delta_checks(
        CDSVar *var,
        CDSVar *qc_var,
        CDSVar *prev_var,
        CDSVar *prev_qc_var,
        int     delta_flag,
        int     bad_flags);

int dsproc_qc_limit_checks(
        CDSVar *var,
        CDSVar *qc_var,
        int     missing_flag,
        int     min_flag,
        int     max_flag);

int dsproc_qc_time_checks(
        CDSVar    *time_var,
        CDSVar    *qc_time_var,
        timeval_t *prev_timeval,
        int        lteq_zero_flag,
        int        min_delta_flag,
        int        max_delta_flag);

int dsproc_standard_qc_checks(
        int         ds_id,
        CDSGroup   *dataset);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DATASTREAM_DOD Internal: DataStream DOD
 */
/*@{*/

int     dsproc_check_for_dsdod_update(
            int    ds_id,
            time_t data_time);

CDSAtt *dsproc_get_dsdod_att(
            int         ds_id,
            const char *var_name,
            const char *att_name);

CDSDim *dsproc_get_dsdod_dim(
            int         ds_id,
            const char *dim_name);

void   *dsproc_get_dsdod_att_value(
            int          ds_id,
            const char  *var_name,
            const char  *att_name,
            CDSDataType  type,
            size_t      *length,
            void        *value);

char   *dsproc_get_dsdod_att_text(
            int         ds_id,
            const char *var_name,
            const char *att_name,
            size_t     *length,
            char       *value);

size_t  dsproc_get_dsdod_dim_length(
            int         ds_id,
            const char *dim_name);

const char *dsproc_get_dsdod_version(
            int  ds_id,
            int *major,
            int *minor,
            int *micro);

int     dsproc_set_dsdod_att_value(
            int          ds_id,
            const char  *var_name,
            const char  *att_name,
            CDSDataType  type,
            size_t       length,
            void        *value);

int     dsproc_set_dsdod_att_text(
            int         ds_id,
            const char *var_name,
            const char *att_name,
            const char *format, ...);

int     dsproc_set_dsdod_dim_length(
            int         ds_id,
            const char *dim_name,
            size_t      dim_length);

int     dsproc_set_runtime_metadata(int ds_id, CDSGroup *cds);

int     dsproc_update_datastream_dsdod(int ds_id, time_t data_time);
int     dsproc_update_datastream_dsdods(time_t data_time);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DSDB Internal: DSDB
 */
/*@{*/

int     dsproc_db_connect(void);
void    dsproc_db_disconnect(void);

int     dsproc_get_datastream_properties(int ds_id, DSProp ***dsprops);
int     dsproc_get_datastream_property(
            int          ds_id,
            const char  *var_name,
            const char  *prop_name,
            time_t       data_time,
            const char **prop_value);

int     dsproc_get_input_ds_classes(DSClass ***ds_classes);
int     dsproc_get_output_ds_classes(DSClass ***ds_classes);

int     dsproc_get_location(ProcLoc **proc_loc);
int     dsproc_get_site_description(const char **site_desc);

int     dsproc_get_config_value(
            const char  *config_key,
            char       **config_value);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_DQRDB Internal: DQRDB
 */
/*@{*/

int     dsproc_dqrdb_connect(void);
void    dsproc_dqrdb_disconnect(void);

void    dsproc_free_dqrs(DQR **dqrs);
int     dsproc_get_dqrs(
            const char *site,
            const char *facility,
            const char *dsc_name,
            const char *dsc_level,
            const char *var_name,
            time_t      start_time,
            time_t      end_time,
            DQR      ***dqrs);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_QC_UTILS Internal: QC Utilities */
/*@{*/

unsigned int dsproc_get_qc_assessment_mask(
    CDSVar     *qc_var,
    const char *assessment,
    int        *nfound,
    int        *max_bit_num);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup INTERNAL_DSPROC_TRANSFORMS Internal: Transformation
 */
/*@{*/

/**
 *  Transform .
 */
typedef struct {

    CDSVar     *ret_coord_var; /**< pointer to the retrieved variable that
                                    will be used for the coordinate variable
                                    in the transformed dataset */

    int         ret_dsid;      /**< datastream ID of the ret_coord_var.      */

    const char *name;          /**< coordinate dimension name.               */
    const char *data_type;     /**< user defined data type.                  */
    const char *units;         /**< user defined  units.                     */

    double      start;         /**< dimension start value                    */
    double      length;        /**< dimension length                         */
    double      interval;      /**< dimension interval                       */

    const char *trans_type;    /**< transformation type                      */
    double      trans_range;   /**< range value used by the transformation   */
    double      trans_align;   /**< bin alignment used by the transformation */

} TransDimInfo;

int dsproc_get_trans_dim_info(
        CDSVar       *ret_var,
        int           dim_index,
        TransDimInfo *dim_info);

int dsproc_get_trans_qc_rollup_bits(
        CDSVar       *qc_var,
        unsigned int *bad_flag,
        unsigned int *ind_flag);

int dsproc_is_transform_qc_var(CDSVar *qc_var);

int dsproc_load_transform_params_file(
        CDSGroup   *group,
        const char *site,
        const char *facility,
        const char *name,
        const char *level);

/*@}*/

/******************************************************************************/
/**
 *  @defgroup DSPROC_DEPRECATED Deprecated
 */
/*@{*/

/*
void    dsproc_ingest_main(
            int            argc,
            char          *argv[],
            char          *version_tag,
            char          *proc_name,
            const char **(*valid_proc_names)(int *nproc_names),
            void        *(*init_process)(void),
            void         (*finish_process)(void *user_data),
            int          (*process_file)(void *user_data,
                            const char *input_dir, const char *file_name));
*/

void    dsproc_vap_main(
            int            argc,
            char          *argv[],
            char          *version_tag,
            char          *proc_name,
            const char **(*valid_proc_names)(int *nproc_names),
            void        *(*init_process)(void),
            void         (*finish_process)(void *user_data),
            int          (*process_data)(void *user_data,
                            time_t begin_date, time_t end_date,
                            CDSGroup *ret_data));

void    dsproc_transform_main(
            int            argc,
            char          *argv[],
            char          *version_tag,
            char          *proc_name,
            const char **(*valid_proc_names)(int *nproc_names),
            void        *(*init_process)(void),
            void         (*finish_process)(void *user_data),
            int          (*process_data)(void *user_data,
                             time_t begin_date, time_t end_date,
                             CDSGroup *trans_data));

CDSGroup *dsproc_create_dataset(
            int      ds_id,
            time_t   data_time,
            int      set_location);

int dsproc_trans_dataset_pass_through(
        CDSGroup *trans_cds,
        CDSGroup *out_dataset,
        int       flags);

CDSGroup *dsproc_get_trans_ds_by_group_name(
    const char *coordsys_name,
    const char *ret_group_name);

/*@}*/

#endif /* _DSPROC3_INTERNAL_H */
