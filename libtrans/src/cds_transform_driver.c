/*******************************************************************************
*
*  Your copyright notice
*
********************************************************************************
*
*  Author:
*     name:  Tim Shippert
*     phone: 5093755997
*     email: tim.shippert@pnl.gov
*
********************************************************************************
*
*  REPOSITORY INFORMATION:
*    $Revision: 58819 $
*    $Author: shippert $
*    $Date: 2014-12-09 00:40:32 +0000 (Tue, 09 Dec 2014) $
*    $State: $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file transform.c
 *  transform one cds group into another, by somehow pulling out the
 *  transformation details from the aether.
 */
# include <stdio.h>
# include <math.h>
# include <string.h>
# include <time.h>
# include <getopt.h>
# include <regex.h>
# include "trans.h"

int QC_BAD=1;
int QC_INDETERMINATE=2;
int QC_INTERPOLATE=3;
int QC_EXTRAPOLATE=4;
int QC_NOT_USING_CLOSEST=5;
int QC_SOME_BAD_INPUTS=6;
int QC_ZERO_WEIGHT=7;
int QC_OUTSIDE_RANGE=8;
int QC_ALL_BAD_INPUTS=9;
int QC_ESTIMATED_INPUT_BIN=10;
int QC_ESTIMATED_OUTPUT_BIN=11;

int QC_BAD_SDEV=12;
int QC_INDETERMINATE_SDEV=13;
int QC_BAD_GOODFRAC=14;
int QC_INDETERMINATE_GOODFRAC=15;

static size_t _one=1;

/* We'll be doing a lot of dynamic allocation, so let's make it easier */
# define CALLOC(n,t)  (t*)calloc(n, sizeof(t))
# define REALLOC(p,n,t)  (t*) realloc((char *) p, (n)*sizeof(t))


// This depends on the interface_s literal struct, defined in trans.h so
// the interface functions can understand it.  This lets me hide the
// horrendous syntax, and al
// I'm putting some defaults in here, just for fun.
#define do_transform(trans,...) ((*(trans->func))((interface_s){ \
	.input_missing_value=-9999, .output_missing_value=-9999, __VA_ARGS__}))
  
// This is how we map the tranform parameters that hold the name of the
// transform to actual functions.  Uses the TRANSf1unc structure defined in
// trans.h 

// Now these are the default transforms - we'll always search these to
// match the text given in the params.
static TRANSfunc DefaultTransFuncs[] = {
  {"TRANS_INTERPOLATE", trans_interpolate_interface}, 
  {"TRANS_SUBSAMPLE", trans_subsample_interface}, 
  {"TRANS_BIN_AVERAGE", trans_bin_average_interface},
  {"TRANS_PASSTHROUGH", trans_passthrough_interface},
};

// Apparently, this works
static int NumDefaultTransFuncs=sizeof(DefaultTransFuncs)/sizeof(TRANSfunc);

// This holds the user defined ones, which we'll set by dedicated functions
// that the user will have to call, probably right up near the main
// function. 
static TRANSfunc *UserTransFuncs;
static int NumUserTransFuncs=0;

// Allows for a user defined qc mapping from non-standard aqc
static int (*qc_mapping_function)() = NULL;
static int *qc_bad_values=NULL;
static size_t num_qc_bad_values=0;
  
// So, here is the dedicated function to allocate space for and assign the
// user functions.  Incidentally, I have no idea how to prototype this...
//int assign_transform_function(char *name,
//			      int (*fptr)(double *, int*, double*,
//					  int*, CDSVar*, CDSVar*, int, TRANSmetric **)) {

int assign_transform_function(char *name,int (*fptr)(interface_s)) {

  int i;
  static int nalloc=0; // Keep track of how much we've allocated, to see if
		       // we need to realloc

  // Allocation shabadoo; do it in blocks of 10, because that's how I roll
  if (NumUserTransFuncs == 0) {
    nalloc=10;
    if (! (UserTransFuncs = CALLOC(nalloc, TRANSfunc))) {
      ERROR(TRANS_LIB_NAME,
	    "Initial alloc of UserTransFuncs failed (%d)\n", nalloc);
      return(-1);
    } 
  } else {
    // Check to see if this tag is already used, and replace if so
    for (i=0;i<NumUserTransFuncs;i++) {
      if (strcmp(name, UserTransFuncs[i].name) == 0) {
	LOG(TRANS_LIB_NAME,
	    "Warning: replacing user-defined function %s\n",
	    name);
	UserTransFuncs[i].func=fptr;
	return(0);
      }
    }
  }
      
      
  // Now, we have to check to see if we have enough space allocated.  I
  // expect never to have to run this code, so it may not actually work,
  // because it's probably never been tested.
  if (NumUserTransFuncs >= nalloc) {
    nalloc +=10;
    if (! REALLOC(UserTransFuncs, nalloc, TRANSfunc)) {
      ERROR(TRANS_LIB_NAME,
	    "Realloc of UserTransFuncs failed (%d)", nalloc);
      return(-1);
    }
  }

  // Finally, just assign stuff
  UserTransFuncs[NumUserTransFuncs].name = name;
  UserTransFuncs[NumUserTransFuncs].func = fptr;
  NumUserTransFuncs++;

  return(0);
}

