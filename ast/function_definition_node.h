#ifndef __OG_AST_FUNCTION_DEFINITION_NODE_H__
#define __OG_AST_FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/basic_type.h>

namespace og {

  /**
   * Class for describing function definition nodes.
   */
  class function_definition_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    og::block_node *_block;

  public:
    function_definition_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &identifier, 
        cdk::sequence_node *arguments, og::block_node *block) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _block(block){
            this->type(type);
    }

    /* Procedure */
    function_definition_node(int lineno, int qualifier, const std::string &identifier, 
        cdk::sequence_node *arguments, og::block_node *block) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _block(block){
            this->type( std::shared_ptr<cdk::basic_type>( new cdk::primitive_type(0, cdk::typename_type::TYPE_VOID)));
    }

  public:
    int qualifier() {
      return _qualifier;
    }
    const std::string &identifier() const {
      return _identifier;
    }
    cdk::sequence_node *arguments() {
      return _arguments;
    }
    og::block_node *block() {
      return _block;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // og

#endif
