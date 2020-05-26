#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

//---------------------------------------------------------------------------

static std::string qualifier_name(int qualifier) {
  if (qualifier == 0) return "private";
  if (qualifier == 1) return "public";
  if (qualifier == 2) return "require";

  return "unknown qualifier";
}

//---------------------------------------------------------------------------

void og::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl + 4);
  closeTag(node, lvl + 4);
}

void og::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void og::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}
void og::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void og::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

void og::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}

void og::xml_writer::do_tuple_node(og::tuple_node *const node, int lvl) {
  openTag(node, lvl);
  node->expressions()->accept(this, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void og::xml_writer::do_identity_node(og::identity_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_position_indicator_node(og::position_indicator_node * const node, int lvl) {
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("left operand", lvl + 2);
  node->left()->accept(this, lvl + 4);
  closeTag("left operand", lvl + 2);
  openTag("right operand", lvl + 2);
  node->right()->accept(this, lvl + 4);
  closeTag("right operand", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void og::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void og::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void og::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl + 0);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_for_node(og::for_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("initialization", lvl + 2);
  node->inits()->accept(this, lvl + 4);
  closeTag("initialization", lvl + 2);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("incrementation", lvl + 2);
  node->incrs()->accept(this, lvl + 4);
  closeTag("incrementation", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_if_node(og::if_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_if_else_node(og::if_else_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_input_node(og::input_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_write_node(og::write_node *const node, int lvl) {
  //ASSERT_SAFE;

  std::string nl = "";

  node->newLine()? nl = "true" : nl = "false"; //FIX ME
  os() << std::string(lvl, ' ') << "<" << node->label() << " newLine='" << nl << "'>" << std::endl;

  openTag("expressions", lvl + 2);
  node->expressions()->accept(this, lvl + 4);
  closeTag("expressions", lvl + 2);

  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_nullptr_node(og::nullptr_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_break_node(og::break_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_continue_node(og::continue_node *const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void og::xml_writer::do_return_node(og::return_node *const node, int lvl) {
  openTag(node, lvl);

  if (node->returnValue())
    node->returnValue()->accept(this, lvl);

  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_function_call_node(og::function_call_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "'>" << std::endl;
  openTag("arguments", lvl + 2);
  if (node->arguments())
    node->arguments()->accept(this, lvl + 4);
  closeTag("arguments", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_function_declaration_node(og::function_declaration_node *const node, int lvl) {
  //ASSERT_SAFE;

  /* Nested functions checked in parser. No need to check here */

  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' qualifier='"
       << qualifier_name(node->qualifier()) << "' type='" << to_string(node->type()) << "'>" << std::endl;

  openTag("arguments", lvl);
  if (node->arguments()) {
    node->arguments()->accept(this, lvl + 4);
  }
  closeTag("arguments", lvl);
  closeTag(node, lvl);

}


void og::xml_writer::do_function_definition_node(og::function_definition_node *const node, int lvl) {
  //ASSERT_SAFE;
  
  /* Nested functions checked in parser. No need to check here */

  os() << std::string(lvl, ' ') << "<" << node->label() << " name='" << node->identifier() << "' qualifier='"
       << qualifier_name(node->qualifier()) << "' type='" << to_string(node->type()) << "'>" << std::endl;

  openTag("arguments", lvl+2);
  if (node->arguments()) {
    node->arguments()->accept(this, lvl + 4);
  }
  closeTag("arguments", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag(node, lvl + 2);

}

void og::xml_writer::do_block_node(og::block_node *const node, int lvl) {
  openTag(node, lvl);
  if (node->declarations())
    do_sequence_node(node->declarations(), lvl + 2);
  if (node->instructions())
    do_sequence_node(node->instructions(), lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_memalloc_node(og::memalloc_node *const node, int lvl) {
  openTag(node, lvl);
  node->spaces()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void og::xml_writer::do_sizeof_node(og::sizeof_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("argument", lvl + 2);
  node->argument()->accept(this, lvl + 4);
  closeTag("argument", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_tuple_declaration_node(og::tuple_declaration_node *const node, int lvl) {

  os() << std::string(lvl, ' ') << "<" << node->label() << " qualifier=\'" << qualifier_name(node->qualifier())
       << "\' type=\'" << to_string(node->type()) << "\'>" << std::endl;

  os() << std::string(lvl + 2, ' ') << "<" << "identifiers" << "> ";
  std::vector<std::string> * identifiers = node->identifiers();
  if (identifiers) {
    for (size_t i = 0; i < identifiers->size(); i++){
      if (i != 0)
        os() << ", ";
      os() << identifiers->at(i);
    }
    os() << " </" << "identifiers" << "> " << std::endl;
  }

  openTag("initializers", lvl + 2);
  if (node->initializers()) {
    node->initializers()->accept(this, lvl + 4);
  }
  closeTag("initializers", lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_variable_declaration_node(og::variable_declaration_node *const node, int lvl) {

  os() << std::string(lvl, ' ') << "<" << node->label() << " name=" << node->identifier() << " qualifier="
       << qualifier_name(node->qualifier()) << " type=" << to_string(node->type()) << ">" << std::endl;

  if (node->initializer()) {
    openTag("initializer", lvl + 2);
    node->initializer()->accept(this, lvl + 4);
    closeTag("initializer", lvl + 2);
  }

  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_pointer_index_node(og::pointer_index_node *const node, int lvl) {
  os() << std::string(lvl, ' ') << "<" << node->label() << " type=" << to_string(node->type()) << ">" << std::endl;
  node->pointer()->accept(this, lvl + 2);
  node->index()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void og::xml_writer::do_tuple_index_node(og::tuple_index_node *const node, int lvl) {
  openTag(node, lvl);
  openTag("expression", lvl + 2);
  node->tuple()->accept(this, lvl + 4);
  closeTag("expression", lvl + 2);
  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void og::xml_writer::do_evaluation_node(og::evaluation_node *const node, int lvl) {
  // EMPTY
}