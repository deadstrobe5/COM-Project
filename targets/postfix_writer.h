#ifndef __OG_TARGETS_POSTFIX_WRITER_H__
#define __OG_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <set>
#include <stack>
#include <string>
#include <iostream>
#include <sstream>
#include <cdk/symbol_table.h>
#include <cdk/emitters/basic_postfix_emitter.h>
#include "targets/basic_ast_visitor.h"
#include "targets/symbol.h"

namespace og {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<og::symbol> &_symtab;


    std::set<std::string> _functions_to_declare;

    // semantic analysis
    bool _errors, _inFunction, _inFunctionName, _inFunctionArgs, _inFunctionBody;
    bool _inForInit;
    bool _returnSeen; // when building a function
    std::stack<int> _forCond, _forIncrs, _forEnd; // for break/repeat
    std::stack<bool> _globals; // for deciding whether a variable is global or not
    std::shared_ptr<og::symbol> _function; // for keeping track of the current function and its arguments
    int _offset; // current framepointer offset (0 means no vars defined)

    // remember function name for resolving '@'
    std::string _currentFunctionName;
    int _currentBodyRetLabel; // where to jump when a return occurs of an exclusive section ends
    int _currentBodyExitLabel; // where to jump when a return occurs in the end section

    
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<og::symbol> &symtab, cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _errors(false), _inFunction(false), _inFunctionName(false), _inFunctionArgs(
            false), _inFunctionBody(false), _inForInit(false), _returnSeen(false), _function(nullptr), _offset(0), _currentFunctionName(""), _currentBodyRetLabel(0), _currentBodyExitLabel(0), _pf(pf), _lbl(0) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

    void error(int lineno, std::string s) {
      std::cerr << "error: " << lineno << ": " << s << std::endl;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // og

#endif
