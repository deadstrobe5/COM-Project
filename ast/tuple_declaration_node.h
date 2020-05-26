#ifndef __OG_AST_TUPLE_DECLARATION_NODE_H__
#define __OG_AST_TUPLE_DECLARATION_NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/types/basic_type.h>

namespace og {

  /**
   * Class for describing tuple declaration nodes.
   */
  class tuple_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::vector<std::string> *_identifiers; 
    og::tuple_node *_initializers;

  public:
    tuple_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type, 
      std::vector<std::string> *identifiers, og::tuple_node *initializers):
        cdk::typed_node(lineno), _qualifier(qualifier), _identifiers(identifiers), _initializers(initializers) { 
          this->type(type);
    }

  public:
    int qualifier() {
      return _qualifier;
    }
    std::vector<std::string> *identifiers(){
      return _identifiers;
    }
    og::tuple_node *initializers() {
      return _initializers;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_tuple_declaration_node(this, level);
    }

  };

} //og

#endif