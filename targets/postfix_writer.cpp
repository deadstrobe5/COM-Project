#include <string>
#include <sstream>
#include <memory>
#include "targets/postfix_writer.h"
#include "targets/type_checker.h"
#include "targets/symbol.h"
#include "targets/frame_size_calculator.h"
#include "ast/all.h"  // all.h is automatically generated
// must come after other #includes
#include "og_parser.tab.h"


//---------------------------------------------------------------------------

void og::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  _pf.NIL();
}
void og::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void og::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(0);
  _pf.EQ();
}
void og::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int end_and;

  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JZ(mklbl(end_and = ++_lbl));
  node->right()->accept(this, lvl);
  _pf.EQ();
  _pf.ALIGN();
  _pf.LABEL(mklbl(end_and));

}
void og::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int end_or;

  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.INT(1);
  _pf.JEQ(mklbl(end_or = ++_lbl));

  node->right()->accept(this, lvl);
  _pf.NE();
  _pf.ALIGN();
  _pf.LABEL(mklbl(end_or));

}

//---------------------------------------------------------------------------

void og::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_inFunctionBody){
    _pf.INT(node->value()); // push an integer
  } else {
    _pf.SINT(node->value()); 
  }
}

void og::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_inFunctionBody) {
    _pf.DOUBLE(node->value());
  } else {
    _pf.SDOUBLE(node->value());
  }
}

void og::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  if (_function){
    /* leave the address on the stack */
    _pf.TEXT(); // return to the TEXT segment
    _pf.ADDR(mklbl(lbl1));
  } else {
    // global variable initilizer
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

void og::postfix_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  size_t size = node->expressions()->size();
  for (size_t i = 0; i < size; i++) {
    node->expressions()->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

void og::postfix_writer::do_identity_node(og::identity_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);

}

//---------------------------------------------------------------------------

void og::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->type());
    _pf.INT(p->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->type());
    _pf.INT(p->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DADD();
  else
  _pf.ADD();
}


void og::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->type());
    _pf.INT(p->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DSUB();
  else{
    _pf.SUB();
    if (node->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)){
      std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->left()->type());
      _pf.INT(p->referenced()->size());
      _pf.DIV();
    }

  }
}

void og::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DMUL();
  else
  _pf.MUL();
}

void og::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->left()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE))
  _pf.DDIV();
  else
  _pf.DIV();
}
void og::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}



//---------------------------------------------------------------------------


void og::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
}
void og::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LE();
}
void og::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GE();
}
void og::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.GT();
}
void og::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.NE();
}
void og::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.EQ();
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->name();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);
  if (symbol->global()) {
    _pf.ADDR(symbol->name());
  }
  else {
    _pf.LOCAL(symbol->offset());
  }
}


void og::postfix_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // FIX ME: Igual ao GR8

  if (_inFunctionBody) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}

void og::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;


  node->lvalue()->accept(this, lvl);
  if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDDOUBLE();
  }
  else {
    _pf.LDINT();
  }
}

void og::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {

  ASSERT_SAFE_EXPRESSIONS;

  node->rvalue()->accept(this, lvl + 2);
  if (node->is_typed(cdk::TYPE_DOUBLE)){
    if (node->rvalue()->is_typed(cdk::TYPE_INT))
    _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)){
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_for_node(og::for_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _forCond.push(++_lbl); // after init, before body
  _forIncrs.push(++_lbl);
  _forEnd.push(++_lbl);// after for

  //_inForInit = true;

  if(node->inits())
    node->inits()->accept(this, lvl);

  _pf.ALIGN();
  _pf.LABEL(mklbl(_forCond.top()));

  if(node->condition())
    node->condition()->accept(this, lvl);

  _pf.JZ(mklbl(_forEnd.top()));

  if(node->block())
    node->block()->accept(this, lvl + 2);

  _pf.ALIGN();
  _pf.LABEL(mklbl(_forIncrs.top()));

  if(node->incrs())
    node->incrs()->accept(this, lvl +2);

  _pf.JMP(mklbl(_forCond.top()));

  _pf.ALIGN();
  _pf.LABEL(mklbl(_forEnd.top()));

  _forCond.pop(); // after init, before body
  _forIncrs.pop();
  _forEnd.pop();

}

void og::postfix_writer::do_break_node(og::break_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_forCond.size() != 0) {
    _pf.JMP(mklbl(_forEnd.top())); // jump to for end
  } else
    error(node->lineno(), "'break' outside 'for'");
}