// Assignment function for qc mapping, which either the user or we can use,
// depending on the flat file.
void assign_qc_mapping_function(int (*fptr)(CDSVar *, double , int)) {
  qc_mapping_function=(void *) fptr;
}

// Default qc mapping func, for use when we list bad values in the qc_bad
// transform param
int default_qc_mapping_function(CDSVar *qc_var, double val, int qc_val) {
  int k, qc=0;

  // I'm not sure I like the use of globals here.  Oh well.

  // Scan up our list of bad values, set via transform params before this
  // call.  If any match, set the QC_BAD bit and return.
  for (k=0;k<num_qc_bad_values; k++) {
    if (qc_val == qc_bad_values[k]) {
      qc_set(qc, QC_BAD);
      return(qc);
    }
  }

  if (qc_val != 0) {
    qc_set(qc, QC_INDETERMINATE);
  }
  return(qc);
}
  

// Now, here's the function to find and return the transform function.  I'm
// returning the whole TRANSfunc structure, so I don't have to prototype
// things the hard way.  Also, it will help with debugging to keep the name
// tagged along.
TRANSfunc *get_transform(char *name) {
  int i;

  // Look through the user defined ones first, so that they can overide the
  // defaults 
  for (i=0;i<NumUserTransFuncs;i++) {
    // Should I use strcmp, or some kind of regex? Nah, we'll be hardcore
    // and force complete compliance
    if (strcmp(name, UserTransFuncs[i].name) == 0) {
      return(&UserTransFuncs[i]);
    }
  }

  for (i=0;i<NumDefaultTransFuncs;i++) {
    if (strcmp(name, DefaultTransFuncs[i].name) == 0) {
      return(&DefaultTransFuncs[i]);
    }
  }
  // Okay, we've whiffed.  So, let them know.

  ERROR(TRANS_LIB_NAME,
	"Transform function %s not known; check spelling and documentation\n", name);
  return(NULL);
}


/**
*  Run the transform engine on an input variable, given input QC and an
*  allocated and dimensioned output variable (and QC) structure.
*
*  @param  invar - pointer to input CDSVar
*  @param  qc_invar - pointer to input QC CDSVar
*  @param  outvar - pointer to output CDSVar; must have dimensions and data
*            spaces allocated, and the dimensions must have coordinate
*            variables already created and attached (we use this
*            information to build the output grid to transform to)
*  @param  qc_outvar - pointer to output QC CDSVar; must be dimensioned and
*            allocated as above for outvar
*
*  @return
*    - 0 if successful
*    - -1 if something failed, usually deeper in CDS
*
*  Upon successful output, outvar and qc_outvar will contain the
*  transformed data and QC. 
*/

