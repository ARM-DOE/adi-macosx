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
*    $Revision: 56056 $
*    $Author: shippert $
*    $Date: 2014-08-07 16:26:54 +0000 (Thu, 07 Aug 2014) $
*    $State: $
*
********************************************************************************
*
*  NOTE: DOXYGEN is used to generate documentation for this file.
*
*******************************************************************************/

/** @file trans_bin_average
 *  1-D Bin average transformation functions
 */
# include <stdio.h>
# include <math.h>
# include <string.h>
# include <time.h>
# include <getopt.h>
# include <regex.h>
# include <cds3.h>
# include "trans.h"

static size_t _one=1;

/* We'll be doing a lot of dynamic allocation, so let's make it easier */
# define CALLOC(n,t)  (t*)calloc(n, sizeof(t))
# define REALLOC(p,n,t)  (t*) realloc((char *) p, (n)*sizeof(t));

// Average values into our bins - this is why we need the exact
// specification of our edge, so we can properly weight everything that
// goes into the average.

// Our bins will be given by _start and _end, which allows gaps
// in our bins.

/*
int trans_quadrature(float *array,
		int *qc_array,
		unsigned int qc_mask, 
		float *index_start,
		float *index_end,
		float *weights,
		int ni,
		float *output,
		int *qc_output,
		float *target_start,
		float *target_end,
		int nt,
		float missing_value) { 
*/

int quadrature(double *array,
		int *qc_array,
		unsigned int qc_mask, 
		double *index_start,
		double *index_end,
		double *weights,
		int ni,
		double *output,
		int *qc_output,
		double *target_start,
		double *target_end,
		int nt,
	       double missing_value);


int trans_quadrature_interface
(double *data, int *qc_data, double *odata, int *qc_odata,
 CDSVar *invar, CDSVar *outvar, int d) 
{

  int ni, nt, i, status;
  double *index_start=NULL, *index_end=NULL, *target_start=NULL,
    *target_end=NULL, *weights=NULL;
  double missing_value;
  unsigned int qc_mask;
  size_t  len;
  CDSVar *incoord, *outcoord;

  // Unlike other interfaces, these are just markers for the dimensions,
  // which we'll use to infer the bin edges.  
  double *index=NULL, *target=NULL;

  // Pull out the stuff we need from invar, outvar, the dimension index d,
  // and the right calls to the transfrom parameter functions
  ni=invar->dims[d]->length;
  nt=outvar->dims[d]->length;

  // These mostly work straight up because coord vars are always 1D, so we
  // don't have to worry about indexing or casting correctly
  incoord = cds_get_coord_var(invar, d);
  index=(double *) cds_copy_array(incoord->type, ni,
				  incoord->data.vp,
				  CDS_DOUBLE, NULL,
				  0, NULL, NULL, NULL, NULL, NULL, NULL); 

  outcoord = cds_get_coord_var(outvar, d);
  target=(double *) cds_copy_array(outcoord->type, nt,
				  outcoord->data.vp,
				  CDS_DOUBLE, NULL,
				   0, NULL, NULL, NULL, NULL, NULL, NULL); 

  // The easy lookups
  if (cds_get_transform_param_by_dim(invar, invar->dims[d],
				     "missing_value", CDS_DOUBLE,
				     &_one, &missing_value) == NULL) {
    missing_value=-9999.;
  }

  if (cds_get_transform_param_by_dim(invar, invar->dims[d],
				     "qc_mask", CDS_INT,
				     &_one, &qc_mask) == NULL) {
    qc_mask=get_qc_mask(invar);
    // I should now set the transform param so we don't have to calculate
    // it again.  Hmm.
  }

  ///////////////////////////////////////////////////////////////////////////
  // Weights - a little idiot checking
  if ((weights=cds_get_transform_param_by_dim(invar, invar->dims[d],
					      "weights", CDS_DOUBLE,
					      &len, weights)) == NULL) {
    weights=CALLOC(ni, double);
    for (i=0;i<ni;i++) {weights[i]=1.0;}
  } else if (len != ni) {
    ERROR("Bin average weights array for %s (%s) different size then input data (%s, %s); setting weights=1.0", 
	  invar->name, invar->dims[d]->name, len, ni);
    if (weights) free(weights);
    weights=CALLOC(ni, double);
    for (i=0;i<ni;i++) {weights[i]=1.0;}
  }

  // To do the edges, we'll drop into a subfunction, because we have to do
  // it with both input and output edges.  We'll use Brian's trick of
  // passing in a pointer to our pointers, so we can set the addresses.

  status = get_bin_edges(&index_start, &index_end, index, ni, invar, d);
  status = get_bin_edges(&target_start, &target_end, target, nt, outvar, d);

  // Now just call our core function
  status=quadrature(data, qc_data, qc_mask, index_start, index_end, weights, ni,
		     odata, qc_odata, target_start, target_end, nt, missing_value);

  // Set the qc bits if we estimated the bin boundaries
  set_estimated_bin_qc(qc_odata, invar, outvar, d, nt); 

  if (index) free(index);
  if (target) free(target);
  if (weights) free(weights);
  if (index_start) free(index_start);
  if (index_end) free(index_end);
  if (target_start) free(target_start);
  if (target_end) free(target_end);

  return(status);
}

// removed get_bin_edges(), since it should be the exact same one as
// defined in trans_bin_average.c, and we have it prototyped above.  There
// may be some issues with the transform parameter thing, making sure we
// read the same inputs for the sigma fields as we do for the regular
// fields, but we'll worry about that as we test.

