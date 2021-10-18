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

  static cl::opt<int> LoopLimit("llimit",
                                cl::desc("Maximum number of iterations any loop should run."),
                                cl::value_desc("Maximum number of iteration in loop."), cl::init(5));

  /***
   * The main pass.
   */
  struct LimitLoopIterationPass : public ModulePass {
  public:
    static char ID;

    // Map of loops to custom counter variable.
    std::map<Loop *, AllocaInst *> LoopCounterMap;

    LimitLoopIterationPass() : ModulePass(ID) {
    }

    ~LimitLoopIterationPass() {
    }

    AllocaInst *getLoopCounter(Loop *L, Function *TF) {
      if (LoopCounterMap.find(L) == LoopCounterMap.end()) {
        auto &EBB = TF->getEntryBlock();
        // Insert a new alloca instruction into EBB
        // and store into LoopCounterMap[L]
        // TODO: fill this.
      }
      return LoopCounterMap[L];
    }

    void initializeCounter(IRBuilder<> &IR, AllocaInst *A) {
      // Set the loop counter value to 0
      // TODO: fill this.
    }

    void incrementAndStore(IRBuilder<> &IR, AllocaInst *A) {
      // Increment A i.e., A++
      // TODO: fill this.
    }

    void checkAndBranch(IRBuilder<> &IR, AllocaInst *A, BasicBlock *TBB, BasicBlock *FBB) {
      // Check A against LoopLimit i.e., A < LoopLimit
      // if true go to TBB
      // else go to FBB
      // TODO: fill this.
    }

    void initializeLoopCounter(Loop *L, Function *F) {
      BasicBlock *LHdr = L->getHeader();
      for (const auto &PBB : predecessors(LHdr)) {
        // In each of these predecessors.
        // Initialize the loop counter for the loop to 0.
        // TODO: fill this.
      }

    }

    bool runOnModule(Module &M) override {
      bool Edited = false;
      for (auto &CurrF: M) {
        // does this function have body?
        if(!CurrF.isDeclaration()) {
          // get information about loops.
          auto &LoopAnalysis = getAnalysis<LoopInfoWrapperPass>(CurrF);
          LoopInfo &LInfo = LoopAnalysis.getLoopInfo();
          // iterate over all the loops
          for (auto *LObj: LInfo.getLoopsInPreorder()) {
            // Initialize the new loop counter to zero.
            AllocaInst *LoopCounter = getLoopCounter(LObj, &CurrF);
            initializeLoopCounter(LObj, &CurrF);

            // get the exit basic blocks.
            SmallVector<BasicBlock *, 32> ExitBBs;
            ExitBBs.clear();
            LObj->getExitBlocks(ExitBBs);
            // We will pick the first exit BB
            BasicBlock *TargetExitBB = &(*ExitBBs.front());

            // Our Logic:
            // We split the old loop header into the two parts.
            // LHdr : Top half.
            // NewHdr: Bottom half.
            // Next, we insert LHdr -> CBB
            // In CBB:
            // if (loopcounter < max) jump to StoreBB else ExitBB (Exit the loop).
            // New basic block: StoreBB:
            // loopcounter++; jump to NewHdr <- Enter the loop.

            // Split the old loop header.
            if (LoopCounter != nullptr) {
                BasicBlock *LHdr = LObj->getHeader();
                auto *OldTI = LHdr->getTerminator();
                BasicBlock *NewHdr = LHdr->splitBasicBlock(OldTI, "LoopIncr");

                // CBB
                BasicBlock *CheckBB = BasicBlock::Create(M.getContext(), "CheckBB", &CurrF);
                // Old hdr -> CBB
                LHdr->getTerminator()->replaceSuccessorWith(NewHdr, CheckBB);

                // SBB: This Basic block increments the loop counter.
                BasicBlock *StoreBB = BasicBlock::Create(M.getContext(), "StoreBB", &CurrF);
                IRBuilder<> SIR(StoreBB);
                // loopcounter++;
                incrementAndStore(SIR, LoopCounter);
                // SBB->NewHdr
                SIR.CreateBr(NewHdr);
                // The following will make sure that all phi instructions
                // that use LHdr will be replaced with StoreBB.
                NewHdr->replacePhiUsesWith(LHdr, StoreBB);

                IRBuilder<> CIR(CheckBB);
                // if (loopcounter < max) jump to StoreBB else TargetExitBB (Exit the loop).
                checkAndBranch(CIR, LoopCounter, StoreBB, TargetExitBB);
                // handle PHi instruction.
                // We are adding CheckBB as the new predecessor.
                // So, make sure that all phi nodes in TargetExitBB are updated to use
                // first value as the incoming value from CheckBB.
                // TODO: fill this

                Edited = true;
            }
          }
        }
      }
      return Edited;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<LoopInfoWrapperPass>();
    }

  };

  char LimitLoopIterationPass::ID = 0;
  // pass arg, pass desc, cfg_only, analysis only
  static RegisterPass<LimitLoopIterationPass> x("limitloop",
                                                "Instrument the bitcode to limit number of loop iterations "
                                                "to a fixed number.",
                                                    false, false);
}