void og::postfix_writer::do_continue_node(og::continue_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
if (_forCond.size() != 0) {
    _pf.JMP(mklbl(_forIncrs.top())); // jump to next cycle
  } else
    error(node->lineno(), "'continue' outside 'for'");
} 

//---------------------------------------------------------------------------

void og::postfix_writer::do_if_node(og::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}


void og::postfix_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void og::postfix_writer::do_input_node(og::input_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _functions_to_declare.insert("readd");
    _pf.CALL("readd");
    _pf.LDFVAL64();
  } else if (node->type()->name() == cdk::TYPE_INT) {
    _functions_to_declare.insert("readi");
    _pf.CALL("readi");
    _pf.LDFVAL32();
  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
    return;
  }

}


void og::postfix_writer::do_write_node(og::write_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  for (size_t i = 0; i < node->expressions()->size(); i++) {

    node->expressions()->node(i)->accept(this, lvl);
    cdk::expression_node* expr = dynamic_cast<cdk::expression_node*>(node->expressions()->node(i));
    std::shared_ptr<cdk::basic_type> etype = expr->type();
    
    if (etype->name() == cdk::TYPE_INT) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // trash int
    } else if (etype->name() == cdk::TYPE_DOUBLE) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8); // trash double
    } else if (etype->name() == cdk::TYPE_STRING) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // trash char pointer
    } else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }
  if (node->newLine()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
  
}



//---------------------------------------------------------------------------


void og::postfix_writer::do_return_node(og::return_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if(node->returnValue()){
    node->returnValue()->accept(this, lvl);
    _function->return_type(node->returnValue()->type());
    if(node->returnValue()->is_typed(cdk::TYPE_DOUBLE)){
      _pf.STFVAL64();
    }
    else{
      _pf.STFVAL32();
    }
  }
}


//---------------------------------------------------------------------------

void og::postfix_writer::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<cdk::basic_type> type = node->argument()->type();

  node->argument()->accept(this, lvl);
  _pf.TRASH(type->size());
}


//---------------------------------------------------------------------------

void og::postfix_writer::do_block_node(og::block_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _symtab.push();
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();

}

void og::postfix_writer::do_function_call_node(og::function_call_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->identifier();
  auto s = _symtab.find(id);


  size_t argsSize = 0;
  if (node->arguments()) {
    for (int ax = node->arguments()->size(); ax > 0; ax--) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax - 1));
      arg->accept(this, lvl + 2);
      if (arg->is_typed(cdk::TYPE_INT) && s->arguments()->at(ax-1)->name() == cdk::TYPE_DOUBLE)
        _pf.I2D();
      argsSize += s->arguments()->at(ax-1)->size();
    }
  }
  _pf.CALL(node->identifier());
  if (argsSize != 0) {
    _pf.TRASH(argsSize);
  }

  std::shared_ptr<og::symbol> symbol = _symtab.find(node->identifier());

  std::shared_ptr<cdk::basic_type> type = symbol->type();
  if (type->name() == cdk::TYPE_INT || type->name() == cdk::TYPE_POINTER || type->name() == cdk::TYPE_STRING) {
    _pf.LDFVAL32();
  }
  else if (type->name() == cdk::TYPE_DOUBLE) {
    const std::string &id = node->identifier();
    auto s = _symtab.find(id);
    if (s->return_type()->name() == cdk::TYPE_INT){
      _pf.LDFVAL32();
      node->type(s->return_type());
    }
    else
      _pf.LDFVAL64();
  }
  else {
    //type void does not have a return value to be loaded!
  }
}
void og::postfix_writer::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs) {
    error(node->lineno(), "cannot declare function in body or in args");
    return;
  }

  if (!new_symbol()) return;

  std::shared_ptr<og::symbol> function = new_symbol();

  if(node->qualifier() == 2) // 2 -> require
    _functions_to_declare.insert(function->name());
  else 
    _pf.GLOBAL(function->name(), _pf.FUNC());

  reset_new_symbol();

}
void og::postfix_writer::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (_inFunctionBody || _inFunctionArgs) {
    error(node->lineno(), "cannot define function in body or in arguments");
    return;
  }

  // remember symbol so that args and body know
  _function = new_symbol();

  reset_new_symbol();

  _offset = 8; // prepare for arguments (4: remember to account for return address)
  _symtab.push(); // scope of args
  if (node->arguments()) {
    _inFunctionArgs = true; //FIXME really needed?
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      cdk::basic_node *arg = node->arguments()->node(ix);
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    
    const std::string &id = node->identifier();
    auto s = _symtab.find(id);
    std::vector<std::shared_ptr<cdk::basic_type>> *arguments = new std::vector<std::shared_ptr<cdk::basic_type>>();
    for (size_t i = 0; i < node->arguments()->size(); i++){
      cdk::typed_node *typed = dynamic_cast<cdk::typed_node*>(node->arguments()->node(i));
      arguments->push_back(typed->type());
    }

    if (!s->arguments()){
      s->arguments(arguments);
      _symtab.replace(id, s);
    }

    _inFunctionArgs = false;
  }

  _pf.TEXT();
  _pf.ALIGN();
  if (_function->name() == "og"){
    /* qualifier 1 -> public */ 
    if(node->qualifier() == 1)
      _pf.GLOBAL("_main", _pf.FUNC());
    _pf.LABEL("_main");
  }
  else{
    if (node->qualifier() == 1)
      if(!_symtab.find(_function->name()))
        _pf.GLOBAL(_function->name(), _pf.FUNC());
    _pf.LABEL(_function->name());
  }

  // compute stack size to be reserved for local variables

  frame_size_calculator lsc(_compiler, _symtab);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables


  // the following flag is a slight hack: it won't work with nested functions
  _inFunctionBody = true;
  // prepare for local variables but remember the return value (first below fp)
  _offset = -_function->type()->size();
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl + 4); // block has its own scope
  os() << "        ;; after body " << std::endl;
  _inFunctionBody = false;
  _symtab.pop(); // scope of arguments

  _pf.LEAVE();
  _pf.RET();

  if (node->identifier() == "og") {
    // declare external functions
    for (std::string s : _functions_to_declare)
      _pf.EXTERN(s);
  }
}