int cds_transform_driver(CDSVar *invar, CDSVar *qc_invar, CDSVar *outvar, CDSVar *qc_outvar) {
  int i, d, k, z, s, z0, m, Ndims;
  TRANSfunc *trans=NULL;
  double *data, *odata=NULL,*tdata;
  int *qc_data, *qc_odata=NULL, *qc_tdata, *qc_temp;
  int size;

  int Ntot, *D, *len;
  int tNtot, *tD, *tlen;
  int oNtot, *oD, *olen;
  double input_interval, output_interval;
  double input_missing_value, output_missing_value;

  // To help freeing temp data without freeing original data.
  int iter_count=0;

  char *transform_type;

  double trans_calculate_interval(CDSVar *, int);

  // First, let's make sure that we want a serial 1D transform; if not,
  // well, we'll have to deal with that later.
  if ((transform_type = cds_get_transform_param(outvar, "transform_type",
						      CDS_CHAR, &_one, NULL)) != NULL &&
      (strcmp(transform_type, "Multi_Dimensional") == 0)) {
    LOG(TRANS_LIB_NAME, "Multi D transforms not implemented yet\n");
    return(-3);
  }

  // Now, let's check for qc mapping in the flat files, and set the
  // qc_mapping function to the default integer map, if it's not already
  // set 
  qc_bad_values=NULL;  // reset from last time
  if (qc_mapping_function == NULL &&
      qc_invar != NULL &&
      (qc_bad_values=cds_get_transform_param(qc_invar,"qc_bad", CDS_INT, &num_qc_bad_values,NULL))) {
    LOG(TRANS_LIB_NAME, "Using specified qc value mapping\n");
    assign_qc_mapping_function(default_qc_mapping_function);
  }

  // Okay, proceed with serial 1D transform.  This means looping over dims
  // and building strides and stuff.  This will be fun.
  Ndims=invar->ndims;

  // Allocate our arrays;
  D=CALLOC(Ndims, int);
  tD=CALLOC(Ndims, int);
  oD=CALLOC(Ndims, int);

  len=CALLOC(Ndims, int);
  tlen=CALLOC(Ndims, int);
  olen=CALLOC(Ndims, int);

  // Initial stride and length; it's easier if we go backwards and use
  // D[i]=D[i+1]*len[i+1] 
  D[Ndims-1]=1;  // Always true - unit stride on fastest dimension
  len[Ndims-1] = Ntot = invar->dims[Ndims-1]->length;

  for (d=Ndims-2;d>=0;d--) {
    len[d]=invar->dims[d]->length;
    Ntot *= len[d];
    D[d]=D[d+1]*len[d+1];
  }

  // Saving our original lengths and stuff, because we are going to
  // overwrite our lens as we loop over dimensions and rebuild our data
  // array with the transformed slices.
  memmove(tlen, len, Ndims*sizeof(int));
  memmove(olen, len, Ndims*sizeof(int));

  // Now, pull out our row-major ordered array, and allocate our output
  // array. 

  // There's some fancy stuff with missing values that I'll have to figure
  // out later, having to do with all those NULLS.
  size=Ntot;

  //data=cds_copy_array(invar->type, size, invar->data.vp,
  //CDS_DOUBLE, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
  size_t nsamples;
  //data=dsproc_get_var_data(invar, CDS_DOUBLE, 0, &nsamples,
  //&input_missing_value, NULL);
  data=cds_get_var_data(invar, CDS_DOUBLE, 0, &nsamples,
			   &input_missing_value, NULL);

  // Plus, QC data.  Fazoom
  qc_data=qc_temp=NULL;  // Just in case

  // Figure out output missing value - if it's not in outvar, use the input
  // value we just got.
  // This is more complicated than it oughta be, which probably means I'm
  // doing it wrong.
  void *missing_array;
  double *missing_double;

  //dsproc_get_var_missing_values(outvar, &missing_array);
  cds_get_var_missing_values(outvar, &missing_array);

  if (missing_array) {
    missing_double =
      (double *) cds_get_missing_values_map(outvar->type, 1,
					    missing_array, CDS_DOUBLE, NULL);
    output_missing_value=missing_double[0];
    free(missing_array);
    free(missing_double);
  } else {
    LOG(TRANS_LIB_NAME,
	"No missing value for transformed field %s; using input field value=%g\n",
	outvar->name, input_missing_value);
    output_missing_value=input_missing_value;
  }

  // Set up some infrastructure to allow QC a mapping function, to map
  // straight integer states and/or actual data values to bit-packed QC
  // states.  Also, if qc_invar is NULL (no input qc) then don't bother.
  if (qc_invar) {
    // Okay, right here we have to do something different if we are missing
    // one or more dimensions in our qc array.
    // BLORP
    if (qc_invar->ndims < invar->ndims) {
      // First, figure out which Dims are missing, and build a Dq[p]
      // stride coefficient for the qc var.
      int *mu=CALLOC(Ndims, int);  // 1 if in qc, 0 if not
      int *Dq=CALLOC(Ndims, int);  // stride coeffs for qc var
      int prev_d=INT_MAX;

      for(d=Ndims-1; d>=0;d--) {
	// loop over [dq] in {Nq} to see if [d] is in {Nq}
	// (That's my math-y way of saying if qc_data has [d] as a dim
	for (int dq=0;dq<qc_invar->ndims;dq++) {
	  mu[d]=0;
	  if (strcmp(qc_invar->dims[dq]->name,
		     invar->dims[d]->name) == 0) {
	    mu[d]=1;
	    break;
	  }
	}

	// Now calculate Dq, if [d] is in {Nq}
	if (mu[d]==1) {
	  if (prev_d > Ndims-1) {
	    Dq[d]=1;
	  } else {
	    Dq[d]=len[prev_d]*Dq[prev_d];
	  }
	  prev_d=d;
	}
      }

      // Hooray!  Now we just need to align the flattened k in invar space
      // with the flattened kp in qc_invar space, which will naturally
      // duplicate QC values over missing dimensions.
      // Note - we've already calculated our stride coefficients D[d], above.

      qc_temp=CALLOC(Ntot, int);
      //int *qc_foo=dsproc_get_var_data(qc_invar, CDS_INT, 0, &nsamples,
      //NULL, NULL);
      int *qc_foo=cds_get_var_data(qc_invar, CDS_INT, 0, &nsamples, NULL, NULL);

      for(k=0;k<Ntot;k++) {
	int kq=mu[0]*(k/D[0])*Dq[0]; // d=0, special form (k%D[-1]=k, by
				     // def and inspection)

	for(d=1;d<Ndims;d++) {
	  kq += mu[d]*((k % D[d-1])/D[d])*Dq[d];
	}
	qc_temp[k]=qc_foo[kq];
      }

      // Finally, free stuff up
      free(qc_foo);free(mu);free(Dq);
    } else if (qc_invar->ndims == invar->ndims) {
      //qc_temp=dsproc_get_var_data(qc_invar, CDS_INT, 0, &nsamples, NULL, NULL);
      qc_temp=cds_get_var_data(qc_invar, CDS_INT, 0, &nsamples, NULL, NULL);

      //qc_temp=cds_copy_array(qc_invar->type, size, qc_invar->data.vp,
      //CDS_INT, NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL);
    } else {
      // I don't know what to do if we have *more* QC dimensions than data
      // dimensions.  It's probably an error.  I should probably also free
      // up some stuff. Whatever
      ERROR(TRANS_LIB_NAME,
	    "Qc field %s has %d dimensions, while data field %s has %d dimensions\n", 
	    qc_invar->name, qc_invar->ndims, invar->name, invar->ndims);
      ERROR(TRANS_LIB_NAME,
	    "This literally makes no sense, so I'm exiting...\n");
      return(-1);
    }

    // If we have a mapping function, we have to apply it here
    if (qc_mapping_function) {
      qc_data=CALLOC(size, int);
      // Passing both data and qc values in, if they exist, just in
      // case there is some combo of both of these things that we need
      // to map qc to.
      for (k=0;k<size;k++) {
	qc_data[k]=(*qc_mapping_function)(qc_invar, data[k],qc_temp[k]);
      }
      free(qc_temp);
      qc_temp=NULL;
    } else {
      qc_data=qc_temp;
    }
  }

  // Okay, we have our data, so now we start looping over dims to do our
  // Serial 1D.  In RIPBE we start with the slowest (lowest) index
  // first, which may not be completely logical, but I'm going for
  // consistency now.  We can probably implement a transform param that
  // changes this without too much trouble.

  // The prefixes get a little confusing: no prefix means original input
  // data; 't' means temp data, the input for this dimension's transform,
  // and 'o' means output data, the output of this dimension's transform.
  // When we iterate over d, we set 't' variables to 'o' variables - the
  // output of each iteration is the input for the next one.  That's what
  // serial 1D means.

  // Perhaps I don't need tdata, and we can just reset data, D, len, and
  // Ntot each iteration, but I don't want to overwrite anything orignal
  // until the code is fully debugged, and after that I won't want to mess
  // with something that works.  So there - we got some memmoves and
  // pointer reassigns in our future.

  // Note that we rebuild oD and olen each iteration, so we copy values, we
  // don't pointer assign.
  tdata=data; 
  qc_tdata=qc_data;  // Right here - if qc_data is not of the same
		     // dimension as data, we need to remap it onto the
		     // same grid.
  memmove(tD,D,Ndims*sizeof(int));
  memmove(tlen,len,Ndims*sizeof(int));
  tNtot=Ntot;

  // This will help us with metrics later on - it's the shape of the output
  // of this particular transformation.
  int *shape=CALLOC(Ndims, int);
  for (d=0;d<Ndims;d++) {
    shape[d]=invar->dims[d]->length;
  }
  
  for (d=0; d<Ndims;d++) {

    // First, find the size of our output data in this dimension.  This
    // keeps the other dims what they were, which is what we want - we
    // transform one d at a time.
    olen[d]=outvar->dims[d]->length;

    // This is the shape of the arrays coming out of this transformation
    shape[d]=outvar->dims[d]->length;

    // Now, we have to build *output* stride arrays.  It's easier if I just
    // do it from scratch each time, just in case we change the order of
    // dims or something.
    oD[Ndims-1]=1;
    oNtot=olen[Ndims-1];
    for (i=Ndims-2;i>=0;i--) {
      oD[i] = oD[i+1]*olen[i+1];
      oNtot *= olen[i];
    }
     
    // Now, allocate our output arrays for this iter.  The old odata
    // pointers are still in use as the new tdata, so we never end up
    // freeing these pointers; we'll have to do that when we reassign tdata
    // at the end.
    odata=CALLOC(oNtot, double);
    qc_odata=CALLOC(oNtot, int);
			     
    // get the CDSdims associated with this dimension, because sometimes we
    // use index, and sometimes not.  This is used mainly to get tranform
    // params, but who knows.
    CDSDim *dim=invar->dims[d]; //apparently, we don't use this yet, but we
				//probably will as we add more transform params
    CDSDim *odim=outvar->dims[d];

    // Now get the transform function for this var for this dimension.  It
    // needs to be by outvar and dimension name, like:
    // temp:time:transform = "TRANS_INTERPOLATE";
    char *transform_name = cds_get_transform_param_by_dim(outvar, odim, "transform",
							  CDS_CHAR, &_one, NULL); 

    if (transform_name != NULL) {
      trans=get_transform(transform_name);
    } else {
      // Okay, figure out the intervals to choose; this is stupid, but we
      // gotta do something like this.

      CDSVar *incoord = cds_get_coord_var(invar, d);
      CDSVar *outcoord = cds_get_coord_var(outvar, d);

      // Check to see if the coord fields exist, first
      if (incoord && outcoord) {

	if (! cds_get_transform_param(incoord, "interval", CDS_DOUBLE,
				      &_one, &input_interval)) {
	  // have to calculate it myself...
	  input_interval = trans_calculate_interval(invar, d);
	}

	if (! cds_get_transform_param(outcoord, "interval", CDS_DOUBLE,
				      &_one, &output_interval)) {
	  output_interval = trans_calculate_interval(outvar, d);
	}
	
	// So, our default is to average if output interval is larger, or
	// interpolate if input is larger
	
	if (output_interval > input_interval) {
	  trans=get_transform("TRANS_BIN_AVERAGE");
	  LOG(TRANS_LIB_NAME, "Using bin averaging for dim %s in field %s\n",
	      odim->name, outvar->name);
	} else {
	  trans=get_transform("TRANS_INTERPOLATE");
	  LOG(TRANS_LIB_NAME, "Using interpolation for dim %s in field %s\n",
	      odim->name, outvar->name);
	}
      } else if (dim->length == odim->length) {
	// At least one of the coord field does not exist, but input and
	// output dims are the same length, so set it as a passthrough
	trans=get_transform("TRANS_PASSTHROUGH");
	LOG(TRANS_LIB_NAME, "Using passthrough for dim %s in field %s\n",
	    odim->name, outvar->name);
      } else {
	// Just record the log, and let the next step dump
	LOG(TRANS_LIB_NAME,
	    "Coordinate field missing and dimensions do not match; no transformation possible\n");
      }
    }
      
    // Make sure we have an actual transform
    if (trans == NULL) {
      ERROR(TRANS_LIB_NAME,"No valid transform for dim %s in field %s; exiting transform code...\n"); 
      return(-1);
    }
    
    // Allocate space to hold our input and output slices.  We have to have
    // holding arrays, because our N-dimensional flattened array won't map
    // pointers or anything.

    // These will get overwritten with each iteration, which means the
    // following loop is not thread safe.  We'll have to allocate inside
    // the loop to make it that way.
    double *data1d=CALLOC(tlen[d], double);  
    int *qc1d=CALLOC(tlen[d], int);   // If no qc_invar, this will stay 0s,
				     // so it's cool.
    double *odata1d=CALLOC(olen[d], double);
    int *oqc1d=CALLOC(olen[d], int);  

    // Now, loop over all the slices, and pull out the input data and qc
    // for that slice.  This is fun.
    int nslice=tNtot/tlen[d];  
    int oz0=z0=0;  // First slice *always* starts at origin in our
		   // flattened array

    // Metric holders
    TRANSmetric *met1d=NULL, *metNd=NULL;

    for (s=0;s<nslice;s++) {

      // Now build up our input slice, which is k strides up from z0
      for (k=0;k<tlen[d];k++) {
	z=z0+k*tD[d];  // striding
	data1d[k]=tdata[z];
	if (qc_tdata) qc1d[k]=qc_tdata[z];

      }
      // Great.  We now have our input and output arrays, so it's just a
      // matter of calling the transform interface function.  This takes a
      // mixture of 1D data arrays and CDS pointers; the latter are used to
      // track down the various parameters used in the transformation,
      // including such basic information as the size of the arrays and
      // what not.
      //int status = (*(trans->func))(data1d,qc1d,odata1d,oqc1d,invar,outvar,d, &met1d);
      int status = do_transform(trans,
				.input_data=data1d,
				.input_qc=qc1d,
				.input_missing_value=input_missing_value,
				.output_data=odata1d,
				.output_qc=oqc1d,
				.output_missing_value=output_missing_value,
				.invar=invar,
				.outvar=outvar,
				.d=d,
				.met=&met1d); 

      // We don't know how many metrics to fill until right here, so:
      // Make sure to trap out met1==NULL, because that means this
      // transform doesn't make metrics.
      if (s==0 && met1d) { // again, not thread safe 
	allocate_metric(&metNd, met1d->metricnames,met1d->metricunits,met1d->nmetrics, oNtot);
      }

      // Now that we've filled odata1d with our transformed data, fill in
      // our output slice, using the exact process above.  Make sure
      // we use output strides, z0s, and lens.
      for (k=0;k<olen[d];k++) {
	z=oz0+k*oD[d];
	odata[z]=odata1d[k];
	qc_odata[z]=oqc1d[k];

	// Fill our metrics array, if we've got one
	if (met1d && met1d->metrics) {
	  for (m=0;m<met1d->nmetrics;m++) {
	    metNd->metrics[m][z]=met1d->metrics[m][k];
	  }
	}
      }

      // We need to free up met1d and point it back to null, so it will get
      // properly allocated and not leave hangers.
      free_metric(&met1d);

      // Now, find our next slice, and reset our pointers.  The math behind
      // this is non-trivial, but sound.  Qualitatively, the idea is that
      // when we increment our faster indeces than d, we 
      // increment z0 by just 1 (since the stride of our fastest dimension
      // is always 1).  We can do that D[d] times, until we have to
      // increment the slower dimensions than d; to do that, we stride in
      // d-1, i.e. increment z0 by D[d-1].  We then subtract D[d]-1 to
      // reset our faster indeces to 0, and begin the cycle again.  Hence,
      // if z0+1 is a multiple of D[d], we set z0 += D[d-1] - (D[d] - 1);
      // otherwise, z0 += 1.

      // (Note the hidden +1 by doing ++z0 first).
      if ((++z0 % tD[d]) == 0 && d > 0) {
	z0 +=  tD[d-1] - tD[d];  
      }

      // I don't know if oz0 will always be a multiple of oD whenever z0 is
      // a multiple of D, so repeat the check just to be sure.
      if ((++oz0 % oD[d]) == 0 && d > 0) {
	oz0 +=  oD[d-1] - oD[d];  
      }

      // That's it - ready for the next stride.  Our s index just counts,
      // nothing else.
    }

    // Hokay.  At this point we've gone through all the slices in this
    // dimension, so it's time to see if we need to store our metrics in
    // sibling variables.  The decision tree is this: 
    // (a) does there exist a sibling variable to outvar with the proper
    // metric name?
    // (b) Does that sibling variable have the same shape as the metrics we
    // have right now in metNd.

    // If both of these hold true, save the metric in that sibling
    // variable.
    if (metNd) {
      // Obviously, we need some metrics or this doesn't make any sense

      // Way up here, we first check to see if this iteration has the same
      // shape as the output var.  If so, we can continue on with the
      // transformation.  The shape of this iteration is in shape, which
      // has dimensionality of output for dm<=d and input for dm>d.

      int okshape=1;
      int msize=1;
      int msample=1;
      int dm;
      for (dm=0;dm<outvar->ndims;dm++) {
	if (shape[dm] == outvar->dims[dm]->length) {
	  // I think we should know the total size of our metric (outvar) array
	  // already, but I can't figure out which variable holds it, so
	  // I'm recalculating it here.
	  msize *= outvar->dims[dm]->length;  

	  // Find the number of samples - i.e. size of time dimension
	  if (strcmp(outvar->dims[dm]->name, "time") == 0) {
	    msample=outvar->dims[dm]->length;
	  }

	} else {
	  okshape=0;
	}
      }

      if (okshape) {

	// Now loop over our metrics.  For each one, we first look for an
	// existing CDSVar; if it exists, as a sibling to outvar, we'll try
	// to use it, but it means we have to double check the shape and
	// dimensions.  If it does not exist, we create one as a clone to
	// outvar, to get the same shape and type.
	for (m=0;m<metNd->nmetrics;m++) {

	  char sibname[300];
	  sprintf(sibname,"%s_%s",outvar->name, metNd->metricnames[m]);

	  // Check to see if this sibling variable exists; if so, log that
	  // fact, and escape.  This will trap out cases where the user has
	  // already defined the field name in the PCM - the user wins over
	  // default behavior when there is a  name collision.

	  // But!  This is a little tricky - it means that we
	  // will store the metric only for the very first dimension which
	  // we transform that gives us the same shape as the outvar.
	  // Thus, if we average time and then average onto a grid of the
	  // same size (maybe a five-rolling mean), we'll only get metrics
	  // for the time average.  I will probably add transform
	  // parameters to control this behavior: avoid metrics when
	  // transforming certain dimensions, or to modify the metric names
	  // so we don't have a name collision.
	  CDSVar *mvar = cds_get_var((CDSGroup *) (outvar->parent), sibname);

	  if (mvar) {
	    LOG(TRANS_LIB_NAME, 
		"Metric field %s already exists; no metrics stored while transforming dimension %d (%s)\n", 
		sibname, d, outvar->dims[d]->name);
	    continue;
	  }

	  // Okay, mvar doesn't exist, so we have to create and fill it

	  // I really think the metric is a clone - same type, same
	  // dimensions, same dataset.  Variable mitosis.
	  // The 0 means we don't copy data, which is good.
	  
	  // Fine, I'll clone it myself.  The only hassle is dim_names
	  char **dim_names=CALLOC(outvar->ndims, char*);
	  for (dm=0;dm<outvar->ndims;dm++) {
	    dim_names[dm]=outvar->dims[dm]->name;
	  }
	  
	  if ((mvar = cds_define_var((CDSGroup *) outvar->parent, sibname, outvar->type,
				     outvar->ndims, (const char **) dim_names))) {
	    // Very base attributes
	    cds_define_att_text(mvar, "long_name", "Metric %s for field %s",
				metNd->metricnames[m], outvar->name);

	    // Now do units.  If set to SAME, then use same units as outvar
	    if (strcmp(metNd->metricunits[m],"SAME") == 0) {
	      size_t lu;
	      CDSAtt *unit_att=cds_get_att(outvar,"units");
	      char *outvar_units = unit_att ? cds_get_att_text(unit_att,&lu,NULL) : NULL;
	      if (outvar_units == NULL) {
		ERROR(TRANS_LIB_NAME,
		      "Transformed variable %s does not have valid units attribute\n",
		      outvar->name);
		cds_define_att_text(mvar, "units", "unknown");
	      } else {
		cds_define_att_text(mvar, "units", "%s", outvar_units);
		free(outvar_units);
	      }
	    } else {
	      cds_define_att_text(mvar, "units", metNd->metricunits[m]); 
	    }

	    // Missing
	    void *missing_value=NULL;
	    int nms = cds_get_var_missing_values(outvar, &missing_value);

	    if (nms > 0 && missing_value) {
	      cds_define_att(mvar, "missing_value", outvar->type,
			     nms, missing_value); 
	    } else {
	      // Great.  Now I have to make up my own missing value
	      // Make sure this holds enough bytes for all data types
	      if (! (missing_value = malloc(cds_data_type_size(outvar->type)))) {
		ERROR(TRANS_LIB_NAME,
		      "Cannot allocate %d bytes for missing value\n",
		      cds_data_type_size(outvar->type));
		return(-1);
	      }

	      cds_get_default_fill_value(mvar->type, missing_value);
	      cds_define_att(mvar, "missing_value", mvar->type, 1,missing_value); 	      
	    }
	    
	    // Either way, have to free up 
	    free(missing_value);
	  } else {
	    LOG(TRANS_LIB_NAME,
		"Warning: Cannot create metric field %s; continuing...\n", sibname);
	  }
	  
	  free(dim_names);


#ifdef notdef	  
	  // This stuff, I think, is redundant, as I no longer need to
	  // do checks for if mvar previously existed.  If it exists, then
	  // we don't fill it, because it means the user wants it for
	  // something else.  But I'm keeping this code in here in case we
	  // change our mind and have upstream code create our metric vars
	  // at some later date.

	  // Now we idiot check everything.  A Null mvar or dimension
	  // mismatch is an easy no go.  Should I log this?
	  if (! mvar || mvar->ndims != Ndims) continue;

	  // Double check mvar's shape, in case it pre-existed
	  okshape=1;
	  for (dm=0;dm<mvar->ndims;dm++) {
	    if (shape[dm] != mvar->dims[dm]->length) {
	      okshape=0;
	      break;
	    }
	  }
	  if (!okshape)  continue;
#endif

	  // Hooray!  We can finally store the metric
	  if (! cds_set_var_data(mvar, CDS_DOUBLE, 0, msample,NULL, 
				    metNd->metrics[m])) {
	    LOG(TRANS_LIB_NAME,
		"Warning: Could not write data to metric field %s\n",
		mvar->name);
	  }
	  
	  // Okay, now tag this variable in outvars user data, to make it
	  // easier to find.
	  if (!cds_set_user_data(outvar, metNd->metricnames[m], (void *) mvar, NULL)) {
	  //if (!cds_set_user_data(outvar, metNd->metricnames[m], (void *) mvar, free)) {
	    LOG(TRANS_LIB_NAME, "Warning: vould not attach metric field %s to user data for %s\n",
		metNd->metricnames[m], outvar->name);
	  }
	} // m loop over metrics
      } // okshape
    } // if metNd

    // Either way, we need to free metNd for the next iteration of our
    // transform code
    free_metric(&metNd);

    // Now that we've transformed all the slices, time to move on to the
    // next dimension.  The output of this transform is the input to the
    // next, so reset our pointers and values.  This gets tricky.

    // First, free up - But! I don't want to free data and qc_data, the
    // original input yet.  So'll I kludge a free only if it's not our
    // first iteration.  And, I'll do it with a seperate counting variable,
    // so we can change the order of dims later.

    if (iter_count++ > 0) {
      if (tdata) free(tdata);
      if (qc_tdata) free(qc_tdata);
    }

    // This assign should work.
    tdata=odata;
    qc_tdata=qc_odata;
    memmove(tD,oD,Ndims*sizeof(int));
    memmove(tlen,olen,Ndims*sizeof(int));
    tNtot=oNtot;

    // do some freeing of our working arrays
    free(data1d);free(qc1d);free(odata1d);free(oqc1d);

    if (transform_name) free(transform_name);
  }

  // Great - we've transformed all the dimensions.  Our new data should be
  // in odata and qc_odata, with dims set correctly.  So now we just need
  // to copy to outvar and return happy.
  if (cds_set_var_data(outvar, CDS_DOUBLE, 0, olen[0],
			  &output_missing_value, odata) == NULL ||
      cds_set_var_data(qc_outvar, CDS_INT, 0, olen[0],
			  NULL, qc_odata) == NULL) {
    ERROR(TRANS_LIB_NAME, "Problem writing output data for %s or %s\n",
	  outvar->name, qc_outvar->name);
    return(-1);
  }
 
  // Old way - probably exactly the same
  //if (cds_put_var_data(outvar, 0, olen[0], CDS_DOUBLE, odata) == NULL ||
  //    cds_put_var_data(qc_outvar, 0, olen[0], CDS_INT, qc_odata) == NULL) {
  //  ERROR(TRANS_LIB_NAME, "Problem writing output data for %s or %s",
  //	  outvar->name, qc_outvar->name);
  //  return(-1);
  //  }

  // I'll free stuff up down here, only if things go well.  Brian will
  // probably hate that.
  if (data) free(data);
  if (qc_data) free(qc_data);
  if (odata) free(odata);
  if (qc_odata) free(qc_odata);
  // if (qc_temp) free(qc_temp);

  // Free our dimensional arrays
  free(D);free(tD);free(oD);
  free(len);free(tlen);free(olen);
  free(shape);

  // Free up our default qc_mapping_function.  Note that the way I've coded
  // this, you can only have one qc_mapping function for all input fields.
  // That's no good - I need a lookup table kind of thing; foo.
  if (qc_mapping_function == default_qc_mapping_function) {
    qc_mapping_function=NULL;
  }

  return(0);
}

