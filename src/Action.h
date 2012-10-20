#ifndef INC_ACTION_H
#define INC_ACTION_H
#include "DispatchObject.h"
#include "FrameList.h"
#include "ArgList.h"
#include "DataSetList.h"
#include "DataFileList.h"
#include "TopologyList.h"
// Class: Action 
/// The base class that all other actions inherit. 
/** Each action currently holds the memory address of all important state 
  * information: the master dataset list, datafile list, parm file list, 
  * and reference frame list. By convention actions have 4 main phases: init, 
  * setup, and action which in the main action list are called from Init,
  * Setup, and DoAction respectively. A fourth function, print, is optional
  * and is called after all frames are processed.
  */
class Action : public DispatchObject {
  public:
    /// Enumerate potential return states from DoAction.
    enum ActionReturnType { ACTION_OK=0, ACTION_ERR, ACTION_USEORIGINALFRAME,
                            ACTION_SUPPRESSCOORDOUTPUT 
                          };
    /// Action initialization and setup status.
    enum ActionStatusType { NO_INIT=0, INIT, NO_SETUP, SETUP, INACTIVE };
    void SetStatus(ActionStatusType stat) { status_ = stat; }
    ActionStatusType Status() { return status_; }
  
    Action();               // Constructor
    virtual ~Action() {}    // Destructor - virtual since this class is inherited

    void SetArg(const ArgList&); ///< Set the argument list
    void SetDebug(int dIn) { debug = dIn; }
    const char *ActionCommand(); ///< Print the command that calls the action
    const char *CmdLine();       ///< Print the entire argument line

    /// Initialize action, call init()
    int Init(DataSetList*, FrameList*, DataFileList*, TopologyList *,int); 
    /// Set up action for parm, call setup()
    ActionReturnType Setup(Topology **);
    /// Perform action on frame, call action()
    ActionReturnType DoAction(Frame **,int);    

    // --== Inherited by child classes ==--
    /// Print anything besides datasets, called at end of execution
    /** Perform any output not related to master dataset output, or any 
      * necessary post-trajectory calculations, e.g. in the 2drms command
      * several passes must be made over the input frames, which are stored
      * by that action.
      */
    virtual void print() = 0; 

  protected:
    int debug;                   ///< action debug level
    ArgList actionArgs;          ///< The action arguments (setArg)
    Topology* currentParm;       ///< The current parmtop (setup)
    Frame* currentFrame;         ///< The current frame (action)
    DataSetList* DSL;            ///< Pointer to the data set list in Cpptraj (init)
    DataFileList* DFL;           ///< Pointer to the data file list in Cpptraj (init)
    TopologyList* PFL;           ///< Pointer to the parm file list in Cpptraj (init)
    FrameList* FL;               ///< Pointer to the reference frame list in Cpptraj (init)
    int frameNum;                ///< # of current frame being processed, set by ActionList
    // --== Inherited by child classes ==--
    /// actions internal setup routine, called by Setup
    /** Setup action. Process any parm-dependent things like masks.
      */
    virtual int setup() = 0;
    /// actions internal action routine, called by DoAction
    /** Perform action. Only parts of the action which depend on input
      * coordinates should be implemented here.
      */
    virtual int action() = 0;
    /// actions internal init routine, called by Init
    /** Initialize action. Parse arguments from actionArgs. Called prior to reading
      * input trajectories. Expected order of checking args is: 1) Keywords, 2) 
      * Masks, 3) dataset names.
      */
    virtual int init() = 0;
  private:
    ActionStatusType status_; 
};
#endif  