//---------------------------------------------------------------------------


void og::postfix_writer::do_memalloc_node(og::memalloc_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->type());

  node->spaces()->accept(this, lvl);
  _pf.INT(p->referenced()->size());
  _pf.MUL();
  _pf.ALLOC(); // allocate
  _pf.SP();// put base pointer in stack
}

void og::postfix_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _pf.INT(node->argument()->type()->size());
}

//---------------------------------------------------------------------------


void og::postfix_writer::do_pointer_index_node(og::pointer_index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->pointer()->type());

  node->pointer()->accept(this, lvl);
  _pf.INT(p->referenced()->size());
  node->index()->accept(this, lvl);
  _pf.MUL();
  _pf.ADD();
}


void og::postfix_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->tuple()->accept(this, lvl);
  int i;

  if (node->is_typed(cdk::TYPE_STRUCT)){
    std::shared_ptr<cdk::structured_type> type = structured_type_cast(node->type());
    for (i = 1; i <= node->index()->value()-1; i++) {
      _pf.INT(type->component(i-1)->size());
      _pf.ADD();
    }
    if (type->component(i-1)->name() == cdk::TYPE_DOUBLE)
      _pf.LDDOUBLE();
    else
      _pf.LDINT();
  }

}


void og::postfix_writer::do_position_indicator_node(og::position_indicator_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
}


//---------------------------------------------------------------------------

