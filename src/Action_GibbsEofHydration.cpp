#include <cmath> // log
#include "Action_GibbsEofHydration.h"
#include "CpptrajStdio.h" // mprintf, mprinterr

// CONSTRUCTOR
Action_GibbsEofHydration::Action_GibbsEofHydration() :
  maxVoxelOccupancyCount_(600) // NOTE: See header for comments.
{}

// Action_GibbsEofHydration::init()
int Action_GibbsEofHydration::init() {
  // Get output filename
  filename_ = actionArgs.GetStringNext();
  if (filename_.empty()) {
    mprinterr("Error: GibbsEofHydration: no filename specified.\n");
    return 1;
  }
  // Get grid options (<nx> <dx> <ny> <dy> <nz> <dz> [box|origin] [negative])
  if (GridInit( "GibbsEofHydration", actionArgs ))
    return 1;

  // Get mask
  char* maskexpr = actionArgs.getNextMask();
  if (maskexpr==NULL) {
    mprinterr("Error: GRID: No mask specified.\n");
    return 1;
  }
  mask_.SetMaskString(maskexpr);

  // Info
  GridInfo();
  mprintf("\tGrid will be printed to file %s\n",filename_.c_str());
  mprintf("\tMask expression: [%s]\n",mask_.MaskString());

  // Allocate grid
  if (GridAllocate()) return 1;

  return 0;
}

// Action_GibbsEofHydration::setup()
int Action_GibbsEofHydration::setup() {
  // Setup grid, checks box info.
  if (GridSetup( currentParm )) return 1;

  // Setup mask
  if (currentParm->SetupIntegerMask( mask_ ))
    return 1;
  mprintf("\t[%s] %i atoms selected.\n", mask_.MaskString(), mask_.Nselected());
  if (mask_.None()) {
    mprinterr("Error: GibbsEofHydration: No atoms selected for parm %s\n", currentParm->c_str());
    return 1;
  }

  return 0;
}

// Action_GibbsEofHydration::action()
int Action_GibbsEofHydration::action() {
  double XYZ[3], boxcrd[3];
  
  if (GridBox()) {
    // Grid based on box dimensions - get box center.
    currentFrame->BoxXYZ( boxcrd );
    boxcrd[0] /= 2.0;
    boxcrd[1] /= 2.0;
    boxcrd[2] /= 2.0;
    for (AtomMask::const_iterator atom = mask_.begin();
                                  atom != mask_.end(); ++atom)
    {
      currentFrame->GetAtomXYZ( XYZ, *atom );
      double xx = XYZ[0] - boxcrd[0];
      double yy = XYZ[1] - boxcrd[1];
      double zz = XYZ[2] - boxcrd[2];
      //mprintf("BATM %6i ", *atom + 1);
      GridPoint( xx, yy, zz );
    }
  } else {
    // Normal grid
    for (AtomMask::const_iterator atom = mask_.begin();
                                  atom != mask_.end(); ++atom)
    {
      currentFrame->GetAtomXYZ( XYZ, *atom );
      //mprintf("ATOM %6i ", *atom + 1);
      GridPoint( XYZ[0], XYZ[1], XYZ[2] );
    }
  }

  return 0;
}

// Action_GibbsEofHydration::print()
void Action_GibbsEofHydration::print() {
  CpptrajFile outfile;
  /* How times does this occupancy count value arise?
   *    i.e. if  
   *                 voxelOccupancyCount[50] = 10 
   * 
   *         then there are 10 voxels with an occupancy count 50
   */
  std::vector<int> voxelOccupancyCount;
  // Largest occupancy count
  int currentLargestVoxelOccupancyCount;
  /* Value of most frequent voxel occupancy count
   *
   * i.e. if 
   *                 voxelOccupancyCount[50] = 10 
   *                 voxelOccupancyCount[51] = 100
   *                 voxelOccupancyCount[52] = 5
   *      then
   *                 mostFrequentVoxelOccupancy would be 51
   */
  int mostFrequentVoxelOccupancy;

  // Zero the voxelOccupancyCount array
  voxelOccupancyCount.assign(maxVoxelOccupancyCount_, 0);
  // Determine frequency for bin populations
  for (Grid::iterator gval = begin(); gval != end(); ++gval) {
        int bincount = (int) *gval;
        // Sanity check: if bin count >= size of voxelOccupancyCount, 
        //               increase size.
        if (bincount >= (int)voxelOccupancyCount.size())
          voxelOccupancyCount.resize( bincount+1, 0 );
        // Add one to the counter for the fequency of this bin population
        ++voxelOccupancyCount[ bincount ];
  }
  // Walk the voxelOccupancyCount[] array and determine which
  // occupancy is the most frequent (excluding 0).
  currentLargestVoxelOccupancyCount = voxelOccupancyCount[1];
  mostFrequentVoxelOccupancy = 1;
  //mprintf("CDBG: voxelOccupancyCount[1] = %i\n", voxelOccupancyCount[1]);
  for (int i = 2; i < (int)voxelOccupancyCount.size(); ++i) {
    //mprintf("CDBG: voxelOccupancyCount[%i] = %i\n", i, voxelOccupancyCount[i]);
    // Determine which voxel has the higest occupancy count
    // i.e. which is the most frequent value.
    if (voxelOccupancyCount[i] > currentLargestVoxelOccupancyCount) {
      currentLargestVoxelOccupancyCount = voxelOccupancyCount[i];
      mostFrequentVoxelOccupancy = i;
    }
  }
  mprintf("CDBG: Most frequent occupancy is %i (%i occurences)\n", mostFrequentVoxelOccupancy,
          currentLargestVoxelOccupancyCount);
  // The assumption here is that mostFrequentVoxelOccupancy corresponds to the
  // the occupancy count of bulk solvent
  int normalisationFactor = mostFrequentVoxelOccupancy;

  for (Grid::iterator gval = begin(); gval != end(); ++gval) {
    double gridval = (double)(*gval);
    gridval = -1.0 * log( (gridval / normalisationFactor) + 0.00000001 );
    *gval = (float)gridval;
  }

  PrintXplor( filename_, "", "REMARKS Change in Gibbs energy from bulk solvent with bin normalisation of " + integerToString(normalisationFactor) );
}

