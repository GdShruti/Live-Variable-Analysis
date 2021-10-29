#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/ADT/BreadthFirstIterator.h"

using namespace llvm;
using namespace std;

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct LiveVariableAnalysis : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    LiveVariableAnalysis() : FunctionPass(ID) {}

	std::vector<BasicBlock*> workList;
	map<string, std::vector<string>> gen;
	map<string, std::vector<string>> kill;
	std::vector<string> inst_name;
	map<string, std::vector<string>> bb_inst;
	map<string, std::vector<string>> bb_in;
	map<string, std::vector<string>> bb_out;
	map<string, std::vector<string>> inst_in;
	map<string, std::vector<string>> inst_out;
	map<string, std::vector<string>> phi;
	map<Instruction*, string> inst_instname;
	map<int, int> hist;


	int store_count=0;
	int inst_count=0;

	bool iterate_again = true;

	std::string *getValueName(Use *it)
	{
        std::string *op = new std::string;
        if ((*it)->hasName()){
            *op = (*it)->getName().str();
        }
        else {
            return NULL;
        }
        return op;
	}

	std::vector<string> set_Union(std::vector<string> v1, std::vector<string> v2 )
	{
	   for(auto i= v2.begin(); i!=v2.end() ; i++)
	   {
	        string x = *i;
		    auto it=find(v1.begin() , v1.end(), x);
		    if(it==v1.end())
		        v1.push_back(x);
	   }
	   return v1;
	}

	std::vector<string> set_Difference(std::vector<string> v1, std::vector<string> v2 )
	{
	   for(auto i= v2.begin(); i!=v2.end() ; i++)
	   {
	       string x = *i;
	       auto it =find(v1.begin() , v1.end(), x);
	       if(it!=v1.end())
	            v1.erase(it);
	   }
	   return v1;
	}

	 std::vector<string> compute_in(auto& inst)
	{
	  		std::vector<string> g = gen.find(inst)->second;
			std::vector<string> k = kill.find(inst)->second;
			std::vector<string> o = inst_out.find(inst)->second;
			std::vector<string> o_k ;
			std::vector<string> in ;  

			//IN[B] = OUT[B] - Kill[B] U Gen[B]     		
       		o_k= set_Difference(o,k);
   			in = set_Union(o_k,g);
       		return in;
	}
		
	 
	bool compute_in_out()
	{
		 	for(auto itr = workList.end()-1; itr >= workList.begin(); --itr)
			{
				BasicBlock* bb = *itr;
				string bb_name = bb->getName().str();
				std::vector<string> temp_inst = bb_inst.find(bb_name)->second;
				std::vector<string> temp_out;
				std::vector<string> temp_in;
				std::vector<string> old_out;
				std::vector<string> old_in;
				
				//OUT[B] = U IN[successros(B)]
				for(BasicBlock* s : successors(bb)) 
				{
					string s_name = s->getName().str();
					if(bb_in.find(s_name) != bb_in.end() && bb_in.find(s_name)->second.size()!=0 )
					{
						temp_out = set_Union( temp_out,(bb_in.find(s_name)->second));
					}
      			}
      			if(phi.find(bb_name) != phi.end())
	            {
	                   temp_out = set_Union(temp_out, phi.find(bb_name)->second);
	            }
	            
	            //To check and update if any values have changed in next iterations 
  				if(bb_out.find(bb_name) != bb_out.end())
  				{
  						old_out = bb_out.find(bb_name)->second;
  						std::vector<string> v = set_Difference(old_out,temp_out);
  						std::vector<string> v1 = set_Difference(temp_out,old_out);
  						if(!(v1.size()==0 && v.size()==0)) 
  							iterate_again = true;					//If there is any change in value
  						bb_out.find(bb_name)->second = temp_out;
  				}
  				else
  				{
					bb_out.insert({bb_name , temp_out});
				}
  				if(temp_inst.size()!=0)
				{
					for (auto i = temp_inst.end()-1; i >= temp_inst.begin(); --i)
					{
						string inst = *i;
						if( i == temp_inst.end()-1 )
						{
							inst_out.insert({inst , temp_out}); 
						}
						else
						{
							inst_out.insert({inst, inst_in.find(*(i+1))->second});
						}
						temp_in  = compute_in(inst);
						inst_in.insert({inst, temp_in});
					}
					bb_in.insert({bb_name , temp_in});
  				}
  				else
  				{
					bb_in.insert({bb_name , temp_out});
  				}		
			}
			return iterate_again;
	}


	void compute_gen_kill(auto& inst)
	{
		std::vector<string> temp_gen, temp_kill;			 
		if(isa<StoreInst>(inst))
		{
			Value* op = inst.getOperand(0);
		    if(ConstantInt *CI = dyn_cast<ConstantInt>(op));
		    else
            {
				temp_gen.push_back(op->getName().str());
		    }
			
			temp_kill.push_back(inst.getOperand(1)->getName().str());
		    gen.insert({"store" + to_string(store_count) , temp_gen});
			kill.insert({ "store" + to_string(store_count), temp_kill});
			store_count++;					 	
		}	
		
		else if(inst.isBinaryOp() ||  inst.getOpcode()==Instruction::ICmp)
		{					
 			auto op_kill = dyn_cast<Value>(&inst);
			auto op_gen = dyn_cast<User>(&inst);
			std::vector<string> s;

 			for(auto it = op_gen->op_begin(); it != op_gen->op_end(); ++it) 
 			{ 
				auto var=getValueName(it) ;
				if(var != NULL)
				{
					if((*var)!="printf")
						temp_gen.push_back((*var));
				}
			}
			
			temp_kill.push_back(op_kill->getName().str());
		}

		else if(inst.getOpcode()==Instruction::PHI)
		{	
		 	auto op_kill = dyn_cast<Value>(&inst);
		 	temp_kill.push_back(op_kill->getName().str());

		 	if(PHINode* phi_insn = dyn_cast<PHINode>(&inst))
			{   	
	            for(int i = 0; i < phi_insn->getNumIncomingValues(); i++)
	            {
	                Value* op = phi_insn->getIncomingValue(i);
	                BasicBlock* Pred = phi_insn->getIncomingBlock(i);
	                string pred_name = Pred->getName().str();
	                string op_name = op->getName().str();
	 
                    if(phi.find(pred_name) != phi.end())
                    {
	                   	phi.find(pred_name)->second.push_back(op_name);
	                   	temp_kill.push_back(op_name);
	                }
	                else
	                {
	                   	std::vector<string> s={op_name}; 
	                   	phi.insert({pred_name,s});
	                }
	            }
          	}
	    }

		gen.insert({inst.getName().str() , temp_gen});
		kill.insert({inst.getName().str() , temp_kill});		
	}
	
    bool runOnFunction(Function &F) override 
    {
		for (BasicBlock* bb : breadth_first(&F.getEntryBlock()))
	    {
			  workList.push_back(bb);
			  std::vector<string> temp_inst; 
			  for(auto& inst : *bb)
				{
					if(inst.isBinaryOp() || inst.getOpcode()==Instruction::PHI || inst.getOpcode()==Instruction::ICmp)
					{
						inst_name.push_back(inst.getName().str());
						temp_inst.push_back(inst.getName().str());
						inst_instname.insert({&inst, inst.getName().str()});
					}
					else if(isa<StoreInst>(inst))
					{
						inst_name.push_back("store"+to_string(store_count));
						temp_inst.push_back("store"+to_string(store_count));
						inst_instname.insert({&inst, "store"+to_string(store_count)});
					}
					compute_gen_kill(inst);
				}
			  bb_inst.insert({bb->getName().str() , temp_inst});
	    } 

      
		compute_in_out();
	  	while(iterate_again)
	  	{
	  		iterate_again = false;
	  		compute_in_out();
	  	}
	 	

		errs()<<"\n\n\n\n\t*********************************************\n\t|    Live values out of each Basic Block    |\n\t*********************************************\n";
		errs() <<"\n     BasicBlock  |   Live Variables \t  \n------------------------------------------------------------------\n";
	  	for (auto itr = workList.begin(); itr != workList.end(); ++itr)
		{
			auto bb = (*itr)->getName().str();
			errs()<<"\t"<< bb << "\t | \t";
			std::vector<string> o = bb_out.find(bb)->second;
			for (auto i = o.begin(); i != o.end(); ++i) 
       			{
       				errs() << *i << ", ";
       			} 
			errs() <<"\n"; 
		}


		
		errs()<<"\n\n\n\n\t**************************************************************\n\t|   Live values at each program point in each Basic Block    |\n\t**************************************************************\n";
		std::vector<string> temp_in1;
		std::vector<string> temp_out1;
		for(auto itr = workList.end()-1; itr >= workList.begin(); --itr)
		{
			BasicBlock* bb = *itr;
			int prg_pt = bb->size();
			string bb_name = bb->getName().str();
			errs()<<"\n\n"<<bb_name<<" :\n";
			errs() <<"\t\tInst\t|  Live Variables at the program point \n\t------------------------------------------------------------------\n";
			for(auto itr1 = bb->rbegin() ; itr1 != bb->rend() ; itr1++)
			{
				auto& inst = *itr1;
				int t1;
				errs()<<"\t\t  " <<prg_pt <<"\t|\t";
				if( inst_instname.find(&inst) != inst_instname.end())
				{
					string i_name =  inst_instname.find(&inst)->second;
					std::vector<string> in= inst_in.find(i_name)->second;
					std::vector<string> o = inst_out.find(i_name)->second;
					if(inst.getOpcode()==Instruction::ICmp)
					{
						auto temp = dyn_cast<Value>(&inst);
						o.push_back(temp->getName().str());
					}
					for (auto i = o.begin(); i != o.end(); ++i) 
		       			 errs() << *i << "  ";
					errs() <<"\n"; 
					t1 = o.size(); 
					temp_in1=in;
					temp_out1=in;
				}
				else
				{	
					if(phi.find(bb_name) != phi.end())
	                {
	                  	temp_out1 = set_Union(temp_out1, phi.find(bb_name)->second);
	                }

					for (auto i = temp_out1.begin(); i != temp_out1.end(); ++i) 
		       			 errs() << *i << "  ";
		       		errs()<<"\n";
					t1 = temp_out1.size(); 
				}

				if(hist.find(t1) != hist.end())
				{
					int x=  hist.find(t1)->second;
					hist.find(t1)->second = x + 1;
				}
				else
				{
					hist.insert({t1,1});
				}
				prg_pt--;
			}
			errs()<<"\t\t  "<<prg_pt<<"\t|\t";
			temp_in1=bb_in.find(bb_name)->second;
			for (auto i = temp_in1.begin(); i != temp_in1.end(); ++i) 
		       	errs() << *i << "  ";
			errs() <<"\n";
		}
				

		errs()<<"\n\n\n\n\t  ************************************\n\t  |            HISTOGRAM             |\n\t  ************************************\n";
		errs() <<"\n\t    Live values |   Program points \n\t-------------------------------------\n";
		for (auto itr = hist.begin(); itr != hist.end(); ++itr)
		{
			errs()<<"\t\t"<< itr->first <<"\t|\t"<< itr->second <<"\n";
		}
      return false;
    }
  };
}

char LiveVariableAnalysis::ID = 0;
static RegisterPass<LiveVariableAnalysis> X("LiveVariableAnalysis", "LiveVariableAnalysis Pass");