int quadrature(double *array,
		int *qc_array,
		unsigned int qc_mask, 
		double *index_start,
		double *index_end,
		double *weights,
		int ni,
		double *output,
		int *qc_output,
		double *target_start,
		double *target_end,
		int nt,
		double missing_value) { 

  int i,i0,j,qco;
  double u, v, w, bin;
  double sum_array, sum_weight, max_weight;
  int sign;

  // Monotonically increasing or decreasing.  There are cleverer ways of
  // doing this, but this is at least clear
  if (index_start[0] < index_start[1] && target_start[0] < target_start[1]) {
    sign = 1;
  } else if (index_start[0] > index_start[1] && target_start[0] > target_start[1]) {
    sign = -1;
  } else {
    ERROR(TRANS_LIB_NAME, "Target and index are not monotonically aligned");
    return(-5);
  }

  // Set our weights=1.0 if not given
  if (weights == NULL) {
    weights=CALLOC(ni,double);
    for(i=0;i<ni;i++) {
      weights[i]=1.0;
    }
  }

  // again, i indexes our input field, j indexes the target field
  i0=0;
  for (j=0; j<nt; j++) {
    // reinit
    sum_array=sum_weight=max_weight=0.0;
    qco=0;
    i=i0;  // start with previous first input bin

    // run up i until we find an input bin that overlaps our target bin;
    // this means the upper index edge is at or above the lower target edge
    while (i < ni && sign*index_end[i] < sign*target_start[j]) { i++;}

    // Some idiot checking on i goes here

    // So now we know that input bin i has an end greater than the start of
    // output bin j.  Which is good - they (might) overlap.

    // Save this as i0 to start the next iteration from; this will allow
    // rolling averages and overlapping output bins, as long as the
    // target_start[j] are monotonically increasing (and, possibly, equal)
    i0=i;

    // Alright, so now we continue to run up i until the lower index edge
    // is at or above the upper target edge - this time, however, we also
    // start doing the average
    while (i < ni && sign*index_start[i] < sign*target_end[j]) {

      // need to check the qc of each bin i; if any of them fail, set
      // qcval=1 and don't use them in the average.  If we miss to many,
      // we'll hcange qcval=2 later on
      // ignore qc_array for now - we are revising the whole thing


      // We find our maximum input weights up here - if this is nonzero but
      // the sum_weight is zero, we know it's because of bad data, rather
      // than zero weighting in this region.
      if (weights[i] > max_weight) {max_weight=weights[i];}

      if (array[i] <= missing_value || (qc_array[i] & qc_mask)) {
	qc_set(qc_output[j], QC_SOME_BAD_INPUTS);
	i++;
	continue;
      }
	
      // Now we'll use the weights given on input, with u and v the
      // fraction of an input bin that's outside our output bin.
      //w=1.0;
      w=1.0;

      // This is tricky for monotonic decreasing, but I think the negatives
      // all cancel out correctly.  Even though negative bin size is weird.
      bin=index_end[i]-index_start[i];
      if ((u=(target_start[j]-index_start[i])/bin) > 0) w-=u;
      if ((v=(index_end[i]-target_end[j])/bin) > 0) w-=v;

      if (u>1.0 || v>1.0 || u+v>1.0) {
	ERROR(TRANS_LIB_NAME,
	      "Problem with bin average: input bin %d [%f,%f], output bin %d [%f,%f]",
		 i, index_start[i], index_end[i], j, target_start[j], target_end[j]);
	return(-1);
      }

#ifdef notdef	
      // Don't think this is right - we want to figure out the fraction of
      // input bin that is within the output bin boundaries - it's hard to
      // do this generically, because we might have larger input bins than
      // output bins, or two huge input bins that are nevertheless
      // straddled by the output bin.  Weird.

      // DEFINITELY NOT RIGHT!
      //if ((u=(target_end[j]-index_end[i])/bin) < 1) w=u;
      // if ((v=(index_start[i]-target_start[j])/bin) < 1) w=v;
#endif
	
      // now mult by the actual weight of this bin
      w *= weights[i];
	
      //sum_array += w*array[i];
      //sum_weight += w;
      sum_array += w*w*array[i]*array[i];  // adding in quadrature, which
					   // should square the weights,
					   // too.  I think.
      sum_weight += w;

      // If we are actually weighting by this value, then we need to pass
      // through the qc as well.
      if (w > 0) {
	qco |= qc_array[i];
      }
	
      // Finally, don't forget to increment
      i++;

    }

    // So our average value is what it is
    if (max_weight==0) {
      // This means all the weights were zero, so we set the output to zero
      output[j]=0;
      qc_set(qc_output[j], QC_ZERO_WEIGHT);
    } else if (sum_weight == 0) {
      // Now we know we had a positive weight, so this means all the data
      // failed QC, and should be crunched.
      //fprintf(stderr, "quadrature: target bin [%f,%f] has no good values to average\n",
      //target_start[j],target_end[j]);
      output[j]=missing_value;
      qc_set(qc_output[j], QC_ALL_BAD_INPUTS);
      qc_set(qc_output[j], QC_BAD);
    } else {
      output[j]=sqrt(sum_array)/sum_weight;

      // If any of the input points were yellow (i.e. if they intersect
      // with the non-qc_masked bits), set output to indeterminate
      // We  used to just pass through the bits like this:
      // qc_output[j] |= qco
      if ((qco & ~qc_mask) != 0) {
	qc_set(qc_output[j], QC_INDETERMINATE);
      }
    }
  }

  return(0);
}

