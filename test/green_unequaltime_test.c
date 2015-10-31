#include "greens_func.h"
#include "util.h"
#include <mkl.h>
#include <math.h>
#include <stdio.h>


int GreenUnequalTimeTest()
{
	int status;

	const int N = 5;	// block size
	const int L = 4;	// number of time steps

	printf("Computing the unequal time Green's function for N = %i and L = %i...\n", N, L);

	// load B matrices from disk
	double **B = (double **)MKL_malloc(L * sizeof(double *), MEM_DATA_ALIGN);
	int l;
	for (l = 0; l < L; l++)
	{
		B[l] = (double *)MKL_malloc(N*N * sizeof(double), MEM_DATA_ALIGN);

		char path[1024];
		sprintf(path, "../test/green_unequaltime_test_B%i.dat", l);
		status = ReadData(path, B[l], sizeof(double), N*N);
		if (status != 0) { return status; }
	}

	// temporary H matrix
	double *H = (double *)MKL_malloc(N*N*L*L * sizeof(double), MEM_DATA_ALIGN);

	// compute unequal time Green's functions
	double *Gtau0 = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *G0tau = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *Geqlt = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	// averages
	double *Gtau0_avr = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *G0tau_avr = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *Geqlt_avr = (double *)MKL_malloc(  N*N * sizeof(double), MEM_DATA_ALIGN);	// dimension N x N
	ComputeUnequalTimeGreensFunction(N, L, B, H, Gtau0, G0tau, Geqlt, Gtau0_avr, G0tau_avr, Geqlt_avr);

	// load reference data from disk
	double *Gtau0_ref = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *G0tau_ref = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *Geqlt_ref = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	// averages
	double *Gtau0_avr_ref = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *G0tau_avr_ref = (double *)MKL_malloc(L*N*N * sizeof(double), MEM_DATA_ALIGN);
	double *Geqlt_avr_ref = (double *)MKL_malloc(  N*N * sizeof(double), MEM_DATA_ALIGN);	// dimension N x N
	status = ReadData("../test/green_unequaltime_test_Gtau0.dat",     Gtau0_ref,     sizeof(double), L*N*N); if (status != 0) { return status; }
	status = ReadData("../test/green_unequaltime_test_G0tau.dat",     G0tau_ref,     sizeof(double), L*N*N); if (status != 0) { return status; }
	status = ReadData("../test/green_unequaltime_test_Geqlt.dat",     Geqlt_ref,     sizeof(double), L*N*N); if (status != 0) { return status; }
	status = ReadData("../test/green_unequaltime_test_Gtau0_avr.dat", Gtau0_avr_ref, sizeof(double), L*N*N); if (status != 0) { return status; }
	status = ReadData("../test/green_unequaltime_test_G0tau_avr.dat", G0tau_avr_ref, sizeof(double), L*N*N); if (status != 0) { return status; }
	status = ReadData("../test/green_unequaltime_test_Geqlt_avr.dat", Geqlt_avr_ref, sizeof(double),   N*N); if (status != 0) { return status; }

	// entrywise absolute error of matrix entries
	double err = fmax(fmax(fmax(fmax(fmax(
		UniformDistance(L*N*N, Gtau0,     Gtau0_ref),
		UniformDistance(L*N*N, G0tau,     G0tau_ref)),
		UniformDistance(L*N*N, Geqlt,     Geqlt_ref)),
		UniformDistance(L*N*N, Gtau0_avr, Gtau0_avr_ref)),
		UniformDistance(L*N*N, G0tau_avr, G0tau_avr_ref)),
		UniformDistance(  N*N, Geqlt_avr, Geqlt_avr_ref));
	printf("Largest entrywise absolute error: %g\n", err);

	// clean up
	MKL_free(Geqlt_avr_ref);
	MKL_free(G0tau_avr_ref);
	MKL_free(Gtau0_avr_ref);
	MKL_free(Geqlt_ref);
	MKL_free(G0tau_ref);
	MKL_free(Gtau0_ref);
	MKL_free(Geqlt_avr);
	MKL_free(G0tau_avr);
	MKL_free(Gtau0_avr);
	MKL_free(Geqlt);
	MKL_free(G0tau);
	MKL_free(Gtau0);
	MKL_free(H);
	for (l = 0; l < L; l++)
	{
		MKL_free(B[l]);
	}
	MKL_free(B);

	return (err < 2e-15 ? 0 : 1);
}
