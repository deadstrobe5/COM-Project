#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void og::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void og::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void og::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

/* ====================================================================== */
/* ====[                          LITERALS                          ]==== */
/* ====================================================================== */

void og::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(4, cdk::TYPE_STRING));
}

void og::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
}

void og::type_checker::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::make_reference_type(4, nullptr));
}

/* ====================================================================== */
/* ====[                      UNARY OPERATORS                       ]==== */
/* ====================================================================== */

void og::type_checker::processIDUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!(node->argument()->is_typed(cdk::TYPE_INT) || node->argument()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("wrong type in argument of unary expression");

  node->type(node->argument()->type());
}

void og::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processIDUnaryExpression(node, lvl);
}
void og::type_checker::do_identity_node(og::identity_node *const node, int lvl) {
  processIDUnaryExpression(node, lvl);
}

void og::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  if (!(node->argument()->is_typed(cdk::TYPE_INT)))
    throw std::string("wrong type in argument of unary expression");

  /* ~INT */
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_position_indicator_node(og::position_indicator_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  if (!(node->lvalue()->is_typed(cdk::TYPE_INT) || node->lvalue()->is_typed(cdk::TYPE_DOUBLE)
     || node->lvalue()->is_typed(cdk::TYPE_STRING) || node->lvalue()->is_typed(cdk::TYPE_POINTER) 
     || node->lvalue()->is_typed(cdk::TYPE_STRUCT) )) 
    throw std::string("wrong type in argument of position indicator expression");

  node->type(cdk::make_reference_type(4, node->lvalue()->type()));
}

/* ====================================================================== */
/* ====[                      BINARY OPERATORS                      ]==== */
/* ====================================================================== */

void og::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in left argument of additive expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in right argument of additive expression");

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  /* INT + DOUBLE = DOUBLE */  
  else if(node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  /* POINTER + INT = POINTER (deslocamento) */
  else if(node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else if(node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
    node->type(node->right()->type());
  else
    throw std::string("wrong types in additive expression");
}


void og::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in left argument of subtractive expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in right argument of subtractive expression");

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  /* POINTER - POINTER = INT (nmro de objetos entre eles) */
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)){
    /* Types pointed by the TYPE_POINTER need to agree aswell */
    std::shared_ptr<cdk::reference_type> subtype1 = std::dynamic_pointer_cast<cdk::reference_type>(node->left()->type());
    std::shared_ptr<cdk::reference_type> subtype2 = std::dynamic_pointer_cast<cdk::reference_type>(node->right()->type());
    if (subtype1->name() == cdk::TYPE_UNSPEC || subtype2->name() == cdk::TYPE_UNSPEC) // ptr<auto> goes along with everyone
      node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
    else if (subtype1->name() == subtype2->name())
      node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
    else
      throw std::string("wrong pointer types in subtractive expression");
  }
  /* INT - DOUBLE = DOUBLE */  
  else if(node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  /* POINTER - INT = POINTER (deslocamento) */
  else if(node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(node->left()->type());
  else
    throw std::string("wrong types in subtractive expression");
}

void og::type_checker::processIBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT)))
    throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT)))
    throw std::string("wrong type in right argument of binary expression");

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::processIDBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("wrong type in right argument of binary expression");

  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  /* INT op DOUBLE = DOUBLE */  
  else if(node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else
    throw std::string("wrong types in binary expression");
}

void og::type_checker::processComparativeExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("wrong type in left argument of comparative expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE)))
    throw std::string("wrong type in right argument of comparative expression");

  /* All comparisons between INT and DOUBLE are valid */

  /* Return type is always 1 or 0 */
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::processEqualityExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!(node->left()->is_typed(cdk::TYPE_INT) || node->left()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in left argument of equality expression");

  node->right()->accept(this, lvl + 2);
  if (!(node->right()->is_typed(cdk::TYPE_INT) || node->right()->is_typed(cdk::TYPE_DOUBLE) || node->left()->is_typed(cdk::TYPE_POINTER)))
    throw std::string("wrong type in right argument of equality expression");

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_POINTER))
    throw std::string("wrong types in equality expression");
  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_DOUBLE))
    throw std::string("wrong types in equality expression");

  if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_POINTER)){
    std::shared_ptr<cdk::reference_type> subtype1 = std::dynamic_pointer_cast<cdk::reference_type>(node->left()->type());
    std::shared_ptr<cdk::reference_type> subtype2 = std::dynamic_pointer_cast<cdk::reference_type>(node->right()->type());
    if (subtype1->name() != cdk::TYPE_UNSPEC && subtype2->name() != cdk::TYPE_UNSPEC) // ptr<auto> goes along with everyone
      if (subtype1->name() != subtype2->name()) 
        throw std::string("wrong types in equality expression");
  }

  /* Return type is always 1 or 0 */
  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}


