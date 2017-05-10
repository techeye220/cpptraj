#ifndef INC_ANALYSIS_TI_H
#define INC_ANALYSIS_TI_H
#include "Analysis.h"
#include "Array1D.h"
class Analysis_TI : public Analysis {
  public:
    Analysis_TI();
    DispatchObject* Alloc() const { return (DispatchObject*)new Analysis_TI(); }
    void Help() const;

    Analysis::RetType Setup(ArgList&, AnalysisSetup&, int);
    Analysis::RetType Analyze();
  private:
    /// Averaging type: normal, skip #s of points, incremental avg
    enum AvgType { AVG = 0, SKIP, INCREMENT };
    /// Integration type
    enum ModeType { GAUSSIAN_QUAD = 0, TRAPEZOID };
    typedef std::vector<int> Iarray;
    typedef std::vector<double> Darray;
    typedef std::vector<DataSet*> DSarray;

    int SetQuadAndWeights(int);
    void DoBootstrap(int, DataSet_1D*);
    int Calc_Nskip(Darray&);
    int Calc_Avg(Darray&);
    int Calc_Increment(Darray&);

    Array1D input_dsets_; ///< Input DV/DL data sets
    Iarray nskip_;        ///< Numbers of data points to skip in calculating <DV/DL>
    DataSet* dAout_;      ///< Free energy data set
    DataSet* orig_avg_;   ///< Average DV/DL
    DataSet* bs_avg_;     ///< Bootstrap average DV/DL
    DataSet* bs_sd_;      ///< Bootstrap DV/DL standard deviation
    DataSetList* masterDSL_;
    DSarray curve_;       ///< TI curve data set for each skip/increment value
    Darray xval_;         ///< Hold abscissas corresponding to data sets.
    Darray wgt_;          ///< Hold Gaussian quadrature weights
    ModeType mode_;       ///< Integration mode
    AvgType avgType_;     ///< Type of averaging to be performed.
    int debug_;
    int n_bootstrap_pts_; ///< # points for bootstrap error analysis
    int n_bootstrap_samples_; ///< # of times to resample for bootstrap analysis
    int bootstrap_seed_;  ///< # RNG seed. Input data set index added for each set.
    int avg_interval_;    ///< # points to skip between each average calc
    int avg_max_;         ///< Max number of points to use in average (default all) 
    int avg_skip_;        ///< Number of points to skip when calculating the average.
};
#endif
