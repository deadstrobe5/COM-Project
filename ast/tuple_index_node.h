#ifndef __OG_AST_TUPLE_INDEX_NODE_H__
#define __OG_AST_TUPLE_INDEX_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include <cdk/ast/integer_node.h>

namespace og {

  /**
   * Class for describing tuple index nodes.
   */
  class tuple_index_node: public cdk::lvalue_node {
    cdk::expression_node *_tuple;
    cdk::integer_node *_index;

  public:
    inline tuple_index_node(int lineno, cdk::expression_node *tuple, cdk::integer_node *index) :
        cdk::lvalue_node(lineno), _tuple(tuple), _index(index) {
    }

  public:
    cdk::expression_node *tuple(){
      return _tuple;
    }
    cdk::integer_node *index(){
      return _index;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_tuple_index_node(this, level);
    }

  };

} // og

#endif
