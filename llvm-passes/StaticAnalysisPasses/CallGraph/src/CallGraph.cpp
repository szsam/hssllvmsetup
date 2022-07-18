//
// Created by machiry at the beginning of time.
//

#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <llvm/Support/Debug.h>
#include <llvm/IR/Module.h>
#include <vector>
#include <algorithm>


using namespace llvm;

namespace HSS {


  /***
   * This pass construct call graph.
   */
  struct CallGraphPass : public ModulePass {
  private:
      std::vector<std::string> ftable;
      std::vector<std::vector<int>> adjList;

  public:
    static char ID;

    CallGraphPass() : ModulePass(ID) {
    }

    ~CallGraphPass() {
    }

    int getFuncID(const std::string &funcName) {
        auto iter = std::find(ftable.begin(), ftable.end(), funcName);
        int id = iter - ftable.begin();
        if (iter == ftable.end()) {
            ftable.push_back(funcName);
            adjList.push_back(std::vector<int>());
        }
        return id;
    }

    void addEdge(int s, int d) {
        adjList[s].push_back(d);
    }

	// A recursive function to print all paths from 'u' to 'd'.
	// visited[] keeps track of vertices in current path.
	// path[] stores actual vertices and path_index is current
	// index in path[]
	void printAllPathsUtil(int u, int d, std::vector<int> &visited,
                           std::vector<int> &path)
	{
	    // Mark the current node and store it in path[]
	    visited[u] = 1;
        path.push_back(u);

	    // If current vertex is same as destination, then print
	    // current path[]
	    if (u == d) {
            for (auto &v : path)
	            dbgs() << v << " ";
            dbgs() << "\n";
	    }
	    else // If current vertex is not destination
	    {
	        // Recur for all the vertices adjacent to current
	        // vertex
            for (auto &v : adjList[u]) {
                if (!visited[v])
                    printAllPathsUtil(v, d, visited, path);
            }
	    }

	    // Remove current vertex from path[] and mark it as
	    // unvisited
        path.pop_back();
        visited[u] = 0;
	}


	// Prints all paths from 's' to 'd'
	void printAllPaths(int s, int d)
	{
	    // Mark all the vertices as not visited
        std::vector<int> visited(adjList.size(), 0);

	    // Create an array to store paths
        std::vector<int> path;

	    // Call the recursive helper function to print all paths
	    printAllPathsUtil(s, d, visited, path);
	}


    bool checkFunction(Function &currFunc) {
      bool hasAccesses = false;
      std::string funcName = "NONAME";
      if(currFunc.hasName()) {
        funcName = currFunc.getName().str();
      }
      int funcID = getFuncID(funcName);
      dbgs() << "[+] Checking call instructions in function:" << funcName
          << " (" << funcID << ")\n";

      for(auto &currBB: currFunc) {
        for(auto &currIns: currBB) {
          Instruction *currInstrPtr = &currIns;
          // check if this is a CALL instruction.
          if(CallInst *callInstrPtr = dyn_cast<CallInst>(currInstrPtr)) {
              Function *calledFunc = callInstrPtr->getCalledFunction();
              std::string calledFuncName = "NONAME";
              if (calledFunc && calledFunc->hasName()) {
                  calledFuncName = calledFunc->getName().str();
              }
              int calledFuncID = getFuncID(calledFuncName);
              dbgs() << funcName << " -> " << calledFuncName
                  << " (" << funcID << " -> " << calledFuncID << ")\n";
              addEdge(funcID, calledFuncID);
          }
        }
      }
      dbgs() << "[+] Finished Checking CALL instructions in function:" << funcName << "\n";
      return hasAccesses;
    }


    bool runOnModule(Module &m) override {
      // iterate through all the functions.
      for (auto &currFunc: m) {
        // if this is not a declaration.
        if(!currFunc.isDeclaration()) {
          checkFunction(currFunc);
        }
      }

      printAllPaths(0, 2);
      // visit graph
      // for (auto &s : adjList) {
      //     for (auto &d: s)
      //         dbgs() << d << " ";
      //     dbgs() << "\n";
      // }

      // this is just an analysis pass,
      // we do not change the module.
      return false;
    }

  };

  char CallGraphPass::ID = 0;
  // pass arg, pass desc, cfg_only, analysis only
  static RegisterPass<CallGraphPass> x("mycg",
                                       "Construct Call Graph",
                                       false,
                                       true);
}
