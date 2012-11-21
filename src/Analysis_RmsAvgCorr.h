#ifndef INC_ANALYSIS_RMSAVGCORR_H
#define INC_ANALYSIS_RMSAVGCORR_H
#include "Analysis.h"
#include "DataSet_Coords.h"
// Class: Analysis_RmsAvgCorr
/// Calculate rmsd using running avg structures
class Analysis_RmsAvgCorr: public Analysis {
  public:
    Analysis_RmsAvgCorr();

    static DispatchObject* Alloc() { return (DispatchObject*)new Analysis_RmsAvgCorr(); }
    static void Help();

    Analysis::RetType Setup(ArgList&,DataSetList*,TopologyList*,int);
    Analysis::RetType Analyze();
    void Print(DataFileList*);
  private:
    AtomMask mask_;
    std::string separateName_;
    std::string outfilename_;
    DataSet_Coords* coords_;
    DataSet* Ct_;
    int maxwindow_;
    bool useMass_;
};
#endif  