void og::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processIDBinaryExpression(node, lvl);
}
void og::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processIDBinaryExpression(node, lvl);
}
void og::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processIBinaryExpression(node, lvl);
}
void og::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processComparativeExpression(node, lvl);
}
void og::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processComparativeExpression(node, lvl);
}
void og::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processComparativeExpression(node, lvl);
}
void og::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processComparativeExpression(node, lvl);
}
void og::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processEqualityExpression(node, lvl);
}
void og::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processEqualityExpression(node, lvl);
}
void og::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processIBinaryExpression(node, lvl);
}
void og::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processIBinaryExpression(node, lvl);
}



/* ====================================================================== */
/* ====[                         VARIABLES                          ]==== */
/* ====================================================================== */


void og::type_checker::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {
  const std::string &id = node->identifier();
  auto symbol = make_symbol(node->type(), id, 0, nullptr, 0, nullptr);
  if(!_symtab.insert(id, symbol))
    throw id + " redeclared.";

  _parent->set_new_symbol(symbol);

  if(node->initializer()){
    node->initializer()->accept(this, lvl+2);
    /* Types pointed by the TYPE_POINTER need to agree */
    if (node->is_typed(cdk::TYPE_POINTER) && node->initializer()->is_typed(cdk::TYPE_POINTER)){
      std::shared_ptr<cdk::reference_type> subtype1 = std::dynamic_pointer_cast<cdk::reference_type>(node->type());
      std::shared_ptr<cdk::reference_type> subtype2 = std::dynamic_pointer_cast<cdk::reference_type>(node->initializer()->type());
      if (subtype1->name() != cdk::TYPE_UNSPEC && subtype2->name() != cdk::TYPE_UNSPEC) // ptr<auto> goes along with everyone
        if (subtype1->name() != subtype2->name()) 
          throw std::string("wrong types in assignment expression");
    }
    /* TYPE_NAME = TYPE_NAME -> OK*/
    else if (node->type()->name() == node->initializer()->type()->name()); 
    /* DOUBLE = INT -> OK*/
    else if (node->is_typed(cdk::TYPE_DOUBLE) && node->initializer()->is_typed(cdk::TYPE_INT));
    else
      throw std::string("wrong types in assignment expression");

  }

}

void og::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);
  if (symbol == nullptr)
    throw "undeclared variable '" + id + "'";
  
  node->type(symbol->type());
}

void og::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->type(node->lvalue()->type());
}

void og::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  if (!(node->lvalue()->is_typed(cdk::TYPE_INT) || node->lvalue()->is_typed(cdk::TYPE_DOUBLE)
     || node->lvalue()->is_typed(cdk::TYPE_STRING) || node->lvalue()->is_typed(cdk::TYPE_POINTER) )) 
    throw std::string("wrong type in left argument of assignment expression");

  node->rvalue()->accept(this, lvl + 2);
  if (!(node->rvalue()->is_typed(cdk::TYPE_INT) || node->rvalue()->is_typed(cdk::TYPE_DOUBLE)
     || node->rvalue()->is_typed(cdk::TYPE_STRING) || node->rvalue()->is_typed(cdk::TYPE_POINTER) )) 
    throw std::string("wrong type in right argument of assignment expression");

  if (node->lvalue()->is_typed(cdk::TYPE_INT) && node->rvalue()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE) && node->rvalue()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING) && node->rvalue()->is_typed(cdk::TYPE_STRING))
    node->type(cdk::make_primitive_type(4, cdk::TYPE_STRING));
  /* Types pointed by the TYPE_POINTER need to agree */
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER) && node->rvalue()->is_typed(cdk::TYPE_POINTER)){
    std::shared_ptr<cdk::reference_type> subtype1 = std::dynamic_pointer_cast<cdk::reference_type>(node->lvalue()->type());
    std::shared_ptr<cdk::reference_type> subtype2 = std::dynamic_pointer_cast<cdk::reference_type>(node->rvalue()->type());
    if (subtype1->name() == cdk::TYPE_UNSPEC) // ptr<TYPE> = ptr<auto>
      node->type(node->rvalue()->type());
    else if (subtype2->name() == cdk::TYPE_UNSPEC) // ptr<auto> = ptr<TYPE>
      node->type(node->lvalue()->type());
    else if (subtype1->name() == subtype2->name())
      node->type(node->lvalue()->type());
    else
      throw std::string("wrong types in assignment expression");
  }
  /* DOUBLE = INT */
  else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE) && node->rvalue()->is_typed(cdk::TYPE_INT))
    node->type(cdk::make_primitive_type(8, cdk::TYPE_DOUBLE));
  else
    throw std::string("wrong types in assignment expression");
}


