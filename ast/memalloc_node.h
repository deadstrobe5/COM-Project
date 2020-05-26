#ifndef __OG_AST_memalloc_NODE_H__
#define __OG_AST_memalloc_NODE_H__

#include <cdk/ast/expression_node.h>

namespace og {

  /**
   * Class for describing memalloc nodes.
   */
  class memalloc_node: public cdk::expression_node {
    cdk::expression_node *_spaces;

  public:
    inline memalloc_node(int lineno, cdk::expression_node *spaces) :
        cdk::expression_node(lineno), _spaces(spaces) {
    }

  public:
    cdk::expression_node *spaces(){
      return _spaces;
    }
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_memalloc_node(this, level);
    }

  };

} // og

#endif
