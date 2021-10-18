//
// Created by machiry at the beginning of time.
//

#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <llvm/Analysis/CallGraph.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/CFGPrinter.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Analysis/CFG.h>
#include <llvm/IR/LLVMContext.h>
#include <set>


using namespace llvm;


namespace HSS {

  static cl::opt<std::string> TargetFunc("targetFunc",
                                         cl::desc("The target function to which we should force the execution."),
                                         cl::value_desc("full name of the function"), cl::init(""));

  static void checkArgs(Module &m) {
    // Validate the given function name.
    if (!TargetFunc.empty()) {
      auto *F = m.getFunction(TargetFunc);
      if (F == nullptr || F->isDeclaration()) {
        errs() << "[-] Given function does not exist or it is not defined in the given module.\n";
        exit(-1);
      }
      // All Good.
      return;
    }
    errs() << "[-] No target function provided.\n";
    exit(-1);
  }

  /***
   * The main pass.
   */
  struct FunctionPrioritizationPass : public ModulePass {
  public:
    static char ID;
    // For each of the function in the module,
    // this map contains <function> <-> sets of instructions
    // that are calling the function.
    std::map<Function *, std::set<Instruction *>> CallerInsts;

    // All functions reachable from the given function.
    // We should not instrument these functions.
    std::set<Function *> ReachableFuncs;

    // For each function, this map contains the set of interesting basicblocks.
    std::map<Function *, std::set<BasicBlock *>> InterestingBBs;

    // Exit Function.
    Function *EF;

    FunctionPrioritizationPass() : ModulePass(ID) {
      this->EF = nullptr;
    }

    ~FunctionPrioritizationPass() {
      CallerInsts.clear();
      ReachableFuncs.clear();
    }

    Function *getExitFunction(Module &M) {
      if (this->EF == nullptr) {
        // Get "exit" function.
        // Signature: void exit(int)
        //TODO: fill this code.
      }
      return this->EF;
    }

    void computeCallerInstrs(Function *TF) {
      for (auto &CBB: *TF) {
        for (auto &CINS: CBB) {
          Instruction *I = &CINS;

          // Check if current instruction is a call instruction
          // and the called function is defined in the module?
          // if yes, get the called function and populate CallerInsts.
          // TODO: fill this.
        }
      }
    }

    void computeReachableFunctions(CallGraphNode *GN, std::set<CallGraphNode *> &Visited) {
      if (Visited.find(GN) == Visited.end()) {
        Visited.insert(GN);
        Function *CF = GN->getFunction();
        // If the current function is not an external function?
        // populate ReachableFuncs with the function.
        // TODO: fill your code here.
        // Find all the functions called from GN,
        // i.e., all the edges from GN and call computeReachableFunctions on the reached node.
        // TODO: Fill your code here.
      }
    }

    void computeInterestingBBs(Function *TF, std::set<Function *> &Visited) {
      if (Visited.find(TF) == Visited.end()) {
        Visited.insert(TF);
        dbgs() << "[+] Computing interesting basic blocks for the function:" << TF->getName().str() << "\n";
        // Iterate through all the call instructions
        // that are calling the target function
        // and consider the corresponding basic blocks as interesting.
        for (auto *I: CallerInsts[TF]) {
          auto *BB = I->getParent();
          auto *TFB = BB->getParent();
          InterestingBBs[TFB].insert(BB);
          computeInterestingBBs(TFB, Visited);
        }
      }
    }

    bool instrumentFunction(Function *TF) {
      bool Edited = false;
      // First, if this function is reachable from TargetFunc,
      // then do not instrument this.
      // hint: ReachableFuncs
      // TODO: fill this.

      // Next,
      for (auto &CurBB: *TF) {
          bool IsUseful = false;
          // For each basic block in TF, check if it can reach
          // any of the InterestingBBs of TF.
          // if yes, DO NOT instrument it.
          // else, instrument the basic block by inserting a
          // call to exit function.
          // TODO: fill this.

          // This basic block is not useful.
          if (!IsUseful) {
            dbgs() << "[+] Trying to insert call into basic block:" << CurBB.getName().str() << "\n";
            // Insert call to exit function.
            // TODO: fill this.
            Edited = true;
          }
        }

      if (Edited) {
        dbgs() << "[+] Instrumented function:" << TF->getName().str() << "\n";
      } else {
        dbgs() << "[+] Not instrumenting function:" << TF->getName().str() << "\n";
      }
      return Edited;
    }


    bool runOnModule(Module &M) override {
      bool Edited = false;
      // sanity
      checkArgs(M);

      // First get all the reachable function from the given function.
      auto &CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();
      Function *MF = M.getFunction(TargetFunc);
      std::set<CallGraphNode *> VisitedCGNodes;
      dbgs() << "[+] Trying to compute all reachable functions from the target function.\n";
      computeReachableFunctions(CG.getOrInsertFunction(MF), VisitedCGNodes);

      // Next compute caller instructions map by
      // iterating through each function.
      for (auto &F: M) {
        if (!F.isDeclaration()) {
          dbgs() << "[+] Computing caller instructions for the function:" << F.getName().str() << "\n";
          computeCallerInstrs(&F);
        }
      }

      // Compute all interesting basic blocks in each function
      // starting from TargetFunc.
      std::set<Function *> VisitedFuncs;
      computeInterestingBBs(MF, VisitedFuncs);

      // Now instrument each function.
      for (auto &F: M) {
        if (!F.isDeclaration()) {
          llvm::dbgs() << "[+] Trying to instrument function:" << F.getName().str() << "\n";
          Edited = instrumentFunction(&F) || Edited;
        }
      }
      return Edited;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<CallGraphWrapperPass>();
    }

  };

  char FunctionPrioritizationPass::ID = 0;
  // pass arg, pass desc, cfg_only, analysis only
  static RegisterPass<FunctionPrioritizationPass> x("funcprio",
                                                    "Instrument the bitcode to force execution of the given function.",
                                                    false, false);
}