/* ====================================================================== */
/* ====[                           TUPLES                           ]==== */
/* ====================================================================== */

void og::type_checker::do_tuple_declaration_node(og::tuple_declaration_node *const node, int lvl) {
  node->initializers()->accept(this, lvl);

  // Tuple declaration can only be in the forms: 
  //  a = 1,2,3; (tuple declaration)
  if (node->identifiers()->size() == 1){
    /* The type of the left side is equal to the type of the right side */
    const std::string &id = node->identifiers()->at(0);
    auto symbol = make_symbol(node->initializers()->type(), id, 0, nullptr, 0, nullptr);
    if(!_symtab.insert(id, symbol))
      throw id + " redeclared."; 
    _parent->set_new_symbol(symbol);
  }
  //  a,b = 1,2; (n variable declarations)
  else if (node->identifiers()->size() == node->initializers()->expressions()->size());
  else throw "wrong number of arguments in tuple declaration";

}
void og::type_checker::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->tuple()->accept(this, lvl);
  if (!node->tuple()->is_typed(cdk::TYPE_STRUCT) && node->index()->value() != 1)
    throw "tuple index out of range";

  node->index()->accept(this, lvl);

  int i = node->index()->value();
  std::shared_ptr<cdk::structured_type> type = structured_type_cast(node->tuple()->type());
  node->type(type->component(i));
}
void og::type_checker::do_tuple_node(og::tuple_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->expressions()->accept(this, lvl);
  /* 1 element tuple = the element */
  if (node->expressions()->size() == 1){
    cdk::expression_node *expr = dynamic_cast<cdk::expression_node*>(node->expressions()->node(0));
    node->type(expr->type());
  }
  else{
    std::vector<std::shared_ptr<cdk::basic_type>> *v = new std::vector<std::shared_ptr<cdk::basic_type>>();
    for (size_t i = 0; i < node->expressions()->size(); i++){
      cdk::expression_node *expr = dynamic_cast<cdk::expression_node*>(node->expressions()->node(i));
      std::shared_ptr<cdk::basic_type> type = expr->type();
      v->push_back(type);
    }
    node->type(cdk::make_structured_type(*v));
  }
}


/* ====================================================================== */
/* ====[                         FOR LOOPS                          ]==== */
/* ====================================================================== */

void og::type_checker::do_for_node(og::for_node *const node, int lvl) {
  // EMPTY
}

void og::type_checker::do_break_node(og::break_node *const node, int lvl) {
  // EMPTY
}
void og::type_checker::do_continue_node(og::continue_node *const node, int lvl) {
  // EMPTY
}

/* ====================================================================== */
/* ====[                            IFs                             ]==== */
/* ====================================================================== */

