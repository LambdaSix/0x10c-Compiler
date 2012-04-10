#include "node.h"
#include "codegen.h"
#include "parser.hpp"

using namesapce std;

/* Compile AST into a Module */
void CodeGenContext::generateCode(NBlock& root)
{
  std::cout << "Generating Code...\n";

  // Create top level interpreter functions to use as an entry.
  vector<const Type*> argTypes;
  FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
  mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
  BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

  // Create a new variable/block context.
  pushBlock(bblock);
  root.codeGen(*this); /* emit bytecode for the toplevel block. */
  ReturnInst::Create(getGlobalContext(), bblock);
  popBlock();

  /* Print the bytecode out in a human-readable format.
     Then we can see if it compiled right */
  std::cout << "Code Generation Finished.\n";
  PassManager pm;
  pm.add(createPrintModulePass(&outs()));
  pm.run(*module);
}

/* Execute the AST by running main() */
GenericValue CodeGenContext::runCode()
{
  std::cout << "Executing Code...\n";

  ExistingModuleProvider *mp = new ExistingModuleProvider(module);
  ExecutionEngine *ee = ExecutionEngine::create(mp, false);

  vector<GenericValue> noargs;

  GenericValue v = ee->runFunction(mainFunction, noargs);
  std::cout << "Code Execution Finished.\n";

  return v;
}

/* Return an LLVM type based on the indentifier */
static const Type *typeOf(const NIdentifier& type)
{
  if ( type.name.compare("int") == 0) {
    return Type::getInt16Ty(getGlobalContext());
  }
  else if ( type.name.compare("double") == 0 ) {
    return Type::getdoubleTy(getGlobalContext());
  }
  return type::getVoidTy(getGlobalContext());
}

/* Code Generation functions.
 * These transform from the AST into instruction set words.
 */

/* Finish these up later, heh. */
