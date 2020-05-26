#ifndef __OG_AST_RETURN_NODE_H__
#define __OG_AST_RETURN_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing return nodes.
   */
  class return_node: public cdk::basic_node {
    cdk::expression_node *_returnValue;

  public:
    inline return_node(int lineno, cdk::expression_node *returnValue) :
        cdk::basic_node(lineno), _returnValue(returnValue)  {
    }


  public:
    inline cdk::expression_node *returnValue() {
      return _returnValue;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_return_node(this, level);
    }

  };

} // og

#endif