void og::type_checker::do_if_node(og::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void og::type_checker::do_if_else_node(og::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

/* ====================================================================== */
/* ====[                         FUNCTIONS                          ]==== */
/* ====================================================================== */

void og::type_checker::do_function_call_node(og::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->identifier();
  std::shared_ptr<og::symbol> symbol = _symtab.find(id);
  if (symbol == nullptr)
    throw "undeclared function '" + id + "'";
  if (symbol->function() == 0)
    throw "identifier '" + id + "' is a variable, not a function";

  node->type(symbol->type());
  if(node->arguments()) node->arguments()->accept(this, lvl);
}

void og::type_checker::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  const std::string &id = node->identifier();

  int function = 1; // is a function
  if (!node->type())
    function = 2; // is a procedure

  auto symbol = make_symbol(node->type(), id, function, nullptr, 1, node->type());
  if(!_symtab.insert(id, symbol))
    throw id + " redeclared.";

  _parent->set_new_symbol(symbol);

  if(node->arguments()){ 
    node->arguments()->accept(this, lvl+2);
  
    std::vector<std::shared_ptr<cdk::basic_type>> *arguments = new std::vector<std::shared_ptr<cdk::basic_type>>();
    for (size_t i = 0; i < node->arguments()->size(); i++){
      cdk::typed_node *typed = dynamic_cast<cdk::typed_node*>(node->arguments()->node(i));
      arguments->push_back(typed->type());
    }
    symbol->arguments(arguments);
    _symtab.replace(id, symbol);
  }

}

void og::type_checker::do_function_definition_node(og::function_definition_node *const node, int lvl) {

  const std::string &id = node->identifier();

  int function = 1; // is a function
  if (!node->type())
    function = 2; // is a procedure
  
  auto symbol = make_symbol(node->type(), id, function, nullptr, 2, node->type());
  if(!_symtab.insert(id, symbol)){
    auto s = _symtab.find(id);
    if (s->declared() != 1)
      throw id + " redeclared.";
  }

  _parent->set_new_symbol(symbol);
}

void og::type_checker::do_return_node(og::return_node *const node, int lvl) {
  if (_function->function() == 2)
    throw "return instruction cant be used in procedures.";



  node->returnValue()->accept(this, lvl);
  /* Auto function */
  if (_function->is_typed(cdk::TYPE_UNSPEC))
    _function->type(node->returnValue()->type());
  else if (_function->is_typed(cdk::TYPE_POINTER) && node->returnValue()->is_typed(cdk::TYPE_POINTER)){
    /* Types pointed by the TYPE_POINTER need to agree aswell */
    std::shared_ptr<cdk::reference_type> subtype1 = std::dynamic_pointer_cast<cdk::reference_type>(_function->type());
    std::shared_ptr<cdk::reference_type> subtype2 = std::dynamic_pointer_cast<cdk::reference_type>(node->returnValue()->type());
    if (subtype1->name() != cdk::TYPE_UNSPEC && subtype2->name() != cdk::TYPE_UNSPEC) // ptr<auto> goes along with everyone
      if (subtype1->name() != subtype2->name()) 
        throw std::string("wrong type of return instruction, in function '" + _function->name() + "'");
  }
  else if (_function->is_typed(cdk::TYPE_DOUBLE) && node->returnValue()->is_typed(cdk::TYPE_INT)); // OK
  else if (_function->type()->name() != node->returnValue()->type()->name())
    throw std::string("wrong type of return instruction, in function '" + _function->name() + "'");
}

/* ====================================================================== */
/* ====[                        OTHER NODES                         ]==== */
/* ====================================================================== */

void og::type_checker::do_block_node(og::block_node *const node, int lvl) {
  // EMPTY
}

void og::type_checker::do_pointer_index_node(og::pointer_index_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->pointer()->accept(this, lvl + 2);
  if (!node->pointer()->is_typed(cdk::TYPE_POINTER))
    throw std::string("wrong type in pointer of pointer index expression");

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT))
    throw std::string("wrong type in index of pointer index expression");

  std::shared_ptr<cdk::reference_type> p = reference_type_cast(node->pointer()->type());
  node->type(p->referenced()); 
}

void og::type_checker::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl+2);
  if (!(node->argument()->is_typed(cdk::TYPE_INT) || node->argument()->is_typed(cdk::TYPE_DOUBLE)
     || node->argument()->is_typed(cdk::TYPE_STRING) || node->argument()->is_typed(cdk::TYPE_POINTER) 
     || node->argument()->is_typed(cdk::TYPE_STRUCT) )) 
     throw std::string("wrong type in argument of sizeof expression");

  node->type(cdk::make_primitive_type(4, cdk::TYPE_INT));
}

void og::type_checker::do_memalloc_node(og::memalloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->spaces()->accept(this, lvl+2);
  if (!node->spaces()->is_typed(cdk::TYPE_INT))
    throw std::string("wrong type in index of memory allocation expression");
  
  node->type(cdk::make_reference_type(4, cdk::make_primitive_type(8, cdk::TYPE_UNSPEC)));
}

void og::type_checker::do_input_node(og::input_node *const node, int lvl) {
  node->type(cdk::make_primitive_type(0,cdk::TYPE_UNSPEC));
}

void og::type_checker::do_write_node(og::write_node *const node, int lvl) {
  node->expressions()->accept(this, lvl+2);
}

void og::type_checker::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl+2);
}