void og::postfix_writer::do_tuple_declaration_node(og::tuple_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;


  int offset = 0, typesize = node->type()->size();

  int global_offset = 0;

  cdk::expression_node* index;

  if(node->identifiers()->size() == 1){
    auto id = node->identifiers()->at(0);

    if(_inFunctionBody){
      _offset -= typesize;
      offset = _offset;
    }
    else
      offset = 0;

    std::shared_ptr<og::symbol> symbol = new_symbol();
    if (symbol) {
      symbol->set_offset(offset);
      reset_new_symbol();
    }

    if (_inFunctionBody) {
      if (node->initializers()) {
        offset = symbol->offset();
        for (size_t i = 0; i < node->initializers()->expressions()->size(); i++) {
          index = dynamic_cast<cdk::expression_node *>(node->initializers()->expressions()->node(i));
          index->accept(this, lvl);
          if (index->is_typed(cdk::TYPE_INT) || index->is_typed(cdk::TYPE_STRING)
              || index->is_typed(cdk::TYPE_POINTER)) {
            _pf.LOCAL(offset);
            offset += 4;
            _pf.STINT();
          } else if (index->is_typed(cdk::TYPE_DOUBLE)) {
            _pf.LOCAL(offset);
            offset += 8;
            _pf.STDOUBLE();
          } else {
            std::cerr << "cannot initialize" << std::endl;
          }
        }
      }
    } else {
      if (!_function) {
        if (node->initializers() != nullptr) {
          _pf.DATA();
          _pf.ALIGN();
          if (node->qualifier() == 1)
            _pf.GLOBAL(id, _pf.OBJ());
          _pf.LABEL(id);
          _pf.SALLOC(typesize);
          for (size_t i = 0; i < node->initializers()->expressions()->size(); i++){
            index = dynamic_cast<cdk::expression_node *>(node->initializers()->expressions()->node(i)); 
            index->accept(this, lvl);
            _pf.ADDR(id);
            if (index->is_typed(cdk::TYPE_INT) || index->is_typed(cdk::TYPE_STRING) 
                || index->is_typed(cdk::TYPE_POINTER)) {
              _pf.INT(global_offset);
              global_offset += 4;
              _pf.ADD();
              _pf.STINT();
            }else if (index->is_typed(cdk::TYPE_DOUBLE)){
              _pf.INT(global_offset);
              global_offset += 8;
              _pf.ADD();
              _pf.STDOUBLE();
            } else {
              std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
              _errors = true;
            }
          }

        }

      }
    }
    
  } else{
      for (size_t i = 0; i < node->identifiers()->size(); i++) {
        std::shared_ptr<cdk::structured_type> type = structured_type_cast(node->initializers()->type());
        cdk::expression_node* init = dynamic_cast<cdk::expression_node*>(node->initializers()->expressions()->node(i));

        variable_declaration_node* var = new variable_declaration_node(node->lineno(), node->qualifier(), type->component(i), 
          node->identifiers()->at(i), init);
        var->accept(this, lvl);
      }
  }
   
}


void og::postfix_writer::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;



  auto id = node->identifier();
  
  int offset = 0, typesize = node->type()->size();// in bytes

  if (node->qualifier() == 2){ // require declaration
    _pf.EXTERN(id);
    return;
  }

  if (_inFunctionBody) {
    _offset -= typesize;
    offset = _offset;
  } else if (_inFunctionArgs) {
    offset = _offset;
    _offset += typesize;
  } else {
    offset = 0; // global variable
  }


  std::shared_ptr<og::symbol> symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_inFunctionBody) {
    if (node->initializer()) {
      node->initializer()->accept(this, lvl);
      if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING)
          || node->is_typed(cdk::TYPE_POINTER)) {
        _pf.LOCAL(symbol->offset());
        _pf.STINT();
      } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (node->initializer()->is_typed(cdk::TYPE_INT))
          _pf.I2D();
        _pf.LOCAL(symbol->offset());
        _pf.STDOUBLE();
      } else {
        std::cerr << "cannot initialize" << std::endl;
      }
    }
  } else {
    if (!_function) {
      if (node->initializer() == nullptr) {
        _pf.BSS();
        _pf.ALIGN();
        if (node->qualifier() == 1) // public
          _pf.GLOBAL(id, _pf.OBJ());
        _pf.LABEL(id);
        _pf.SALLOC(typesize);
      } else {

        if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE)
            || node->is_typed(cdk::TYPE_POINTER)) {

          
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);

          if (node->is_typed(cdk::TYPE_INT)) {
            node->initializer()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_POINTER)) {
            node->initializer()->accept(this, lvl);
          } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
            if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
              node->initializer()->accept(this, lvl);
            } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
              cdk::integer_node *dclini = dynamic_cast<cdk::integer_node *>(node->initializer());
              cdk::double_node ddi(dclini->lineno(), dclini->value());
              ddi.accept(this, lvl);
            } else {
              std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
              _errors = true;
            }
          }
        } else if (node->is_typed(cdk::TYPE_STRING)) {
          _pf.DATA();
          _pf.ALIGN();
          _pf.LABEL(id);
          node->initializer()->accept(this, lvl);
          
        } else {
          std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
          _errors = true;
        }

      }

    }
  }

}