// calculate the average interval for a given dim of a given var
// If this isn't regular, I don't know what this will tell you, but there
// it is.
double trans_calculate_interval(CDSVar *var, int dim) {
  int i;
  int cmpdbl(const void *, const void *);
  double val;

  int nv=var->dims[dim]->length;
  CDSVar *coord = cds_get_coord_var(var, dim);
  double *index=(double *) cds_copy_array(coord->type, nv,
					  coord->data.vp,
					  CDS_DOUBLE, NULL,
					  0, NULL, NULL, NULL,
					  NULL, NULL, NULL); 

  // Take median diff, which is more robust to slighly irregular grids

  // First, convert index to a diff array, in place.
  for (i=0;i<nv-1;i++) {
    index[i]=fabs(index[i+1]-index[i]);
  }

  // inefficient and even slightly wrong for even nvs; should use some
  // implementation of quickselect, possibly folded with the above diff-ing 
  // Still, O(N ln N) vs. O(N) isn't going to kill us in less we have
  // millions of elements.

  qsort(index,nv-1, sizeof(double), cmpdbl);
  val=index[(nv-1)/2];

  free(index);
  return(val);

  //double input_interval = fabs(index[nv-1]-index[0])/(float) (nv-1);
  //return(input_interval);
}

int cmpdbl(const void *v1, const void *v2) {
  double x1, x2;
  x1= *(double *)v1;
  x2= *(double *)v2;

  if (x1 < x2) return (-1);
  if (x1 > x2) return (1);
  return(0);
}


